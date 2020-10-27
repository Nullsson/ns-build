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
#include <string.h>

// NOTE(Oskar): NSBuild
#include "ns_build.h"
#include "ns_build_util.cpp"
#include "ns_build_config.cpp"
#include "ns_build_dynamic.cpp"

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
    char BuildCommand[2048] = "clang-cl -D_CRT_SECURE_NO_DEPRECATE -nologo /Zi";
    strcat(BuildCommand, " ");
    strcat(BuildCommand, CodePath);
    strcat(BuildCommand, " ");
    strcat(BuildCommand, "/LD /link /out:nsb.dll");

    printf("%s\n", BuildCommand);
    
    // TODO(Oskar): Document arguments for self learning purposes.
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
void PerformBuildStep()
{
    
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

    NSBuildConfig Config = {0};
    {
        char CurrentDirectory[MAX_PATH];
        DWORD PathSize = GetCurrentDirectory(MAX_PATH, CurrentDirectory);

        strcat(CurrentDirectory, "/buildconfig.nsbconf");
        
        Config = NSBuildConfigLoad(CurrentDirectory);
        printf("DynamicCodeFileName: %s\n", Config.DynamicCodeFileName);
        printf("EntryFileName: %s\n", Config.EntryFileName);
        printf("CompilerBackend: %s\n", Config.CompilerBackend);
        printf("CompilerArguments: %s\n", Config.CompilerArguments);
        printf("LinkerArguments: %s\n", Config.LinkerArguments);
    }

    char DynamicCodePath[2048];
    DWORD Size = GetCurrentDirectory(2048, DynamicCodePath);
    if (Size)
    {
        unsigned int FileNameSize = StringLength(Config.DynamicCodeFileName);
        char Buffer[Size + FileNameSize + 1];
        snprintf(Buffer, sizeof(Buffer), "%s\\%s", DynamicCodePath, Config.DynamicCodeFileName);

        Build(Buffer);
    }

    // TODO(Oskar): Logging.
    NSBuildDynamicCode DynamicCode = NSBuildDynamicCodeLoad("nsb.dll");
    if (DynamicCode.InitCallback)
    {
        printf("Callback found, running code!\n");
        DynamicCode.InitCallback();
    }
    
    PerformBuildStep();

    NSBuildConfigUnload(&Config);
    
    return (0);
}
