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
 *      Dec 11 2006 KJB Changed malloc.h to stdlib.h for the Mac port again.
 *                      malloc.h is deprecated.  
 *
 */
/*
	Program to calculate the Discrete Cosine Transform of a 8x8 block of data
*/

#include <stdio.h>
#ifdef __TURBOC__ 
#include <alloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#include <math.h>
#include "jpeg_c.h"

#define TRUE 1
#define FALSE 0

long	*DCTHist[64];
float	*Rn[64];
float	Q[64];
float	q_table[64];
float   U[64];
short   ULS[64];
int	zzseq[] = {0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,
		   48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,
		   22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,
		   47,55,62,63};

void core(void);
void getDCTHist(BitStream *bs, long rows, long cols);
void getRn(void);


void decomp(BitStream *bs,CHARH *Image,long rows,long cols)
{
	short   i,j,k,indexi,indexip8,indexj,indexjp8;
	float   temp;
	long    rowsleft, colsleft, icols;
	short   Done, Diff, Pred;

	/*********************** Image Decompression *********************/
	Done = FALSE;
	rowsleft = rows;
	colsleft = cols;
	indexj = 0;
	indexi = 0;
	Pred = 0;
	
	while ( !Done ) {
		/* Decode block data */
		decode(ULS,bs);
		Diff = ULS[0] + Pred;
		Pred = Diff;
		ULS[0] = Diff;

		/* Calculate Inverse Discrete Cosine Transform */
		/*dequantize(U,ULS1,pt_q_table);  64 multiplications */
		for (i=0; i<64; i++) {
				temp = Rn[i][ULS[i]];
				U[zzseq[i]] = temp * q_table[i];
		}

		core();

		/*ilevelshift(U);*/
		for (i=0; i<64; i++) {
			U[i] += 128.0;
			U[i] = floor( U[i] + 0.5 );
			if ( U[i] > 255.0 ) U[i] = 255.0;
			else if ( U[i] < 0.0 ) U[i] = 0.0;
			else;
		}

		indexip8 = indexi + 8;
		indexjp8 = indexj + 8;

		if ( (rowsleft > 8) && (colsleft > 8) ) {

			for (i=indexi, k=0; i < indexip8; i++)
				for (j=indexj, icols=i*cols; j < indexjp8; j++, k++)
					Image[icols+j] = (char)U[k];

			indexj += 8;
			colsleft -= 8;
		}
		else if ( (rowsleft > 8) && (colsleft <= 8) ) {

			for (i=indexi, k=0; i < indexip8; i++) {
				for (j=indexj, icols=i*cols; j < cols; j++, k++)
					Image[icols+j] = (char)U[k];
				k += (8 - colsleft);
			}

			indexj = 0;
			indexi += 8;
			rowsleft -= 8;
			colsleft = cols;
		}
		else if ( (rowsleft <= 8) && (colsleft > 8) ) {

			for (i=indexi, k=0; i < rows; i++)
				for (j=indexj, icols=i*cols; j < indexjp8; j++, k++)
					Image[icols+j] = (char)U[k];

			indexj += 8;
			colsleft -= 8;
		}
		else {

			for (i=indexi, k=0; i < rows; i++) {
				for (j=indexj, icols=i*cols; j < cols; j++, k++)
					Image[icols+j] = (char)U[k];
				k += (8 - colsleft);
			}

			Done = TRUE;
		}
	}
}

void getDCTHist(BitStream *bs, long rows, long cols)
{
	short	Pred, i;
	long	nblocks;

	nblocks = (rows * cols) / 64;
	Pred = 0;

	while ( nblocks ) {
		/* Decode block data */
		decode(ULS,bs);
		ULS[0] += Pred;
		Pred = ULS[0];

		for (i=0; i<64; i++) DCTHist[i][ULS[i]]++;

		nblocks--;
	}
}

