#include <Windows.h>

int main() {
    SetErrorMode(0);
    HMODULE hModule = LoadLibraryW(L"NotOnArm64_Payload.64.dll");
    if (hModule == NULL) {
        MessageBoxW(NULL, L"Failed to load DLL!", L"NotOnArm64_Launcher", MB_OK | MB_ICONERROR);
        return 1;
    }
    MessageBoxW(NULL, L"Hello, World! 2", L"NotOnArm64_Launcher", MB_OK);
    return 0;
}