/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2009/05/14 20:41:01 $                                                                 
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

#include <float.h>
#include <string>
#include <iostream>
#include "MultivariateStatistics.h"
#include "iException.h"


namespace Isis {
/**                                                                       
 * Constructs a Multivariate Statistics object with accumulators and counters 
 * set to zero.                                      
 */                                                                       
  MultivariateStatistics::MultivariateStatistics () {
    Reset ();
  }
  
  //! Resets all accumulators to zero
  void MultivariateStatistics::Reset() {
    p_x.Reset();
    p_y.Reset();
    p_sumxy = 0.0;
  
    p_validPixels = 0;
    p_invalidPixels = 0;
    p_totalPixels = 0;
  }
  
  //! Destructs a MultivariateStatistics object.
  MultivariateStatistics::~MultivariateStatistics () {};
/**                                                                       
 * Add two arrays of doubles to the accumulators and counters. This method can 
 * be invoked multiple times, for example, once for each line in a cube, before 
 * obtaining statistics.                                                         
 *                                                                        
 * @param x Array of doubles to add.
 * @param y Array of doubles to add.
 * @param count Number of doubles to process.                                       
 */                                                                         
  void MultivariateStatistics::AddData (const double *x, const double *y, 
                             const unsigned int count) {
    for (unsigned int i=0; i<count; i++) {
      double yVal = y[i];
      double xVal = x[i];
      p_totalPixels++;

      if (Isis::IsValidPixel(xVal) && Isis::IsValidPixel(yVal)) {
        p_x.AddData(xVal);
        p_y.AddData(yVal);
        p_sumxy += xVal * yVal;
        p_validPixels++;
      }
      else {
        p_invalidPixels++;
      }
    }
  }

/**                                                                       
 * Remove an array of doubles from the accumulators and counters.                                                         
 *                                                                        
 * @param x Pointer to an array of doubles to remove.
 * @param y Array of doubles to add.
 * @param count Number of doubles to process.
 *                                                                        
 * @return (type)return description
 * 
 * @internal
 *   @todo The description for param y doesn't make sense here. -Leah
 */                                                                         
  void MultivariateStatistics::RemoveData (const double *x, const double *y,
                                const unsigned int count) {
    for (unsigned int i=0; i<count; i++) {
      p_totalPixels--;
  
      if (Isis::IsValidPixel(x[i]) && Isis::IsValidPixel(y[i])) {
        p_x.RemoveData(&x[i],1);
        p_y.RemoveData(&y[i],1);
        p_sumxy -= x[i] * y[i];
        p_validPixels--;
      }
      else {
        p_invalidPixels--;
      }
    }
  
    if (p_totalPixels < 0) {
      std::string m="You are removing non-existant data in [MultivariateStatistics::RemoveData]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  }
      
/**                                                                       
 * Computes and returns the covariance between the two data sets If there are 
 * no valid data (pixels) then NULL8 is returned.
 * 
 * @return Covariance between the two data sets.
 */                                                                       
  double MultivariateStatistics::Covariance () const {
    if (p_validPixels <= 1) return Isis::NULL8;
    double covar = p_sumxy - p_y.Average()*p_x.Sum() - p_x.Average()*p_y.Sum() +
                   p_x.Average()*p_y.Average()*(double)p_validPixels;
    return covar / (double)(p_validPixels - 1);
  }
  
/**                                                                       
 * Computes and returns the coefficient of correlation (between -1.0 and 1.0) of
 * the two data sets.  This can be used as a goodness-of-fit measurement.  The 
 * close the correlation is two -1.0 or 1.0 the more likely the data sets are 
 * related (and therefore the regression equation is valid).                                                           
 * 
 * @return The coefficient of correlation. (between -1.0 and 1.0) 
 * The closer to 0.0 implies there is less correlation between the data sets. 
 * Returns NULL8 if correlation couldn't be computed.
 */                                                                       
  double MultivariateStatistics::Correlation () const {

    if (p_validPixels <= 1) return Isis::NULL8;
    double covar = Covariance();
    double stdX = p_x.StandardDeviation();
    double stdY = p_y.StandardDeviation();
    if (stdX == 0.0 || stdX == Isis::NULL8) return Isis::NULL8;
    if (stdY == 0.0 || stdY == Isis::NULL8) return Isis::NULL8;
    if (covar == Isis::NULL8) return Isis::NULL8;
    return covar/(stdX*stdY);
  }
  
/**                                                                       
 * Returns the total number of pixels processed. 
 * 
 * @return The total number of pixel processed (valid and invalid).                                       
 */                                                                         
  BigInt MultivariateStatistics::TotalPixels () const {
    return p_totalPixels;
  }
  
/**                                                                       
 * Returns the number of valid pixels processed. Only valid pixels are utilized 
 * when computing the average, standard deviation, variance, minimum, and 
 * maximum.
 * 
 * @return The number of valid pixels processed.                                       
 */
  BigInt MultivariateStatistics::ValidPixels () const {
    return p_validPixels;
  }

/**                                                                                                                                 
 * Returns the number of invalid pixels encountered.
 *                                                                                           
 * @return The number of invalid (unprocessed) pixels.                                       
 */                                                                         
  BigInt MultivariateStatistics::InvalidPixels () const {
    return p_invalidPixels;
  }

/**                                                                       
 * Fits a line @f[ y=A+Bx @f] through the data.                                                          
 *                                                                        
 * @param a The additive constant A.
 * @param b The additive constant B.                                       
 */                                                                         
  void MultivariateStatistics::LinearRegression(double &a, double &b) const {
    // From Modern Elementary Statistics - 5th edition, Freund, pp 367
    double denom = (double)p_validPixels*p_x.SumSquare() - p_x.Sum()*p_x.Sum();
    if (denom == 0.0) {
        std::string msg = "Unable to compute linear regression in Multivariate Statistics";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    a = p_y.Sum()*p_x.SumSquare() - p_x.Sum()*p_sumxy;
    a = a / denom;
  
    b = (double)p_validPixels*p_sumxy - p_x.Sum()*p_y.Sum();
    b = b / denom;
  }

/**                                                                       
 * Returns the sum of x*y for all data given through the AddData method.                                                       
 * 
 * @return The sum of x*y for all data.                                       
 */                                                                         
  double MultivariateStatistics::SumXY () const {
    return p_sumxy;
  }

/**                                                                       
 * Returns a Stats object for all of the X data fed through the AddData method.                                                          
 * 
 * @return A Stats object for all X data.                                       
 */                                                                         
  Isis::Statistics MultivariateStatistics::X() const { return p_x; };

/**                                                                       
 * Returns a Stats object for all of the Y data fed through the AddData method.                                                          
 * 
 * @return A Stats object for all Y data.                                       
 */
  Isis::Statistics MultivariateStatistics::Y() const { return p_y; };
}

