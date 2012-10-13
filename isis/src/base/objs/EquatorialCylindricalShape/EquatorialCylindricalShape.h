#ifndef EquatorialCylindricalShape_h
#define EquatorialCylindricalShape_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
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

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#include "DemShape.h"
//#include "SurfacePoint.h"
//#include "Pvl.h"

namespace Isis {
  class Pvl;
  /**
   * @brief Define shapes and provide utilities for Isis3 targets
   *
   * This class will define shapes of Isis3 target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup 
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal
   *  @history
   */
  class EquatorialCylindricalShape : public DemShape {
    public:
      //! Constructor
      EquatorialCylindricalShape(Target *target, Isis::Pvl &pvl);

      //! Destructor
      ~EquatorialCylindricalShape();

      //! Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

    private:
      Distance *m_minRadius;  //!< Minimum radius value in DEM file
      Distance *m_maxRadius;  //!< Maximum radius value in DEM file
  };
};

#endif

