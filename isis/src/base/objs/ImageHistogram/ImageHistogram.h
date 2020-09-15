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
#ifndef ImageHistogram_h
#define ImageHistogram_h

#include "Cube.h"
#include "Message.h"
#include "IException.h"
#include "Progress.h"
#include "Histogram.h"
#include "ImageHistogram.h"

namespace Isis {
  class ControlNet;
  class ControlMeasure;
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
   *                            methods into public space.
   */

  class ImageHistogram : public Histogram{
    public:
      ImageHistogram(double minimum, double maximum,
                     int bins = 1024);
      ImageHistogram(Cube &cube, int statsBand, Progress *progress = NULL,
                     double startSample = 1.0, double startLine = 1.0,
                     double endSample = Null, double endLine = Null, int bins = 0,
                     bool addCubeData = false);

      //constuctors that use ControlNetworks to build histograms of ControlMeasure data
      ImageHistogram(ControlNet &net, double(ControlMeasure::*statFunc)() const, int bins);
      ImageHistogram(ControlNet &net, double(ControlMeasure::*statFunc)() const, double binWidth);

      ~ImageHistogram();

      void AddData(const double *data, const unsigned int count);
      void AddData(const double data);
      void RemoveData(const double *data, const unsigned int count);

      void BinRange(const int index, double &low, double &high) const;

    private:
      //! The array of counts.
      std::vector<BigInt> p_bins;
  };
};

#endif
