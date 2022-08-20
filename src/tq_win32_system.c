
//------------------------------------------------------------------------------

#if defined(TQ_WIN32)

//------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <tchar.h>
#include <windows.h>

//------------------------------------------------------------------------------

// Classic C entry point
extern int main(int, char **);

//------------------------------------------------------------------------------

// Nothing happens here. But in the future, some pre-initialization might be
// done inside this entry point.
// edit: no longer true

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    SetProcessDPIAware();

    return main(__argc, __targv);
}

//------------------------------------------------------------------------------

#endif // defined(TQ_WIN32)

//------------------------------------------------------------------------------
