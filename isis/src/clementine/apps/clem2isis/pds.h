#ifndef pds_h
#define pds_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#if defined(__BORLANDC__) && !defined(__WIN32__)
#define CHARH   unsigned char huge
#else
#define CHARH unsigned char
#endif

typedef struct {
  char          *text;          /* pointer to text header */
  long          *hist;      /* pointer to histogram   */
  unsigned char   *brw_imag;      /* pointer to browse image (if available) */
  int           browse_nrows;   /* number of rows in browse image */
  int           browse_ncols;   /* number of columns in browse image */
  CHARH   *image;         /* pointer to decompressed or uncompressed image */
  int           image_nrows;    /* number of rows in the image */
  int           image_ncols;    /* number of columns in the image */
} PDSINFO;

//extern FILE     *qparm;          //removed qparm references BMG 2006-07-18

extern PDSINFO *PDSR(char *fname, long *rows, long *cols);

#endif
