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
//static char *sccsid = "@(#)image_io.c  1.1 10/04/99";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "image_io.h"

void image_change_header(struct image_header *header)
{
  static int hdr_buf[IMAGE_HEADER_LENGTH / sizeof(int)];

  hdr_buf[0] = MAGIC;
  hdr_buf[1] = header->height;
  hdr_buf[2] = header->width;
  hdr_buf[3] = header->bpe;
  sprintf((char *)hdr_buf + IMAGE_LABEL_OFFSET, "%s", header->label);

  /* write the new header */
  lseek(header->fd, 0L, 0);
  if(write(header->fd, hdr_buf, IMAGE_HEADER_LENGTH) < IMAGE_HEADER_LENGTH) {
    printf("image_change_header : unable to write new header\n");
    exit(1);
  }
}

void image_open(char *filename, struct image_header *header, char *mode)
{
  int len;
  char buf[64];
  static int hdr_buf[IMAGE_HEADER_LENGTH / sizeof(int)];
  int  fd;
  char *text_ptr;

  /* check to see if the filename ends in ".ddd" */
  len = strlen(filename);
  if((len > 4) && (!strcmp(&(filename[len-4]), ".ddd"))) {
    strcpy(buf, filename);
  }
  else {
    sprintf(buf, "%s.ddd", filename);
  }
  if(((*mode == 'r') && (mode[1] != 'w')) || (*mode == 'u')) {
    if(strcmp(buf, "-.ddd")) fd = open(buf, (*mode == 'r') ? 0 : 2);
    else fd = 0;
    if(fd < 0) {
      printf("image_open : Unable to open %s\n", buf);
      exit(1);
    }
    if(read(fd, hdr_buf, IMAGE_HEADER_LENGTH) < IMAGE_HEADER_LENGTH) {
      printf("image_open : %s is not a valid image file\n", buf);
      exit(1);
    }
    if((*hdr_buf) != MAGIC) {
      printf("image_open : %s is not a valid image file\n", buf);
      exit(1);
    }
    header->height = hdr_buf[1];
    header->width = hdr_buf[2];
    header->bpe = hdr_buf[3];
    if(header->bpe == 0)
      header->bpe = 8;
    text_ptr = (char *) malloc(strlen(((char *)hdr_buf) + IMAGE_LABEL_OFFSET)+1);
    strcpy(text_ptr, (char *)(((char *)hdr_buf) + IMAGE_LABEL_OFFSET));
    header->label = text_ptr;
    header->fd = fd;
  }
  else if((*mode == 'w') || (!strncmp(mode, "rw", 2))) {
    if((header->width == 0) || (header->height == 0)) {
      printf("image_open : Invalid size for image\n");
      exit(1);
    }
    fd = creat(buf, 0666);
    if(fd < 0) {
      printf("image_open : Unable to create %s\n", buf);
      exit(1);
    }
    if(*mode != 'w') {
      close(fd);
      fd = open(buf, 2);
      if(fd < 0) {
        printf("image_open : Unable ot open %s\n", buf);
        exit(1);
      }
    }
    header->fd = fd;
    hdr_buf[0] = MAGIC;
    hdr_buf[1] = header->height;
    hdr_buf[2] = header->width;
    hdr_buf[3] = header->bpe;
    sprintf((char *)hdr_buf + IMAGE_LABEL_OFFSET, "%s", header->label);
    if(write(fd, hdr_buf, IMAGE_HEADER_LENGTH) < IMAGE_HEADER_LENGTH) {
      printf("image_open : unable to write header for %s\n", buf);
      exit(1);
    }
  }
  else {
    printf("image_open : Invalid mode (%s)\n", mode);
    exit(1);
  }
}

