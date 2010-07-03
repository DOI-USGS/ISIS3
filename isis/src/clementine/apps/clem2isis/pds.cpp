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
 *      Oct 31, 1994 Tracie Sucharski, USGS, Flagstaff  Change so that the
 *                      carriage returns are not removed from the labels.
 *      Sep 29, 1995 Tracie Sucharski, Added a fix to init_qt as given
 *                      by Luiz Perez from ACT. 
 *      Feb 20 2004 Kris Becker - Changed <malloc.h> to "isismalloc.h"
 *      Jun 17 2004 KJB - Added support for the Mac
 *      Jul 18 2006 Brendan George - removed qparm references to stop output
 *                      of paramtrs.dat file.
 *      Dec 11 2006 KJB Changed malloc.h to stdlib.h for the Mac port again.
 *                      malloc.h is deprecated.  
*/
#include <stdio.h>
#include <math.h>
#ifdef __TURBOC__
#include <alloc.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#include "jpeg_c.h"
#include "pds.h"

#define READ_PARAM "rb"
#define WRITE_PARAM "wb"

#if ISIS_LITTLE_ENDIAN
#else
#define sun
#endif

PDSINFO	pds;
//FILE	*qparm;                    //removed qparm references BMG 2006-07-18
extern long	*DCTHist[64];
extern float	*Rn[64];
extern float	Q[64];

void init_q_table(FILE *fptr);
void readhufftbls(FILE *fptr);
void pds_decomp(FILE *fptr,CHARH *p,long sizej,long sizei);
#ifdef	sun 
void PClong2SUNlongVector(unsigned long invec[],int npts) ;
void PCshort2SUNshortVector(unsigned short invec[],int npts) ;
#endif
extern void getDCTHist(BitStream *bs, long rows, long cols);
extern void getRn(void);


