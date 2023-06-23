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

SCCSID @(#)reorder.static.h  1.1 10/04/99
*/
/* This file was automatically generated by 'makeOrder' */

#ifndef reorder_static_h

#define reorder_static_h

#include "fs.h"

/*
* Translation table for going from row ordered vector to
* radially order vector
*/
static uint8 trans[256] = {
  0,  1,  4,  9, 15, 22, 33, 43, 56, 71, 86, 104, 121, 142, 166, 189,
  2,  3,  6, 11, 17, 26, 35, 45, 58, 73, 90, 106, 123, 146, 168, 193,
  5,  7,  8, 13, 20, 28, 37, 50, 62, 75, 92, 108, 129, 150, 170, 195,
  10, 12, 14, 19, 23, 31, 41, 52, 65, 81, 96, 113, 133, 152, 175, 201,
  16, 18, 21, 24, 30, 39, 48, 59, 69, 83, 100, 119, 137, 158, 181, 203,
  25, 27, 29, 32, 40, 46, 54, 67, 79, 94, 109, 127, 143, 164, 185, 210,
  34, 36, 38, 42, 49, 55, 64, 76, 87, 102, 117, 135, 154, 176, 197, 216,
  44, 47, 51, 53, 60, 68, 77, 85, 98, 114, 131, 147, 162, 183, 208, 222,
  57, 61, 63, 66, 70, 80, 88, 99, 112, 124, 140, 159, 179, 199, 214, 227,
  72, 74, 78, 82, 84, 95, 103, 115, 125, 139, 156, 173, 190, 211, 224, 233,
  89, 91, 93, 97, 101, 110, 118, 132, 141, 157, 171, 186, 206, 220, 231, 239,
  105, 107, 111, 116, 120, 128, 136, 148, 160, 174, 187, 205, 218, 229, 237, 244,
  122, 126, 130, 134, 138, 144, 155, 163, 180, 191, 207, 219, 226, 235, 242, 248,
  145, 149, 151, 153, 161, 165, 177, 184, 200, 212, 221, 230, 236, 241, 246, 251,
  167, 169, 172, 178, 182, 188, 198, 209, 215, 225, 232, 238, 243, 247, 250, 253,
  192, 194, 196, 202, 204, 213, 217, 223, 228, 234, 240, 245, 249, 252, 254, 255,
};

#ifdef TEST
/* Table for checking reordering */
static uint8 index[256] = {
  0,  1, 16, 17,  2, 32, 18, 33, 34,  3, 48, 19, 49, 35, 50,  4,
  64, 20, 65, 51, 36, 66,  5, 52, 67, 80, 21, 81, 37, 82, 68, 53,
  83,  6, 96, 22, 97, 38, 98, 69, 84, 54, 99,  7, 112, 23, 85, 113,
  70, 100, 39, 114, 55, 115, 86, 101,  8, 128, 24, 71, 116, 129, 40, 130,
  102, 56, 131, 87, 117, 72, 132,  9, 144, 25, 145, 41, 103, 118, 146, 88,
  133, 57, 147, 73, 148, 119, 10, 104, 134, 160, 26, 161, 42, 162, 89, 149,
  58, 163, 120, 135, 74, 164, 105, 150, 11, 176, 27, 177, 43, 90, 165, 178,
  136, 59, 121, 151, 179, 106, 166, 75, 180, 12, 192, 28, 137, 152, 193, 91,
  181, 44, 194, 122, 167, 60, 195, 107, 182, 76, 196, 153, 138, 168, 13, 92,
  197, 208, 29, 123, 183, 209, 45, 210, 61, 211, 108, 198, 154, 169, 77, 139,
  184, 212, 124, 199, 93, 213, 14, 224, 30, 225, 46, 170, 226, 155, 185, 62,
  109, 214, 227, 140, 200, 78, 228, 125, 215, 94, 171, 186, 229, 15, 156, 201,
  240, 31, 241, 47, 242, 110, 230, 141, 216, 63, 243, 79, 244, 187, 172, 202,
  126, 231, 95, 157, 217, 245, 142, 232, 111, 246, 188, 203, 173, 218, 127, 247,
  158, 233, 204, 143, 248, 189, 219, 174, 234, 159, 249, 205, 220, 190, 235, 175,
  250, 221, 206, 236, 191, 251, 222, 237, 207, 252, 238, 223, 253, 239, 254, 255,
};
#endif

#endif