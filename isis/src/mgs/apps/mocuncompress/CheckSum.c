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
static char *sccsid = "@(#)CheckSum.c	1.1 10/04/99";

/*
* DESCRIPTION
*
* COMMENTARY
*/

#include "fs.h"
#include "CheckSum.h"

uint8 CS8EAC(d,l)register uint8 *d; register uint32 l;{
/*
* Compute the eight bit end-around-carry checksum of a data vector.
*
* Pre:
*	0 < l < 1<<24
*	d[0..(l-1)] is data to compute checksum of.
* Post:
*	rv is checksum of d[0..(l-1)]
* Notes:
*
*	There are two typical applications of this algorithm: Method 1
*	is save checksum of data part in known location (the PDS method)
*	and method 2 is cause the checksum of the data and cs field to
*	be some known value (MOC method, known value is 0xff).
*
*	In both cases, assume a checksum is to be put on uint8 data[n],
*	with the checksum octet to be placed in data[n-1].
*
*	APPLY CS METHOD 1:
*		uint8 t;
*		t = CS8EAC(data,n-1);
*		*(uint8*)&data[n-1] = t;
*	CHECK CS METHOD 1:
*		if(CS8EAC(data,n-1) != *(uint8*)&data[n-1])bad();
*	APPLY CS METHOD 2:
*		uint8 t;
*		t = 0xff - CS8EAC(data,n-1);
*		*(uint8*)&data[n-1] = t;
*	CHECK CS METHOD 2:
*		if(CS8EAC(data,n) != 0xff)bad();
*/
register uint32 cs = 0;
	while(l--){
		cs += *d++;
	};
	while(cs > 255){
		cs = (cs&0xff)+(cs>>8);
	};
	return (uint8)cs;
}

#if defined(TEST)
uint8 tcs8eac(d,l)register uint8 *d; register uint32 l;{
/*
* Canonical version of CS8EAC().  Used to test the fast version.
*/
register uint32 cs = 0;
	while(l--){
		cs += *d++;
		if(cs>255)cs -= 255;
	};
	return cs;
}
#endif

void CS8EACA1(dat,len)register uint8 *dat;uint32 len;{
/*
* Apply type 1 CS8EAC checksum to dat.
* Pre:
*	dat[0..len-2] is value to checksum.
*	dat[len-1] is place to put checksum.
* Post:
*	CS8EAC(dat,len-1) == dat[len-1]
* Notes:
*/
uint8 t;
	t = CS8EAC(dat,len-1);
	*(uint8*)&dat[len-1] = t;
}

void CS8EACA2(dat,len)register uint8 *dat;uint32 len;{
/*
* Apply type 2 CS8EAC checksum to dat.
* Pre:
*	dat[0..len-2] is value to checksum.
*	dat[len-1] is place to put checksum.
* Post:
*	CS8EAC(dat,len) == 0xff
* Notes:
*/
uint8 t;
	t = CS8EAC(dat,len-1);
	*(uint8*)&dat[len-1] = 0xff - t;
}

UINT CS8EACC1(dat,len)register uint8 *dat;UINT len;{
/*
* Check type 1 checksum.
* Pre:
*	dat[0..len-1] is data to check checksum of.
*	CS8EACA1() or equivalent used to apply checksum.
* Post:
*	rv nz iff checksum valid.
* Notes:
*/
UINT rv = 1;
	if(CS8EAC(dat,len-1) != dat[len-1]){
		return rv = 0;
	};
	return rv;
}

UINT CS8EACC2(dat,len)register uint8 *dat;UINT len;{
/*
* Check type 2 checksum.
* Pre:
*	dat[0..len-1] is data to check checksum of.
*	CS8EACA2() or equivalent used to apply checksum.
* Post:
*	rv nz iff checksum valid.
* Notes:
*/
UINT rv = 1;
	if(CS8EAC(dat,len) != 0xff){
		return rv = 0;
	};
	return rv;
}

