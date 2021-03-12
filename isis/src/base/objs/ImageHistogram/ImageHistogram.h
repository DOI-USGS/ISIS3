#ifndef ImageHistogram_h
#define ImageHistogram_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/


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
