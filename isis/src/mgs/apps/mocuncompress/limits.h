/*
NOTICE

The software accompanying this notice (the "Software") is provided to you
free of charge to facilitate your use of the data collected by the Mars
Orbiter Camera (the "MOC Data").  Malin Space Science Systems ("MSSS")
grants to you (either as an individual or entity) a personal,
non-transferable, and non-exclusive right (i) to use and reproduce the
Software solely for the purpose of accessing the MOC Data; (ii) to modify
the source code of the Software as necessary to maintain or adapt the
Software to run on alternate computer platforms; and (iii) to compile, use
and reproduce the modified versions of the Software solely for the purpose
of accessing the MOC Data.  In addition, you may distribute the Software,
including any modifications thereof, solely for use with the MOC Data,
provided that (i) you must include this notice with all copies of the
Software to be distributed; (ii) you may not remove or alter any
proprietary notices contained in the Software; (iii) you may not charge any
third party for the Software; and (iv) you will not export the Software
without the appropriate United States and foreign government licenses.  

You acknowledge that no title to the intellectual property in the Software
is transferred to you.  You further acknowledge that title and full
ownership rights to the Software will remain the exclusive property of MSSS
or its suppliers, and you will not acquire any rights to the Software
except as expressly set forth above.  The Software is provided to you AS
IS.  MSSS MAKES NO WARRANTY, EXPRESS OR IMPLIED, WITH RESPECT TO THE
SOFTWARE, AND SPECIFICALLY DISCLAIMS THE IMPLIED WARRANTIES OF
NON-INFRINGEMENT OF THIRD PARTY RIGHTS, MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE.  SOME JURISDICTIONS DO NOT ALLOW THE EXCLUSION OR
LIMITATION OF INCIDENTAL OR CONSEQUENTIAL DAMAGES, SO SUCH LIMITATIONS OR
EXCLUSIONS MAY NOT APPLY TO YOU.  

Your use or reproduction of the Software constitutes your agreement to the
terms of this Notice.  If you do not agree with the terms of this notice,
promptly return or destroy all copies of the Software in your possession.  

Copyright (C) 1999 Malin Space Science Systems.  All Rights Reserved.

SCCSID @(#)limits.h	1.1 10/04/99
*/
/* SCCShid @(#)limits.h (limits.h) 1.4 */

#if !defined(limits_h)

#define limits_h

/*
* The transform block size.  IMPORTANT NOTE:  If changed to another number
* EXTENSIVE re-thinking of the transform compressor is necessary.  MANY
* calculations will not fit in their allocated word sizes (including the
* transform coefficients), the "unwound" loops will not execute the correct
* number of times, etc.  This is defined here so SOME dependencies can use
* a common definition.
*/
#define BLOCKSIZE	 256
/* The log (base 2) of the block size (used to avoid calculation overflow) */
#define LOGBLOCKSIZE	   8
/* The size of the image block in the x and y direction */
#define BLOCKDIMENSION	  16

/*
* The maximum number of blocks in an image (and its log base 2).  IMPORTANT
* NOTE:  If changed to another number SUBSTANTIAL re-thinking of the
* transform compressor is necessary.  SEVERAL calculations will not fit
* in their allocated word sizes, etc.  This is defined here so SOME
* dependencies can use a common definition.
*/
#define MAXBLOCKS	1024
#define LOGMAXBLOCKS	  10

/*
* The maximum number of ranking groups (and its log base 2).  IMPORTANT
* NOTE:  If changed to another number re-thinking of the transform
* compressor is necessary.  This is defined here so SEVERAL dependencies
* can use a common definition.
*/
#define MAXGROUPS	   8
#define LOGMAXGROUPS	   3

/*
* The maximum number of encoding tables (and its log base 2).  IMPORTANT
* NOTE:  If changed to another number re-thinking of the transform
* compressor is necessary.  This is defined here so SEVERAL dependencies
* can use a common definition.
*/
#define MAXCODES	   8
#define LOGMAXCODES	   3

/*
* The maximum size of an entry in the weighting table for the "AC energy"
* calculation (and its log base 2).  IMPORTANT NOTE:  If changed to another
* number re-thinking of the transform compressor is necessary.  This is
* defined here so SEVERAL dependencies can use a common definition.
*/
#define MAXWEIGHT	  32
#define LOGMAXWEIGHT	   5

#endif
