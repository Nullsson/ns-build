/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

static int UseLogging = 0;
#define Log(...) if(UseLogging) { fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); }
#define LogError(...) fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    exit(0);
