#include <Windows.h>

int main() {
    SetErrorMode(0);
    HMODULE hModule = LoadLibraryW(L"NotOnArm64_Payload.64.dll");
    if (hModule == NULL) {
        MessageBoxW(NULL, L"Failed to load DLL!", L"NotOnArm64_Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcessW(L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!success) {
        MessageBoxW(NULL, L"Failed to launch exe!", L"NotOnArm64_Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }
    return 0;
}