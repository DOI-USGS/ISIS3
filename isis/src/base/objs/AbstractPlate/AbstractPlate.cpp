/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
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
