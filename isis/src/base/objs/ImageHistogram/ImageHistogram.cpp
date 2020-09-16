/**
 * @file
 * $Revision: 1.4 $
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
#include "ImageHistogram.h"

#include "ControlNet.h"
#include "ControlMeasure.h"

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string>

using namespace std;

namespace Isis {

  /**
   * Constructs a histogram object. Only data between the minimum and maximum
   * will be binned, and the bin range will be from the minimum to the maximum.
   *
   * @param minimum Minimum value for binning the data into the histogram.
   * @param maximum Maximum value for binning the data into the histogram.
   * @param nbins The number of bins to use
   */
  ImageHistogram::ImageHistogram(double minimum, double maximum, int nbins) :
  Histogram(minimum, maximum, nbins) {
  }


  /**
   * Constructs a histogram object using a cube. This constructor computes
   *   the minimum, maximum for the binning range and number of bins
   *   automatically. All statistics will still be collected, though data at
   *   either end of the histogram will be put into one bin in order to attempt to
   *   achieve better histogram statistics.
   *
   * TODO: progress needs to be a bool.
   *
   * @param cube  The cube to used to determine min/max and bins
   * @param statsBand  The band number the histogram will be collected from
   * @param progress  The Progress object to be used to output the percent
   *                  processed information
   * @param startSample  The sample to start reading cube data from
   * @param startLine  The line to start reading cube data from
   * @param endSample  The sample to stop reading cube data at (Null for nsamps)
   * @param endLine  The line to stop reading cube data at (Null for nlines)
   * @param bins The number of histogram bins to create (0 for automatic)
   * @param addCubeData True to fill the histogram with data in addition to
   *                    initializing the binning ranges.
   */
  ImageHistogram::ImageHistogram(Cube &cube, int statsBand, Progress *progress,
      double startSample, double startLine,
      double endSample, double endLine,
      int bins, bool addCubeData) :
  Histogram(cube, statsBand, progress,
                startSample, startLine,
                endSample, endLine,
                bins, addCubeData) {
  }


  /**
   * Constructs a histogram from a control netowrk
   *
   * @param net: Reference to a ControlNetwork used to access all the measures.
   * @param statFunc: Pointer to a ControlMeasure acessor, the returns of this
   *    function call will be used to build up the network.
   * @param bins:  The number of bins to divide the histogram into.
   * @throws The number of Histogram Bins must be greater than 0.
   */
  ImageHistogram::ImageHistogram(ControlNet &net, double(ControlMeasure::*statFunc)() const, int bins) :
  Histogram(net, statFunc, bins){
  }


  /**
   * Constructs a histogram from a control netowrk
   *
   * @param Net:  Reference to a ControlNetwork used to access all the measures.
   * @param statFunc:  Pointer to a ControlMeasure acessor, the returns of this.
   *    function call will be used to build up the network.
   * @param binWidth:  The width of histogram bins.
   * @throws The width of Histogram Bins must be greater than 0.
   */
  ImageHistogram::ImageHistogram(ControlNet &net, double(ControlMeasure::*statFunc)() const,
                       double binWidth) :
  Histogram(net, statFunc, binWidth) {
  }


  //! Destructs a histogram object.
  ImageHistogram::~ImageHistogram() {
  }

  /**
   * Add an array of doubles to the histogram counters. This method can be
   * invoked multiple times. For example, once for each line in a cube, before
   * obtaining statistics and histogram information.
   *
   * @param data Pointer to array of double to add.
   * @param count Number of doubles to process.
   */
  void ImageHistogram::AddData(const double *data,
                          const unsigned int count) {
    std::cout << "IN IMAGEHIST ADDDATA LIST" << '\n';
    Statistics::AddData(data, count);

    int nbins = p_bins.size();
    int index;
    for (unsigned int i = 0; i < count; i++) {
      if (IsValidPixel(data[i]) && InRange(data[i]) ) {
        if (BinRangeStart() == BinRangeEnd() ) {
          index = 0;
        }
        else {
          index = (int) floor((double)(nbins - 1) / (BinRangeEnd() - BinRangeStart()) *
                              (data[i] - BinRangeStart() ) + 0.5);
        }
        if (index < 0) index = 0;
        if (index >= nbins) index = nbins - 1;
        p_bins[index] += 1;
      }
    }
  }

  /**
   * Add a single double data to the histogram.  Of course this can be invoke multiple times.
   * e.g. once for each residual in a network for instance.
   *
   * @param data a single observation to be added to the histogram
   */
  void ImageHistogram::AddData(const double data) {
    std::cout << "IN IMAGEHIST ADDDATA" << '\n';
    Statistics::AddData(data);

    int nbins = p_bins.size();
    int index;
    if (IsValidPixel(data) && InRange(data) ) {
      if (BinRangeStart() == BinRangeEnd() ) {
        index = 0;
      }
      else {
        index = (int) floor((double)(nbins - 1) / (BinRangeEnd() - BinRangeStart() ) *
                            (data - BinRangeStart() ) + 0.5);
      }
      if (index < 0) index = 0;
      if (index >= nbins) index = nbins - 1;
      p_bins[index] += 1;
    }
  }

  /**
   * Remove an array of doubles from the histogram counters. Note that this
   * invalidates the absolute minimum and maximum. They will no longer be
   * useable.
   * @see Stats
   *
   * @param data Pointer to array of doubles to remove.
   * @param count number of doubles to process.
   */
  void ImageHistogram::RemoveData(const double *data,
                             const unsigned int count) {
    std::cout << "IN IMAGEHIST REMOVEDATA LIST" << '\n';
    Statistics::RemoveData(data, count);

    int nbins = p_bins.size();
    int index;
    for (unsigned int i = 0; i < count; i++) {
      if (IsValidPixel(data[i]) ) {

        if (BinRangeStart() == BinRangeEnd() ) {
          index = 0;
        }
        else {
          index = (int) floor((double)(nbins - 1) / (BinRangeEnd() - BinRangeStart()) *
                              (data[i] - BinRangeStart()) + 0.5);
        }
        if (index < 0) index = 0;
        if (index >= nbins) index = nbins - 1;
        p_bins[index] -= 1;
      }
    }
  }

  /**
   * Returns the left edge and right edge values of a bin. That is the range of
   * data the bin covers.
   *
   * @throws iException The programmer has passed in an index outside of 0
   * to Bins()-1.
   *
   * @param index Index of the desired bin 0 to Bins()-1.
   * @param low The value at the left edge of the requested bin.
   * @param high The value at the right edge of the requested bin.
   */
  void ImageHistogram::BinRange(const int index,
                                double &low, double &high) const {
    std::cout << "IN IMAGE HIST BINRANGE" << '\n';
    if ( (index < 0) || (index >= (int)p_bins.size() ) ) {

      QString message = Message::ArraySubscriptNotInRange(index);
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    double binSize = (BinRangeEnd() - BinRangeStart()) / (double)(p_bins.size() - 1);
    low = BinRangeStart() - binSize / 2.0 + binSize * (double) index;
    high = low + binSize;
  }
}
