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
   * This class is used to accumulate an image histogram on double arrays.  In
   * particular, it is highly useful for obtaining a histogram on cube data.
   * Parameters which can be computed are the 1) median, 2) mode, and 3) skew.
   * The histogram consists of a fixed set of distinct bins. When an object is
   * created the programmer must provide a minimum and maximum which defines how
   * data is further placed in the bins.  The minimum is mapped to the middle
   * of the first bin [0] and the maximum is mapped to the middle of the
   * last bin [Bins()-1]. There are a set of methods which return bin information
   * such as 1) count, 2) size, 3) middle value, 4) range, and 5) maximum bin
   * count.
   *
   * @ingroup Statistics
   *
   * @author 2020-09-22 Adam Paquette
   *
   * @internal
   */

  class ImageHistogram : public Histogram {
    public:
      ImageHistogram(double minimum, double maximum,
                     int bins = 1024);
      ImageHistogram(Cube &cube, int statsBand, Progress *progress = NULL,
                     double startSample = 1.0, double startLine = 1.0,
                     double endSample = Null, double endLine = Null, int bins = 0,
                     bool addCubeData = false);

      ~ImageHistogram();

      virtual void AddData(const double *data, const unsigned int count);
      virtual void AddData(const double data);
      virtual void RemoveData(const double *data, const unsigned int count);

      virtual void BinRange(const int index, double &low, double &high) const;

    private:
      void InitializeFromCube(Cube &cube, int statsBand,
          Progress *progress, int nbins, double startSample, double startLine,
          double endSample, double endLine);
  };
};

#endif