uint16 CS16EAC(d,len)register uint8 *d;uint32 len;{
/*
* Compute the sixteen bit end-around-carry checksum of a data vector.
*
* Pre:
*	d[0..(l-1)] is data to compute checksum of.
* Post:
*	rv is checksum of d[0..(l-1)]
* Notes:
*	If l is odd then last word summed has last byte in d[] in
*	its low byte, zero in its high byte.
*
*	There are two typical applications of this algorithm: Method 1
*	is save checksum of data part in known location (the PDS method)
*	and method 2 is cause the checksum of the data and cs field to
*	be some known value (MOC method, known value is 0xffff).
*
*	In both cases, assume a checksum is to be put on uint8 data[n],
*	with the two checksum octets to be placed in data[n-2] and
*	data[n-1].
*
*	APPLY CS METHOD 1:
*		uint16 t;
*		t = CS16EAC(data,n-2);
*		*(uint16*)&data[n-2] = t;
*	CHECK CS METHOD 1:
*		if(CS16EAC(data,n-2) != *(uint16*)&data[n-2])bad();
*	APPLY CS METHOD 2:
*		uint16 t;
*		t = 0xffff - CS16EAC(data,n-2);
*		if(n&1)t = (t<<8)|(t>>8);
*		*(uint16*)&data[n-2] = t;
*	CHECK CS METHOD 2:
*		if(CS16EAC(data,n) != 0xffff)bad();
*/
register uint32 cs = 0;
register uint32 l;
register uint16 t;
	do{
		l = (len<100)?len:100;
		len -= l;
		for(;l > 1;l -= 2){
		        t = *d | (*(d+1) << 8);
			cs += t;
			d += sizeof(uint16);
		};
		if(l)cs += *d++;
		while(cs > 0xffff)cs = (cs&0xffff) + (cs>>16);
	}while(len);
	return cs;
}

#if defined(TEST)
uint16 tcs16eac(d,l)register uint8 *d; register uint32 l;{
/*
* Canonical version of CS16EAC().  Used to test the fast version.
*/
register uint32 cs = 0;
	for(;l > 1;l -= 2){
		cs += (uint16) (*d | (*(d+1) << 8));
		if(cs > 0xffff)cs -= 0xffff;
		d += sizeof(uint16);
	};
	if(l)cs += *d++;
	if(cs > 0xffff)cs -= 0xffff;
	return cs;
}
#endif

void CS16EACA1(dat,len)register uint8 *dat;uint32 len;{
/*
* Apply type 1 CS16EAC checksum to dat.
* Pre:
*	dat[0..len-3] is value to checksum.
*	dat[len-2..len-1] is place to put checksum.
* Post:
*	CS16EAC(dat,len-2) == dat[len-2..len-1]
* Notes:
*/
uint16 t;
	t = CS16EAC(dat,len-2);
	dat[len-2] = t & 0xff;
	dat[len-1] = t >> 8;
	/* *(uint16*)&(dat[len-2]) = t; */
}

void CS16EACA2(dat,len)register uint8 *dat;uint32 len;{
/*
* Apply type 2 CS16EAC checksum to dat.
* Pre:
*	dat[0..len-3] is value to checksum.
*	dat[len-2..len-1] is place to put checksum.
* Post:
*	CS16EAC(dat,len) == 0xffff
* Notes:
*/
uint16 t;
	t = 0xffff - CS16EAC(dat,len-2);
	if(len&1)t = (t<<8)|(t>>8);
	dat[len-2] = t&0xff;
	dat[len-1] = t >> 8;
	/* *(uint16*)&(dat[len-2]) = t; */
}

UINT CS16EACC1(dat,len)register uint8 *dat;UINT len;{
/*
* Check type 1 checksum.
* Pre:
*	dat[0..len-1] is data to check checksum of.
*	CS16EACA1() or equivalent used to apply checksum.
* Post:
*	rv nz iff checksum valid.
* Notes:
*/
UINT rv = 1;
uint16 t;

	t = dat[len-2] | (dat[len-1] << 8);
	if(CS16EAC(dat,len-2) != t){
		return rv = 0;
	};
	return rv;
}

UINT CS16EACC2(dat,len)register uint8 *dat;UINT len;{
/*
* Check type 2 checksum.
* Pre:
*	dat[0..len-1] is data to check checksum of.
*	CS16EACA2() or equivalent used to apply checksum.
* Post:
*	rv nz iff checksum valid.
* Notes:
*/
UINT rv = 1;
	if(CS16EAC(dat,len) != 0xffff){
		return rv = 0;
	};
	return rv;
}

