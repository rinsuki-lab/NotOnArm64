#include <Windows.h>
#include <PathCch.h>
#include <stdio.h>

#include <MinHook.h>

#include "utils.hpp"
#include "hook_create_process.hpp"

using CreateProcessW_t = decltype(&CreateProcessW);

CreateProcessW_t Original_CreateProcessW;

bool InjectDll(HANDLE hProcess, const wchar_t* dllPath, size_t dllPathSize) {
    LPVOID remoteString = VirtualAllocEx(hProcess, NULL, dllPathSize, MEM_COMMIT, PAGE_READWRITE);
    if (!remoteString) {
        OurErrorReport_WithLastError(L"Failed to allocate remoteString on remote process");
        return false;
    }
    if (!WriteProcessMemory(hProcess, remoteString, dllPath, dllPathSize, NULL)) {
        OurErrorReport_WithLastError(L"Failed to write dllPath to remote process");
        return false;
    }

    // なんと Windows は kernel32.dll (とか)をどのプロセスでも同じアドレスに読み込んでいるらしい
    // ので、自分のプロセスで GetProcAddress したアドレスがリモートプロセスでも使えるらしい
    // TODO: ASLR とかどうなるのさ?
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    LPVOID loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryW");
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteString, 0, NULL);
    if (!hRemoteThread) {
        OurErrorReport_WithLastError(L"Failed to create remote thread in target process");
        return false;
    }
    WaitForSingleObject(hRemoteThread, INFINITE);
    CloseHandle(hRemoteThread);
}

extern "C" __declspec(dllexport)
void InjectDllToTargetProcessW(HWND hWnd, HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow) {
    int pid = _wtoi(lpCmdLine);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        OurErrorReport_WithLastError(L"Failed to open target process to inject DLL");
        return;
    }
    HMODULE hDllModule;
    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)InjectDllToTargetProcessW,
        &hDllModule
    )) {
        OurErrorReport_WithLastError(L"Failed to get module handle for DLL to inject");
        return;
    };
    wchar_t dllPath[MAX_PATH];
    if (!GetModuleFileNameW(hDllModule, dllPath, MAX_PATH)) {
        OurErrorReport_WithLastError(L"Failed to get module file name for DLL to inject");
        return;
    }
    if (!InjectDll(hProcess, dllPath, sizeof(dllPath))) {
        OurErrorReport(L"Failed to inject DLL to target process");
        return;
    }
}

BOOL WINAPI Our_CreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    HMODULE hDllModule;
    if (!GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)Our_CreateProcessW,
        &hDllModule
    )) {
        return FALSE;
    };
    wchar_t dllPath[MAX_PATH];
    if (!GetModuleFileNameW(hDllModule, dllPath, MAX_PATH)) {
        return FALSE;
    }

    BOOL shouldNotResume = !!(dwCreationFlags & CREATE_SUSPENDED);
    dwCreationFlags |= CREATE_SUSPENDED;
    BOOL res = Original_CreateProcessW(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );
    if (!res) return res;

    // Inject DLL
    #ifdef _WIN64
    const BOOL isOur32 = false;
    #else
    const BOOL isOur32 = true;
    #endif
    BOOL isTarget32 = false;
    if (!IsWow64Process(lpProcessInformation->hProcess, &isTarget32)) goto failed_after_create_process;
    
    if (isTarget32 != isOur32) {
        // 違うビット数の場合は他プロセスにやってもらう
        wchar_t dllDirPath[MAX_PATH];
        CopyMemory(dllDirPath, dllPath, sizeof(dllPath));
        PathCchRemoveFileSpec(dllDirPath, MAX_PATH);

        STARTUPINFOW si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;

        wchar_t args[256];
        swprintf(args, 256, L"rundll32.exe NotOnArm64_Payload.%d.dll,InjectDllToTargetProcess %d", isTarget32 ? 32 : 64, lpProcessInformation->dwProcessId);

        if (!Original_CreateProcessW(
            L"C:\\Windows\\System32\\rundll32.exe",
            args,
            NULL, NULL, FALSE, 0, NULL, dllDirPath,
            &si, &pi
        )) {
            OurErrorReport_WithLastError(L"Failed to create child process to inject DLL for different bitness process");
            goto failed_after_create_process;
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
    } else {
        // 同じビット数の場合は自分で行う
        if (!InjectDll(lpProcessInformation->hProcess, dllPath, sizeof(dllPath))) {
            goto failed_after_create_process;
        }
    }

continue_process:
    if (!shouldNotResume) {
        ResumeThread(lpProcessInformation->hThread);
    }
    return TRUE;
failed_after_create_process:
    TerminateProcess(lpProcessInformation->hProcess, 1);
    return FALSE;
}

bool HookCreateProcess() {
    if (MH_CreateHook(&CreateProcessW, &Our_CreateProcessW, (LPVOID*)&Original_CreateProcessW) != MH_OK) {
        OurErrorReport(L"Failed to create hook for CreateProcessW");
        return FALSE;
    }
    if (MH_EnableHook(&CreateProcessW) != MH_OK) {
        OurErrorReport(L"Failed to enable hook for CreateProcessW");
        return FALSE;
    }
    wchar_t modulePath[MAX_PATH];
    GetModuleFileNameW(NULL, modulePath, MAX_PATH);
    MessageBoxW(NULL, L"Hooked CreateProcessW!", modulePath, MB_OK);
    return TRUE;
}