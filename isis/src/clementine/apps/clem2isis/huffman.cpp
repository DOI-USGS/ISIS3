/*
 *	THIS ROUTINE IS PART OF THE CLEMENTINE PDS FILE READER PROGRAM.
 *	IT WAS WRITTEN BY ACT CORP. IN DIRECT SUPPORT TO THE 
 *	CLEMENTINE (DSPSE) PROGRAM.
 *
 *	IF YOU FIND A PROBLEM OR MAKE ANY CHANGES TO THIS CODE PLEASE CONTACT
 *	Dr. Erick Malaret at ACT Corp. 
 *			tel: (703) 742-0294
 *			     (703) 683-7431
 *                       email:	nrlvax.nrl.navy.mil
 *	
 *
 */
#include <stdio.h>
#include "jpeg_c.h"
#include "pds.h"

#define ZRL 240
#define EOB 0

void inithuffcode();
void genhuffsize(char *, short *, short *);
void genhuffcode(short *, char *);
void genehuf(short *, short *, short *, char *, char *, short);
void gendectbls(short *, short *, short *, short *, short *);


short	dcbits[16], acbits[16];
char	dchuffval[12], achuffval[162];

short	dcehufco[16];
short	dcehufsi[16];
short	dcmincode[16];
short	dcmaxcode[16];
short	dcvalptr[16];

short	acehufco[256];
short	acehufsi[256];
short	acmincode[16];
short	acmaxcode[16];
short	acvalptr[16];

/******************* Initialization of Huffman tables ********************/
void inithuffcode()
{
	char    dchuffsize[13], achuffsize[163];
	short   dchuffcode[12], achuffcode[162];
	short   dclastk, aclastk;

	/* generate dc Huffman codes */
	genhuffsize(dchuffsize,dcbits,&dclastk);
	genhuffcode(dchuffcode,dchuffsize);
//	fprintf(qparm,"dc huffman tables:\n");            //removed qparm references BMG 2006-07-18
//	fprintf(qparm,"(symbol length  code)\n");         //removed qparm references BMG 2006-07-18
	genehuf(dcehufco,dcehufsi,dchuffcode,dchuffsize,dchuffval,dclastk);

	/* generate ac Huffman codes */
	genhuffsize(achuffsize,acbits,&aclastk);
	genhuffcode(achuffcode,achuffsize);
//	fprintf(qparm,"ac huffman tables:\n");         //removed qparm references BMG 2006-07-18 
//	fprintf(qparm,"(symbol length  code)\n");      //removed qparm references BMG 2006-07-18 
	genehuf(acehufco,acehufsi,achuffcode,achuffsize,achuffval,aclastk);

	/* generate decoding tables */
	gendectbls(dcmincode,dcmaxcode,dcvalptr,dchuffcode,dcbits);
	gendectbls(acmincode,acmaxcode,acvalptr,achuffcode,acbits);
}

void genhuffsize(char *huffsize, short *bits, short *lastk)
{
	short     i = 0,j = 1,k = 0;

	while ( i < 16 )
		if ( j > bits[i] ) {
			i++;
			j = 1;
		} else {
			huffsize[k] = i+1;
			k++;
			j++;
		}
	huffsize[k] = 0;
	*lastk = k;
}

void genhuffcode(short *huffcode, char *huffsize)
{
	short   code, k;
	char    si;

	k = code = 0;
	si = huffsize[0];

	while ( huffsize[k] ) {
		if ( huffsize[k] == si ) {
			huffcode[k] = code;
			code++; k++;
		} else {
			code <<= 1;
			si++;
		}
	}
}

void genehuf(short *ehufco, short *ehufsi, short *huffcode, char *huffsize,
			 char *huffvalue, short lastk)
{
	short   k;
	short   value;

	for (k=0; k < lastk; k++) {
		value = ((short)huffvalue[k])&0x00ff;
		ehufco[value] = huffcode[k];
		ehufsi[value] = huffsize[k];
//		fprintf(qparm,"%#.2x\t%d\t%#x\n",        //removed qparm reference, BMG 2006-07-18
//				huffvalue[k]&0x00ff,
//				ehufsi[value],
//                                ehufco[value] );
	}
}

void gendectbls(short *mincode, short *maxcode, short *valptr, short *huffcode, short *bits)
{
	short     k, l;

	l = k = 0;

	while ( l < 16 ) {
		if ( bits[l] ) {
			valptr[l] = k;
			mincode[l] = huffcode[k];
			maxcode[l] = huffcode[k + bits[l] -1];
			k += bits[l];
		} else
			maxcode[l] = -1;
		l++;
	}
}


/************************** Decoding Section *****************************/

unsigned short mask[] = {0x0000,
0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

short   code;

void decode(short *u, BitStream *ibs)
{
	short     symbol, coeff, i, run, cat, j, l;

        /* get dc coefficient */
	l = 0;
	code = BitStream_read(ibs,1);
	while ( code > dcmaxcode[l] ) {
		code = (short)(code << 1) | BitStream_read(ibs,1);
		l++;
	}
	symbol = (dchuffval[dcvalptr[l] + code - dcmincode[l]]) & 0x00ff;
	if ( symbol ) {
		coeff = BitStream_read(ibs,symbol);
		if ( coeff & mask[symbol] )
			u[0] = coeff;
		else
			u[0] = ((0xffff << symbol) | coeff) + 1;
	} else
		u[0] = 0;

	/* get ac coefficients */
	i = 1;
	while ( i < 64 ) {
		l = 0;
		code = BitStream_read(ibs,1);
		while ( code > acmaxcode[l] ) {
			code = (short)(code << 1) | BitStream_read(ibs,1);
			l++;
		}
		symbol = (achuffval[acvalptr[l] + code - acmincode[l]]) & 0x00ff;

		if ( symbol == ZRL ) {
			for (j=0; j < 16; j++) u[i++] = 0;
		} else if ( symbol == EOB ) {
			for (j=i; j < 64; j++) u[i++] = 0;
		} else {
			run = (symbol >> 4) & 0x000f;
			cat = symbol&0x000f;
			while ( run-- ) u[i++] = 0;

			coeff = BitStream_read(ibs,cat);
			if ( coeff & mask[cat] )
				u[i++] = coeff;
			else
				u[i++] = ((0xffff << cat) | coeff) + 1;
		}
	}
}
