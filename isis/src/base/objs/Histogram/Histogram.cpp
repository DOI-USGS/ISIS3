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
#include "Histogram.h"
#include "Message.h"
#include "LineManager.h"
#include <string>
#include <iostream>

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
  Histogram::Histogram (const double minimum, const double maximum,
                        const int nbins) {
    SetValidRange(minimum,maximum);
    SetBinRange(minimum, maximum);
    SetBins(nbins);
  }


  /**
   * Constructs a histogram object using a cube. This constructor computes
   *   the minimum, maximum for the binning range and number of bins
   *   automatically. All statistics will still be collected, though data at
   *   either end of the histogram will be put into one bin in order to attempt to
   *   achieve better histogram statistics.
   *
   * @param cube  The cube to used to determine min/max and bins
   * @param band  The band number the histogram will be collected from 
   * @param progress  The Isis::Progress object to be used to output the percent 
   *                  processed information
   */
  Histogram::Histogram (Cube &cube, const int band, Progress *progress) {
    InitializeFromCube(cube, band, progress);
  }


  void Histogram::InitializeFromCube(Cube &cube, const int band, Progress *progress) {
    // Make sure band is valid
    if ((band < 0) || (band > cube.Bands())) {
      string msg = "Invalid band in [Histogram constructor]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    double min,max;
    int nbins;

    if (cube.PixelType() == Isis::UnsignedByte) {
      min = 0.0 * cube.Multiplier() + cube.Base();
      max = 255.0 * cube.Multiplier() + cube.Base();
      nbins = 256;
    }
    else if (cube.PixelType() == Isis::SignedWord) {
      min = -32768.0 * cube.Multiplier() + cube.Base();
      max = 32767.0 * cube.Multiplier() + cube.Base();
      nbins = 65536;
    }
    else if (cube.PixelType() == Isis::Real) {
      // Determine the band for statistics
      int bandStart = band;
      int bandStop = band;
      int maxSteps = cube.Lines();
      if (band == 0){
        bandStart = 1;
        bandStop = cube.Bands();
        maxSteps = cube.Lines() * cube.Bands();
      }

      // Construct a line buffer manager and a statistics object
      LineManager line(cube);
      Statistics stats = Statistics();

      // Prep for reporting progress if necessary
      if (progress != NULL) {
        string save = progress->Text ();
        progress->SetText("Computing min/max for histogram");
        progress->SetMaximumSteps(maxSteps);
        progress->CheckStatus();
      }

      for (int useBand = bandStart ; useBand <= bandStop ; useBand++){
        // Loop and get the statistics for a good minimum/maximum
        for (int i=1; i<=cube.Lines(); i++) {
          line.SetLine(i,useBand);
          cube.Read(line);
          stats.AddData (line.DoubleBuffer(),line.size());
          if (progress != NULL) progress->CheckStatus();
        }
      }

      // Get the min/max for constructing a histogram object
      if (stats.ValidPixels() == 0) {
        min = 0.0;
        max = 1.0;
      }
      else {
        min = stats.BestMinimum ();
        max = stats.BestMaximum ();
      }

      nbins = 65536;
    }
    else {
      std::string msg = "Unsupported pixel type";
      throw iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    // Set the bins and range
    SetBinRange(min,max);
    SetBins(nbins);
  }


  //! Destructs a histogram object.
  Histogram::~Histogram() {
  }

  void Histogram::SetBinRange(double binStart, double binEnd) {
    if(binEnd < binStart) {
      string msg = "The binning range start [" + iString(binStart) + 
        " must be less than the end [" + iString(binEnd) + ".";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    p_binRangeStart = binStart;
    p_binRangeEnd = binEnd;
  }


  //! Resets histogram counters to zero.
  void Histogram::Reset () {
    Isis::Statistics::Reset();
    for (int i=0; i<(int)p_bins.size(); i++) {
      p_bins[i] = 0;
    }
  }


  //! Change the number of bins in the histogram and reset counters
  void Histogram::SetBins(const int nbins) {
    p_bins.resize(nbins);
    Histogram::Reset();
  }

  /**
   * Add an array of doubles to the histogram counters. This method can be
   * invoked multiple times. For example, once for each line in a cube, before
   * obtaining statistics and histogram information.
   *
   * @param data Pointer to array of double to add.
   * @param count Number of doubles to process.
   */
  void Histogram::AddData (const double *data,
                               const unsigned int count) {
    Isis::Statistics::AddData (data,count);

    int nbins = p_bins.size();
    int index;
    for (unsigned int i=0; i<count; i++) {
      if (Isis::IsValidPixel(data[i]) && InRange(data[i])) {
        if (BinRangeStart() == BinRangeEnd()) {
          index = 0;
        }
        else {
          index = (int) floor((double) (nbins - 1) / (BinRangeEnd() - BinRangeStart()) *
                        (data[i] - BinRangeStart()) + 0.5);
        }
        if (index < 0) index = 0;
        if (index >= nbins) index = nbins - 1;
        p_bins[index] += 1;
      }
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
  void Histogram::RemoveData (const double *data,
                                  const unsigned int count) {
    Isis::Statistics::RemoveData (data,count);

    int nbins = p_bins.size();
    int index;
    for (unsigned int i=0; i<count; i++) {
      if (Isis::IsValidPixel(data[i])) {
        if (BinRangeStart() == BinRangeEnd()) {
          index = 0;
        }
        else {
          index = (int) floor((double) (nbins - 1) / (BinRangeEnd() - BinRangeStart()) *
                         (data[i] - BinRangeStart()) + 0.5);
        }
        if (index < 0) index = 0;
        if (index >= nbins) index = nbins - 1;
        p_bins[index] -= 1;
      }
    }
  }

  /**
   * Returns the median.
   *
   * @returns The median
   */
  double Histogram::Median () const {
    return Percent (50.0);
  }

  /**
   * Returns the mode.
   *
   * @returns The mode
   */  
  double Histogram::Mode () const {
    int mode = 0;
    for (int i=0; i<(int)p_bins.size(); i++) {
      if (p_bins[i] > p_bins[mode]) mode = i;
    }

    if (p_bins[mode] < 1) return Isis::NULL8;

    return BinMiddle (mode);
  }


  /**
   * Computes and returns the value at X percent of the histogram.  For example,
   * Percent(50.0) is equivalent to the computing the median. While Percent(0.5)
   * and Percent(99.5) would obtain a minimum and maximum for the data that
   * could be used for a good contrast stretch.
   *
   * @param percent X percent of the histogram to compute.
   *
   * @returns The value at X percent of the histogram.
   */
  double Histogram::Percent (double percent) const {
    if ((percent < 0.0) || (percent > 100.0)) {
      string m = "Argument percent outside of the range 0 to 100 in";
      m += " [Histogram::Percent]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    if (ValidPixels() < 1) return Isis::NULL8;

    Isis::BigInt currentPixels = 0;
    double currentPercent;

    for (int i=0; i<(int)p_bins.size(); i++) {
      currentPixels += p_bins[i];
      currentPercent = (double) currentPixels / (double) ValidPixels() * 100.0;
      if (currentPercent >= percent) {
        return BinMiddle (i);
      }
    }

    return BinMiddle ((int)p_bins.size() - 1);
  }


  /**
   * Computes and returns the skew. If there are no valid pixels then NULL8 is
   * returned. Recognize that because of the binning which occurs, in order to
   * generate the histogram, the skew may not be precise but will be very close.
   *
   * @return The skew.
   */
  double Histogram::Skew() const {
    if (ValidPixels() < 1) return Isis::NULL8;
    double sdev = StandardDeviation();
    if (sdev == 0.0) return 0.0;
    return 3.0 * (Average() - Median()) / sdev;
  }


  /**
   * Returns the count at a bin position in the histogram.
   *
   * @param index Index of the desired bin 0 to Bins()-1.
   *
   * @return The count at a bin position in the histogram.
   */
  BigInt Histogram::BinCount (const int index) const {
    if ((index < 0) || (index >= (int)p_bins.size())) {
      string message = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_);
    }

    return p_bins[index];
  }


  /**
   * Returns the left edge and right edge values of a bin. That is the range of
   * data the bin covers.
   *
   * @throws Isis::iException The programmer has passed in an index outside of 0
   * to Bins()-1.
   *
   * @param index Index of the desired bin 0 to Bins()-1.
   * @param low The value at the left edge of the requested bin.
   * @param high The value at the right edge of the requested bin.
   */
  void Histogram::BinRange (const int index,
                                double &low, double &high) const {
    if ((index < 0) || (index >= (int)p_bins.size())) {
      string message = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_);
    }

    double binSize = (BinRangeEnd() - BinRangeStart()) / (double) (p_bins.size() - 1);
    low = BinRangeStart() - binSize / 2.0 + binSize * (double) index;
    high = low + binSize;
  }


  /**
   * Returns the value represented by a bin. This is not the count, but the
   * actual data value at the middle of the bin.
   *
   * @param index Index of the desired bin 0 to Bins()-1.
   *
   * @returns The middle value of the bin.
   */
  double Histogram::BinMiddle (const int index) const {
    if ((index < 0) || (index >= (int)p_bins.size())) {
      string message = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_);
    }

    double low,high;
    BinRange (index,low,high);
    return (low + high) / 2.0;
  }


  /**
   * Returns the size of an individual bin. Essentially, the difference of the
   * high and low edge values (BinRange) of a bin.  This value is constant for
   * all bins.
   *
   * @return The size of the individual bin.
   */
  double Histogram::BinSize () const {
    double low,high;
    BinRange (0,low,high);
    return high - low;
  }


  /**
   * Returns the number of bins in the histogram.
   *
   * @return The number of bins in the histogram.
   */
  int Histogram::Bins () const {
    return (int)p_bins.size();
  }


  /**
   * Returns the highest bin count.
   *
   * @return The highest bin count.
   */
  BigInt Histogram::MaxBinCount () const {

    Isis::BigInt maxBinCount = 0;
    for (int i=0; i<(int)p_bins.size(); i++) {
      if (p_bins[i] > maxBinCount) maxBinCount = p_bins[i];
    }

    return maxBinCount;
  }
}
