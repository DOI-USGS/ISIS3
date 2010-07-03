/*	HiRISE Instrument

PIRL CVS ID: $Id: Instrument.cpp,v 1.2 2009/02/23 16:36:10 slambright Exp $

Copyright (C) 2003-2006  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
*/
#include	"Instrument.hh"

#include	<cmath>
#include	<iostream>
#include	<iomanip>
using std::endl;

#include	<sstream>
using std::ostringstream;


namespace UA
{
namespace HiRISE
{
/*=*****************************************************************************
	Instrument
*/
/*==============================================================================
	Constants:
*/
const char* const
	Instrument::ID = "UA::HiRISE::Instrument ($Revision: 1.2 $ $Date: 2009/02/23 16:36:10 $)";

const unsigned int
	Instrument::CCDS							= 14;

const unsigned int
	Instrument::CCD_UNKNOWN						= 99;

const char* const
	Instrument::CCD_NAMES[] =
		{
		"RED0",
		"RED1",
		"RED2",
		"RED3",
		"BG12",
		"RED4",
		"IR10",
		"IR11",
		"RED5",
		"BG13",
		"RED6",
		"RED7",
		"RED8",
		"RED9"
		};
const char* const
	Instrument::CCD_FILTER_NAMES[] =
		{
		"RED",
		"RED",
		"RED",
		"RED",
		"BLUE-GREEN",
		"RED",
		"NEAR-INFRARED",
		"NEAR-INFRARED",
		"RED",
		"BLUE-GREEN",
		"RED",
		"RED",
		"RED",
		"RED"
		};

const unsigned int
	Instrument::CCD_BY_CPMM[] = 
		{
		 0,
		 1,
		 2,
		 3,
		12,
		 4,
		10,
		11,
		 5,
		13,
		 6,
		 7,
		 8,
		 9
		};

const unsigned int
	Instrument::CPMM_BY_CCD[] =
		{
		 0,
		 1,
		 2,
		 3,
		 5,
		 8,
		10,
		11,
		12,
		13,
		 6,
		 7,
		 4,
		 9
		};


const char
	* const Instrument::WAVELENGTH_UNITS		= "NANOMETERS";

const unsigned int
	Instrument::CCD_CENTER_WAVELENGTHS[] =
		{
		700,
		700,
		700,
		700,
		500,
		700,
		900,
		900,
		700,
		500,
		700,
		700,
		700,
		700
		};

const unsigned int
	Instrument::CCD_BANDWIDTHS[] =
		{
		300,
		300,
		300,
		300,
		200,
		300,
		200,
		200,
		300,
		200,
		300,
		300,
		300,
		300
		};


const double
	Instrument::CCD_FOCAL_PLANE_X_OFFSETS_MM[] = 
		{
    	-96.0000,
    	-71.9985,
    	-48.0499,
    	-24.0400,
          0.0000,
          0.0000,
          0.0000,
    	 24.0000,
    	 24.0000,
    	 24.0000,
    	 48.0000,
    	 72.0045,
    	 96.0014,
    	120.0025
		};

const double
	Instrument::CCD_FOCAL_PLANE_Y_OFFSETS_MM[] =
		{
    	-14.6257,
    	-21.5130,
    	-14.0498,
    	-21.4869,
    	-28.7552,
    	-14.4594,
          0.0000,
    	 -7.2767,
    	-21.4662,
    	-36.0247,
    	-14.2603,
    	-21.4359,
    	-14.5152,
    	-21.8000
		};

const double
  Instrument::CCD_PIXEL_SIZE_MM					= 0.012;


const unsigned int
	Instrument::CCD_CHANNELS					= 2;

const unsigned int
	Instrument::CCD_CHANNEL_UNKNOWN				= 9;


const unsigned int
	Instrument::CCD_IMAGE_SENSORS				= 1024;

const unsigned int
	Instrument::MAX_BYTES_PER_PIXEL				= 2;

const unsigned int
	Instrument::MAX_PIXEL_VALUES[MAX_BYTES_PER_PIXEL] =
		{
		254,
		0x3FFF	//	14 bits.
		};


/*	The maximum number of image lines: CPMM RAM limited.

    Subject: Re: Maximum HiRISE image lines
       Date: Mon, 21 Feb 2005 11:53:20 -0800
       From: Nathan Bridges <Nathan.T.Bridges@jpl.nasa.gov>
         To: hisys@pirl.lpl.Arizona.EDU

HiSYS,

My understanding is that LUTing occurs after putting data on the CPMMs,
such that we can only use 16-bits to compute the maximum number of
lines.  Can someone confirm if this correct or not?  If so,  the maximum
number of pre-binned lines (L) is

L = (2x1024^3)*B/(16[2048/B+56])

where the CPMM data volume is 2x1024^3 bits and B is the bin factor. 
This equation takes into account that the buffer and dark pixels are not
serially binned.  The number of post-binned lines (L') is simply the
above equation divided by the bin factor:

L' = (2x1024^3)/(16[2048/B+56])

the result is (I rounded down all numbers):

Bin     Maximum # Pre-Binned Lines      Maximum # Post-Binned Lines
1                           63,791                           63,791
2                          248,551                          124,275
3                          545,108                          181,702
4                          945,195                          236,298
8                        3,441,480                          430,185
16                      11,671,106                          729,444

The attached spreadsheet has the computations

I guess the actual numbers are even slightly smaller because of line
headers and maybe other extra things I'm  not including.  However, using
the above as a maximum should work.

                                Nathan
*/
/*	The upper limit (exclusive) of the number of image lines based on the
	available number of line header line number field bits.
*/
const unsigned int
	Instrument::MAX_IMAGE_LINES					= (1 << 23);


const unsigned int
	Instrument::TOTAL_TDI_STAGES				= 4;

const unsigned int
	Instrument::TDI_STAGES[TOTAL_TDI_STAGES] =
		{
		8,
		32,
		64,
		128
		};


const unsigned int
	Instrument::TOTAL_BINNING_FACTORS			= 6;

const unsigned int
	Instrument::BINNING_FACTORS[TOTAL_BINNING_FACTORS] =
		{
		1,
		2,
		3,
		4,
		8,
		16
		};

const unsigned int
	Instrument::CCD_BINNED_PIXELS[TOTAL_BINNING_FACTORS] =
		{
		2048,
		1024,
		684,
		512,
		256,
		128
		};


const unsigned int
	Instrument::STORED_LUTS						= 28;


const unsigned int
	Instrument::STIMULATOR_LEDS					= 3;

const char* const
	Instrument::STIMULATOR_LED_NAMES[] =
		{
		"RED",
		"BLUE-GREEN",
		"NEAR-INFRARED"
		};


const double
	Instrument::EXPOSURE_SETUP_MICROS			= 99.48;

const unsigned int
	Instrument::DELTA_LINE_TIME_MAX				= 4194303;

const double
	Instrument::DELTA_LINE_TIME_TICK_NANOS		= 62.5;

const double
	Instrument::LINE_TIME_PRE_OFFSET			= 74.0;

const unsigned int
	Instrument::TRIM_ADDITION_LINES				= 180,
	Instrument::REVERSE_READOUT_LINES			= 20,
	Instrument::MASKED_LINES					= 20;


const unsigned int
	Instrument::DLL_LOCKED						= 0x11,
	Instrument::DLL_NOT_LOCKED					= 0x5A;


#ifndef MRO_EPOCH
#define MRO_EPOCH		1980
#endif
const unsigned int
	Instrument::SPACECRAFT_EPOCH				= MRO_EPOCH;

const double
	Instrument::MRO_CLOCK_SUBTICK_MICROS		= (1000000.0 / (1 << 16)),
	Instrument::HIRISE_CLOCK_SUBTICK_MICROS		= 16.0;

const int
	Instrument::SPACECRAFT_NAIF_ID				= -74;


const unsigned int
	Instrument::SSR_BYTE_BOUNDARY				= 4;


const int
	        Instrument::UNKNOWN_NUMBER			= -9999,
	        Instrument::NOT_APPLICABLE_NUMBER	= -9998;
const char
	* const Instrument::UNKNOWN_STRING			= "UNKNOWN";
const char
	* const Instrument::NOT_APPLICABLE_STRING	= "N/A";

/*==============================================================================
	Functions
*/
unsigned int
Instrument::calibration_lines_minimum
	(
	unsigned int	TDI,
	unsigned int	binning
	)
{
if (binning == 0)
	binning = 1;
return
	REVERSE_READOUT_LINES
	+ static_cast<unsigned int>(ceil (static_cast<double>(MASKED_LINES + TDI)
		/ static_cast<double>(binning)));
}


int
Instrument::focal_plane_x_offset
	(
	unsigned int	CPMM,
	unsigned int	binning
	) 
	throw (Out_of_Range, Invalid_Argument)
{
if ((CPMM < 0) || (CPMM >= CCDS))
	{
	ostringstream
		message;
    message << "Unable to determine the focal plane offset" << endl
			<< "for CPMM " << CPMM
				<< " with binning factor " << binning << '.' << endl
			<< "The CPMM number is invalid.";
	throw Out_of_Range (message.str(), ID);
	}

if (binning == 0)
	{
	ostringstream
		message;
    message << "Unable to determine the focal plane offset" << endl
			<< "for CPMM " << CPMM
				<< " with binning factor " << binning << '.' << endl
			<< "The binning factor must not be zero.";
	throw  Invalid_Argument (message.str(), ID);
	}

return
	(int)rint (CCD_FOCAL_PLANE_X_OFFSETS_MM[CPMM]
		/ (CCD_PIXEL_SIZE_MM * binning));
}

}   //  namespace HiRISE
}   //  namespace UA
