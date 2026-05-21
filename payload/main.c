#include <Windows.h>

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved
) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;
    }
}