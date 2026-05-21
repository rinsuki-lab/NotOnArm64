#include <Windows.h>
#include <stdio.h>

#include "utils.hpp"

void OurErrorReport(const wchar_t* message) {
    MessageBoxW(NULL, message, L"NotOnArm64_Payload", MB_OK | MB_ICONERROR);
}

void OurErrorReport_WithLastError(const wchar_t* message) {
    wchar_t fullMessage[512];
    DWORD lastError = GetLastError();
    swprintf(fullMessage, 512, L"%s\nGetLastError: %u", message, lastError);
    OurErrorReport(fullMessage);
}