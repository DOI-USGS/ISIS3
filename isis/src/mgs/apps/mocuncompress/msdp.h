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

SCCSID @(#)msdp.h	1.1 10/04/99
*/
typedef unsigned char pixel, byte;

#define round(x) ((x) > 0.0 ? (x) + 0.5 : (x) - 0.5)

/*
    MSDP in a (large) nutshell:

Offset  Length  Name    Definition
(Octet) (Octet)
 0       2      SDID    The ID number of the entire image.
 2       2      SDNUM   The subimage number of this datagram.
 4       2      SDOFF   The offset downtrack of this datagram.
 6       2      SDLINE  The length downtrack of this datagram.
 8       5      SDTIME  The timestamp of the start of the entire image.
13       1      SDSTAT  Some of this datagram's status.
14      17      SDCMD   The command that caused the entire image.
31       5      SDCTXT  The context image parameters.
36       2      SDGO    The camera gain and offset at the start of the entire
                        image.
38       2      SDGONM  The number of additional gain and offset values in
                        SDDAT.
40       2      SDDOWN  The number of lines downtrack in the entire image.
42       2      SDEDIT  The crosstrack editing performed.
44       8      SDCOMP  The compression table entry used for the entire image.
52       2      SDSENS  The sensor values associate with the entire image.
54       4      SDOTHER The clocking rate of the camera CCD and dark reference
                        pixel flag at the start of the entire image.
58       4      SDLEN   The number of octets in SDDAT part of this datagram.
62      SDLEN   SDDAT   The data portion of this datagram.
62+SDLEN 1      SDCS    The checksum redundancy of this datagram.
*/

struct msdp_header
{
    byte id[2];
    byte fragment[2];
    byte down_offset[2];
    byte down_length[2];
    byte time[5];
    byte status;
    byte cmd[17];
    byte context[5];
    byte gain;
    byte offset;
    byte gain_count[2];
    byte down_total[2];
    byte edit_start;
    byte edit_length; /* units of 16 pixels */
    byte compression[8];
    byte sensors[2];
    byte other[4];
    byte len[4];
};

#define MAKESHORT(p) ((p)[0] | ((p)[1] << 8))
#define MAKE24BIT(p) ((p)[0] | ((p)[1] << 8) | ((p)[2] << 16))
#define MAKELONG(p) ((p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24))

#define BYTE0(i) ((i)&0xff)
#define BYTE1(i) (((i)&0xff00)>>8)
#define BYTE2(i) (((i)&0xff0000)>>16)
#define BYTE3(i) (((i)&0xff000000)>>24)

#define STUFFSHORT(p,v) ((p)[0] = (v)&0xff, (p)[1] = (v)>>8)
