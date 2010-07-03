/*	StaticStats

PIRL CVS ID: $Id: staticStats.h,v 1.1 2007/01/11 20:59:17 kbecker Exp $

Copyright (C) 2005, 2006  Arizona Board of Regents on behalf of the
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

#ifndef staticStats_h
#define staticStats_h

namespace PIRL
{
/** A <i>StaticStats</i> instance allows the user to set statistics for a cube

  This class exists to extend the functionality of the ISIS stats class, which 
  can construct a Stats object, but can only gather statistics from a cube
  as opposed to an input pvl or table, both of which are features of the
  StaticStats class.
  
  The use of this class is considered somewhat limited by the author. It cannot
  be emphasized enough that the contents of a cub ARE NOT CHANGED via the 
  mutators of this class. Rather, the class allows the user to change PERCEIVED
  VALUES for the class. 
  
  This class can only be used to impose artificial statistics. For example,
  the class was created so that a user might gather statistics using the normal
  statistics class, write those statistics in a table, and then use those table
  values to create a StaticStats instance so that statistics from the cube would
  not have to be gathered over and over again. 
  
  @author		Drew Davidson, UA/PIRL

  $Revision: 1.1 $
*/
class StaticStats{
public:

/*==============================================================================
	Constants
==============================================================================*/
//!	Class identification name with source code version and date.
static const char* const
	ID;
	
/*==============================================================================
	Constructors
==============================================================================*/
   /**	Constructs a default StaticStats.

	The staticStats has no statistics, they should be set via the mutator
	methods below
  */  
  StaticStats(){};
  
  /**	Constructs a StaticStats with given statistics.

	The staticStats is given an initial set of statistics.
	
	@param	initialMean 
	  The initial mean DN for pixels in the cub
	
	@param	initialStandardDeviation 
	  The initial DN standard deviation for pixels in the cub
	
	@param	initialValidPixels 
	  The initial number of valid pixels in the cub
	
	@param	initialMinimum
	  The initial minimum DN value in the cub
	
	@param	initialMaximum
	  The initial maximum DN value in the cub
  */
  StaticStats(double initialMean, 
              double initialStandardDeviation, 
				     int initialValidPixels, 
				  double initialMinimum, 
				  double initialMaximum);
  
/*==============================================================================
	Accessors
==============================================================================*/
  /**	Gets the perceived mean pixel DN value.

	@return	DN mean for the cub
  */
  double Average();
  
  /**	Gets the perceived standard deviation for all pixel DN values.

	@return	DN standard deviaton for the cub
  */
  double StandardDeviation();

  /**	Gets the perceived number of valid pixels.

	@return	Number of valid pixels in the cub
  */
  int ValidPixels();  

  /**	Gets the perceived minimum DN value.

	@return	Lowest pixel DN in the cub
  */
  double Minimum();

  /**	Gets the perceived maximum DN value.

	@return	Highest pixel DN in the cub
  */
  double Maximum();

/*==============================================================================
	Mutators
==============================================================================*/
  /**	Sets the perceived mean DN value.

	@param	newMean New mean DN value
  */
  void setMean(double newMean);
  
  /**	Sets the perceived standard deviation of DN values in the cub.

	@param	newStandardDeviation New standard deviation for the cub
  */
  void setStandardDeviation(double newStandardDeviation);

  /**	Sets the perceived count of valid pixels.

	@param	newValidPixels New count of valid pixels in the cub
  */
  void setValidPixels(int newValidPixelCount);

  /**	Sets the perceived maximum DN in the cub

	@param	newMaximum New maximum DN value
  */
  void setMaximum(double newMaximum);
  
  /**	Sets the perceived minimum DN in the cub

	@param	newMinimum New minimum DN value
  */
  void setMinimum(double newMinimum);
    
private:
  //!	Perceived mean DN
  double myMean;

  //!	Perceived DN standard deviation
  double myStandardDeviation;

  //!	Perceived minimum DN
  double myMinimum; 

  //!	Perceived maximum DN
  double myMaximum;

  //!	Perceived valid pixel count 
  int myValidPixels;
};
}/* End namespace PIRL */
#endif
