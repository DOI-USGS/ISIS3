#ifndef AbstractShape_h
#define AbstractShape_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
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