PDSINFO *PDSR(char *fname, long *rows, long *cols)
{
	FILE    *fptr;
	int     n;
	int     j,i;
	CHARH   *c;
	char    nstring[84],sdummy[80],buffer[50],low;
	long    sizej;                  /* number of rows in the image */
	long    sizei;                  /* number of columns in the image */
	int     bitpix;                 /* bits per pixel */
	int     record_bytes;
	int     hist_rec,brw_rec,image_rec,ns,brwsize;
	char    cval, *sptr, *ptr='\0';
	char    record_type[20];
	int     COMPRESSED=0;
	long    k,hdr_size;

	if( (fptr = fopen(fname,READ_PARAM)) == NULL){  /* open disk file */
		printf("Can't open %s.\n",fname);
		return NULL;
	}

	/* initialize some basic variables */
	bitpix			=0;
	sizej=sizei		=0;
	pds.browse_nrows	=0;
	pds.browse_ncols	=0;
	pds.image_nrows		=0;
	pds.image_ncols		=0;
	hist_rec = brw_rec = image_rec = -1;

	/* read header */
	do{
		/* read next line of text */
		for (n=0; (cval=fgetc(fptr)); n++) {
			nstring[n]=cval;
			if(cval=='\n') {
				if( (cval=fgetc(fptr)) != '\r') ungetc(cval,fptr);
				nstring[++n]='\0';
				break;
			}
		}

		/* find line's first non-space character */
		for (ns=0; nstring[ns]==' ';ns++);
		sptr = &nstring[ns];

		if (strncmp("^IMAGE_HISTOGRAM ",sptr,17)==0) {
                	/*printf("image histogram found \n"); */ 
			n=sscanf(nstring,"%s = %d", sdummy, &hist_rec);
                        /*printf("hist_rec = %d\n",hist_rec);*/
		}

		if (strncmp("^BROWSE_IMAGE ",sptr,14)==0) {
			n=sscanf(nstring,"%s = %d", sdummy, &brw_rec);
                	/*printf("brw_rec = %d\n",brw_rec);*/
		}

		if (strncmp("^IMAGE ",sptr,7)==0) {
			n=sscanf(nstring,"%s = %d", sdummy, &image_rec);
		}

		if (strncmp("RECORD_TYPE",sptr,9)==0) {
			n=sscanf(nstring,"%s = %s", sdummy, record_type);
		}

		if (strncmp("RECORD_BYTES",sptr,12)==0) {
			n=sscanf(nstring,"%s = %d", sdummy, &record_bytes);
		}

		if (strncmp("ENCODING_TYPE",sptr,13)==0) {
			n=sscanf(nstring,"%s = \"%[^\"]\"", sdummy, buffer);
			if ( strstr(buffer,"N/A") )
				COMPRESSED = 0;
			else if ( strstr(buffer,"DECOMPRESSED") )
				COMPRESSED = 0;
			else
				COMPRESSED = 1;
		}

		if (strncmp("LINES ",sptr,6)==0) {
			n=sscanf(nstring,"%s = %ld", sdummy, &sizej);
		}

		if (strncmp("LINE_SAMPLES",sptr,12)==0) {
			n=sscanf(nstring,"%s = %ld", sdummy, &sizei);
		}

		if (strncmp("SAMPLE_BITS",sptr,11)==0) {
			n=sscanf(nstring,"%s = %d", sdummy, &bitpix);
		}

		if (strncmp("END",sptr,3)==0) {
			if ( *(sptr+3)=='\n' || *(sptr+3)==' ' || *(sptr+3)=='\r' ) {
				hdr_size = ftell(fptr);
				break;
			}
		}
	} while(1);

	/**************   read histogram  ***************/
	if ( hist_rec != -1 ) {
		fseek(fptr,hist_rec-1,0);
		pds.hist = (long *)malloc(256*sizeof(long));
		if(pds.hist == NULL) {
			printf(" histogram memory not allocated \n");
		}
		if ( pds.hist ) {
		      fread(pds.hist,sizeof(long),256,fptr);
#ifdef sun
		      PClong2SUNlongVector((unsigned long*)pds.hist,256);
#endif
                	/*printf("pds.hist = %x\n",pds.hist);*/ 
                }
	}

	/**************   read browse image **********/
		if ( brw_rec != -1 ) {
			pds.browse_ncols	= sizei/8;
                        pds.browse_nrows	= sizej/8;
			fseek(fptr,brw_rec-1,0);
			brwsize = (sizej/8) * (sizei/8);
			pds.brw_imag = (unsigned char *)malloc(brwsize);
			if ( pds.brw_imag )
				fread(pds.brw_imag,sizeof(char),brwsize,fptr);
		}

	/*************   read image data ***************/
	if (strncmp(record_type,"UNDEFINED",9)==0) {
		record_bytes=1;
		fseek(fptr,(image_rec-1),0);
	} else {
		fseek(fptr,(image_rec-1)*record_bytes,0);
	}

	switch (bitpix) {
    case 8:
		c = (CHARH *)MALLOC(sizej*sizei);
		if ( c == NULL ) {
			printf("Can't allocate memory for image array.\n");
			fclose(fptr);
			return NULL;
		}

		if ( COMPRESSED ) {
			//qparm = fopen("paramtrs.dat","w");     //removed qparm references BMG 2006-07-18
			init_q_table(fptr);
			readhufftbls(fptr); 
			pds_decomp(fptr,c,sizej,sizei);
//			fclose(qparm);                         //removed qparm references BMG 2006-07-18
		} else {
                	
			for (j=0, k=0; j<sizej ;j++) {
				for (i=0; i<sizei; i++) {
					/*low= fgetc(fptr); replace fgetc by fread 1/24/94 */
                                        if(1!=fread(&low,sizeof(char),1,fptr)){
						printf("error: possible EOF found at (%d,%d)\n",j,i);
						break;
					} else {
						c[k++]=(unsigned char) low;
					}
				}
			}
		}

		pds.image = c;
		break;
	default:
		printf("invalid number of bits per pixel\n");
		pds.image = NULL;
	}

	/************    Allocate string buffer    **************/
	rewind(fptr);

	pds.text = (char *)malloc(hdr_size+1);
	if ( pds.text ) {
    ptr=pds.text;
		for (i=0; i<hdr_size; i++) {
		       /*	*(ptr) = fgetc(fptr); */
			fread(ptr,sizeof(char),1,fptr);
		/*	if ( *ptr == '\r' ) {
				 do nothing 
			} else {
				ptr++;
			}
                */
		ptr++;
		}
	}
	*(ptr)='\0';

        /*****/
	fclose(fptr);

	*rows = pds.image_nrows=sizej;
	*cols = pds.image_ncols=sizei;

	return &pds;
}


/******** Routines that deal with compressed images *******/
float   dfac[8] = {
0.35355339,0.35355339,0.653281482,0.27059805,
0.449988111,0.254897789,0.300672443,1.281457724
};

