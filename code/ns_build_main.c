/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

#if BUILD_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif BUILD_LINUX
#endif

// NOTE(Oskar): CRT
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// NOTE(Oskar): NSBuild
#include "ns_build.h"

// TODO(Oskar): Create an ns_template.c that uses ns_build.h and compile it using the clang process.
// TODO(Oskar): Run example_ns_config.exe using CreateProcess to build the project. 
// TODO(Oskar): Add helper functions to ns_build.h if needed.

void
Build()
{
    // TODO(Oskar): Clean up!
#if BUILD_WIN32
    STARTUPINFO Info;
    PROCESS_INFORMATION ProcessInformation;

    ZeroMemory(&Info, sizeof(Info));
    Info.cb = sizeof(Info);
    ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

    // TODO(Oskar): Build compilation string dynamically and change theese values later on.
    // TODO(Oskar): Document arguments for self learning purposes.
    if (CreateProcess(NULL,
                      "clang-cl -D_CRT_SECURE_NO_DEPRECATE -nologo /Zi -I ../code/ ../code/ns_template.c /LD /link /out:nsb.dll",
                      NULL,
                      NULL,
                      FALSE,
                      0,
                      NULL,
                      NULL,
                      &Info,
                      &ProcessInformation))
    {
        // TODO(Oskar): Maybe don't wait infinite time?
        WaitForSingleObject(ProcessInformation.hProcess, INFINITE);

        // NOTE(Oskar): Close the process when its done.
        CloseHandle(ProcessInformation.hProcess);
        CloseHandle(ProcessInformation.hThread);
    }

    printf("Exiting safe and sound!\n");
#endif
}

// TODO(Oskar): Implement!
void Run()
{
    
}

int
main(int argc, char **args)
{
    printf("Hello World!\n");

    Build();
    Run();
    
    return 0;
}
