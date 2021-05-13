#ifndef AbstractPlate_h
#define AbstractPlate_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
   */
  class AbstractPlate {
    public:
      /**
       * Empty destructor for an AbstractPlate object.
       */
      virtual ~AbstractPlate();
  
      /**
       * Gets the name of this Plate type
       * 
       * @return QString AbstractPlate
       */
      virtual QString name() const = 0;
  
      /**
       * Gets the minimum radius. This is a pure virtual function.
       * 
       * @return Distance The minimum radius
       */
      virtual Distance minRadius() const = 0;
      
      /**
       * Gets the maximum radius. This is a pure virtual function.
       * 
       * @return Distance The maximum radius
       */
      virtual Distance maxRadius() const = 0;
  
      /**
       * Gets the area of the plate. This is a pure virtual function.
       * 
       * @return double The area of the plate.
       */
      virtual double area() const = 0;
      
      /**
       * Gets the normal. This is a pure virtual function.
       * 
       * @return NaifVector The normal
       */
      virtual NaifVector normal() const = 0;
      
      /**
       * Gets the separation angle. This is a pure virtual function.
       * 
       * @param raydir Given a direction vector, compute the angle of separation 
       *               between it and the plate normal vector
       * 
       * @return Angle The separation angle
       */
      virtual Angle separationAngle(const NaifVector &raydir) const = 0;
  
      /**
       * @brief Determines if a look direction from a point intercepts the plate 
       *  
       * Given a point in space in body fixed coordinates and a look direction, this 
       * method determines the point of intercept on the plate. This is a pure virtual function.
       * 
       * @param vertex An observer point in space in body fixed coordinates
       * @param raydir A look direction vector 
       * 
       * @return bool Returns true if the look direction from the observer intercepts 
       *              the plate, otherwise returns false
       */
      virtual bool hasIntercept(const NaifVertex &vertex, 
                                const NaifVector &raydir) const = 0;
                                
      /**
       * @brief Determines the give lat/lon point intercept the triangular plate 
       *  
       * Given a latitude/longitude point, this method determines if it intercepts the 
       * plate. This is a pure virtual function.
       * 
       * @param lat  The latitude of the given grid point
       * @param lon  Longitude of the given point
       * 
       * @return bool Returns true if the lat/lon point intercepts the plate, false 
       *              otherwise
       */  
      virtual bool hasPoint(const Latitude &lat, const Longitude &lon) const = 0;
        
      /**
       * @brief Conpute the intercept point on a triangular plate 
       *  
       * Given a point in space and a look direction, compute the intercept point on a 
       * triangular plate. If the intercept point does not exist, return a null 
       * pointer. This is a pure virtual function.
       * 
       * @param vertex Specifies a point in space of a body fixed coordinate
       * @param raydir Specifies a look direction from the vertex in body fixed 
       *               coordinates.  It can be of any magnitude.
       * 
       * @return Intercept* Returns the intercept point if it exists on the 
       *                    triangular plate, otherwise returns a null pointer.
       */
      virtual Intercept *intercept(const NaifVertex &vertex, 
                                      const NaifVector &raydir) const = 0;  

      /**
       * @brief Determine the intercept point of a lat/lon location for the plate 
       *  
       * Determines if a lat/lon point intercepts a plate.  Given a latitude and 
       * longitude coordinate, this method converts the point to a body fixed X/Y/Z 
       * value and computes intercept point within the boundaries if the plate. If no 
       * intercept is found, a null pointer is returned. This is a pure virtual function.
       * 
       * @param lat Latitude of the point
       * @param lon Longitude of the point
       * 
       * @return SurfacePoint* Pointer to the intersection of the point on the 
       *                       triangle. If an intersection does not exist a null
       *                       pointer is returned.
       */
      virtual SurfacePoint *point(const Latitude &lat, 
                                  const Longitude &lon) const = 0;
  
      /**
       * @brief Returns a clone of the current plate 
       *  
       * Provides replication of the current triangular plate. Note this 
       * implementation returns a shared copy of the triangular plate as long as the 
       * plate type is shared by copy (TNT library is). This is a pure virtual function.
       * 
       * @return AbstractPlate* Returns a copy of the plate 
       */
      virtual AbstractPlate *clone() const = 0;
  
    protected:
      AbstractPlate();
      Intercept *construct(const NaifVertex &vertex, const NaifVector &raydir,
                           SurfacePoint *ipoint) const;  
    private:
      /**
       * Copy contructor. Do not use, clone instead.
       * 
       * @param plate AbstractPlate to copy
       */
      AbstractPlate(const AbstractPlate &plate);
      
      /**
       * Assignment operator
       * 
       * @param plate AbstractPlate to copy
       * 
       * @return AbstractPlate
       */
      AbstractPlate &operator=(const AbstractPlate &plate);
  };


} // namespace Isis

#endif
