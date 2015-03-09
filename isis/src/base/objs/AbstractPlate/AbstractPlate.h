#ifndef AbstractPlate_h
#define AbstractPlate_h
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

#include <QString>

#include "NaifDskApi.h"

namespace Isis {

  class Angle;
  class Distance;
  class Intercept;
  class Latitude;
  class Longitude;
  class SurfacePoint;
  
  /**
   * @brief Abstract interface to a TIN plate 
   *  
   * This abstract class defines the interface for triangular plate.  The plate is 
   * assumed to be a set of 3 body-fixed vertex points that describe a portion of 
   * the surface digital elevation model (DEM). 
   *  
   * The interface allows for repeated queries (e.g., ray intersection, point 
   * containment) of the plate represented by the object containing the plate. 
   *  
   * This class is not directly instantiable but is typically provided by a 
   * distinct plate model implementation (e.g., NAIF DSK). 
   *  
   * This class can be cloned but not copied directly. 
   * 
   * @author 2014-02-25 Kris Becker 
   * @internal 
   *   @history 2014-02-15 Kris Becker - Original Version 
   *   @history 2015-03-08 Jeannie Backer - Added documentation and test. Added class to ISIS trunk.
   *                           References #2035
   */
  class AbstractPlate {
    public:
      virtual ~AbstractPlate();
  
      virtual QString name() const = 0;
  
      virtual Distance minRadius() const = 0;
      virtual Distance maxRadius() const = 0;
  
      virtual double area() const = 0;
      virtual NaifVector normal() const = 0;
      virtual Angle separationAngle(const NaifVector &raydir) const = 0;
  
      virtual bool hasIntercept(const NaifVertex &vertex, 
                                const NaifVector &raydir) const = 0;
      virtual bool hasPoint(const Latitude &lat, const Longitude &lon) const = 0;
  
      virtual Intercept    *intercept(const NaifVertex &vertex, 
                                      const NaifVector &raydir) const = 0;
      virtual SurfacePoint *point(const Latitude &lat, 
                                  const Longitude &lon) const = 0;
  
      virtual AbstractPlate *clone() const = 0;
  
    protected:
      AbstractPlate();
      Intercept *construct(const NaifVertex &vertex, const NaifVector &raydir,
                           SurfacePoint *ipoint) const;  
    private:
      // Disallow direct copies - use clones
      AbstractPlate(const AbstractPlate &plate);
      AbstractPlate &operator=(const AbstractPlate &plate);
  };


} // namespace Isis

#endif
