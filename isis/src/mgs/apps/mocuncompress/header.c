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
static char *sccsid = "@(#)header.c	1.2 10/06/99";
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_io.h"
#include <errno.h>

int prefix(s, pre)
char *s, *pre;
{
    return strncmp(s, pre, strlen(pre)) == 0;
}

/* write the image header and return the file descriptor to the open file */
/*int write_header(width, height, infile, outfname)*/
FILE *write_header(width, height, infile, outfname)
FILE *infile;
char *outfname;
{
    static int out = -1;
    int pos;
    FILE *outf;
    char line[256];
    int nrec;
    int i, pad;

    if(out < 0) {
	i = remove(outfname);
	if (errno == 0 || errno == 2) out = 1;
	else {
	    fprintf(stderr, "can't create %s\n", outfname);
	    exit(1);
        }
/*	if((out = creat(outfname, 0666)) < 0) {
	    fprintf(stderr, "can't create %s\n", outfname);
	    exit(1);
	}*/
        outf = fopen(outfname, "w+");
    }
/*    i = lseek(out, 0l, 0);*/
      outf = fopen(outfname, "r+");
/*    outf = fdopen(dup(out), "r+");*/

    nrec = 2048/width+(2048%width?1:0);
    
    /* copy the PDS header with a few modifications */
    pos = ftell(infile);
    i = fseek(infile, 0, 0);
    while(fgets(line, sizeof(line), infile)) {
	i = strlen(line);
	if(prefix(line, "RECORD_BYTES")) {
	    i = fprintf(outf, "RECORD_BYTES = %d\r\n", width);
	}
	else if(prefix(line, "FILE_RECORDS")) {
	    i = fprintf(outf, "FILE_RECORDS = %d\r\n", height+nrec);
	}
	else if(prefix(line, "^IMAGE")) {
	    i = fprintf(outf, "^IMAGE = %d\r\n", nrec+1);
	}
	else if(prefix(line, "LABEL_RECORDS")) {
	    i = fprintf(outf, "LABEL_RECORDS = %d\r\n", nrec);
	}
	else if(prefix(line, "DATA_SET_ID")) {
	    i = fprintf(outf, "DATA_SET_ID = \"MGS-M-MOC-NA/WA-2-DSDP-L0-V1.0\"\r\n");
        }
	else if(prefix(line, "ENCODING_TYPE")) {
	    ; /* skip this in output */
	}
	else if(prefix(line, "FILE_NAME")) {
	    i = fprintf(outf, "FILE_NAME = \"%s\"\r\n", outfname);
	}
	else if(prefix(line, "LINES")) {
	    i = fprintf(outf, "LINES = %d\r\n", height);
	}
	else if(prefix(line, "END\r")) {
	    i = fputs(line, outf);
	    break;
	}
	else {
/*	    i = fputs(line, outf); */
	    i = fprintf(outf, "%s", line);
	}
    }
    pad = nrec*width-(i = ftell(outf));
    if(pad < 0) {
	fprintf(stderr, "Error: header too large\n");
	exit(1);
    }
    
    for(i = 0; i < pad; i++) fputc(' ', outf);

/*    fclose(outf);*/
    i = fseek(outf, nrec*width, 0);
    i = fseek(infile, pos, 0);
/*    return out;*/
    return outf;
}    
