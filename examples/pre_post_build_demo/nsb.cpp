/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

#include "../../code/ns_build.h"
#include <stdio.h>

NSBUILD_FUNC void
NSBuildCustomInitCallback(void)
{
    printf("This is a custom InitCallback function\n");
    printf("This will be ran before the build.\n");
}

NSBUILD_FUNC void
NSBuildCustomCleanUpCallback(void)
{

}
