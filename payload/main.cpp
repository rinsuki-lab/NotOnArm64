#include <Windows.h>

#include <MinHook.h>

#include "hook_create_process.hpp"
#include "hook_hide_arm64.hpp"

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved
) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH: {
            if (MH_Initialize() != MH_OK) {
                return FALSE;
            }
            if (!HookCreateProcess()) {
                return FALSE;
            }
            if (!HookHideArm64()) {
                return FALSE;
            }
            break;
        }
    }

    return TRUE;
}