#if defined(SIGNAL_COMPILATION)

#include <windows.h>
#include "defines.h"

int main(int argc, char** argv)
{
    if (argc < 2) return EXIT_FAILURE;

    const char* event_name = argv[ 1 ];

    HANDLE e = OpenEventA(EVENT_MODIFY_STATE, FALSE, event_name);
    if (e == INVALID_HANDLE_VALUE || !e) return EXIT_FAILURE;

    WINBOOL res = SetEvent(e);
    
    CloseHandle(e);

    return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif