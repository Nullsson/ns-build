/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

struct NSBuildDynamicCode
{
    NSBuildInitCallback    *InitCallback;
    NSBuildCleanUpCallback *CleanUpCallback;

#if BUILD_WIN32
    HMODULE DLLHandle;
#elif BUILD_LINUX
    void  *DLLHandle;
#endif
};

static NSBuildDynamicCode
NSBuildDynamicCodeLoad(char *DLLPath)
{
    NSBuildDynamicCode DynamicCode = {0};

#if BUILD_WIN32
    DynamicCode.DLLHandle = LoadLibraryA(DLLPath);
    if(DynamicCode.DLLHandle)
    {
        // TODO(Oskar): Implement Log function.
        DynamicCode.InitCallback    = (NSBuildInitCallback *)GetProcAddress(DynamicCode.DLLHandle, "NSBuildCustomInitCallback");
        DynamicCode.CleanUpCallback = (NSBuildCleanUpCallback *)GetProcAddress(DynamicCode.DLLHandle, "NSBuildCustomCleanUpCallback");
    }
#elif BUILD_LINUX
#endif

    if(!DynamicCode.InitCallback && !DynamicCode.CleanUpCallback)
    {
        // TODO(Oskar): Logging.
    }
    
    return (DynamicCode);
}

static void
NSBuildDynamicCodeUnload(NSBuildDynamicCode *DynamicCode)
{
#if BUILD_WIN32
    FreeLibrary(DynamicCode->DLLHandle);
#elif BUILD_LINUX
#endif

    DynamicCode->InitCallback = 0;
    DynamicCode->CleanUpCallback = 0;
    DynamicCode->DLLHandle = 0;
}
