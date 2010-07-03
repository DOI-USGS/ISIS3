/*

==================================================
2003-APR-17 Glenn Bennett - US Geological Survey

Due to a conflict with XV - by John Bradley
- XV program also uses a version of this 'readmoc'
program ISIS system has renamed this file

  from: readmoc
  to:   readmocisis

an accompanying program has also been renamed:

  from: readmocdrv
  to:   readmocdrvisis

This will prevent problems encountered where two
packages have the same named underlying programs.
==================================================
==================================================
2003-OCT-15 Stuart Sides - US Geological Survey

Converted calls to "gets" to "fgets" to satisfy
of compiler warning about "gets" being unsafe. These
calls were used to get the input and output filenames.
Changing from "gets" to "fgets" also required a check
to make sure there was no '\n' at the end of the input
string.
==================================================


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
static char *sccsid = "@(#)readmocisis.c	1.2 04/10/00";

/*
    SDP interpretation program
    Mike Caplinger, MOC GDS Design Scientist
    SCCS @(#)readmocisis.c	1.2 04/10/00

    Reads and decompresses MOC SDP files to create PDS images.
    Derived from the GDS readmsdp program.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "image_io.h"
#include "array.h"
#include "msdp.h"

extern FILE *write_header(int width, int height, 
                          FILE *infile, char *outfname);
static int in;/*, out;*/
static FILE *out;
static struct image_header inf;
static FILE *infile;
int errors;
static int test_pred;
static int rawencode = 0;

static int verbose = 0;

static int frag_offset[128];

static char infname[256], outfname[256];
static int mbr = 0;

byte *decode();

#define FRAGSIZE (256*1024)

static char decode_file[128];

static unsigned int sync;

static char label[1024];

static int status;
#define STAT_SHORT 2
#define STAT_BADSEQ 4
#define STAT_BADCS 8

enum {
  RAW = 0, PRED, XFORM
} MocCompress = RAW;

