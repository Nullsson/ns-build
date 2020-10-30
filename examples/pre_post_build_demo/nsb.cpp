/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

#include "../../code/ns_build.h"
#include <stdio.h>
#include <Windows.h>

LARGE_INTEGER Frequency;
LARGE_INTEGER StartingTime;
LARGE_INTEGER EndingTime;

NSBUILD_FUNC void
NSBuildCustomInitCallback(void)
{
    printf("This is a custom InitCallback function\n");
    printf("Code here will be ran before the compile of your application.\n");

    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&StartingTime);
}

NSBUILD_FUNC void
NSBuildCustomCleanUpCallback(void)
{
    printf("This is a custom CleanupCallback function\n");
    printf("Code here will be ran after the compile of your application.\n");
    QueryPerformanceCounter(&EndingTime);
    double ElapsedTime = (double)(EndingTime.QuadPart - StartingTime.QuadPart) / (double)Frequency.QuadPart;
    ElapsedTime *= 1000.0;

    printf("Build took %fms", ElapsedTime);
}
