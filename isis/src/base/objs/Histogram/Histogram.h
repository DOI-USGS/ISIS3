/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/08/15 22:03:32 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#ifndef Histogram_h
#define Histogram_h

#include "Statistics.h"
#include "iException.h"
#include "Constants.h"
#include "Progress.h"
#include "Cube.h"

namespace Isis {
/**
 * @brief Container of a cube histogram
 *
 * This class is used to accumulate a histogram on double arrays.  In
 * particular, it is highly useful for obtaining a histogram on cube data.
 * Parameters which can be computed are the 1) median, 2) mode, and 3) skew.
 * The histogram consists of a fixed set of distinct bins. When an object is
 * created the programmer must provide a minimum and maximum which defines how
 * data is further placed in the bins.  The minimum is mapped to the left edge
 * of the first bin [0] and the maximum is mapped to the right edge of the
 * last bin [Bins()-1]. There are a set of methods which return bin information
 * such as 1) count, 2) size, 3) middle value, 4) range, and 5) maximum bin
 * count.
 *
 * @ingroup Statistics
 *
 * @author 2002-05-13 Jeff Anderson
 *
 * @internal
 *   @todo This class needs an example.
 *   @history 2002-05-22 Jeff Anderson moved Reset, AddData, and RemoveData
 *   methods into public space.
 *   @history 2002-10-05 Tracie Sucharski added MaxBinCount, a method to return
 *   the maximum bin count.
 *   @history 2005-06-21 Modified index computations in AddData and RemoveData
 *   to round
 *   @history 2008-08-15 Added BinRange methods and functionality. Now, you can
 *            collect statistics over all of the data and also set where the
 *            binning will start and end. Increased the default number of bins
 *            for floating point cubes.
 */
  class Histogram : public Isis::Statistics {
    public:
      Histogram (const double minimum, const double maximum,
                 const int bins=1024);
      Histogram (Cube &cube, const int band, Progress *progress=NULL);

      ~Histogram ();

      void SetBins (const int bins);

      void Reset ();
      void AddData (const double *data, const unsigned int count);
      void RemoveData (const double *data, const unsigned int count);

      double Median () const;
      double Mode () const;
      double Percent (const double percent) const;
      double Skew () const;

      BigInt BinCount (const int index) const;
      void BinRange (const int index, double &low, double &high) const;
      double BinMiddle (const int index) const;
      double BinSize () const;
      int Bins () const;
      BigInt MaxBinCount () const;

      double BinRangeStart() const { return p_binRangeStart; }
      double BinRangeEnd() const { return p_binRangeEnd; }
      void SetBinRange(double binStart, double binEnd);

    private:
      void InitializeFromCube(Cube &cube, const int band, Progress *progress);
      //! The array of counts.
      std::vector<BigInt> p_bins;
      double p_binRangeStart, p_binRangeEnd;
  };
};

#endif
