#include "main.hpp"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);

        base::SAMP::getRef()->events.onInitNetGame.connect([]() {
            base::SAMP::getRef()->addMessage(0xFFAA00, "Hello wolrd");
            base::SAMP::getRef()->addCmd("test_plg", [](const char* param) { base::SAMP::getRef()->addMessage(-1, "Hello wolrd"); });
        });
    }
    return TRUE;
}
