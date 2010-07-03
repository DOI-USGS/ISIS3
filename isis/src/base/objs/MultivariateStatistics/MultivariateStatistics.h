/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:08 $                                                                 
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

#ifndef MultivariateStatistics_h
#define MultivariateStatistics_h

#include "Statistics.h"
#include "SpecialPixel.h"
#include "Constants.h"

namespace Isis {
/**                                                                       
 * @brief Container of multivariate statistics.                
 *                                                                        
 * This class is used to accumulate multivariate statisics on two double arrays.
 * In particular, it is highly useful for obtaining the covariance, correlation,
 * and linear regression analysis on the the data.   It ignores input values 
 * which are Isis special pixel values.  That is, if either co-aligned double 
 * value is a special pixel then both values are not used in any statistical 
 * computation.  
 * 
 * @see Histogram
 * @see Stats
 *                                                                        
 * @ingroup Statistics                                                  
 *                                                                        
 * @author 2004-06-08 Jeff Anderson                                                                                 
 *                                                                        
 * @internal
 *   @history 2005-03-28 Leah Dahmer modified file to support Doxygen 
 *    documentation.
 *   @history 2005-05-23 Jeff Anderson - Added 2GB+ file support
 * 
 *   @todo This class needs an example.
 *   @todo For the below methods we will need to compute log x, loy y, sumx3, 
 *    sumx4,sumx2y:
 *    void ExponentialRegression (double &a, double &b) const;
 *    void PowerRegression (double &a, double &b) const;
 *    void parabolicRegression (double &a, double &b, double &c);
 */                                                                       
  class MultivariateStatistics {
    public:
      MultivariateStatistics ();
      ~MultivariateStatistics ();
  
      void Reset ();
      void AddData (const double *x, const double *y, 
                    const unsigned int count);
      void RemoveData (const double *x, const double *y, 
                       const unsigned int count);
  
      Isis::Statistics X() const;
      Isis::Statistics Y() const;
      double SumXY () const;
      double Covariance () const;
      double Correlation() const;
  
      void LinearRegression (double &a, double &b) const;
  
      BigInt ValidPixels () const;
      BigInt InvalidPixels () const;
      BigInt TotalPixels () const;
  
    private:
      //! A Statistics object holding x data.
      Isis::Statistics p_x;
      //! A Statistics object holding y data.
      Isis::Statistics p_y;

      //! The sum of x and y.
      double p_sumxy;
      /**
       * The number of valid (computed) pixels.
       * @internal
       */
      BigInt p_validPixels;
      /** 
       * The number of invalid (ignored) pixels.
       * @internal
       */
      BigInt p_invalidPixels; 
      /** 
       * The total number of pixels (invalid and valid).
       * @internal
       */
      BigInt p_totalPixels; 
  }; 
};

#endif
