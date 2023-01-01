//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
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