void getRn()
{
	short	i, j;
	float	lb, ub, a, b, num, den, phi;
	float	m, pa, pap, pb, pbp, pm, pmp;

	for (i=0; i<64; i++) {
		Rn[i][-256] = (float)-256;
		Rn[i][256] = (float)256;
		for (j=-255; j<256; j++) {
			if ( DCTHist[i][j] > 0 ) {
				lb = ((float)j - 0.5)*Q[i];
				ub = ((float)j + 0.5)*Q[i];

				m  = ((float)j)*Q[i];
				pm = DCTHist[i][j];
				pmp = m*pm;

				a = ceil(lb);
				phi = a/Q[i] - (float)(j-1);
				pa = phi*DCTHist[i][j] + (1.0-phi)*DCTHist[i][j-1];
				pap = ((float)j*pa - (1.0-phi)*DCTHist[i][j-1])*Q[i];

				b = ceil(ub);
				phi = b/Q[i] - (float)j;
				pb = phi*DCTHist[i][j+1] + (1.0-phi)*DCTHist[i][j];
				pbp = ((float)j*pb + phi*DCTHist[i][j+1])*Q[i];

				num = (m-a)*(pap+pmp)+(b-m)*(pbp+pmp);
				den = (m-a)*(pa+pm)+(b-m)*(pb+pm);
				Rn[i][j] = (num/den)/Q[i];
			} else {
				Rn[i][j] = (float)j;
			}
		}
	}
}