void init_q_table(FILE *fptr)
{
	short   i;
	short   scalef;
	short   table[64];
	float   ftable[64];

	fread(&scalef,sizeof(short),1,fptr);
#ifdef sun
	PCshort2SUNshortVector((unsigned short*)&scalef,1) ;
#endif
//	fprintf(qparm,"tabf: %d\n",scalef);         //removed qparm references BMG 2006-07-18
	fread(table,sizeof(short),64,fptr);
#ifdef sun
	PCshort2SUNshortVector((unsigned short*)table,64);
#endif
//	fprintf(qparm,"tabq:\n");                    //removed qparm references BMG 2006-07-18

	for (i=0; i<64; i++) {
	        table[i] = table[i]&0x00ff;    /*  TLS 9-29-95  */
//		fprintf(qparm,"%3d ",table[i]);            //removed qparm references BMG 2006-07-18
//		if ( (i+1) % 8 == 0 ) fprintf(qparm,"\n"); //removed qparm references BMG 2006-07-18

		ftable[i] = ( (float)scalef*(float)table[i] )/64.0 + 0.5;
		ftable[i] = 4096.0 / (float)floor(ftable[i]);
	}

	for (i=0; i<64; i++) Q[i] = ftable[zzseq[i]];

	ftable[0] = dfac[0]*dfac[0]*ftable[0];
	ftable[32] = dfac[0]*dfac[1]*ftable[32];
	ftable[16] = dfac[0]*dfac[2]*ftable[16];
	ftable[48] = dfac[0]*dfac[3]*ftable[48];
	ftable[8] = -dfac[0]*dfac[4]*ftable[8];
	ftable[24] = -dfac[0]*dfac[5]*ftable[24];
	ftable[56] = dfac[0]*dfac[6]*ftable[56];
	ftable[40] = -dfac[0]*dfac[7]*ftable[40];

	ftable[4] = dfac[1]*dfac[0]*ftable[4];
	ftable[36] = dfac[1]*dfac[1]*ftable[36];
	ftable[20] = dfac[1]*dfac[2]*ftable[20];
	ftable[52] = dfac[1]*dfac[3]*ftable[52];
	ftable[12] = -dfac[1]*dfac[4]*ftable[12];
	ftable[28] = -dfac[1]*dfac[5]*ftable[28];
	ftable[60] = dfac[1]*dfac[6]*ftable[60];
	ftable[44] = -dfac[1]*dfac[7]*ftable[44];

	ftable[2] = dfac[2]*dfac[0]*ftable[2];
	ftable[34] = dfac[2]*dfac[1]*ftable[34];
	ftable[18] = dfac[2]*dfac[2]*ftable[18];
	ftable[50] = dfac[2]*dfac[3]*ftable[50];
	ftable[10] = -dfac[2]*dfac[4]*ftable[10];
	ftable[26] = -dfac[2]*dfac[5]*ftable[26];
	ftable[58] = dfac[2]*dfac[6]*ftable[58];
	ftable[42] = -dfac[2]*dfac[7]*ftable[42];

	ftable[6] = dfac[3]*dfac[0]*ftable[6];
	ftable[38] = dfac[3]*dfac[1]*ftable[38];
	ftable[22] = dfac[3]*dfac[2]*ftable[22];
	ftable[54] = dfac[3]*dfac[3]*ftable[54];
	ftable[14] = -dfac[3]*dfac[4]*ftable[14];
	ftable[30] = -dfac[3]*dfac[5]*ftable[30];
	ftable[62] = dfac[3]*dfac[6]*ftable[62];
	ftable[46] = -dfac[3]*dfac[7]*ftable[46];

	ftable[1] = -dfac[4]*dfac[0]*ftable[1];
	ftable[33] = -dfac[4]*dfac[1]*ftable[33];
	ftable[17] = -dfac[4]*dfac[2]*ftable[17];
	ftable[49] = -dfac[4]*dfac[3]*ftable[49];
	ftable[9] = dfac[4]*dfac[4]*ftable[9];
	ftable[25] = dfac[4]*dfac[5]*ftable[25];
	ftable[57] = -dfac[4]*dfac[6]*ftable[57];
	ftable[41] = dfac[4]*dfac[7]*ftable[41];

	ftable[3] = -dfac[5]*dfac[0]*ftable[3];
	ftable[35] = -dfac[5]*dfac[1]*ftable[35];
	ftable[19] = -dfac[5]*dfac[2]*ftable[19];
	ftable[51] = -dfac[5]*dfac[3]*ftable[51];
	ftable[11] = dfac[5]*dfac[4]*ftable[11];
	ftable[27] = dfac[5]*dfac[5]*ftable[27];
	ftable[59] = -dfac[5]*dfac[6]*ftable[59];
	ftable[43] = dfac[5]*dfac[7]*ftable[43];

	ftable[7] = dfac[6]*dfac[0]*ftable[7];
	ftable[39] = dfac[6]*dfac[1]*ftable[39];
	ftable[23] = dfac[6]*dfac[2]*ftable[23];
	ftable[55] = dfac[6]*dfac[3]*ftable[55];
	ftable[15] = -dfac[6]*dfac[4]*ftable[15];
	ftable[31] = -dfac[6]*dfac[5]*ftable[31];
	ftable[63] = dfac[6]*dfac[6]*ftable[63];
	ftable[47] = -dfac[6]*dfac[7]*ftable[47];

	ftable[5] = -dfac[7]*dfac[0]*ftable[5];
	ftable[37] = -dfac[7]*dfac[1]*ftable[37];
	ftable[21] = -dfac[7]*dfac[2]*ftable[21];
	ftable[53] = -dfac[7]*dfac[3]*ftable[53];
	ftable[13] = dfac[7]*dfac[4]*ftable[13];
	ftable[29] = dfac[7]*dfac[5]*ftable[29];
	ftable[61] = -dfac[7]*dfac[6]*ftable[61];
	ftable[45] = dfac[7]*dfac[7]*ftable[45];

	for (i=0; i<64; i++) q_table[i] = ftable[zzseq[i]];
}

