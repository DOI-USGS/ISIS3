#ifndef PrincipalComponentAnalysis_h
#define PrincipalComponentAnalysis_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2009/12/22 02:09:54 $
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

#include <vector>
#include "tnt/tnt_array2d.h"
#include "MultivariateStatistics.h"
#include "iException.h"
#include "Constants.h"

namespace Isis
{
 /**                                                                       
  * @brief Principal Component Analysis class                
  * 
  *     This class is used to apply Principal Component Analysis
  * to transform multivariate data into its principal components
  * as well as invert it from component space.
  *                                                                
  * If you would like to see PrincipalComponentAnalysis being used
  *         in implementation, see pca.cpp or decorstretch.cpp
  *                                                                        
  * @ingroup Math and Statistics
  * 
  * @author Jacob Danton - 2006-05-18
  *                                                                                                                                                                                      
  * @internal                                                                                                                           
  */
    class PrincipalComponentAnalysis
    {
    public:
	  PrincipalComponentAnalysis (const int n);
      PrincipalComponentAnalysis (TNT::Array2D<double> transform);
	  ~PrincipalComponentAnalysis () {};
      void AddData (const double *data, const unsigned int count);
      void ComputeTransform ();
      TNT::Array2D<double> Transform (TNT::Array2D<double> data);
      TNT::Array2D<double> Inverse (TNT::Array2D<double> data);
      TNT::Array2D<double> TransformMatrix() {return p_transform;};
      int Dimensions() {return p_dimensions;};

    private:
      void ComputeInverse ();
      bool p_hasTransform;
      int p_dimensions;

      TNT::Array2D<double> p_transform, p_inverse;
      std::vector<Isis::MultivariateStatistics *> p_statistics;
    }; 
}

#endif
