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
static char *sccsid = "@(#)initBlock.c	1.1 10/04/99";
#if (!defined(NOSCCSID) && (!defined(LINT)))
#endif
/*
* DESCRIPTION
*
* COMMENTARY
*/

#include <stdio.h>
#include <stdlib.h>

#include "fs.h"
#include "limits.h"
#include "encodeCoefs.static.h"

#include "initBlock.h"

extern void exit();

BITTREE *encodeTrees[MAXCODES];

static uint32 bitReverse(num) register uint32 num; {
register uint32 rev;
register uint32 i;

	rev = 0;

	for (i = 0; ; i++) {
		if (num & 0x1) {
			rev |= 0x1;
		};

		if (i == 31) {
			break;
		};

		num >>= 1;
		rev <<= 1;
	};

	return(rev);
}

static int32 compare(e1,e2) BITTREE *e1,*e2; {
register uint32 c1,c2;
	c1 = e1->code;
	c2 = e2->code;

	if (c1 < c2) {
		return(-1);
	} else {
	if (c1 > c2) {
		return(1);
	} else {
		return(0);
	};
	};
}

BITTREE *makeTree(start,size,bit) BITTREE *start; uint32 size; uint32 bit; {
BITTREE *cur;
uint32 count;
BITTREE *scan;

	if (size == 1) {
		cur = start;
	} else {
		if ((cur = (BITTREE *)malloc(sizeof(*cur))) == NULL) {
			(void)fprintf(stderr,"Not enough memory for huffman trees\n");
			exit(1);
		};

		for (count = 0, scan = start; count < size && (scan->code & bit) == 0; count++, scan++) {
		};

		cur->zero = makeTree(start,count,bit << 1);
		cur->one  = makeTree(start+count,size-count,bit << 1);
	};

	return(cur);
}

void initBlock() {
uint32 which,n;
uint32 size;
uint8 *count;
uint32 *encoding;
BITTREE *curTree;

	for (which = 0; which < MAXCODES; which++) {
	BITTREE *scanTree;

		size = sizes[which];
		count = counts[which];
		encoding = encodings[which];

		if ((encodeTrees[which] = (BITTREE *)malloc((uint32)(size * sizeof(*encodeTrees[which])))) == NULL) {
			(void)fprintf(stderr,"Not enough memory for huffman trees\n");
			exit(1);
		};

		curTree = encodeTrees[which];
		scanTree = curTree;

		for (n = 0; n < size; n++, scanTree++) {
			if (n == 0) {
				scanTree->value = LARGE_NEGATIVE;
			} else {
			if (n == size-1) {
				scanTree->value = LARGE_POSITIVE;
			} else {
				scanTree->value = n - size/2;
			};
			};
			scanTree->count = count[n];
			scanTree->code  = bitReverse(encoding[n]);
			scanTree->zero  = NULL;
			scanTree->one   = NULL;
		};

		qsort((char *)curTree,(int32)size,sizeof(*curTree),compare);

		scanTree = curTree;

		for (n = 0; n < size; n++, scanTree++) {
			scanTree->code  = bitReverse(scanTree->code);
		};

		encodeTrees[which] = makeTree(curTree,size,0x1);
	};
}

freeTree(p)
BITTREE *p;
{
    if(p->zero) freeTree(p->zero);
    if(p->one) freeTree(p->one);
    free(p);
}

dumpTree(p, top)
BITTREE *p;
{
    if(top) printf("dumping root ");
    if(p->zero) dumpTree(p->zero, 0);
    if(p->one) dumpTree(p->one, 0);
    //printf("%#x(%#x,%#x)", p, p->zero, p->one);
    if(top) putchar('\n');
}

freeAllTrees() {
    int i;
    
    for(i = 0; i < MAXCODES; i++) {
	if(0 && encodeTrees[i]) freeTree(encodeTrees[i]);
	if(encodeTrees[i]) free(encodeTrees[i]);
    }
}

/* seems like maybe the standard Solaris malloc has a bug in it, since this crashes in freeTree, not dumpTree, indicating corruption during the freeing process.   Actually, looking at the code maybe it's just our bug, since it looks like we free stuff that was never allocated. */
