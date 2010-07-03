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
static char *sccsid = "@(#)array.c	1.1 10/04/99";
 
/*
    Auto-expanding array package
    Mike Caplinger, MOC GDS Design Scientist
    SCCS @(#)array.c	1.2 10/18/91

    Implements a simple auto-expanding array data structure.  Include
    "array.h" to use.
*/

#include <stdlib.h>
#include <memory.h>
#include "array.h"

/*
    Create a new array with an initial allocation of len (0 will work if
    you can't make a better guess) and return it.
*/
Array *array_new(len)
int len;
{
    Array *new = (Array *) malloc(sizeof(Array));

    if(new == 0) return 0;
    
    new->len = 0;
    new->free = len;
    new->data = (char *) malloc(len);

    if(new->data == 0) return 0;

    return new;
}

/*
    Append len bytes of data at the end of the array.  Returns 0 if the
    operation fails, otherwise a positive number.
*/
int array_append(a, data, len)
Array *a;
char *data;
int len;
{
    if(a->free < len) {
	/* expand array */
	a->data = (char *) realloc(a->data, len+(a->len+a->free)*2);
	if(a->data == 0) {
	    return 0;
	}
	a->free += len+(a->len+a->free);
    }

    bcopy(data, a->data+a->len, len);
    a->free -= len;
    a->len += len;
    return len;
}

/*
    Delete an entry from the array and shove the remaining contents,
    if any, down.  Returns 0 if the operation fails, otherwise a
    positive number.
*/
int array_slot_delete(a, slot, len)
Array *a;
int slot, len;
{
    if(a->len < (slot+1)*len) return 0;
    bcopy(&a->data[(slot+1)*len], &a->data[(slot)*len],
	  (a->len/len-slot-1)*len);
    a->free += len;
    a->len -= len;
    return 1;
}

/*
    Returns the current size of the array in bytes.
*/
int array_len(a)
Array *a;
{
    return a->len;
}

/*
    Returns the data contained in the array.  This is a contiguous vector
    of bytes guaranteed to be array_len(a) in size.
*/
char *array_data(a)
Array *a;
{
    return a->data;
}

/*
    Deallocate an array and its data.
*/
void array_free(a)
Array *a;
{
    free(a->data);
    free(a);
}

#ifdef TEST
main() {
    Array *t = array_new(0);

    array_append(t, "abcdef", 7);
    array_slot_delete(t, 0, 1);
    printf("%s\n", array_data(t));
    array_slot_delete(t, 2, 1);
    printf("%s\n", array_data(t));
    array_slot_delete(t, 3, 1);
    printf("%s\n", array_data(t));
    array_slot_delete(t, 3, 1);
    array_append(t, "1234", 5);
    printf("%s\n", array_data(t));
}
#endif
