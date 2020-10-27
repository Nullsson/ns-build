/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

#ifndef NS_BUILD_H
#define NS_BUILD_H

#if defined(_MSC_VER)
#define NSBUILD_EXPORT __declspec(dllexport)
#if _MSC_VER < 1900
#define MSVC2013_C 1
#endif
#else
#define NSBUILD_EXPORT
#endif

#if !defined(MSVC2013_C)
#define MSVC2013_C 0
#endif

#if defined(__cplusplus)
#define NSBUILD_EXTERN_C extern "C"
#else
#define NSBUILD_EXTERN_C
#endif

#define NSBUILD_FUNC NSBUILD_EXPORT NSBUILD_EXTERN_C
#define NSBUILD_PROC NSBUILD_FUNC

#if MSVC2013_C
#define NSBUILD_HEADER_PROC static
#else
#define NSBUILD_HEADER_PROC static inline
#endif

// NOTE(Oskar): Callback functions
// TODO(Oskar): Documentation

typedef void NSBuildInitCallback(void);
typedef void NSBuildCleanUpCallback(void);

#endif