int main(argc, argv)
int argc;
char **argv;
{
  int height, width;
  int frag_lines, n_frags;
  pixel *frag;
  int f;
  float quant;
  int actual_height;
  int total_image = 0;
  int total = 0;
  int cs_check = 1;
  int pad_cs = 0;
  int i;
  char s[8];
  int multi = 0;
  int sequence = -1, processor = 0, n_processors = 1;
  int last_frag = -1;

  sync = 0xf0ca;

  if (argc < 3) {
    printf("\nEnter name of file to be decompressed: ");
    fgets(infname,256,stdin);
    int inLen = strlen(infname)-1;
    if (inLen == '\n') infname[inLen] = '\0';
    printf("\nEnter name of uncompressed output file: ");
    int outLen = strlen(outfname)-1;
    if (outLen == '\n') outfname[outLen] = '\0';
    fgets(outfname,256,stdin);
  }
  else {
    strcpy(infname, argv[1]);
    strcpy(outfname, argv[2]);
  }

  infile = fopen(infname, "r");
  if (infile == 0) {
    fprintf(stderr, "Can't open %s\n", argv[1]);
  }

  while (1) {
    int count;
    int len;
    static int first = 1;
    struct msdp_header h, lasth;
    int datlen;
    byte *indat, *chunk;

    lasth = h;
    i = fseek(infile, total+2048, 0);
    count = fread(&h, sizeof(h), 1, infile);
    i = MAKELONG(h.len);
    if (count && MAKELONG(h.len) == 0) {
      /* simulate the EOF even though there's padding */
      count = 0;
      h = lasth;
    }
    i = h.status&2;
    if (count == 0 && MocCompress == PRED && (h.status&2) == 0) {
      /* image was short -- last flag missing */
      h.status = 2;
      frag = decode(h, indat, 0, &len, mbr);
/*	    write(out, frag, len);*/
      i = fwrite(frag, 1, len, out);
      total_image += len;
    }
    if (count == 0) break;
    if (MAKELONG(h.len) == 0) break;
    sequence += 1;

    if (first && !multi) {
      int edit[2];

      width = h.edit_length*16;
      init_output(h);
      first = 0;
      height = MAKESHORT(h.down_total)*16;
    }
    h.edit_length = width/16;
    if (mbr) width = 512;

    datlen = MAKELONG(h.len);

    if (sequence%n_processors != processor) {
      total += sizeof(struct msdp_header) + datlen + 1;
      continue;
    }

    if (!multi && MAKESHORT(h.fragment) != last_frag+1) {
      int n_pad = MAKESHORT(h.fragment)-last_frag-1;
      char *frag = (char *) malloc(240*1024);
      /* don't pad predictively-compressed data */
      if (!(h.compression[0] & 3) && n_pad > 0) {
        errors += 1;
        status |= STAT_BADSEQ;
        bzero(frag, 240*1024);
        total_image += n_pad*240*1024;
        if (verbose) fprintf(stderr, "padding %d frags\n", n_pad);
/*		while(n_pad--) write(out, frag, 240*1024);*/
        while (n_pad--) i = fwrite(frag, 1, 240*1024, out);
      }
      free(frag);
    }
    last_frag = MAKESHORT(h.fragment);

    if (verbose) fprintf(stderr, "id %d/%d, len %d\n", MAKESHORT(h.id),
                         MAKESHORT(h.fragment), MAKELONG(h.len));
    chunk = (byte *) malloc(datlen+sizeof(struct msdp_header)+1);
    indat = chunk+sizeof(struct msdp_header);
    count = fread(indat, 1, datlen, infile);
    if (count != datlen) {
      if (verbose) fprintf(stderr,
                           "Error: short read (%d) of data part of fragment\n",
                           count);
      errors += 1;
      break;
    }

    /* check MSDP checksum */
    if (cs_check) {
      bcopy(&h, chunk, sizeof(h));
      fread(chunk+datlen+sizeof(h), 1, 1, infile);
      if (!CS8EACC2(chunk, datlen+sizeof(h)+1)) {
        if (verbose) fprintf(stderr, "Error: bad MSDP checksum\n");
        status |= STAT_BADCS;
        errors += 1;
        if (pad_cs) {
          char *frag = (char *) malloc(240*1024);
          bzero(frag, 240*1024);
          total_image += 240*1024;
          total += sizeof(struct msdp_header) + datlen + 1;
          if (verbose) fprintf(stderr, "trashing bad frag\n");
/*		    write(out, frag, 240*1024);*/
          i = fwrite(frag, 1, 240*1024, out);
          free(frag);
          continue;
        }
      }
    }

    frag = decode(h, indat, datlen, &len, mbr);
    i = sizeof(frag);
    total_image += len;
    if (verbose) fprintf(stderr, "fragment len %d => %d\n", datlen, len);
    total += sizeof(struct msdp_header) + datlen + 1;
/*	write(out, frag, len);*/
    i = fwrite(frag, 1, len, out);
    if (0) free(frag);
    free(chunk);
    if (h.status&2) break;
  }
  fclose(out);

  actual_height = total_image/width;
  if (!multi && actual_height != height) {
    if (verbose) fprintf(stderr,
                         "Error: total MSDP height (%d) != actual height (%d)\n",
                         height, actual_height);
    (void) write_header(width, actual_height, infile, outfname);
    errors += 1;
    status |= STAT_SHORT;
  }

  if (status && verbose) fprintf(stderr, "error status %c%c%c%c\n",
                                 MocCompress==RAW?'r':(MocCompress==PRED?'p':'t'),
                                 status&STAT_BADCS?'c':'-',
                                 status&STAT_BADSEQ?'n':'-',
                                 status&STAT_SHORT?'s':'-'
                                );
  if (errors) exit((MocCompress << 4) | status | (errors?1:0));
  else exit(0);
}

