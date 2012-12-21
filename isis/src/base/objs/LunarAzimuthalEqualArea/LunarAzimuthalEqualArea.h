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

namespace Isis {
  class Pvl;
  class PvlGroup;

  /**
  * @brief Modified Lambert Azimuthal Equal-Area Map Projection
  *
  * This class provides methods for the forward and inverse equations of a
  * Lunar Azimuthal Equal-Area map projection. 
  *  
  * The code was converted to C++ from the Fortran version of Isis2. This 
  * class inherits Projection and provides the two virtual methods SetGround 
  * (forward) and SetCoordinate (inverse) and a third virtual method, XYRange, 
  * for obtaining projection coordinate coverage for a latitude/longitude 
  * window. 
  *  
  * Please see the Projection class for a full accounting of all the methods 
  * available. 
  *
  *
  * @ingroup MapProjection
  *
  * @author 2009-05-15 Eric Hyer
  *
  * @internal
  *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
  *                           declaration of Pvl, PvlGroup to header file.
  *                           Ordered includes in implementation file.  Moved
  *                           Name and Version methods to the implementation
  *                           file. Minor modifications to comply with some
  *                           coding standards. References #928.
  */
  class LunarAzimuthalEqualArea : public Projection {
    public:
      LunarAzimuthalEqualArea(Pvl &label);
      ~LunarAzimuthalEqualArea();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();

    private:
      double m_maxLibration; /**< Value of the MaximumLibration keyword from the
                                  Mapping group of the labels*/
  };
}

#endif
