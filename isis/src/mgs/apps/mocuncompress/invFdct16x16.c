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
*/
static char *sccsid = "@(#)invFdct16x16.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif
/*
* DESCRIPTION
*
* COMMENTARY
*/

#include "fs.h"

#include "invFdct16x16.h"

#define MULTDOUBLE(r,v,c)	(r) = (v) * (c);

static double cosineDouble[16] = {
	1.00000000000000000000e+00,
	9.95184726672196890000e-01,
	9.80785280403230440000e-01,
	9.56940335732208870000e-01,
	9.23879532511286750000e-01,
	8.81921264348355040000e-01,
	8.31469612302545240000e-01,
	7.73010453362736970000e-01,
	7.07106781186547530000e-01,
	6.34393284163645500000e-01,
	5.55570233019602220000e-01,
	4.71396736825997660000e-01,
	3.82683432365089760000e-01,
	2.90284677254462360000e-01,
	1.95090322016128270000e-01,
	9.80171403295606040000e-02,
};

static void DCTinv16Double(in,out) double *in,*out; {
double tmp[16];
register double tmp1,tmp2;

	tmp[0]  =  in[0];
	tmp[1]  =  in[8];
	tmp[2]  =  in[4];
	tmp[3]  =  in[12];
	tmp[4]  =  in[2];
	tmp[5]  =  in[10];
	tmp[6]  =  in[6];
	tmp[7]  =  in[14];
	MULTDOUBLE(tmp1,in[1],cosineDouble[15]); MULTDOUBLE(tmp2,in[15],cosineDouble[1]);
	tmp[8]  = tmp1 - tmp2;
	MULTDOUBLE(tmp1,in[9],cosineDouble[7]); MULTDOUBLE(tmp2,in[7],cosineDouble[9]);
	tmp[9]  = tmp1 - tmp2;
	MULTDOUBLE(tmp1,in[5],cosineDouble[11]); MULTDOUBLE(tmp2,in[11],cosineDouble[5]);
	tmp[10] = tmp1 - tmp2;
	MULTDOUBLE(tmp1,in[13],cosineDouble[3]); MULTDOUBLE(tmp2,in[3],cosineDouble[13]);
	tmp[11] = tmp1 - tmp2;
	MULTDOUBLE(tmp1,in[3],cosineDouble[3]); MULTDOUBLE(tmp2,in[13],cosineDouble[13]);
	tmp[12] = tmp1 + tmp2;
	MULTDOUBLE(tmp1,in[11],cosineDouble[11]); MULTDOUBLE(tmp2,in[5],cosineDouble[5]);
	tmp[13] = tmp1 + tmp2;
	MULTDOUBLE(tmp1,in[7],cosineDouble[7]); MULTDOUBLE(tmp2,in[9],cosineDouble[9]);
	tmp[14] = tmp1 + tmp2;
	MULTDOUBLE(tmp1,in[15],cosineDouble[15]); MULTDOUBLE(tmp2,in[1],cosineDouble[1]);
	tmp[15] = tmp1 + tmp2;

	out[0]  =  tmp[0];
	out[1]  =  tmp[1];
	out[2]  =  tmp[2];
	out[3]  =  tmp[3];
	MULTDOUBLE(tmp1,tmp[4],cosineDouble[14]); MULTDOUBLE(tmp2,tmp[7],cosineDouble[2]);
	out[4]  = tmp1 - tmp2;
	MULTDOUBLE(tmp1,tmp[5],cosineDouble[6]); MULTDOUBLE(tmp2,tmp[6],cosineDouble[10]);
	out[5]  = tmp1 - tmp2;
	MULTDOUBLE(tmp1,tmp[6],cosineDouble[6]); MULTDOUBLE(tmp2,tmp[5],cosineDouble[10]);
	out[6]  = tmp1 + tmp2;
	MULTDOUBLE(tmp1,tmp[7],cosineDouble[14]); MULTDOUBLE(tmp2,tmp[4],cosineDouble[2]);
	out[7]  = tmp1 + tmp2;
	out[8]  =  tmp[8]  + tmp[9];
	out[9]  = -tmp[9]  + tmp[8];
	out[10] = -tmp[10] + tmp[11];
	out[11] =  tmp[11] + tmp[10];
	out[12] =  tmp[12] + tmp[13];
	out[13] = -tmp[13] + tmp[12];
	out[14] = -tmp[14] + tmp[15];
	out[15] =  tmp[15] + tmp[14];

	tmp1    =  out[0] + out[1];
	MULTDOUBLE(tmp[0],tmp1,cosineDouble[8]);
	tmp1    = -out[1] + out[0];
	MULTDOUBLE(tmp[1],tmp1,cosineDouble[8]);
	MULTDOUBLE(tmp1,out[2],cosineDouble[12]); MULTDOUBLE(tmp2,out[3],cosineDouble[4]);
	tmp[2]  = tmp1 - tmp2;
	MULTDOUBLE(tmp1,out[3],cosineDouble[12]); MULTDOUBLE(tmp2,out[2],cosineDouble[4]);
	tmp[3]  = tmp1 + tmp2;
	tmp[4]  =   out[4]       +      out[5];
	tmp[5]  =  -out[5]       +      out[4];
	tmp[6]  =  -out[6]       +      out[7];
	tmp[7]  =   out[7]       +      out[6];
	tmp[8]  =   out[8];
	MULTDOUBLE(tmp1,out[9],cosineDouble[4]); MULTDOUBLE(tmp2,out[14],cosineDouble[12]);
	tmp[9]  = -tmp1 + tmp2;
	MULTDOUBLE(tmp1,out[10],cosineDouble[12]); MULTDOUBLE(tmp2,out[13],cosineDouble[4]);
	tmp[10] = -tmp1 - tmp2;
	tmp[11] =   out[11];
	tmp[12] =   out[12];
	MULTDOUBLE(tmp1,out[13],cosineDouble[12]); MULTDOUBLE(tmp2,out[10],cosineDouble[4]);
	tmp[13] = tmp1 - tmp2;
	MULTDOUBLE(tmp1,out[14],cosineDouble[4]); MULTDOUBLE(tmp2,out[9],cosineDouble[12]);
	tmp[14] = tmp1 + tmp2;
	tmp[15] =   out[15];

	out[0]  =   tmp[0]  + tmp[3];
	out[1]  =   tmp[1]  + tmp[2];
	out[2]  =  -tmp[2]  + tmp[1];
	out[3]  =  -tmp[3]  + tmp[0];
	out[4]  =   tmp[4];
	tmp1    =  -tmp[5]  + tmp[6];
	MULTDOUBLE(out[5],tmp1,cosineDouble[8]);
	tmp1    =   tmp[6]  + tmp[5];
	MULTDOUBLE(out[6],tmp1,cosineDouble[8]);
	out[7]  =   tmp[7];
	out[8]  =   tmp[8]  + tmp[11];
	out[9]  =   tmp[9]  + tmp[10];
	out[10] =  -tmp[10] + tmp[9];
	out[11] =  -tmp[11] + tmp[8];
	out[12] =  -tmp[12] + tmp[15];
	out[13] =  -tmp[13] + tmp[14];
	out[14] =   tmp[14] + tmp[13];
	out[15] =   tmp[15] + tmp[12];

	tmp[0]  =   out[0]  + out[7];
	tmp[1]  =   out[1]  + out[6];
	tmp[2]  =   out[2]  + out[5];
	tmp[3]  =   out[3]  + out[4];
	tmp[4]  =  -out[4]  + out[3];
	tmp[5]  =  -out[5]  + out[2];
	tmp[6]  =  -out[6]  + out[1];
	tmp[7]  =  -out[7]  + out[0];
	tmp[8]  =   out[8];
	tmp[9]  =   out[9];
	tmp1    =  -out[10] + out[13];
	MULTDOUBLE(tmp[10],tmp1,cosineDouble[8]);
	tmp1    =  -out[11] + out[12];
	MULTDOUBLE(tmp[11],tmp1,cosineDouble[8]);
	tmp1    =   out[12] + out[11];
	MULTDOUBLE(tmp[12],tmp1,cosineDouble[8]);
	tmp1    =   out[13] + out[10];
	MULTDOUBLE(tmp[13],tmp1,cosineDouble[8]);
	tmp[14] =   out[14];
	tmp[15] =   out[15];

	out[0]  =  tmp[0]  + tmp[15];
	out[1]  =  tmp[1]  + tmp[14];
	out[2]  =  tmp[2]  + tmp[13];
	out[3]  =  tmp[3]  + tmp[12];
	out[4]  =  tmp[4]  + tmp[11];
	out[5]  =  tmp[5]  + tmp[10];
	out[6]  =  tmp[6]  + tmp[9];
	out[7]  =  tmp[7]  + tmp[8];
	out[8]  = -tmp[8]  + tmp[7];
	out[9]  = -tmp[9]  + tmp[6];
	out[10] = -tmp[10] + tmp[5];
	out[11] = -tmp[11] + tmp[4];
	out[12] = -tmp[12] + tmp[3];
	out[13] = -tmp[13] + tmp[2];
	out[14] = -tmp[14] + tmp[1];
	out[15] = -tmp[15] + tmp[0];
}

