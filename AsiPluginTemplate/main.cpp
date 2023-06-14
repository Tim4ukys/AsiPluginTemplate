#include "main.hpp"


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);

    }
    else if (fdwReason == DLL_PROCESS_DETACH) {

    }
    return TRUE;
}
