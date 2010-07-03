/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:09 $
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

#include "QuickFilter.h"
#include "iException.h"
#include <float.h>

using namespace std;
namespace Isis {

 /** 
  * Constructs a QuickFilter object with accumulators and counters set to zero.  
  * Because this is a line based filtering object, the number of samples and the 
  * boxcar size must be given to the constructor.
  * 
  * @param ns Number of samples in the cube
  * 
  * @param width Width of the boxcar (must be odd)
  * 
  * @param height Height of the boxcar (must be odd)
  * 
  * @throws Isis::iException::Programmer
  */
  QuickFilter::QuickFilter (const int ns, const int width,
                          const int height) {
    // Error checks
    if (ns <= 0) {
      string msg = "Invalid value for [ns] in QuickFilter constructor";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    
    if (width < 1) {
      string m="[Width] must be must be greater than or equal to one in ";
      m += "QuickFilter constructor";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if ((width % 2) == 0) {
      string m="[Width] must be must be odd in ";
      m += "QuickFilter constructor";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    
    if (height < 1) {
      string m="[Height] must be must be greater than or equal to one in ";
      m += "QuickFilter constructor";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if ((height % 2) == 0) {
      string m="[Height] must be must be odd in ";
      m += "QuickFilter constructor";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    
    // Create buffers
    p_sums = new double[ns];
    p_sumsqrs = new double[ns];
    p_counts = new int[ns];
    p_ns = ns;
  
    // Set defaults for min/max and valid pixels
    p_minimum = -DBL_MAX;
    p_maximum = DBL_MAX;
    p_minimumPixels = 0;
    
    // Set the boxcar size and compute half the size
    p_width = width;
    p_halfWidth = width / 2;
  
    p_height = height;
    p_halfHeight = height / 2;
  
    Reset();
  }
  
  //!  Reset all accumulators and counters to zero.
  void QuickFilter::Reset() {
    // Initialize buffers
    for (int i=0; i<p_ns; i++) {
      p_sums[i] = 0.0;
      p_sumsqrs[i] = 0.0;
      p_counts[i] = 0;
  
    }
  
    // Initialize sums for the last time Average/Variance/Count were called
    p_lastSum = 0.0;
    p_lastSumsqr = 0.0;
    p_lastCount = 0;
    p_lastIndex = -100;
    p_linesAdded = 0;
  }
    
  //! Destroys the QuickFilter object
  QuickFilter::~QuickFilter () {
    delete [] p_sums;
    delete [] p_counts;
    delete [] p_sumsqrs;
  }
  
 /** 
  * This method is used to set the minimum/maximum valid values. Pixels are only  
  * considered valid (usable when computing Average and Variance) if they are 
  * not special (NULL, LIS, etc) and if they fall within the range of minimum 
  * and maximum inclusive. You should only invoke this method once after the 
  * object has been constructed. Further invocations will cause unpredictable 
  * results. If this method is never called then the defaults are DBL_MIN and 
  * DBL_MAX, respectively.
  * 
  * @param minimum Minimum valid pixel
  * 
  * @param maximum Maximum valid pixel
  * 
  * @throws Isis::iException::Programmer
  */
  void QuickFilter::SetMinMax (const double minimum, const double maximum) {
    if (minimum >= maximum) {
      string m="Minimum must be less than maximum in [QuickFilter::SetMinMax]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    p_minimum = minimum;
    p_maximum = maximum;
    p_lastIndex = -100;
  }
  
 /** 
  * This method is used to set the minimum number of valid pixels in the boxcar.  
  * If the minimum requirement can not be satisfied then the Average and 
  * Variance methods will return Isis:NULL8. The default value is zero.
  * 
  * @param pixels Number of minimum valid pixels for statistically 
  *               computations to occur
  * 
  * @throws Isis::iException::Programmer
  */
  void QuickFilter::SetMinimumPixels (const int pixels) {
    if (pixels < 0) {
      string m="Pixels must be greater than or equal to zero in ";
      m += "[QuickFilter::SetMinimumPixels]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    p_minimumPixels = pixels;
    if (p_minimumPixels > p_width * p_height) {
      p_minimumPixels = p_width * p_height;
    }
  }
  
 /** 
  * Add an array of doubles to the accumulators and counters. This method must  
  * be invoked enough times to satisfy the height requirements of the boxcar. 
  * Note, however this is not strictly enforced. The method will check to make 
  * sure you have not added beyond the height of the boxcar. Therefore, you must 
  * remove a line before adding a new one. 
  * 
  * @param buf Array of doubles to add
  * 
  * @throws Isis::iException::Programmer
  */
  void QuickFilter::AddLine (const double *buf) {
    // Check for adding too many lines
    p_linesAdded++;
    if (p_linesAdded > p_height) {
      string m = "Number of lines added exceeds boxcar height ... ";
      m+= "use RemoveLine before AddLine";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    for (int i=0; i<p_ns; i++) {
      if (Isis::IsValidPixel(buf[i])) {
        if (buf[i] < p_minimum) continue;
        if (buf[i] > p_maximum) continue;
        p_sums[i] += buf[i];
        p_sumsqrs[i] += buf[i]*buf[i];
        p_counts[i]++;
        p_lastIndex = -100;
      }
    }
  }
  
 /** 
  * Remove an array of doubles from the accumulators and counters. 
  * 
  * @param buf Pointer to array of doubles to remove
  */
  void QuickFilter::RemoveLine (const double *buf) {
    for (int i=0; i<p_ns; i++) {
      if (Isis::IsValidPixel(buf[i])) {
        if (buf[i] < p_minimum) continue;
        if (buf[i] > p_maximum) continue;
        p_sums[i] -= buf[i];
        p_sumsqrs[i] -= buf[i]*buf[i];
        p_counts[i]--;
        p_lastIndex = -100;
      }
    }
    p_linesAdded--;
  }
  
 /** 
  * Computes and returns the boxcar average at pixel index (zero based). No 
  * error checks are done for out of array bounds conditions. If there are not 
  * enough valid pixels in the boxcar then Isis::NULL8 is returned.  The routine
  * works the fastest when sequentially accessing the averages 
  * (e.g., index = 0,1,2,...).
  * 
  * @param index Zero based sample position
  * 
  * @return double
  */
  double QuickFilter::Average (const int index) {
    // Move the boxcar if necessary
    Compute (index);
  
    // Return NULL8 if we have invalid conditions
    if (p_lastCount < p_minimumPixels) return Isis::NULL8;
    if (p_lastCount <= 0) return Isis::NULL8;
    
    // Return the average
    return p_lastSum / (double) p_lastCount;
  }
   
 /** 
  * Computes and returns the boxcar variance at pixel index (zero based). No 
  * error checks are done for out of array bounds conditions. If there are not 
  * enough valid pixels in the boxcar then Isis::NULL8 is returned. The routine 
  * works the fastest when sequentially accessing the variances 
  * (e.g., index = 0,1,2,...).
  * 
  * @param index Zero based sample position
  * 
  * @return double
  */
  double QuickFilter::Variance (const int index) {
    // Move the boxcar if necessary
    Compute(index);
    
    // Return NULL8 if we have invalid conditions
    if (p_lastCount < p_minimumPixels) return Isis::NULL8;
    if (p_lastCount <= 1) return Isis::NULL8;
  
    // Return the variance
    double temp = p_lastCount * p_lastSumsqr - p_lastSum * p_lastSum;
    if (temp < 0.0) temp = 0.0; // This shouldn't happen unless roundoff occurs
    return temp / ((double) (p_lastCount - 1) * (double) p_lastCount);
  }
  
 /** 
  * Computes and returns the number of valid pixels in the boxcar at pixel index
  * (zero based). No error checks are done for out of array bounds conditions. 
  * The routine works the fastest when sequentially accessing the counts 
  * (e.g., index = 0,1,2,...). 
  * 
  * @param index Zero based sample position
  * 
  * @return double
  */
  int QuickFilter::Count(const int index) {
    // Move the boxcar if necessary
    Compute(index);
    
    // Return the valid count
    return p_lastCount;
  }
  
 /** 
  * Returns the width of the boxcar 
  * 
  * @return int
  */
  int QuickFilter::Width() const {
    return p_width;
  }
  
 /** 
  * Returns the half the width of the boxcar rounded down because the boxcar 
  * size is odd. 
  * 
  * @return int
  */
  int QuickFilter::HalfWidth() const {
    return p_halfWidth;
  }
  
 /** 
  * Returns the height of the boxcar
  * 
  * @return int
  */
  int QuickFilter::Height() const {
    return p_height;
  }
  
 /** 
  * Returns the half the height of the boxcar rounded down because the boxcar 
  * size is odd.
  * 
  * @return int
  */
  int QuickFilter::HalfHeight() const {
    return p_halfHeight;
  }
  
 /** 
  * Returns the number of samples in a line
  * 
  * @return int
  */
  int QuickFilter::Samples() const {
    return p_ns;
  }
  
 /** 
  * Returns the lowest pixel value included in filtering computations
  * 
  * @return double
  */
  double QuickFilter::Low() const {
    return p_minimum;
  }
  
 /** 
  * Returns the highest pixel value included in filtering computations
  * 
  * @return double
  */
  double QuickFilter::High() const {
    return p_maximum;
  }
  
 /** 
  * Returns the minimum number of pixels which need to be valid inside the 
  * boxcar. If there are not enough valid pixels then invoking Average and 
  * Variance methods will result in a NULL output.
  * 
  * @return int
  */
  int QuickFilter::MinimumPixels () const {
    return p_minimumPixels;
  }
  
 /** 
  * Computes the moving boxcar sums and counts for the Average, Variance, and 
  * count methods. No error checks are done for out of array bounds conditions. 
  * The routine works the fastest when sequentially accessing the averages 
  * (e.g., index = 0,1,2,...).
  * 
  * @param index Zero based sample position            
  */
  void QuickFilter::Compute (const int index) {
    // If the index hasn't changed just return
    if (index == p_lastIndex) return;
    
    // Determine start and stop indeces
    int start = index - p_halfWidth;
    int stop = index + p_halfWidth;
    
    // See if the index has increased by one 
    if (index == p_lastIndex + 1) {
      // Remove one column
      start--;
      if (start < 0) start = -1 * start;
      p_lastSum -= p_sums[start];
      p_lastSumsqr -= p_sumsqrs[start];
      p_lastCount -= p_counts[start];
  
      // Add another column
      if (stop >= p_ns) stop = p_ns - (stop - p_ns + 2);
      p_lastSum += p_sums[stop];
      p_lastSumsqr += p_sumsqrs[stop];
      p_lastCount += p_counts[stop];
    }
  
    // Recompute everything
    else {
      p_lastSum = 0.0;
      p_lastCount = 0;
      p_lastSumsqr = 0.0;
      int j;
      for (int i=start; i<=stop; i++) {
        j = i;
        if (i < 0) {
          j = -1 * i;
        }
        else if (i >= p_ns) {
          j = p_ns - (i - p_ns + 2);
        }
  
        p_lastSum += p_sums[j];
        p_lastSumsqr += p_sumsqrs[j];
        p_lastCount += p_counts[j];
      }
    }
    
    // Save the current index
    p_lastIndex = index;
  }
} // end namespace isis