void core(void)
{
	float   out[64], out1[64];
	float   temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8;
	float   dummy1, dummy2, dummy3;
	float   buffer11,buffer12,buffer13,buffer14,buffer15,buffer16,buffer17,buffer18;
	float   buffer21,buffer22,buffer23,buffer24,buffer25,buffer26,buffer27,buffer28;

	/******** Preadditions ********/
	out[0] = U[0];
	out[1] = U[32];
	out[2] = U[16] - U[48];
	out[3] = U[16] + U[48];
	dummy1 = U[8] - U[56];
	dummy2 = U[24] - U[40];
	out[4] = dummy1 - dummy2;
	out[5] = dummy1 + dummy2;
	out[6] = -U[8] - U[56];
	out[7] = U[24] + U[40]; /* 8 additions */
	
	out[8] = U[4];
	out[9] = U[36];
	out[10] = U[20] - U[52];
	out[11] = U[20] + U[52];
	dummy1 = U[12] - U[60];
	dummy2 = U[28] - U[44];
	out[12] = dummy1 - dummy2;
	out[13] = dummy1 + dummy2;
	out[14] = -U[12] - U[60];
	out[15] = U[28] + U[44]; /* 8 additions */

	temp1 = U[2] - U[6];
	temp2 = U[34] - U[38];
	temp3 = U[18] - U[22];
	temp4 = U[50] - U[54];
	temp5 = U[10] - U[14];
	temp6 = U[26] - U[30];
	temp7 = U[58] - U[62];
	temp8 = U[42] - U[46];
	out[16] = temp1;
	out[17] = temp2;
	out[18] = temp3 - temp4;
	out[19] = temp3 + temp4;
	dummy1 = temp5 - temp7;
	dummy2 = temp6 - temp8;
	out[20] = dummy1 - dummy2;
	out[21] = dummy1 + dummy2;
	out[22] = -temp5 - temp7;
	out[23] = temp6 + temp8; /* 16 additions */

	temp1 = U[2] + U[6];
	temp2 = U[34] + U[38];
	temp3 = U[18] + U[22];
	temp4 = U[50] + U[54];
	temp5 = U[10] + U[14];
	temp6 = U[26] + U[30];
	temp7 = U[58] + U[62];
	temp8 = U[42] + U[46];
	out[24] = temp1;
	out[25] = temp2;
	out[26] = temp3 - temp4;
	out[27] = temp3 + temp4;
	dummy1 = temp5 - temp7;
	dummy2 = temp6 - temp8;
	out[28] = dummy1 - dummy2;
	out[29] = dummy1 + dummy2;
	out[30] = -temp5 - temp7;
	out[31] = temp6 + temp8; /* 16 additions */

	buffer11 = U[1] - U[7]; buffer21 = U[3] - U[5];
	buffer12 = U[33] - U[39]; buffer22 = U[35] - U[37];
	buffer13 = U[17] - U[23]; buffer23 = U[19] - U[21];
	buffer14 = U[49] - U[55]; buffer24 = U[51] - U[53];
	buffer15 = U[9] - U[15]; buffer25 = U[11] - U[13];
	buffer16 = U[25] - U[31]; buffer26 = U[27] - U[29];
	buffer17 = U[57] - U[63]; buffer27 = U[59] - U[61];
	buffer18 = U[41] - U[47]; buffer28 = U[43] - U[45];
	temp1 = buffer11 - buffer21;
	temp2 = buffer12 - buffer22;
	temp3 = buffer13 - buffer23;
	temp4 = buffer14 - buffer24;
	temp5 = buffer15 - buffer25;
	temp6 = buffer16 - buffer26;
	temp7 = buffer17 - buffer27;
	temp8 = buffer18 - buffer28;
	out[32] = temp1;
	out[33] = temp2;
	out[34] = temp3 - temp4;
	out[35] = temp3 + temp4;
	dummy1 = temp5 - temp7;
	dummy2 = temp6 - temp8;
	out[36] = dummy1 - dummy2;
	out[37] = dummy1 + dummy2;
	out[38] = -temp5 - temp7;
	out[39] = temp6 + temp8;
	temp1 = buffer11 + buffer21;
	temp2 = buffer12 + buffer22;
	temp3 = buffer13 + buffer23;
	temp4 = buffer14 + buffer24;
	temp5 = buffer15 + buffer25;
	temp6 = buffer16 + buffer26;
	temp7 = buffer17 + buffer27;
	temp8 = buffer18 + buffer28;
	out[40] = temp1;
	out[41] = temp2;
	out[42] = temp3 - temp4;
	out[43] = temp3 + temp4;
	dummy1 = temp5 - temp7;
	dummy2 = temp6 - temp8;
	out[44] = dummy1 - dummy2;
	out[45] = dummy1 + dummy2;
	out[46] = -temp5 - temp7;
	out[47] = temp6 + temp8; /* 48 additions */

	temp1 = -U[1] - U[7];
	temp2 = -U[33] - U[39];
	temp3 = -U[17] - U[23];
	temp4 = -U[49] - U[55];
	temp5 = -U[9] - U[15];
	temp6 = -U[25] - U[31];
	temp7 = -U[57] - U[63];
	temp8 = -U[41] - U[47];
	out[48] = temp1;
	out[49] = temp2;
	out[50] = temp3 - temp4;
	out[51] = temp3 + temp4;
	dummy1 = temp5 - temp7;
	dummy2 = temp6 - temp8;
	out[52] = dummy1 - dummy2;
	out[53] = dummy1 + dummy2;
	out[54] = -temp5 - temp7;
	out[55] = temp6 + temp8; /* 16 additions */

	temp1 = U[3] + U[5];
	temp2 = U[35] + U[37];
	temp3 = U[19] + U[21];
	temp4 = U[51] + U[53];
	temp5 = U[11] + U[13];
	temp6 = U[27] + U[29];
	temp7 = U[59] + U[61];
	temp8 = U[43] + U[45];
	out[56] = temp1;
	out[57] = temp2;
	out[58] = temp3 - temp4;
	out[59] = temp3 + temp4;
	dummy1 = temp5 - temp7;
	dummy2 = temp6 - temp8;
	out[60] = dummy1 - dummy2;
	out[61] = dummy1 + dummy2;
	out[62] = -temp5 - temp7;
	out[63] = temp6 + temp8; /* 16 additions */
	/*for (i=0; i<64; i++) printf("out[%d] = %f\n",i,out[i]);*/

	/********* Core Processing *********/
	out1[0] = out[0];
	out1[1] = out[1];
	out1[2] = out[2];
	out1[3] = 0.707106781 * out[3];
	out1[4] = out[4];
	out1[5] = 0.707106781 * out[5];
	dummy1 = 1.306562964 * out[6];
	dummy2 = 0.923879532 * (out[6] + out[7]);
	dummy3 = -0.5411961 * out[7];
	out1[6] = dummy1 - dummy2;
	out1[7] = dummy2 + dummy3;   /* 5 multiplications, 3 additions */

	out1[8] = out[8];
	out1[9] = out[9];
	out1[10] = out[10];
	out1[11] = 0.707106781 * out[11];
	out1[12] = out[12];
	out1[13] = 0.707106781 * out[13];
	dummy1 = 1.306562964 * out[14];
	dummy2 = 0.923879532 * (out[14] + out[15]);
	dummy3 = -0.5411961 * out[15];
	out1[14] = dummy1 - dummy2;
	out1[15] = dummy2 + dummy3;   /* 5 multiplications, 3 additions */

	out1[16] = out[16];
	out1[17] = out[17];
	out1[18] = out[18];
	out1[19] = 0.707106781 * out[19];
	out1[20] = out[20];
	out1[21] = 0.707106781 * out[21];
	dummy1 = 1.306562964 * out[22];
	dummy2 = 0.923879532 * (out[22] + out[23]);
	dummy3 = -0.5411961 * out[23];
	out1[22] = dummy1 - dummy2;
	out1[23] = dummy2 + dummy3;   /* 5 multiplications, 3 additions */

	out1[32] = out[32];
	out1[33] = out[33];
	out1[34] = out[34];
	out1[35] = 0.707106781 * out[35];
	out1[36] = out[36];
	out1[37] = 0.707106781 * out[37];
	dummy1 = 1.306562964 * out[38];
	dummy2 = 0.923879532 * (out[38] + out[39]);
	dummy3 = -0.5411961 * out[39];
	out1[38] = dummy1 - dummy2;
	out1[39] = dummy2 + dummy3;   /* 5 multiplications, 3 additions */

	out1[24] = 0.707106781 * out[24];
	out1[25] = 0.707106781 * out[25];
	out1[26] = 0.707106781 * out[26];
	out1[27] = 0.5 * out[27];
	out1[28] = 0.707106781 * out[28];
	out1[29] = 0.5 * out[29];
	dummy1 = 0.923879532 * out[30];
	dummy2 = 0.653281481 * (out[30] + out[31]);
	dummy3 = -0.382683432 * out[31];
	out1[30] = dummy1 - dummy2;
	out1[31] = dummy2 + dummy3;   /* 9 multiplications, 3 additions */

	out1[40] = 0.707106781 * out[40];
	out1[41] = 0.707106781 * out[41];
	out1[42] = 0.707106781 * out[42];
	out1[43] = 0.5 * out[43];
	out1[44] = 0.707106781 * out[44];
	out1[45] = 0.5 * out[45];
	dummy1 = 0.923879532 * out[46];
	dummy2 = 0.653281481 * (out[46] + out[47]);
	dummy3 = -0.382683432 * out[47];
	out1[46] = dummy1 - dummy2;
	out1[47] = dummy2 + dummy3;   /* 9 multiplications, 3 additions */

	dummy1 = 1.306562964 * out[48];
	dummy2 = 0.923879532 * (out[48] + out[56]);
	dummy3 = -0.5411961 * out[56];
	out1[48] = dummy1 - dummy2;
	out1[56] = dummy2 + dummy3;
	dummy1 = 1.306562964 * out[49];
	dummy2 = 0.923879532 * (out[49] + out[57]);
	dummy3 = -0.5411961 * out[57];
	out1[49] = dummy1 - dummy2;
	out1[57] = dummy2 + dummy3;  
	dummy1 = 1.306562964 * out[50];
	dummy2 = 0.923879532 * (out[50] + out[58]);
	dummy3 = -0.5411961 * out[58];
	out1[50] = dummy1 - dummy2;
	out1[58] = dummy2 + dummy3;  
	dummy1 = 1.306562964 * out[52];
	dummy2 = 0.923879532 * (out[52] + out[60]);
	dummy3 = -0.5411961 * out[60];
	out1[52] = dummy1 - dummy2;
	out1[60] = dummy2 + dummy3;  
	dummy1 = 0.923879532 * out[51];
	dummy2 = 0.653281481 * (out[51] + out[59]);
	dummy3 = -0.382683432 * out[59];
	out1[51] = dummy1 - dummy2;
	out1[59] = dummy2 + dummy3;  
	dummy1 = 0.923879532 * out[53];
	dummy2 = 0.653281481 * (out[53] + out[61]);
	dummy3 = -0.382683432 * out[61];
	out1[53] = dummy1 - dummy2;
	out1[61] = dummy2 + dummy3;  
	temp1 = 0.5 * (out[54] + out[63]);
	temp2 = 0.5 * (out[55] - out[62]);
	temp3 = out[54] - out[63];
	temp4 = out[55] + out[62];
	temp5 = 0.35355339 * (temp3 - temp4);
	temp6 = 0.35355339 * (temp3 + temp4);
	out1[54] = temp1 - temp6;
	out1[55] = temp2 + temp5;
	out1[62] = temp5 - temp2;
	out1[63] = temp1 + temp6; /* 22 multiplications 28 additions */
	/*for (i=0; i<64; i++) printf("out1[%d] = %f\n",i,out1[i]);*/

	/********* Post Additions *********/
	temp1 = out1[0] + out1[8];
	temp2 = out1[1] + out1[9];
	temp3 = out1[2] + out1[10];
	temp4 = out1[3] + out1[11];
	temp5 = out1[4] + out1[12];
	temp6 = out1[5] + out1[13];
	temp7 = out1[6] + out1[14];
	temp8 = out1[7] + out1[15];
	out[0] = temp1 + temp2;
	out[1] = temp1 - temp2;
	out[2] = temp4;
	out[3] = temp3 - temp4;
	out[4] = temp7 - temp6;
	out[5] = temp8;
	out[6] = -temp5 - temp7;
	out[7] = temp6 + temp8;
	temp1 = out1[0] - out1[8];
	temp2 = out1[1] - out1[9];
	temp3 = out1[2] - out1[10];
	temp4 = out1[3] - out1[11];
	temp5 = out1[4] - out1[12];
	temp6 = out1[5] - out1[13];
	temp7 = out1[6] - out1[14];
	temp8 = out1[7] - out1[15];
	out[8] = temp1 + temp2;
	out[9] = temp1 - temp2;
	out[10] = temp4;
	out[11] = temp3 - temp4;
	out[12] = temp7 - temp6;
	out[13] = temp8;
	out[14] = -temp5 - temp7;
	out[15] = temp6 + temp8;
	out[16] = out1[24] + out1[25];
	out[17] = out1[24] - out1[25];
	out[18] = out1[27];
	out[19] = out1[26] - out1[27];
	out[20] = out1[30] - out1[29];
	out[21] = out1[31];
	out[22] = -out1[28] - out1[30];
	out[23] = out1[29] + out1[31];
	temp1 = out1[16] - out1[24];
	temp2 = out1[17] - out1[25];
	temp3 = out1[18] - out1[26];
	temp4 = out1[19] - out1[27];
	temp5 = out1[20] - out1[28];
	temp6 = out1[21] - out1[29];
	temp7 = out1[22] - out1[30];
	temp8 = out1[23] - out1[31];
	out[24] = temp1 + temp2;
	out[25] = temp1 - temp2;
	out[26] = temp4;
	out[27] = temp3 - temp4;
	out[28] = temp7 - temp6;
	out[29] = temp8;
	out[30] = -temp5 - temp7;
	out[31] = temp6 + temp8;
	temp1 = out1[48] - out1[40];
	temp2 = out1[49] - out1[41];
	temp3 = out1[50] - out1[42];
	temp4 = out1[51] - out1[43];
	temp5 = out1[52] - out1[44];
	temp6 = out1[53] - out1[45];
	temp7 = out1[54] - out1[46];
	temp8 = out1[55] - out1[47];
	out[32] = temp1 + temp2;
	out[33] = temp1 - temp2;
	out[34] = temp4;
	out[35] = temp3 - temp4;
	out[36] = temp7 - temp6;
	out[37] = temp8;
	out[38] = -temp5 - temp7;
	out[39] = temp6 + temp8;
	out[40] = out1[56] + out1[57];
	out[41] = out1[56] - out1[57];
	out[42] = out1[59];
	out[43] = out1[58] - out1[59];
	out[44] = out1[62] - out1[61];
	out[45] = out1[63];
	out[46] = -out1[60] - out1[62];
	out[47] = out1[63] + out1[61];
	temp1 = -out1[32] - out1[48];
	temp2 = -out1[33] - out1[49];
	temp3 = -out1[34] - out1[50];
	temp4 = -out1[35] - out1[51];
	temp5 = -out1[36] - out1[52];
	temp6 = -out1[37] - out1[53];
	temp7 = -out1[38] - out1[54];
	temp8 = -out1[39] - out1[55];
	out[48] = temp1 + temp2;
	out[49] = temp1 - temp2;
	out[50] = temp4;
	out[51] = temp3 - temp4;
	out[52] = temp7 - temp6;
	out[53] = temp8;
	out[54] = -temp5 - temp7;
	out[55] = temp6 + temp8;
	temp1 = out1[40] + out1[56];
	temp2 = out1[41] + out1[57];
	temp3 = out1[42] + out1[58];
	temp4 = out1[43] + out1[59];
	temp5 = out1[44] + out1[60];
	temp6 = out1[45] + out1[61];
	temp7 = out1[46] + out1[62];
	temp8 = out1[47] + out1[63];
	out[56] = temp1 + temp2;
	out[57] = temp1 - temp2;
	out[58] = temp4;
	out[59] = temp3 - temp4;
	out[60] = temp7 - temp6;
	out[61] = temp8;
	out[62] = -temp5 - temp7;
	out[63] = temp6 + temp8; /* 96 additions */
	/*for (i=0; i<64; i++) printf("out[%d] = %f\n",i,out[i]);*/

	temp1 = out[0] + out[16];
	temp2 = out[1] + out[17];
	temp3 = out[2] + out[18];
	temp4 = out[3] + out[19];
	temp5 = out[4] + out[20];
	temp6 = out[5] + out[21];
	temp7 = out[6] + out[22];
	temp8 = out[7] + out[23];
	out1[0] = temp1 + temp3;
	out1[1] = temp2 + temp4;
	out1[2] = temp2 - temp4;
	out1[3] = temp1 - temp3;
	out1[4] = temp5;
	out1[5] = temp6;
	out1[6] = temp7;
	out1[7] = temp8;
	temp1 = out[8] + out[24];
	temp2 = out[9] + out[25];
	temp3 = out[10] + out[26];
	temp4 = out[11] + out[27];
	temp5 = out[12] + out[28];
	temp6 = out[13] + out[29];
	temp7 = out[14] + out[30];
	temp8 = out[15] + out[31];
	out1[8] = temp1 + temp3;
	out1[9] = temp2 + temp4;
	out1[10] = temp2 - temp4;
	out1[11] = temp1 - temp3;
	out1[12] = temp5;
	out1[13] = temp6;
	out1[14] = temp7;
	out1[15] = temp8;
	temp1 = out[8] - out[24];
	temp2 = out[9] - out[25];
	temp3 = out[10] - out[26];
	temp4 = out[11] - out[27];
	temp5 = out[12] - out[28];
	temp6 = out[13] - out[29];
	temp7 = out[14] - out[30];
	temp8 = out[15] - out[31];
	out1[16] = temp1 + temp3;
	out1[17] = temp2 + temp4;
	out1[18] = temp2 - temp4;
	out1[19] = temp1 - temp3;
	out1[20] = temp5;
	out1[21] = temp6;
	out1[22] = temp7;
	out1[23] = temp8;
	temp1 = out[0] - out[16];
	temp2 = out[1] - out[17];
	temp3 = out[2] - out[18];
	temp4 = out[3] - out[19];
	temp5 = out[4] - out[20];
	temp6 = out[5] - out[21];
	temp7 = out[6] - out[22];
	temp8 = out[7] - out[23];
	out1[24] = temp1 + temp3;
	out1[25] = temp2 + temp4;
	out1[26] = temp2 - temp4;
	out1[27] = temp1 - temp3;
	out1[28] = temp5;
	out1[29] = temp6;
	out1[30] = temp7;
	out1[31] = temp8;
	out1[32] = out[32] + out[34];
	out1[33] = out[33] + out[35];
	out1[34] = out[33] - out[35];
	out1[35] = out[32] - out[34];
	out1[36] = out[36];
	out1[37] = out[37];
	out1[38] = out[38];
	out1[39] = out[39];
	out1[40] = out[40] + out[42];
	out1[41] = out[41] + out[43];
	out1[42] = out[41] - out[43];
	out1[43] = out[40] - out[42];
	out1[44] = out[44];
	out1[45] = out[45];
	out1[46] = out[46];
	out1[47] = out[47];
	out1[48] = out[48] + out[50];
	out1[49] = out[49] + out[51];
	out1[50] = out[49] - out[51];
	out1[51] = out[48] - out[50];
	out1[52] = out[52];
	out1[53] = out[53];
	out1[54] = out[54];
	out1[55] = out[55];
	out1[56] = out[56] + out[58];
	out1[57] = out[57] + out[59];
	out1[58] = out[57] - out[59];
	out1[59] = out[56] - out[58];
	out1[60] = out[60];
	out1[61] = out[61];
	out1[62] = out[62];
	out1[63] = out[63]; /* 64 additions */
	/*for (i=0; i<64; i++) printf("out1[%d] = %f\n",i,out1[i]);*/

	temp1 = out1[0] + out1[32];
	temp2 = out1[1] + out1[33];
	temp3 = out1[2] + out1[34];
	temp4 = out1[3] + out1[35];
	temp5 = out1[4] + out1[36];
	temp6 = out1[5] + out1[37];
	temp7 = out1[6] + out1[38];
	temp8 = out1[7] + out1[39];
	U[0] = temp1 + temp5;
	U[8] = temp2 + temp6;
	U[16] = temp3 + temp7;
	U[24] = temp4 + temp8;
	U[32] = temp4 - temp8;
	U[40] = temp3 - temp7;
	U[48] = temp2 - temp6;
	U[56] = temp1 - temp5;
	temp1 = out1[8] + out1[40];
	temp2 = out1[9] + out1[41];
	temp3 = out1[10] + out1[42];
	temp4 = out1[11] + out1[43];
	temp5 = out1[12] + out1[44];
	temp6 = out1[13] + out1[45];
	temp7 = out1[14] + out1[46];
	temp8 = out1[15] + out1[47];
	U[1] = temp1 + temp5;
	U[9] = temp2 + temp6;
	U[17] = temp3 + temp7;
	U[25] = temp4 + temp8;
	U[33] = temp4 - temp8;
	U[41] = temp3 - temp7;
	U[49] = temp2 - temp6;
	U[57] = temp1 - temp5;
	temp1 = out1[16] + out1[48];
	temp2 = out1[17] + out1[49];
	temp3 = out1[18] + out1[50];
	temp4 = out1[19] + out1[51];
	temp5 = out1[20] + out1[52];
	temp6 = out1[21] + out1[53];
	temp7 = out1[22] + out1[54];
	temp8 = out1[23] + out1[55];
	U[2] = temp1 + temp5;
	U[10] = temp2 + temp6;
	U[18] = temp3 + temp7;
	U[26] = temp4 + temp8;
	U[34] = temp4 - temp8;
	U[42] = temp3 - temp7;
	U[50] = temp2 - temp6;
	U[58] = temp1 - temp5;
	temp1 = out1[24] + out1[56];
	temp2 = out1[25] + out1[57];
	temp3 = out1[26] + out1[58];
	temp4 = out1[27] + out1[59];
	temp5 = out1[28] + out1[60];
	temp6 = out1[29] + out1[61];
	temp7 = out1[30] + out1[62];
	temp8 = out1[31] + out1[63];
	U[3] = temp1 + temp5;
	U[11] = temp2 + temp6;
	U[19] = temp3 + temp7;
	U[27] = temp4 + temp8;
	U[35] = temp4 - temp8;
	U[43] = temp3 - temp7;
	U[51] = temp2 - temp6;
	U[59] = temp1 - temp5;
	temp1 = out1[24] - out1[56];
	temp2 = out1[25] - out1[57];
	temp3 = out1[26] - out1[58];
	temp4 = out1[27] - out1[59];
	temp5 = out1[28] - out1[60];
	temp6 = out1[29] - out1[61];
	temp7 = out1[30] - out1[62];
	temp8 = out1[31] - out1[63];
	U[4] = temp1 + temp5;
	U[12] = temp2 + temp6;
	U[20] = temp3 + temp7;
	U[28] = temp4 + temp8;
	U[36] = temp4 - temp8;
	U[44] = temp3 - temp7;
	U[52] = temp2 - temp6;
	U[60] = temp1 - temp5;
	temp1 = out1[16] - out1[48];
	temp2 = out1[17] - out1[49];
	temp3 = out1[18] - out1[50];
	temp4 = out1[19] - out1[51];
	temp5 = out1[20] - out1[52];
	temp6 = out1[21] - out1[53];
	temp7 = out1[22] - out1[54];
	temp8 = out1[23] - out1[55];
	U[5] = temp1 + temp5;
	U[13] = temp2 + temp6;
	U[21] = temp3 + temp7;
	U[29] = temp4 + temp8;
	U[37] = temp4 - temp8;
	U[45] = temp3 - temp7;
	U[53] = temp2 - temp6;
	U[61] = temp1 - temp5;
	temp1 = out1[8] - out1[40];
	temp2 = out1[9] - out1[41];
	temp3 = out1[10] - out1[42];
	temp4 = out1[11] - out1[43];
	temp5 = out1[12] - out1[44];
	temp6 = out1[13] - out1[45];
	temp7 = out1[14] - out1[46];
	temp8 = out1[15] - out1[47];
	U[6] = temp1 + temp5;
	U[14] = temp2 + temp6;
	U[22] = temp3 + temp7;
	U[30] = temp4 + temp8;
	U[38] = temp4 - temp8;
	U[46] = temp3 - temp7;
	U[54] = temp2 - temp6;
	U[62] = temp1 - temp5;
	temp1 = out1[0] - out1[32];
	temp2 = out1[1] - out1[33];
	temp3 = out1[2] - out1[34];
	temp4 = out1[3] - out1[35];
	temp5 = out1[4] - out1[36];
	temp6 = out1[5] - out1[37];
	temp7 = out1[6] - out1[38];
	temp8 = out1[7] - out1[39];
	U[7] = temp1 + temp5;
	U[15] = temp2 + temp6;
	U[23] = temp3 + temp7;
	U[31] = temp4 + temp8;
	U[39] = temp4 - temp8;
	U[47] = temp3 - temp7;
	U[55] = temp2 - temp6;
	U[63] = temp1 - temp5; /* 128 additions */
}
