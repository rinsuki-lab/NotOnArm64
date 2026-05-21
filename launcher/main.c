#include <Windows.h>

#ifndef _WIN64
#error payload側で AMD64 決め打ちで上書きしているので、実行環境がx64を実行できることを担保するため、ランチャーはx64である必要があります (古い Windows 10 などは x64 エミュレーションが未実装のため)
#endif

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    SetErrorMode(0);
    HMODULE hModule = LoadLibraryW(L"NotOnArm64_Payload.64.dll");
    if (hModule == NULL) {
        MessageBoxW(NULL, L"Failed to load DLL!", L"NotOnArm64_Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }

    wchar_t fileNameBuf[MAX_PATH];
    ZeroMemory(fileNameBuf, sizeof(fileNameBuf));
    
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = fileNameBuf;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Windows Executable\0*.exe\0\0";
    ofn.lpstrTitle = L"Select an installer executable to launch with NotOnArm64";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (!GetOpenFileNameW(&ofn)) {
        return 1;
    }

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcessW(fileNameBuf, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!success) {
        MessageBoxW(NULL, L"Failed to launch exe!", L"NotOnArm64_Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}