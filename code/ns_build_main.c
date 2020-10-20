#if BUILD_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif BUILD_LINUX
#endif

#include <stdio.h>

// TODO(Oskar): Create ns_build.h file with function definitions to be used by a dll
// TODO(Oskar): Create an example_ns_config.c that uses ns_build.h and compile it using the clang process.
// TODO(Oskar): Run example_ns_config.exe using CreateProcess to build the project. 

int main(int argc, char **args)
{
    printf("Hello World!\n");
    
#if META_BUILD
    printf("This was successfully built using NS-Build!\n");
#endif

#if BUILD_WIN32
    STARTUPINFO Info;
    PROCESS_INFORMATION ProcessInformation;

    ZeroMemory(&Info, sizeof(Info));
    Info.cb = sizeof(Info);
    ZeroMemory(&ProcessInformation, sizeof(ProcessInformation));

    // TODO(Oskar): Build compilation string dynamically and change theese values later on.
    // TODO(Oskar): Document arguments for self learning purposes.
    if (CreateProcess(NULL,
                      "clang-cl -D_CRT_SECURE_NO_DEPRECATE -nologo /Zi -DBUILD_WIN32=1 -DBUILD_LINUX=0 -DMETA_BUILD=1 ../code/ns_build_main.c /link /out:ns_meta.exe",
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
    
    return 0;
}
