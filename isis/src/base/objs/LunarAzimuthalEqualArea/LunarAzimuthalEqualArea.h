#ifndef LunarAzimuthalEqualArea_h
#define LunarAzimuthalEqualArea_h
/**
* @file                                                                  
* $Revision: 1.1 $                                                             
* $Date: 2009/08/07 22:52:23 $                                                  
              
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

#include "Projection.h"
#include "Constants.h"
#include <string>


namespace Isis
{
  
  /**
  * @brief Modified Lambert Azimuthal Equal-Area Map Projection 
  *                                                                        
  * This class provides methods for the forward and inverse equations of a 
  * Lunar Azimuthal Equal-Area map projection. The code was converted 
  * to C++ from the Fortran version of Isis2. This class inherits Projection
  * and provides the two virtual methods SetGround (forward) and SetCoordinate
  * (inverse) and a third virtual method, XYRange, for obtaining projection
  * coordinate coverage for a latitude/longitude window. Please see the
  * Projection class for a full accounting of all the methods available.
  *
  *
  * @ingroup MapProjection
  *
  * @author 15 MAY 2009 Eric Hyer
  *
  */
  class LunarAzimuthalEqualArea : public Isis::Projection
  {
    public:
      LunarAzimuthalEqualArea(Isis::Pvl & label);
      ~LunarAzimuthalEqualArea();
  
      bool SetGround (const double lat, const double lon);
      bool SetCoordinate (const double x, const double y);
      bool XYRange(double & minX, double & maxX, double & minY, double & maxY);
      PvlGroup Mapping();
      bool operator== (const Isis::Projection & proj);
 
      /**
      * Returns the name of the map projection
      *
      * @return string Name of projection
      */
      std::string Name() const
      {
        return "LunarAzimuthalEqualArea";
      }
  
      /**
      * Returns the version of the map projection
      * 
      * 
      * @return std::string Version number
      */
      std::string Version () const
      {
        return "0.1";
      }
      
    
    private:
      double p_maxLibration;
  };
}

#endif
