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

SCCSID @(#)types.h	1.1 10/04/99
*/
/* SCCShid @(#)types.h (types.h) 1.1 */

#if !defined(types_h)

#define types_h

/* 'real' data types */

typedef unsigned char         cr;
typedef short                 sr;
typedef int                   ir;
typedef float                 fr;
typedef double                dr;

/* 'complex' data types */

typedef struct char_complex   cc;
typedef struct short_complex  sc;
typedef struct int_complex    ic;
typedef struct float_complex  fc;
typedef struct double_complex dc;

struct char_complex   { unsigned char r,i; };
struct short_complex  { short         r,i; };
struct int_complex    { int           r,i; };
struct float_complex  { float         r,i; };
struct double_complex { double        r,i; };

/* Header constant (the third) flagging picture data type */

#define CHAR_REAL	0
#define SHORT_REAL	1
#define INT_REAL	2
#define FLOAT_REAL	3
#define DOUBLE_REAL	4

#define CHAR_COMPLEX	5
#define SHORT_COMPLEX	6
#define INT_COMPLEX	7
#define FLOAT_COMPLEX	8
#define DOUBLE_COMPLEX	9

#endif
