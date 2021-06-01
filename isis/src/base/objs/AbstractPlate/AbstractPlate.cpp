/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractPlate.h"

#include <QString>

#include "Angle.h"
#include "Distance.h"
#include "Intercept.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifDskApi.h"
#include "SurfacePoint.h"

namespace Isis {

  /**
   * Empty constructor for an AbstractPlate object. This constructor is protected. 
   */
  AbstractPlate::AbstractPlate() {}


  /**
   * Empty destructor for an AbstractPlate object.
   *
   */
  AbstractPlate::~AbstractPlate() {}


  /**
   * Gets the name of this Plate type. 
   *  
   * @return QString The name of this plate, "AbstractPlate" 
   *
   */
  QString AbstractPlate::name() const {
    return "AbstractPlate";
  }


  /**
   * Construct an intercept from a clone of this plate as well as the given 
   * vertex, direction vector, and surface point. 
   * 
   * @param vertex Observer position
   * @param raydir Look direction
   * @param ipoint Surface point of the intercept location on the body 
   *  
   * @return <b>Intercept *</b> A pointer to an intercept constructed with the vertex, raydir, 
   *         ipoint and clone of this AbstractPlate.
   */
  Intercept *AbstractPlate::construct(const NaifVertex &vertex, const NaifVector &raydir,
                                      SurfacePoint *ipoint) const {
    return (new Intercept(vertex, raydir, ipoint, clone()));
  }

}
// namespace Isis
