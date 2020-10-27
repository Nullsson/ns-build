/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Oskar Mendel $
   $Notice: (C) Copyright 2020 by Nullsson, Inc. All Rights Reserved. $
   ======================================================================== */

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

inline unsigned int
StringLength(char *String)
{
    unsigned int Count = 0;
    if (String)
    {
        while (*String++)
        {
            ++Count;
        }
    }
    
    return (Count);
}
