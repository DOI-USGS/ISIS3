#ifndef OverlapStatistics_h
#define OverlapStatistics_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2009/06/24 22:03:01 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.                                                           
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,              
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */ 

 #include "Cube.h"
 #include "Filename.h"
 #include "Projection.h"
 #include "MultivariateStatistics.h"

namespace Isis {
/**                                                                       
 * @brief Calculates statistics in the area of overlap between two projected cubes                
 *                                                                        
 * This class finds the overlap between two cubes.  It allows the user to check
 * whether or not two cubes overlap, and also creates a MultivariateStatistics 
 * object containing the data from each cube in the overlapping area.  The cubes
 * entered into the constructor for this class must both be projections, and 
 * must have the same projection parameters.  
 * 
 * If you would like to see OverlapStatistics                             
 * being used in implementation, see equalizer.cpp                                     
 *                                                                        
 * @ingroup Statistics                                                  
 *                                                                        
 * @author 2005-07-18 Elizabeth Ribelin                                                                   
 *                                                                        
 * @internal                           
 *  @history 2005-11-18 Elizabeth Miller - added 1e-9 to the min and max values
 *                         when computing the ranges to fix rounding issue  
 *  @history 2006-01-12 Elizabeth Miller - removed unwanted print statements
 *  @history 2006-03-31 Elizabeth Miller - added unitTest
 *  @history 2006-04-03 Elizabeth Miller - added .001 to the min and max values
 *                         when computing the ranges to re-fix rounding issue
 *  @history 2007-08-27 Steven Koechle - removed space from standard deviation
 *                         keyword
 *  @history 2008-06-18 Steven Koechle - fixed Documentation Errors
 *  @history 2009-03-12 Travis Addair - added tracking for
 *           percent processed
 *  @history 2009-06-24 Travis Addair - optimized statistic gathering, changed 
 *           PVL print-out for readability, and added functionality to allow the
 *           user to specify a "sampling percent" when gathering statistics to 
 *           save processing time
 *  
 */ 

  class OverlapStatistics {
    public:
      OverlapStatistics(Isis::Cube &x, Isis::Cube &y, 
        std::string progressMsg="Gathering Overlap Statistics", 
        double sampPercent=100.0);

     /**
      * Checks the specified band for an overlap
      * 
      * @param band The band number of the cubes to be checked for an overlap
      * 
      * @returns bool Returns true if the cubes overlap in the specified band, 
      *               and false if they do not overlap
      */
      bool HasOverlap(int band) { return (p_stats[band-1].ValidPixels()>0); };
      bool HasOverlap();

     /**
      * Returns the filename of the first cube
      * 
      * @return string The name of the first cube
      */
      Isis::Filename FilenameX() { return p_xFile; };

     /**
      * Returns the filename of the second cube
      * 
      * @return string The name of the second cube
      */
      Isis::Filename FilenameY() { return p_yFile; };

     /**
      * Returns the MultivariateStatistics object containing all the data from 
      * both cubes in the overlapping area
      * 
      * @param band The band number the MultivariateStatistics object needs to 
      *             contain data from
      * 
      * @return MultivariateStatistics The MultivariateStatistics object 
      *         containing all data from both cubes in the overlapping area from 
      *         the specified band
      */
      Isis::MultivariateStatistics GetMStats(int band) { 
        return p_stats[band-1]; 
      };

     /**
      * Returns the number of lines in the overlapping area
      * 
      * @return int The number of lines in the overlapping area
      */
      int Lines() { return p_lineRange; };

     /**
      * Returns the number of samples in the overlapping area
      * 
      * @return int The number of samples in the overlapping area
      */
      int Samples() { return p_sampRange; };

     /**
      * Returns the number of bands both cubes have
      * 
      * @return int The number of bands both cubes have
      */
      int Bands() { return p_bands; }; 
    
     /**
      * Returns the percentage of cube lines sampled
      * 
      * @return int The percentage of lines sampled
      */
      double SampPercent() { return p_sampPercent; }; 

     /**
      * Returns the starting sample position of the overlap in the first cube
      * 
      * @return int The starting sample of the overlap in the first cube
      */
      int StartSampleX() { return p_minSampX; };

     /**
      * Returns the ending sample position of the overlap in the first cube
      * 
      * @return int The ending sample of the overlap in the first cube
      */
      int EndSampleX() { return p_maxSampX; };

     /**
      * Returns the starting line position of the overlap in the first cube
      * 
      * @return int The starting line of the overlap in the first cube
      */
      int StartLineX() { return p_minLineX; };

     /**
      * Returns the ending line position of the overlap in the first cube
      * 
      * @return int The ending line of the overlap in the first cube
      */
      int EndLineX() { return p_maxLineX; };

     /**
      * Returns the starting sample position of the overlap in the second cube
      * 
      * @return int The starting sample of the overlap in the second cube
      */
      int StartSampleY() { return p_minSampY; };

     /**
      * Returns the ending sample position of the overlap in the second cube
      * 
      * @return int The ending sample of the overlap in the second cube
      */
      int EndSampleY() { return p_maxSampY; };

     /**
      * Returns the starting line position of the overlap in the second cube
      * 
      * @return int The starting line of the overlap in the second cube
      */
      int StartLineY() { return p_minLineY; };

     /**
      * Returns the ending line position of the overlap in the second cube
      * 
      * @return int The ending line of the overlap in the second cube
      */
      int EndLineY() { return p_maxLineY; };
    
     /**
      * Sets the minimum number of valid pixels for the overlap to be
      * considered valid for PVL output
      * 
      * @param mincnt The minimum valid pixel value to set
      */
      void SetMincount(unsigned int mincnt) { p_mincnt = mincnt; };
    
     /**
      * Returns whether the overlap meets the minimum valid pixel requirement
      * 
      * @param band The band to check
      * 
      * @return bool Is minimum requirement met
      */
      bool IsValid(unsigned int band) { return GetMStats(band).ValidPixels()>p_mincnt; };


    private:
      int p_bands;               //!<Number of bands
      double p_sampPercent;      //!<Percentage of lines sampled
      Isis::Filename p_xFile;       //!<Filename of X cube
      Isis::Filename p_yFile;       //!<Filename of Y cube                        
      int p_sampRange;           //!<Sample range of overlap
      int p_lineRange;           //!<Line range of overlap
      int p_minSampX;            //!<Starting Sample of overlap in X cube
      int p_maxSampX;            //!<Ending Sample of overlap in X cube
      int p_minSampY;            //!<Starting Sample of overlap in Y cube
      int p_maxSampY;            //!<Ending Sample of overlap in Y cube
      int p_minLineX;            //!<Starting Line of overlap in X cube
      int p_maxLineX;            //!<Ending Line of overlap in X cube
      int p_minLineY;            //!<Starting Line of overlap in Y cube
      int p_maxLineY;            //!<Ending Line of overlap in Y cube
      int p_mincnt;              //!<Minimum valid pixels to be valid overlap

      //!Multivariate Stats object for overlap data from both cubes 
      std::vector<Isis::MultivariateStatistics> p_stats;                                                
  };
  std::ostream& operator<<(std::ostream &os, Isis::OverlapStatistics &stats);
};

#endif
