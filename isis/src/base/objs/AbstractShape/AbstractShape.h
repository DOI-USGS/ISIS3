#ifndef AbstractShape_h
#define AbstractShape_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractPlate.h"

namespace Isis {
  /**
   * 
   * @author 2014-02-25 Kris Becker 
   * @internal 
   *   @history 2014-02-15 Kris Becker - Original Version 
   *   @history 2015-03-08 Jeannie Backer - Added class to ISIS trunk. References #2035
   */

//class AbstractPlate;

//  The following definition is a placeholder for when the design for an 
//  abstract shape is implemented. When AbstractShape is implemented, this
//  should be removed.
// 
//  Examples of AbstractShape types in addition to AbstractPlate (NaifDSK):  
//  AbstractUniverse, AbstractSphere, AbstractEllipsoid, 
//  AbstractTriaxialEllipsoid and AbstractRingsSystem, AbstractPlane, etc...
#define AbstractShape AbstractPlate;

} // namespace Isis

#endif
