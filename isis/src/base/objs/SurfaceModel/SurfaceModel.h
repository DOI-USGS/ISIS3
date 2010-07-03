#ifndef SurfaceModel_h
#define SurfaceModel_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2008/06/18 20:42:32 $                                                                 
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

#include <vector>
#include "PolynomialBivariate.h"
#include "LeastSquares.h"

namespace Isis {
  /**
   * @brief Model a 3-D surface
   * 
   * Given a set of (x,y,z) triplets, this class will model the surface
   * that best fits the points.  The equation to be modelled is:
   * 
   * @f[ 
   * z = a + b*x + c*y + d*x^2 + e*x*y + f*y^2 
   * @f]
   * 
   * @ingroup Math                                       
   *                                                    
   * @author 2005-05-09 Jeff Anderson 
   *  
   * @internal 
   *   @history 2008-06-18 Steven Lambright Fixed ifndef command 
   *
   * @todo Add plot and/or visualize method
   */
  class SurfaceModel {
    public:
      SurfaceModel ();
      ~SurfaceModel();

      void AddTriplet (const double x, const double y, const double z);
      void AddTriplets (const double *x, const double *y, const double *z,
                        const int n);
      void AddTriplets (const std::vector<double> &x, 
                        const std::vector<double> &y, 
                        const std::vector<double> &z);

      void Solve();
      double Evaluate (const double x, const double y);

      int MinMax(double &x, double &y);

    private:
      LeastSquares *p_lsq;
      PolynomialBivariate *p_poly2d;
  };
};

#endif
