#ifndef LunarAzimuthalEqualArea_h
#define LunarAzimuthalEqualArea_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "TProjection.h"

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
  *  @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
  *                           References #775.
  */
  class LunarAzimuthalEqualArea : public TProjection {
    public:
      LunarAzimuthalEqualArea(Pvl &label);
      ~LunarAzimuthalEqualArea();
      bool operator== (const TProjection &proj);

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
