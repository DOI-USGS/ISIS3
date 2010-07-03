#ifndef NUMERICALATMOSAPPROX_H
#define NUMERICALATMOSAPPROX_H
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/11/07 23:48:13 $
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

#include <string>
#include <vector>

#include "NumericalApproximation.h"
using namespace std;
namespace Isis {
  class AtmosModel;
/**                                                                       
 * This class extends @b Isis::NumericalApproximation. It was
 * created to handle numerical integration methods for specific 
 * atmospheric functions in the @b Isis::AtmosModel class. 
 * Rather than using a data set to interpolate a function, these
 * methods can take in a pointer to an AtmosModel object and an 
 * enumerated value for the specific function that needs to be 
 * integrated. 
 *                                                                        
 * @ingroup Math and RadiometricAndPhotometricCorrection
 * 
 * @author 2008-11-05 Jeannie Walldren 
 * @internal 
 *   @history 2008-11-05 Jeannie Walldren - Original Version.
 *   @history 2008-11-07 Jeannie Walldren - Fixed documentation
 */                                                                       
  class NumericalAtmosApprox : public NumericalApproximation{
    public:
      //! Uses @b Isis::NumericalApproximation constructor
      NumericalAtmosApprox(const NumericalApproximation::InterpType &itype=CubicNatural):NumericalApproximation(itype){};
      //! Empty destructor.
      virtual ~NumericalAtmosApprox(){};

      /**
       * This enum defines function to be integrated by Romberg's 
       * method.
       */
      enum IntegFunc { OuterFunction, //!< Indicates that Romberg's method will integrate the function OutrFunc2Bint() 
                       InnerFunction  //!< Indicates that Romberg's method will integrate the function InrFunc2Bint()
      };
      double RombergsMethod (AtmosModel *am, IntegFunc sub, double a, double b) throw (iException &);
      double RefineExtendedTrap(AtmosModel *am, IntegFunc sub, double a, double b, double s, unsigned int n) throw (iException &);

      static double OutrFunc2Bint(AtmosModel *am, double phi);
      static double InrFunc2Bint(AtmosModel *am, double mu);

  };
};
#endif