void invFdct16x16(in,out) int16 *in,*out; {
uint32 i,j;
double data[256],*scanData,*other;
double temp;

	scanData = data;

	*(scanData++) = (uint16)(*(in++));

	for (i = 1; i < 256; i++) {
		*(scanData++) = *(in++);
	};

	for (i = 0, scanData = data; i < 16; i++, scanData += 16) {
		DCTinv16Double(scanData,scanData);
	};

	for (i = 0, scanData = data, other = data+16; i < 16; i++, other += 17) {
	double *scanOther;

		scanData += i+1;
		scanOther = other;

		for (j = i+1; j < 16; j++, scanData++, scanOther += 16) {
			temp       = *scanData;
			*scanData  = *scanOther;
			*scanOther = temp;
		};
	};

	for (i = 0, scanData = data; i < 16; i++, scanData += 16) {
		DCTinv16Double(scanData,scanData);
	};

	for (i = 0, scanData = data, other = data+16; i < 16; i++, other += 17) {
	double *scanOther;

		scanData += i+1;
		scanOther = other;

		for (j = i+1; j < 16; j++, scanData++, scanOther += 16) {
			temp       = *scanData;
			*scanData  = *scanOther;
			*scanOther = temp;
		};
	};

	for (i = 0, scanData = data; i < 256; i++) {
	int16 cur;
		cur = *(scanData++) / 127.0 + 0.5;

		if (cur < 0) {
			cur = 0;
		};

		if (cur > 255) {
			cur = 255;
		};

		*(out++) = cur;
	};
}