UINT ParityOf(d,l)register uint8 *d; register uint32 l;{
/*
* Compute parity of data vector.
* Pre:
*	d[0..(l-1)] is data to compute parity of.
* Post:
*	rv is number of "one" bits in d[0..(l-1)] modulo 2.
* Notes:
*	"Odd parity" means that there are an odd number of "one" bits
*	in the data covered by the parity.  "Even parity" means that
*	there are an even number of "one" bits in the data covered by
*	the parity.  It follows that this routine returns one if the
*	data is odd parity, and zero if it is even parity.
*	REFINE make ParityOf() faster if used often or with large l endREFINE
*/
register UINT cs=0;
register uint8 t;
	while(l--){
		t = *d++;
		while(t){
			cs ^= (t&1);
			t >>= 1;
		};
	};
	return cs;
}

#if defined(TEST)

/*
* #define TESTCS8
* #define TESTCS16
* #define TESTPARITY
*/
#define TESTCS8
#define TESTCS16
#define TESTPARITY

#include <stdio.h>
main(){
uint8 d[100];
UINT i,ii,iii;
UINT cs1,cs2;
UINT oldii = 0;UINT nbad=0;
#if defined(TESTCS8) || defined(TESTCS16)
	for(i=0;i<=sizeof(d)*255;i+=1 /*65537*/){
		ii = 0;
		iii = i;
		while(iii>255){
			d[ii++] = 255;
			iii -= 255;
		};
		d[ii++] = iii;
		if(ii != oldii){
			oldii = ii;
			printf("CS test %d of %d\n",ii,sizeof(d));
		};
#if defined(TESTCS8)
		cs1 = CS8EAC(d,ii);
		cs2 = tcs8eac(d,ii);
		if(cs1 != cs2){
			printf("cs8 failed %d %d %d \n",i,cs1,cs2);
			nbad += 1;
		};
		CS8EACA1(d,ii);
		if(!CS8EACC1(d,ii)){
			printf("CS8EACT1() failed\n");
			nbad += 1;
		};
		if(CS8EAC(d,ii-1) != d[ii-1]){
			printf("CS8EAC1() failed\n");
			nbad += 1;
		};
		CS8EACA2(d,ii);
		if(!CS8EACC2(d,ii)){
			printf("CS8EACT2() failed\n");
			nbad += 1;
		};
		if(CS8EAC(d,ii) != 0xff){
			printf("CS8EAC2() failed\n");
			nbad += 1;
		};
#endif
#if defined(TESTCS16)
		cs1 = CS16EAC(d,ii);
		cs2 = tcs16eac(d,ii);
		if(cs1 != cs2){
			printf("cs16 failed %d %d %d \n",i,cs1,cs2);
			nbad += 1;
		};
	if(ii > 1){
		CS16EACA1(d,ii);
		if(!CS16EACC1(d,ii)){
			printf("CS16EACT1() failed\n");
			nbad += 1;
		};
		d[1] ^= 0x12;
		if(CS16EACC1(d,ii)){
			printf("CS16EACT1() failed\n");
			nbad += 1;
		};
		d[1] ^= 0x12;
		if(CS16EAC(d,ii-2) != (d[ii-2] | (d[ii-1] << 8))){
			printf("CS16EAC1() failed\n");
			nbad += 1;
		};
		CS16EACA2(d,ii);
		if(!CS16EACC2(d,ii)){
			printf("CS16EACT2() failed\n");
			nbad += 1;
		};
		d[1] ^= 0x12;
		if(CS16EACC2(d,ii)){
			printf("CS16EACT2() failed\n");
			nbad += 1;
		};
		d[1] ^= 0x12;
		if(CS16EAC(d,ii) != 0xffff){
			printf("CS16EAC2() failed\n");
			nbad += 1;
		};
	};
#endif
	};
#endif

#if defined(TESTPARITY)
	for(i=0;i<256*256;i+=1){UINT t,cs;
		if(!(i%256)){
			printf("Parity test %d of %d\n",i,65536);
		};
		cs = 0;
		t = i;
		while(t){
			if(t&1)cs += 1;
			t >>= 1;
		};
		if(cs&1 != ParityOf(&i,4)){
			printf("ParityOf() failed\n");
			nbad += 1;
		};
	};
#endif
	if(nbad){
		printf("MODULE FAILED %d TESTS\n",nbad);
	}else{
		printf("Module passed tests\n");
	};
}
#endif
