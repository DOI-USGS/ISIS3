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
static char *sccsid = "@(#)getdecode.c	1.1 10/04/99";
 
/*
    Huffman code tree module
    Mike Caplinger, MOC GDS Design Scientist
    SCCS @(#)getdecode.c	1.1 11/19/91

    This module manages the Ligocki-style Huffman decoding trees for
    the predictive decompressor.  It is a little roundabout in that
    it builds a Huffman code tree in node form from the flight software
    encoding tables and then "tablefies" it; that way, separate decoding
    tables don't have to be maintained.  One can also just load an
    existing decode file in (for testing.)

    This will probably all go away if the clean canonical decompressor
    is ever written.
*/

#include <stdlib.h>
#include <stdio.h>

#include "fs.h"

#define DEFINE_CODE_TABLES
#include "fs.h"
#include "predcode.h"

typedef struct ht_node {
    int value;
    struct ht_node *zero, *one;
} Huffman_node;

/* in TJL terminology, left is 0 and right is 1. */
extern uint8 code[], left[], right[];

decodeLoad(decodefile)
char *decodefile;
{
    int decodeSize;
    FILE *fd;
    
    if ((fd = fopen(decodefile,"r")) == NULL) {
	(void)fprintf(stderr,"Unable to open '%s' for reading\n",
		      decodefile);
	exit(1);
    }
    
    if (fread((char *)(&decodeSize),sizeof(decodeSize),1,fd) != 1) {
	fprintf(stderr,"Unable to read decode file, '%s'\n",decodefile);
	exit(1);
    }
    
    if (fread(code,sizeof(code[0]),decodeSize,fd) != decodeSize) {
	fprintf(stderr,"Unable to read decode file, '%s'\n",decodefile);
	exit(1);
    }
    
    if (fread(left,sizeof(left[0]),decodeSize,fd) != decodeSize) {
	fprintf(stderr,"Unable to read decode file, '%s'\n",decodefile);
	exit(1);
    }
    
    if (fread(right,sizeof(right[0]),decodeSize,fd) != decodeSize) {
	fprintf(stderr,"Unable to read decode file, '%s'\n",decodefile);
	exit(1);
    }
    
    (void)fclose(fd);
}

Huffman_node *ht_insert(root, value, code, len)
Huffman_node *root;
int value, code, len;
{
    int bit;
    Huffman_node **branch;

    if(!root) {
	root = (Huffman_node *) malloc(sizeof(Huffman_node));
	root->value = 0;
	root->zero = root->one = NULL;
    }
    
    if(len == 0) {
	root->value = value;
    }
    else {
	bit = code&0x1;
	if(bit == 0) branch = &root->zero;
	else branch = &root->one;

	if(*branch == 0) {
	    *branch = (Huffman_node *) malloc(sizeof(Huffman_node));
	    (*branch)->value = 0;
	    (*branch)->zero = 0;
	    (*branch)->one = 0;
	}
	ht_insert(*branch, value, code>>1, len-1);
    }
    return root;
}

int ht_lookup(root, code, len)
Huffman_node *root;
int code, len;
{
    int bit;

    if(root->zero == 0 && root->one == 0) return root->value;
    bit = code&1;
    if(bit == 0) return ht_lookup(root->zero, code>>1, len-1);
    else return ht_lookup(root->one, code>>1, len-1);
}

ht_dump(root, code, len)
Huffman_node *root;
int code, len;
{
    if(root->zero == 0 && root->one == 0) {
	printf("%d %x(%d)\n", root->value, code, len);
    }
    else {
	if(root->zero) {
	    ht_dump(root->zero, code, len+1);
	}
	if(root->one) {
	    ht_dump(root->one, code|(1<<len), len+1);
	}
    }
}

#define ZERO (1<<0)
#define ONE (1<<1)

/*
    Convert a Huffman tree to TJL table form. Call initially
    with index = 0.
*/
ht_tablefy(root, flags, zero, one, index)
Huffman_node *root;
unsigned char *flags, *zero, *one;
{
    int local_index = index;
    int i;
    
    if(root->zero) {
	if(root->zero->zero == 0 && root->zero->one == 0) {
	    flags[index] &= ~ZERO;
	    zero[index] = root->zero->value;
	}
	else {
	    i = ZERO;
	    flags[local_index] |= ZERO;
	    index += 1;
	    zero[local_index] = index;
	    index = ht_tablefy(root->zero, flags, zero, one, index);
	}
    }
    if(root->one) {
	if(root->one->zero == 0 && root->one->one == 0) {
	    flags[local_index] &= ~ONE;
	    one[local_index] = root->one->value;
	}
	else {
	    flags[local_index] |= ONE;
	    index += 1;
	    one[local_index] = index;
	    index = ht_tablefy(root->one, flags, zero, one, index);
	}
    }
    return index;
}

Huffman_node *ht_tree_gen(i)
int i;
{
    Huffman_node *tree = 0;
    uint16 *code;
    uint8 *len;
    uint8 *requant;

    code = CodeBitsVec[i];
    len = CodeLenVec[i];
    requant = CodeRequantVec[i];
    
    tree = ht_insert(tree, requant[0], code[0], len[0]);
    
    for(i = 1; i < 128; i++) {
	if(requant[i] != requant[i-1])
	  tree = ht_insert(tree, requant[i], code[i], len[i]);
    }

    tree = ht_insert(tree, requant[255], code[255], len[255]);
    
    for(i = 254; i >= 128; i--) {
	if(requant[i] != requant[i+1])
	  tree = ht_insert(tree, requant[i], code[i], len[i]);
    }
    return tree;
}

ht_free(root)
Huffman_node *root;
{
    if(root->zero) ht_free(root->zero);
    if(root->one) ht_free(root->one);
    free(root);
}

decodeInit(n) {
    Huffman_node *tree = 0;
    int i;
    uint8 flags[256], zero[256], one[256];

    tree = ht_tree_gen(n);
    /* i is the # of slots actually used... */
    i = ht_tablefy(tree, code, left, right, 0) + 1;
    ht_free(tree);
}
