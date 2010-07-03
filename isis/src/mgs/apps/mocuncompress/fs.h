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

SCCSID @(#)fs.h	1.1 10/04/99
*/
/* SCCShid @(#)fs.h (fs.h) 1.1 */

/*
* DESCRIPTION
*
*	Typedefs for commonly used signed and unsigned integers of
*	different length (in bits).
*
* COMMENTARY
*
*	It is intended that these definitions refer to integers of
*	the exact length specified and can be used as primitives in
*	such things as arithmetic operations.
*/

#if !defined(fs_h)

#define fs_h

/* Unsigned integers of lengths 32, 16, and 8 bits, respectively */

typedef unsigned int	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;

/* An unsigned integer of unspecified length */
/* The '#ifndef _TYPES_' is to avoid clash with <sys/types.h> */

#if !defined(_TYPES_) && !defined(_SYS_BSD_TYPES_H)
typedef unsigned int	UINT;
#endif

/* Signed integers of lengths 32, 16, and 8 bits, respectively */

typedef int		int32;
typedef short		int16;
typedef char		int8;

#endif
