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
#include "ns_build_debug.cpp"
#include "ns_build_util.cpp"
#include "ns_build_config.cpp"
#include "ns_build_dynamic.cpp"

void
Build(char *CodePath)
{
#if BUILD_WIN32
    STARTUPINFO Info;
    PROCESS_INFORMATION ProcessInformation;

    ZeroMemory(&Info, sizeof(Info));
    Info.cb = sizeof(Info);
    ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

    // TODO(Oskar): Allow for custom build and link flags in config for the dynamic code.
    char BuildCommand[2048] = "clang-cl -D_CRT_SECURE_NO_DEPRECATE -nologo /Zi";
    strcat(BuildCommand, " ");
    strcat(BuildCommand, CodePath);
    strcat(BuildCommand, " ");
    strcat(BuildCommand, "/LD /link /out:nsb.dll");

    Log("Building DynamicCode with arguments: %s", BuildCommand);
    
    if (CreateProcess(NULL,                    // Application name, null so we will use command line.
                      BuildCommand,            // Command line to be executed.
                      NULL,                    // Process handle cannot be inherited.
                      NULL,                    // Thread handle cannot be inherited.
                      FALSE,                   // Set handle inheritance to false making it not inherited.
                      0,                       // Creation flags, controls the priority class etc.
                      NULL,                    // Pointer to environment block, use parent's environment block.
                      NULL,                    // Full path to the current directory of the process, use parent's starting directory.
                      &Info,                   // Pointer to STARTUPINFO structure.
                      &ProcessInformation))    // Pointer to PROCESS_INFORMATION structure.
    {
        // TODO(Oskar): Maybe don't wait infinite time?
        WaitForSingleObject(ProcessInformation.hProcess, INFINITE);

        // NOTE(Oskar): Close the process when its done.
        CloseHandle(ProcessInformation.hProcess);
        CloseHandle(ProcessInformation.hThread);
    }

    Log("DynamicCode successfully built!");
#elif BUILD_LINUX
    
#endif
}

void PerformBuildStep(NSBuildConfig *Config)
{
#if BUILD_WIN32
    STARTUPINFO Info;
    PROCESS_INFORMATION ProcessInformation;

    ZeroMemory(&Info, sizeof(Info));
    Info.cb = sizeof(Info);
    ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

    char BuildCommand[2048];
    snprintf(BuildCommand, sizeof(BuildCommand), "%s %s %s %s",
             Config->CompilerBackend,
             Config->CompilerArguments,
             Config->EntryFileName,
             Config->LinkerArguments);

    Log("Building project with arguments: %s", BuildCommand);
    
    if (CreateProcess(NULL,                    // Application name, null so we will use command line.
                      BuildCommand,            // Command line to be executed.
                      NULL,                    // Process handle cannot be inherited.
                      NULL,                    // Thread handle cannot be inherited.
                      FALSE,                   // Set handle inheritance to false making it not inherited.
                      0,                       // Creation flags, controls the priority class etc.
                      NULL,                    // Pointer to environment block, use parent's environment block.
                      NULL,                    // Full path to the current directory of the process, use parent's starting directory.
                      &Info,                   // Pointer to STARTUPINFO structure.
                      &ProcessInformation))    // Pointer to PROCESS_INFORMATION structure.
    {
        // TODO(Oskar): Maybe don't wait infinite time?
        WaitForSingleObject(ProcessInformation.hProcess, INFINITE);

        // NOTE(Oskar): Close the process when its done.
        CloseHandle(ProcessInformation.hProcess);
        CloseHandle(ProcessInformation.hThread);
    }

    Log("Successfully built application.");
#elif BUILD_LINUX
#endif
}

int
main(int argc, char **args)
{
    if (argc > 1)
    {
        if (StringsAreEqual(args[1], "help") ||
            StringsAreEqual(args[1], "-h") ||
            StringsAreEqual(args[1], "--help"))
        {
            printf("NS Build Flags:\n");
            printf("--verbose    (-v)    Enable logging and more verbose messages.\n");
            printf("--new        (-n)    Initiate new project.\n");
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
                else if (StringsAreEqual(args[Index], "-n") ||
                         StringsAreEqual(args[Index], "--new") ||
                         StringsAreEqual(args[Index], "init") ||
                         StringsAreEqual(args[Index], "--init"))
                {
                    // TODO(Oskar): Create new config and template file.
                }
            }
        }
    }

    NSBuildConfig Config = {0};
    {
        char CurrentDirectory[MAX_PATH];
        DWORD PathSize = GetCurrentDirectory(MAX_PATH, CurrentDirectory);

        // TODO(Oskar): Dynamically find nsbconf file.
        strcat(CurrentDirectory, "/buildconfig.nsbconf");
        
        Config = NSBuildConfigLoad(CurrentDirectory);
        Log("NS Build config found and loaded.");
    }

    char DynamicCodePath[2048];
    DWORD Size = GetCurrentDirectory(2048, DynamicCodePath);
    if (Size)
    {
        unsigned int FileNameSize = StringLength(Config.DynamicCodeFileName);
        char *Buffer = static_cast<char *>(malloc(Size + FileNameSize));
        strcpy(Buffer, DynamicCodePath);
        strcat(Buffer, "\\");
        strcat(Buffer, Config.DynamicCodeFileName);
        Build(Buffer);
        free(Buffer);
    }

    NSBuildDynamicCode DynamicCode = NSBuildDynamicCodeLoad("nsb.dll");
    if (DynamicCode.InitCallback)
    {
        Log("Running init callback ...");
        DynamicCode.InitCallback();
    }
    
    PerformBuildStep(&Config);

    if (DynamicCode.CleanUpCallback)
    {
        Log("Running cleanup callback ...\n");
        DynamicCode.CleanUpCallback();
    }

    NSBuildConfigUnload(&Config);
    
    return (0);
}