void readhufftbls(FILE *fptr)
{
	fread(dcbits,sizeof(short),16,fptr);
#ifdef sun
	PCshort2SUNshortVector((unsigned short*)dcbits,16);
#endif

	fread(dchuffval,sizeof(char),12,fptr);
	fread(acbits,sizeof(short),16,fptr);
#ifdef sun
	PCshort2SUNshortVector((unsigned short*)acbits,16);
#endif

	fread(achuffval,sizeof(char),162,fptr);

	inithuffcode();
}


void pds_decomp(FILE *fptr,CHARH *p,long sizej,long sizei)
{
	BitStream       ibs;
	short   i, npanels;
	long    nbytes,bytesperpanel;
	short   blocks, rem;
	unsigned short  nb;
	long    filepos1, filepos2;
	CHARH   *ptr;
	int     FLAG = 0;

	cBitStream(&ibs,NULL,INPUT);

	filepos1 = ftell(fptr);
	fseek(fptr,0,2);
	filepos2 = ftell(fptr);
	fseek(fptr,filepos1,0);

	nbytes = filepos2 - filepos1;

	ibs.outstring = (CHARH *)MALLOC(nbytes);
	if ( ibs.outstring ) {
		blocks = 1;
		rem = 0;
		nb = (unsigned short)nbytes;
		if ( nbytes > 60000L ) {
			blocks = nbytes / 32768;
			rem = nbytes % 32768;
			nb = 32768;
		};
		ptr = ibs.outstring;
		for (i=0; i < blocks; i++,ptr+=nb) {
			if ( fread(ptr,sizeof(char),nb,fptr) != nb ) {
				printf("Error reading data string.\n");
				FLAG = 1;
				break;
			}
		}
		if ( rem ) {
			if ( (int)fread(ptr,sizeof(char),rem,fptr) != rem ) {
				printf("Error reading data string.\n");
				FLAG = 1;
			}
		}
		ibs.mode = MEMORY;
	} else {
		ibs.bytestream.file = fptr;
		ibs.mode = DISK;
	}

	if ( !FLAG ) {
		npanels = sizej/32;
		bytesperpanel = 32*sizei;

		/* Allocate memory for DCT coefficients histograms */
		for (i=0; i<64; i++) {
			DCTHist[i] = (long FAR *)MALLOC(sizeof(long)*513);
			memset((void *)DCTHist[i],0,sizeof(long)*513);
			DCTHist[i] += 256;
		}
		/* Allocate memory for DCT coefficients table look-up */
		for (i=0; i<64; i++) {
			Rn[i] = (float FAR *)MALLOC(sizeof(float)*513);
			memset((void *)Rn[i],0,sizeof(float)*513);
			Rn[i] += 256;
		}

		for (i=0; i<npanels; i++)
			/* get DCT coeffcients histogram */
			getDCTHist(&ibs,32,sizei);

		/* fill in table look-up */
		getRn();

		ibs.BitBuffer = 0;
		ibs.bytesout = 0;
		ibs.BitBuffMask = 0x00;

		/* do decompression w/ DCT coefficients optimization */
		for (i=0, ptr=p; i<npanels; i++,ptr+=bytesperpanel) {
			decomp(&ibs,ptr,32,sizei);
		}

		for (i=0; i<64; i++) {
			DCTHist[i] -= 256;
			Rn[i] -= 256;
			FREE(DCTHist[i]);
			FREE(Rn[i]);
		}
	}

	if ( ibs.outstring ) FREE( ibs.outstring );
}


#ifdef sun
void PClong2SUNlongVector(unsigned long invec[],int npts){
	int	i;
	unsigned long 	ival,oval;

	for(i=0;i<npts;i++){
		ival	= invec[i];
		oval	= ((ival&0x000000ff)<<24) +
			  ((ival&0x0000ff00)<<8) +
			  ((ival&0x00ff0000)>>8) +
			  ((ival&0xff000000)>>24);
		invec[i]= oval; 
	}
}
void PCshort2SUNshortVector(unsigned short invec[],int npts){
	int	i;
        unsigned short 	ival,oval;
 	for(i=0;i<npts;i++){
		ival	= invec[i];
		oval	= (ival<<8) + ((ival>>8)&0x00ff);
		invec[i]= oval;

        }
}
#endif

