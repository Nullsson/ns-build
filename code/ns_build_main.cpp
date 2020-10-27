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
#include "ns_build_dynamic.cpp"

// TODO(Oskar): Add helper functions to ns_build.h if needed.

static int UseLogging = 0;

void
Build(char *CodePath)
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
    char BuildCommand[2048] = "clang-cl -D_CRT_SECURE_NO_DEPRECATE -nologo /Zi -I ";
    strcat(BuildCommand, CodePath);
    strcat(BuildCommand, " ");
    strcat(BuildCommand, CodePath);
    strcat(BuildCommand, "/nsb.cpp ");
    strcat(BuildCommand, "/LD /link /out:nsb.dll");
    
    if (CreateProcess(NULL,
                      BuildCommand,
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

unsigned int
StringsAreEqual(char *A, char *B)
{
    unsigned int Result = (A == B);
    
    if(A && B)
    {
        while(*A && *B && (*A == *B))
        {
            ++A;
            ++B;
        }
        
        Result = ((*A == 0) && (*B == 0));
    }

    return (Result);
}

int
main(int argc, char **args)
{
    if (argc > 1)
    {
        if (StringsAreEqual(args[1], "help"))
        {
            // TODO(Oskar): Print help information.
        }
        else
        {
            for(int Index = 1; Index < argc; ++Index)
            {
                if (StringsAreEqual(args[Index], "-v") ||
                    StringsAreEqual(args[Index], "--verbose"))
                {
                    UseLogging = 1;
                }
                // TODO(Oskar): Check for argument to create new config.
            }
        }
    }

    // TODO(Oskar): Read config file
    {
        char CurrentDirectory[MAX_PATH];
        DWORD PathSize = GetCurrentDirectory(MAX_PATH, CurrentDirectory);

        
    }

    
    char DynamicCodePath[2048];
    DWORD Size = GetCurrentDirectory(2048, DynamicCodePath);
    if (Size)
    {
        // TODO(Oskar): Name of this file should be gotten from the config.
        strcat(DynamicCodePath, "/code"); 
        printf("%s\n", DynamicCodePath);
        Build(DynamicCodePath);
    }

    // TODO(Oskar): Logging.
    NSBuildDynamicCode DynamicCode = NSBuildDynamicCodeLoad("nsb.dll");
    if (DynamicCode.InitCallback)
    {
        printf("Callback found, running code!\n");
        DynamicCode.InitCallback();
    }
    
    Run();
    
    return (0);
}
