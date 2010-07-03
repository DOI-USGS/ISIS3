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

void *cBitStream( BitStream *bs, char *fn, Fmode fm )
{
	cByteStream( &bs->bytestream, fn, fm );
	bs->BitBuffer = 0;
	bs->bytesout = 0;
	bs->outstring = NULL;
	bs->BitBuffMask =  (fm==OUTPUT) ? 0x80:0x00;
	bs->bitmask[0] = 0x0000; bs->bitmask[1] = 0x0001; bs->bitmask[2] = 0x0002;
	bs->bitmask[3] = 0x0004; bs->bitmask[4] = 0x0008; bs->bitmask[5] = 0x0010;
	bs->bitmask[6] = 0x0020; bs->bitmask[7] = 0x0040; bs->bitmask[8] = 0x0080;
	bs->bitmask[9] = 0x0100; bs->bitmask[10] = 0x0200; bs->bitmask[11] = 0x0400;
	bs->bitmask[12] = 0x0800; bs->bitmask[13] = 0x1000; bs->bitmask[14] = 0x2000;
	bs->bitmask[15] = 0x4000; bs->bitmask[16] = 0x8000;
    return bs;
}

void *dBitStream(BitStream *bs)
{
	if ( bs->bytestream.mode == OUTPUT ) {
		if ( bs->BitBuffMask != 0x80 ) {
			while (bs->BitBuffMask) {
				bs->BitBuffer |= bs->BitBuffMask;
				bs->BitBuffMask >>= 1;
			}
			if ( bs->mode )
				bs->outstring[bs->bytesout] = bs->BitBuffer;
			else
				ByteStream_write( &bs->bytestream, bs->BitBuffer );
			bs->bytesout++;
		}
		if ( bs->mode ) {
			if ( fwrite(bs->outstring,sizeof(char),bs->bytesout,bs->bytestream.file) == 0 )
				printf("Error: writing output bitstream to file.\n");
		}
		if ( fseek(bs->bytestream.file,8,0) )
			printf("Error: fseek in subroutine dBitStream().\n");
		else {
			if ( fwrite(&(bs->bytesout),sizeof(long),1,bs->bytestream.file) == 0 )
				printf("Error: writing bytesout value to file.\n");
		}
	}
	dByteStream(&bs->bytestream);
    return bs;
}

short BitStream_write(BitStream *bs, short bits, short width)
{
	unsigned short BitMask = bs->bitmask[width];

	while ( BitMask ) {
		if ( bits & BitMask ) bs->BitBuffer |= (short)bs->BitBuffMask;
		BitMask >>= 1;
		bs->BitBuffMask >>= 1;
		if ( !bs->BitBuffMask ) {
			if ( bs->mode )
				bs->outstring[bs->bytesout] = bs->BitBuffer;
			else
				ByteStream_write( &bs->bytestream, bs->BitBuffer );
			bs->bytesout++;
			bs->BitBuffer = 0;
			bs->BitBuffMask = 0x80;
		}
	}
	return bs->bytestream.stat;
}

short BitStream_read( BitStream *bs, short w )
{
	unsigned short RetVal = 0, BitMask = bs->bitmask[w];

	while ( BitMask ) {
		if ( !bs->BitBuffMask ) {
			if ( bs->mode ) {
				bs->BitBuffer = ((short)bs->outstring[bs->bytesout]) & 0x00ff;
			} else
				bs->BitBuffer = ByteStream_read(&bs->bytestream);

			bs->bytesout++;
			bs->BitBuffMask = 0x80;
		}
		if ( bs->BitBuffer & bs->BitBuffMask ) RetVal |= BitMask;
		bs->BitBuffMask >>= 1;
		BitMask >>= 1;
	}
	return RetVal;
}

void *cByteStream(ByteStream *Bs, char *FileName, Fmode FileMode)
{
	Bs->mode = FileMode;
	if ( FileName != NULL ) {
		Bs->file = fopen(FileName,(Bs->mode==INPUT) ? "rb":"wb");
		if ( Bs->file == NULL ) printf("ByteStream constructor error.\n");
		Bs->stat = 0;
	}
    return Bs;
}

void *dByteStream(ByteStream *Bs)
{
	if ( Bs->file ) {
		fclose(Bs->file);
		Bs->file = NULL;
	}
    return Bs;
}

short ByteStream_read(ByteStream *Bs)
{
	short     c;
	char	cval;
        int	n;

	if ( Bs->mode == INPUT ) {
		/* c = fgetc( Bs->file ); */
		n=fread(&cval,sizeof(char),1,Bs->file);
                c	= cval; 
		if ( n != 1 ) Bs->stat = EOF;
		return c;
	} 
    else
		Bs->stat = EOF;
    return EOF;
}

short ByteStream_write(ByteStream *Bs, short c)
{
	if ( (Bs->mode != OUTPUT) || (fputc(c,Bs->file) == EOF) ) Bs->stat = EOF;
	return Bs->stat;
}

short ByteStream_status(ByteStream *Bs)
{
	return Bs->stat;
}
