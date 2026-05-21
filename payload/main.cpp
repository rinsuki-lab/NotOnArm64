#include <Windows.h>

#include <MinHook.h>

#include "hook_create_process.hpp"
#include "hook_hide_arm64.hpp"

#include <string>

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

            bool onInstaller = false;

            std::wstring wstr(MAX_PATH, L'\0');
            wstr.resize(GetModuleFileNameW(NULL, wstr.data(), MAX_PATH));
            // Inno Setup は TMPDIR に真・インストーラーを解凍して子プロセスとして呼び出す。HideArm64はそちらだけに適用したい
            // e.g. C:\Users\user\AppData\Local\Temp\is-424DH.tmp\setup.tmp
            if (wstr.contains(L"\\is-") && wstr.ends_with(L".tmp")) {
                onInstaller = true;
            }

            if (onInstaller) {
                // インストーラーから起動したアプリまでフックしてしまうと困るのでインストーラーでは CreateProcessW をフックしない
                if (!HookHideArm64()) {
                    return FALSE;
                }
            } else {
                if (!HookCreateProcess()) {
                    return FALSE;
                }
            }
            break;
        }
    }

    return TRUE;
}