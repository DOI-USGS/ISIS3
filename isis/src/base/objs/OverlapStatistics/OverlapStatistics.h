#ifndef OverlapStatistics_h
#define OverlapStatistics_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Cube.h"
#include "FileName.h"
#include "MultivariateStatistics.h"
#include "Projection.h"

namespace Isis {
  class PvlObject;
  
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
   *  @history 2009-03-12 Travis Addair - added tracking for percent processed
   *  @history 2009-06-24 Travis Addair - optimized statistic gathering, changed
   *                          PVL print-out for readability, and added
   *                          functionality to allow the user to specify a
   *                          "sampling percent" when gathering statistics to save
   *                          processing time
   *  @history 2012-04-16 Jeannie Backer - Added forward declaration for
   *                          PvlObject and ordered #includes in the
   *                          implementation file. Added documentation.
   *  @history 2016-07-15 Ian Humphrey - Modified toPvl() method to get the internal multivariate
   *                          statistics. Added init() method to initialize primitive members during
   *                          construction. Added new constructor that creates an OverlapStatistics
   *                          object from a PvlObject. Added private fromPvl() method to implement
   *                          these details. Updated unitTest to test these changes. References 
   *                          #2282.
   *
   */

  class OverlapStatistics {
    public:
      OverlapStatistics(Isis::Cube &x, Isis::Cube &y,
                        QString progressMsg = "Gathering Overlap Statistics",
                        double sampPercent = 100.0);
      OverlapStatistics(const PvlObject &inStats);

      /**
       * Checks the specified band for an overlap
       *
       * @param band The band number of the cubes to be checked for an overlap
       *
       * @return bool Returns true if the cubes overlap in the specified band,
       *               and false if they do not overlap
       */
      bool HasOverlap(int band) const {
        return (p_stats[band-1].ValidPixels() > 0);
      };

      bool HasOverlap() const;

      /**
       * Returns the filename of the first cube
       *
       * @return QString The name of the first cube
       */
      Isis::FileName FileNameX() const {
        return p_xFile;
      };

      /**
       * Returns the filename of the second cube
       *
       * @return QString The name of the second cube
       */
      Isis::FileName FileNameY() const {
        return p_yFile;
      };

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
      Isis::MultivariateStatistics GetMStats(int band) const {
        return p_stats[band-1];
      };

      /**
       * Returns the number of lines in the overlapping area
       *
       * @return int The number of lines in the overlapping area
       */
      int Lines() const {
        return p_lineRange;
      };

      /**
       * Returns the number of samples in the overlapping area
       *
       * @return int The number of samples in the overlapping area
       */
      int Samples() const {
        return p_sampRange;
      };

      /**
       * Returns the number of bands both cubes have
       *
       * @return int The number of bands both cubes have
       */
      int Bands() const {
        return p_bands;
      };

      /**
       * Returns the percentage of cube lines sampled
       *
       * @return int The percentage of lines sampled
       */
      double SampPercent() const {
        return p_sampPercent;
      };

      /**
       * Returns the starting sample position of the overlap in the first cube
       *
       * @return int The starting sample of the overlap in the first cube
       */
      int StartSampleX() const {
        return p_minSampX;
      };

      /**
       * Returns the ending sample position of the overlap in the first cube
       *
       * @return int The ending sample of the overlap in the first cube
       */
      int EndSampleX() const {
        return p_maxSampX;
      };

      /**
       * Returns the starting line position of the overlap in the first cube
       *
       * @return int The starting line of the overlap in the first cube
       */
      int StartLineX() const {
        return p_minLineX;
      };

      /**
       * Returns the ending line position of the overlap in the first cube
       *
       * @return int The ending line of the overlap in the first cube
       */
      int EndLineX() const {
        return p_maxLineX;
      };

      /**
       * Returns the starting sample position of the overlap in the second cube
       *
       * @return int The starting sample of the overlap in the second cube
       */
      int StartSampleY() const {
        return p_minSampY;
      };

      /**
       * Returns the ending sample position of the overlap in the second cube
       *
       * @return int The ending sample of the overlap in the second cube
       */
      int EndSampleY() const {
        return p_maxSampY;
      };

      /**
       * Returns the starting line position of the overlap in the second cube
       *
       * @return int The starting line of the overlap in the second cube
       */
      int StartLineY() const {
        return p_minLineY;
      };

      /**
       * Returns the ending line position of the overlap in the second cube
       *
       * @return int The ending line of the overlap in the second cube
       */
      int EndLineY() const {
        return p_maxLineY;
      };

      /**
       * Sets the minimum number of valid pixels for the overlap to be
       * considered valid for PVL output
       *
       * @param mincnt The minimum valid pixel value to set
       */
      void SetMincount(unsigned int mincnt) {
        p_mincnt = mincnt;
      };

      int MinCount() const {
        return p_mincnt;
      }

      /**
       * Returns whether the overlap meets the minimum valid pixel requirement
       *
       * @param band The band to check
       *
       * @return bool Is minimum requirement met
       */
      bool IsValid(unsigned int band) const {
        return GetMStats(band).ValidPixels() > p_mincnt;
      };

      /**
       * Creates a Pvl containing the following Overlap Statistics information
       * File1
       * File2
       * Width
       * Height
       * Bands
       * SamplingPercent
       * MinCount
       * MutlivariateStatisticsN (N = current band)
       *   Covariance
       *   Correlation
       *   SumXY
       *   ValidPixels
       *   InvalidPixels
       *   TotalPixels
       *   LinearRegression
       *   ValidOverlap
       *   XStatistics
       *     #FileX Statistics information
       *   YStatistics
       *     #FileY Statistics information
       * File1
       *   StartSample
       *   EndSample
       *   StartLine
       *   EndLine
       *   Average
       *   StandardDeviation
       *   Variance
       * File2
       *   StartSample
       *   EndSample
       *   StartLine
       *   EndLine
       *   Average
       *   StandardDeviation
       *   Variance
       *
       * @return PvlObject PvlObject containing the information for the Overlap
       *         Statistics.
       */

      PvlObject toPvl(QString name = "OverlapStatistics") const;


    private:

      void fromPvl(const PvlObject &inStats);
      void init();

      int p_bands;               //!< Number of bands
      double p_sampPercent;      //!< Percentage of lines sampled
      Isis::FileName p_xFile;    //!< FileName of X cube
      Isis::FileName p_yFile;    //!< FileName of Y cube
      int p_sampRange;           //!< Sample range of overlap
      int p_lineRange;           //!< Line range of overlap
      int p_minSampX;            //!< Starting Sample of overlap in X cube
      int p_maxSampX;            //!< Ending Sample of overlap in X cube
      int p_minSampY;            //!< Starting Sample of overlap in Y cube
      int p_maxSampY;            //!< Ending Sample of overlap in Y cube
      int p_minLineX;            //!< Starting Line of overlap in X cube
      int p_maxLineX;            //!< Ending Line of overlap in X cube
      int p_minLineY;            //!< Starting Line of overlap in Y cube
      int p_maxLineY;            //!< Ending Line of overlap in Y cube
      int p_mincnt;              //!< Minimum valid pixels to be valid overlap

      //! Multivariate Stats object for overlap data from both cubes
      std::vector<Isis::MultivariateStatistics> p_stats;
  };
  std::ostream &operator<<(std::ostream &os, Isis::OverlapStatistics &stats);
};

#endif
