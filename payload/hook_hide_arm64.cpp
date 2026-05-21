#include <Windows.h>
#include <MinHook.h>

#include "hook_hide_arm64.hpp"
#include "utils.hpp"

using IsWow64Process2_t = decltype(&IsWow64Process2);
IsWow64Process2_t Original_IsWow64Process2;

BOOL WINAPI Our_IsWow64Process2(HANDLE hProcess, USHORT* pProcessMachine, USHORT* pNativeMachine) {
    BOOL result = Original_IsWow64Process2(hProcess, pProcessMachine, pNativeMachine);
    OurErrorReport(L"called IsWow64Process2");
    if (result && *pNativeMachine == IMAGE_FILE_MACHINE_ARM64) {
        OurErrorReport(L"IsWow64Process2: changing native machine from ARM64 to AMD64");
        *pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
    } else {
        OurErrorReport(L"IsWow64Process2: native machine is not ARM64, no change needed");
    }
    return result;
}

bool HookHideArm64() {
    if (MH_CreateHook(&IsWow64Process2, &Our_IsWow64Process2, (LPVOID*)&Original_IsWow64Process2) != MH_OK) {
        OurErrorReport(L"Failed to create hook for IsWow64Process2");
        return FALSE;
    }
    if (MH_EnableHook(&IsWow64Process2) != MH_OK) {
        OurErrorReport(L"Failed to enable hook for IsWow64Process2");
        return FALSE;
    }
    return TRUE;
}