byte *decode(h, data, datlen, len, mbr)
struct msdp_header h;
byte *data;
int datlen, *len;
int mbr;
{
  int height, width;
  unsigned int xcomp, pcomp, spacing, levels;
  byte *transform_decomp_main();
  byte *predictive_decomp_main();
  byte *image;
  static Array *tbuf;
  static int init_decode;
  int huffman_table;

  if (mbr) {
    width = 512;
    height = 480;
    xcomp = 0;
    pcomp = 0;
  }
  else {
    width = h.edit_length*16;
    height = MAKESHORT(h.down_length)*16;
    xcomp = (h.compression[0] >> 2) & 3;
    pcomp = (h.compression[0] & 3);
    spacing = h.compression[4] | (h.compression[5] << 8);
    levels = (h.compression[1] >> 5)+1;
    huffman_table = h.compression[1]&0xf;
  }

  *len = width*height;

  if (pcomp && xcomp) {
    fprintf(stderr, "error: both pcomp and xcomp set\n");
    exit(1);
  }
  if (pcomp) MocCompress = PRED;
  if (xcomp) MocCompress = XFORM;

  if (!rawencode && pcomp == 0 && xcomp == 0) {
    /* raw image */
    image = data;
    if (datlen > *len) {
      if (verbose) fprintf(stderr, "Warning: MSDP line count (%d) < implied (%d), using latter\n", height, datlen/width);
      *len = datlen;
      height = datlen/width;
    }
    if (verbose) fprintf(stderr, "%d wide by %d high ", width, height);
    if (verbose) fprintf(stderr, "raw fragment%s\n", mbr?" (MBR)":"");
  }
  else
    if (verbose) fprintf(stderr, "%d wide by %d high ", width, height);

  if (xcomp > 0) {
    /* transform compressed; 2 = DCT, 1 = WHT */
    if (verbose) fprintf(stderr, "%s transformed fragment (%d groups, %.2f requant)\n",
                         xcomp == 2 ? "dct" : "wht", levels, spacing/16.0);
    image = transform_decomp_main(data, datlen, height, width,
                                  xcomp-1, spacing, levels);
  }

  if (rawencode || pcomp > 0) {
    /* predictively compressed */
    if (rawencode)
      if (verbose) fprintf(stderr, "raw encoded fragment\n");
      else
        if (verbose) fprintf(stderr, "%s%s predictive fragment, table %d\n",
                             pcomp&1 ? "x" : "", (pcomp&2)>>1 ? "y" : "",
                             huffman_table);

      /* set up decode arrays */
    if (!init_decode) {
      if (*decode_file) decodeLoad(decode_file);
      else decodeInit(huffman_table);
    }

    if (test_pred) {
      int dummy;

      image = predictive_decomp_main(data, datlen, height, width,
                                     (sync != 0), sync,
                                     pcomp&1, (pcomp&2) >> 1,
                                     &dummy);
    }
    else {
      /* squirrel data away */
      if (!tbuf) tbuf = array_new(datlen*8);
      if (datlen && !array_append(tbuf, data, datlen)) {
        // FIXED 2008/10/29, "datalen" was part of the print statement and no arguments
        //   were provided for the %d. - Steven Lambright, pointed out by "novas0x2a" (Support Forum Member)
        fprintf(stderr, "can't allocate temp space (%d bytes)\n", datlen);
        exit(1);
      }
      image = 0;
      *len = 0;
      if (h.status & 2) {
        int got_height;
        extern int pred_past_eof;
        int want_h = MAKESHORT(h.down_total)*16;;
        if (verbose) fprintf(stderr, "decompressing %d wide by %d high image\n",
                             width, want_h);
        image =
        predictive_decomp_main(array_data(tbuf),
                               array_len(tbuf),
                               want_h, width,
                               (sync != 0), sync,
                               pcomp&1, (pcomp&2) >> 1,
                               &got_height);
        /* This is tricky.  We can get bad sync even without
           checksum errors if anomaly 8 occurs.  We want to
           distinguish between this case and the case where
           we just ran out of fragments during the NA image.
           So if we run into a sync error and haven't run off
           the end of the image, we force BADCS on. */
        if (got_height != want_h && !pred_past_eof)
          status |= STAT_BADCS;
        *len = got_height * width;
      }
    }
  }

  return image;
}

int worklist_init() {
  struct msdp_header h;
  int count;
  int frag;
  int height, width, xcomp;
  int datlen;

  while (1) {
    count = fread(&h, sizeof(h), 1, infile);
    if (count == 0) break;
    xcomp = (h.compression[0] >> 2) & 3;
    if (!xcomp) return 0;
    height = MAKESHORT(h.down_length)*16;
    width = h.edit_length*16;
    datlen = MAKELONG(h.len);
    init_output(h);
    frag_offset[frag+1] = frag_offset[frag]+height*width;
    frag += 1;
    fseek(infile, datlen+1, 1);
  }
  return 1;
}

int init_output(h)
struct msdp_header h;
{
  int height, width;
  char s[8];
  int i;
  char buf[1024];

  height = MAKESHORT(h.down_total)*16;
  width = h.edit_length*16;
  sprintf(label, "decompressed-from %s\nid %d time %u:%d\ngain 0x%x \
offset %d\nstart %d cross %d down %d\ncmd ", infname,
          MAKESHORT(h.id),
          MAKELONG(h.time+1), h.time[0],
          h.gain, h.offset,
          h.edit_start*16,
          h.edit_length*16,
          MAKESHORT(h.down_total)*16);
  switch (h.cmd[0]) {
    case 1:
    case 2:
      strcat(label, "na ");
      break;
    case 3:
    case 4:
      strcat(label, "wa ");
      break;
    case 5:
    case 6:
      strcat(label, "global-map ");
      break;
    case 0x15:
    case 0x16:
      strcat(label, "mbr ");
      mbr = 1;
      width = 512;
      height = 512;
      break;
    case 0xd:
      strcat(label, "read-memory ");
      break;
    default:
      strcat(label, "unknown ");
      break;
  }
  for (i = 0; i < 17; i++) {
    sprintf(s, "%02x", h.cmd[i]);
    strcat(label, s);
  }

  sprintf(buf, "\nsensor %d clocking %d system-id 0x%x",
          MAKESHORT(h.sensors),
          MAKESHORT(h.other+1),
          h.other[3]);
  strcat(label, buf);

  out = (FILE *) write_header(width, height, infile, outfname);
}
