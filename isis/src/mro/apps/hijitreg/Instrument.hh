/*	HiRISE Instrument

PIRL CVS ID: $Id: Instrument.hh,v 1.1.1.1 2006/10/31 23:18:15 isis3mgr Exp $

Copyright (C) 2003  Arizona Board of Regents on behalf of the Planetary
Image Research Laboratory, Lunar and Planetary Laboratory at the
University of Arizona.

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

#ifndef _HiRISE_Instrument_
#define _HiRISE_Instrument_

#include "Exceptions.hh"

/**	University of Arizona
*/
namespace UA
{
/**	High Resolution Imaging Science Experiment
*/
namespace HiRISE
{

/*=*****************************************************************************
	Instrument
*/
/**	The <i>Instrument</i> defines constants and static functions used to
	characterize the MRO HiRISE instrument.
<p>
@author		Bradford Castalia, UA/PIRL
$Revision: 1.1.1.1 $ 
*/

class Instrument
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;


//!	Total number of CCD array assemblies.
static const unsigned int
	CCDS;

//!	Special CCD number when the actual CCD/CPMM number is unknown.
static const unsigned int
	CCD_UNKNOWN;

/**	The identification names associated with each CCD, indexed by CPMM number.

	Each CCD sensor array has been given a name that includes an
	abbreviation for the {@link #CCD_FILTER_NAMES color filter} it uses
	and its CCD sensor array number. <b>N.B.</b>: The CCD sensor array
	number is not the same as the CPPM number reported by the
	instrument; use the {@link #CCD_BY_CPMM} array to map CPMM numbers
	to CCD sensor array numbers, and the {@link #CPMM_BY_CCD} to map CCD
	sensor array numbers to CPMM numbers.
*/
static const char* const
	CCD_NAMES[];

//!	The filter names associated with each CCD, indexed by CPMM number.
static const char* const
	CCD_FILTER_NAMES[];

/**	CCD sensor array numbers associated with each CCD, indexed by CPMM number.

	The instrument software refers to CCDs by their CPMM number.
	However, due to the way the CCD sensor arrays were wired to the CPPM
	modules the CCD sensor array numbers are not always the same as the
	CPMM number. This array maps CPMM numbers to the corresponding CCD
	sensor array number.

	@see	#CPMM_BY_CCD
*/
static const unsigned int
	CCD_BY_CPMM[];

/**	CPMM numbers associated with each CCD, indexed by CCD sensor array number.

	<b>N.B.</b>: All other arrays in this Instrument class are indexed by
	CPMM number. 

	@see	#CCD_BY_CPMM
*/
static const unsigned int
	CPMM_BY_CCD[];


//!	The units of wavelength measurement for the CCD filters.
static const char* const
	WAVELENGTH_UNITS;

/**	The filter center wavelength for each CCD, indexed by CPMM number.

	Wavelength values are measured in {@link #WAVELENGTH_UNITS}.
*/
static const unsigned int
	CCD_CENTER_WAVELENGTHS[];

/**	The filter wavelength bandwidth for each CCD, indexed by CPMM number.

	Wavelength values are measured in {@link #WAVELENGTH_UNITS}.
*/
static const unsigned int
	CCD_BANDWIDTHS[];


/**	Focal plane X offset in millimeters for each CCD, indexed by CPMM number.

	The X offset of each CCD's  first detector pixel measured in
	millimeters in the HiRISE focal plane assembly relative to CPMM 6
	(CCD sensor array 10) left fiducial.

	@see	#CCD_FOCAL_PLANE_Y_OFFSETS_MM
*/
static const double
	CCD_FOCAL_PLANE_X_OFFSETS_MM[];

/**	Focal plane Y offset in millimeters for each CCD, indexed by CPMM number.

	The Y offset of each CCD's  first detector pixel measured in
	millimeters in the HiRISE focal plane assembly relative to CPMM 6
	(CCD sensor array 10) left fiducial.

	@see	#CCD_FOCAL_PLANE_X_OFFSETS_MM
*/
static const double
	CCD_FOCAL_PLANE_Y_OFFSETS_MM[];

//!	CCD detector pixel size in millimeters.
static const double 
	CCD_PIXEL_SIZE_MM;


/**	The number of data channels for each CCD assembly.

	Each CCD array assembly is composed of a pair of CCD channels.
	Both channels act together as an image scanning unit, but each
	channel has its own distinct imaging sensors and pixel line data
	store. All instrument observation data is organized by channel.
*/
static const unsigned int
	CCD_CHANNELS;

//!	Special channel number when the actual channel number is unknown.
static const unsigned int
	CCD_CHANNEL_UNKNOWN;


/**	The number of image sensors in a single CCD channel line.

	Each CCD device of an instrument channel is an array of sensors
	organized as across-track lines and down-track TDI line stages. Each
	line contains the same number of sensors.
*/
static const unsigned int
	CCD_IMAGE_SENSORS;

//!	The maximum number of bytes per pixel value.
static const unsigned int
	MAX_BYTES_PER_PIXEL;

/**	The maximum valid pixel values.

	Any pixel value above the maximum is invalid. The instrument is
	expected to guarantee that only valid pixel values are produced.
	This array is indexed by the number of bytes per pixel - 1.

	During data downlink operations from the spacecraft lost packets
	gaps are filled with values with all bits set. Pixels with gap
	values are therefore certain to be above the MAX_PIXEL_VALUES for
	any number of bytes per pixel.
*/
static const unsigned int
	MAX_PIXEL_VALUES[];


/**	Image lines upper limit (exclusive).

	Line header bytes 3-5 contain the observation line number as recorded
	by the instrument. The most significant bit of this field is being
	reserved to accommodate an additional "bad line" flag. The instrument
	can not generate enough observation lines to cause this reserved bit
	to be set by a valid line number.
*/
static const unsigned int
	MAX_IMAGE_LINES;


/**	The number of time delay integration (TDI) stages available to each CCD.

	Each CCD device of an instrument channel is an array of sensors
	organized a across-track lines and down-track TDI line stages. Only
	one line has its sensor values sampled and scanned out to the CPMM
	storage for further processing. The sensor values from lines
	preceeding (up-track) the TDI line are cascaded into the following
	line to accumulate down-track sensor values. Since the rate at
	which sensor line values cascade (the scan line time) is set at the
	time the observation starts to coincide with the rate at which the
	image scene moves across the sensor lines, the effect is to build
	image signal strength and improve the signal-to-noise ratio at the
	expense of some image blurring due to timing and alignment
	inaccuracies.

	@see	#TDI_STAGES
*/
static const unsigned int
	TOTAL_TDI_STAGES;

//!	Valid TDI stages values.
static const unsigned int
	TDI_STAGES[];


/**	The number of binning factors available to each CCD.

	After the selected CCD sensor line has its values read out into
	CPMM storage the accumulated observation sensor readings may be
	summed - binned - by pixel groups in both the cross-track and
	down-track directions. Each non-overlapping pixel group is square
	with a size specified by the binning factor. Each square of pixels
	is a patch of the image observation that, as a result of binning,
	produces a single new pixel value.

	@see	#BINNING_FACTORS
*/
static const unsigned int
	TOTAL_BINNING_FACTORS;

//!	Valid binning factor values.
static const unsigned int
	BINNING_FACTORS[];

/**	The number of image pixels per line for each binning factor.

	Normally the number of pixels per line is the number of {@link
	#CCD_IMAGE_SENSORS} divided by the binning factor in use. However,
	for the odd binning factor 3 the number of pixels per line is
	rounded up to a multiple of 4 due to the contrainst of the
	{@link #SSR_BYTE_BOUNDARY}.
*/
static const unsigned int
	CCD_BINNED_PIXELS[];


//!	The number of stored LUTS available.
static const unsigned int
	STORED_LUTS;


//!	The number of stimulator LEDs.
static const unsigned int
	STIMULATOR_LEDS;

//!	The identification names associated with each stim lamp.
static const char* const
	STIMULATOR_LED_NAMES[];


//!	Exposure operation setup time.
static const double
	EXPOSURE_SETUP_MICROS;
	
//!	Engineering_Header Delta_Line_Time maximum valid value.
static const unsigned int
	DELTA_LINE_TIME_MAX;

//!	Engineering_Header Delta_Line_Time nanoseconds per tick.
static const double
	DELTA_LINE_TIME_TICK_NANOS;

//!	Offset for Engineering_Header Delta_Line_Time when calculating line time.
static const double
	LINE_TIME_PRE_OFFSET;

//!	Number of scan lines after the trim lines before the first observation line.
static const unsigned int
	TRIM_ADDITION_LINES;

//!	Number of initial unbinned reverse readout observation lines.
static const unsigned int
	REVERSE_READOUT_LINES;

//!	Number of binned masked lines following the reverse readout lines.
static const unsigned int
	MASKED_LINES;


//!	96 MHz DLL locked telemetry value.
static const unsigned int
	DLL_LOCKED;
//!	96 MHz DLL out of lock telemetry value.
static const unsigned int
	DLL_NOT_LOCKED;


//!	MRO spacecraft clock epoch (year).
static const unsigned int
	SPACECRAFT_EPOCH;

//!	MRO spacecraft clock microseconds per subseconds tick.
static const double
	MRO_CLOCK_SUBTICK_MICROS;

//!	HiRISE clock microseconds per subseconds tick.
static const double
	HIRISE_CLOCK_SUBTICK_MICROS;

//!	MRO NAIF spacecraft clock identifier.
static const int
	SPACECRAFT_NAIF_ID;


//!	Byte boundary (byte count modulus) for the solid state recorder.
static const unsigned int
	SSR_BYTE_BOUNDARY;


//!	Special integer value when the actual value is unknown.
static const int
	        UNKNOWN_NUMBER;

//!	Special integer value when it is not applicable in context.
static const int
	        NOT_APPLICABLE_NUMBER;

//!	Special text value when the actual value is unknown.
static const char
	* const UNKNOWN_STRING;

//!	Special text value when it is not applicable in context.
static const char
	* const NOT_APPLICABLE_STRING;

/*==============================================================================
	Functions
*/
/**	Gets the minimum number of calibration lines.

	The minimum number of calibration lines is:

	REVERSE_READOUT_LINES + ceil ((MASKED_LINES + TDI) / binning)

	Where the number of #REVERSE_READOUT_LINES and #MASKED_LINES are
	Instrument constants, and TDI and Binning are the Engineering Header
	TDI_Stages and Binning_Factor values respectively. The calculated
	real value is rounded up to the next line boundary; i.e. any partial
	line resulting from the division is entirely included as a
	calibration line.

	<b>N.B.</b>: The calculated number of calibration lines is the
	minimum that contain data only suitable for calibration purposes, as
	opposed to observation image lines that contain data for the target
	image acquisition. However, it is possible for additional image
	lines to be designated as calibration lines.

	@param	TDI	The number of time delay integration stages.
	@param	binning	The line binning factor.
	@return	The minimum number of calibration lines.
*/
static unsigned int calibration_lines_minimum
	(unsigned int TDI, unsigned int binning);

/**	Gets the focal plane X offset pixels.

	The X offset in pixels of the first pixel within a CCD sensor array
	is provided for a given CPMM number and binning mode. This is useful
	for determining how each CCD channel is mapped into the focal
	plane. Note: CCD Channel 1 is to the left, and channel 0 is to the
	right, of the image observation data generated by each CCD array
	assembly.

	@param CPMM	The CPMM number for which the offset is to be calculated.
	@param binning	The applicable binning factor.
	@return The pixel offset relative to CPMM 6 (CCD sensor array 10),
		which is at offset 0.
	@throws Out_of_Range If the CPMM index is invalid.
	@throws Invalid_Argument If binning is 0. Any other binning factor is
		accepted.
	@see	#CCD_CHANNELS
	@see	#BINNING_FACTORS
*/
static int focal_plane_x_offset
	(unsigned int CPMM, unsigned int binning = 1) 
	throw (Out_of_Range, Invalid_Argument);

};	//	class Instrument

}   //  namespace HiRISE
}   //  namespace UA
#endif
