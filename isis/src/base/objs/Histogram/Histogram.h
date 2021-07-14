#ifndef Histogram_h
#define Histogram_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/


#include "Cube.h"
#include "Constants.h"
#include "IException.h"
#include "Progress.h"
#include "SpecialPixel.h"
#include "Statistics.h"

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
   *   @history 2002-10-05 Tracie Sucharski added MaxBinCount, a method to return
   *                            the maximum bin count.
   *   @history 2005-06-21 Modified index computations in AddData and RemoveData
   *                            to round
   *   @history 2008-08-15 Added BinRange methods and functionality. Now, you can
   *                            collect statistics over all of the data and also set where the
   *                            binning will start and end. Increased the default number of bins
   *                            for floating point cubes.
   *   @history 2012-01-19 Steven Lambright and Jai Rideout - Added constructor
   *                            parameters to read from the Cube automatically.
   *   @history 2012-04-10 Orrin Thomas - Added constructor parameters to read
   *                            from ControlNets automatically (For control measure
   *                            data.)
   *   @history 2015-09-03  Tyler Wilson - Overrode Statistics::SetValidRange to
   *                            set the bin range as well as the statistical range
   *                            for the data.  The function Histogram::SetBinRange
   *                            has been removed from this class.
   *   @history 2016-04-20 Makayla Shepherd - Added UnsignedWord pixel type handling.
   *   @history 2017-05-19 Christopher Combs - Modified unitTest.cpp: Removed path of output file
   *                            name in output PVL to allow the test to pass when not using the
   *                            standard data areas. Fixes #4738.
   *   @history 2017-09-08 Summer Stapleton - Included test for Isis::Null being returned from
   *                            accessor method call in Histogram::rangesFromNet(). Fixes #5123,
   *                            #1673.
   *   @history 2018-07-27 Jesse Mapel - Added support for initializing a histogram from
   *                           signed and unsigned word cubes. References #971.
   *   @history 2020-06-11 Kaitlyn Lee - Changed how to detemine which bin a pixel/measure falls
   *                           into in AddData(). Changed how the bin range is calculated in
   *                           BinRange(). The math for these functions were incorrect and not
   *                           intuitive. These changes were made alongside changes made
   *                           to cnethist and hist.
   */

  class Histogram : public Statistics {
    public:
      Histogram() = default;
      Histogram(double minimum, double maximum,
                int bins = 1024);
      Histogram(Cube &cube, int statsBand, Progress *progress = NULL,
                double startSample = 1.0, double startLine = 1.0,
                double endSample = Null, double endLine = Null, int bins = 0,
                bool addCubeData = false);

      //constuctors that use ControlNetworks to build histograms of ControlMeasure data
      Histogram(ControlNet &net, double(ControlMeasure::*statFunc)() const, int bins);
      Histogram(ControlNet &net, double(ControlMeasure::*statFunc)() const, double binWidth);

      ~Histogram();

      void SetBins(const int bins);

      void Reset();
      virtual void AddData(const double *data, const unsigned int count);
      virtual void AddData(const double data);
      virtual void RemoveData(const double *data, const unsigned int count);

      double Median() const;
      double Mode() const;
      double Percent(const double percent) const;
      double Skew() const;

      BigInt BinCount(const int index) const;
      virtual void BinRange(const int index, double &low, double &high) const;
      double BinMiddle(const int index) const;
      double BinSize() const;
      int Bins() const;
      BigInt MaxBinCount() const;

      double BinRangeStart() const {
        return p_binRangeStart;
      }
      double BinRangeEnd() const {
        return p_binRangeEnd;
      }
      //void SetBinRange(double binStart, double binEnd);
      void SetValidRange(const double minimum = Isis::ValidMinimum,
                                       const double maximum = Isis::ValidMaximum);

    protected:
      //! The array of counts.
      std::vector<BigInt> p_bins;

    private:
      double p_binRangeStart, p_binRangeEnd;
      void addMeasureDataFromNet(ControlNet &net, double(ControlMeasure::*statFunc)() const);
      void rangesFromNet(ControlNet &net, double(ControlMeasure::*statFunc)() const);
  };
};

#endif
