#ifndef Intercept_h
#define Intercept_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSharedPointer>

#include "AbstractPlate.h"
#include "NaifDskApi.h"

namespace Isis {

  class Angle;
  class SurfacePoint;
  
  /**
   * @brief Container for a intercept condition 
   *  
   * This class is intended to contain all the necessary elements of an observer 
   * with a look direction and intercept point on an abtract shape.  If efficient,
   * reintrant, thread safe memory management elements (e.g., TNT) are used for 
   * the types, this can be used in threaded environments.
   * 
   *  
   * @author 2012-12-05 Kris Becker 
   *  
   * @internal
   *   @history 2012-12-05 Kris Becker - Original version.
   *   @history 2015-03-08 Jeannie Backer - Added documentation and test. Added class to ISIS trunk.
   *                           References #2035
   */
  class Intercept {
    public:
      Intercept();
      Intercept(const NaifVertex &observer, const NaifVector &raydir,
                SurfacePoint *ipoint, AbstractPlate *shape);
      virtual ~Intercept();
  
      bool isValid() const;
  
      const NaifVertex &observer() const;
      const NaifVector &lookDirectionRay() const;
      SurfacePoint location() const;
  
      NaifVector normal() const;
      Angle emission() const;
  
      Angle separationAngle(const NaifVector &raydir) const;
  
      const AbstractPlate *shape() const;
  
    private:
  
      /** 
       * Enumeration to indicate whether to throw an exception if an error 
       * occurs. 
       */ 
      enum ErrAction { Throw,  //!< Throw an exception if an error occurs.
                       NoThrow //!< Do not throw an exception if an error occurs.
      };  // Error mode to employ
  
      bool verify(const bool &test, const QString &errmsg,
                  const ErrAction &action = Throw) const;
  
      NaifVertex m_observer; //!< Three dimensional coordinate position of the observer, in body fixed.
      NaifVector m_raydir;   //!< Three dimensional ray representing the look direction.
  
      QSharedPointer<SurfacePoint>  m_point; /**< Surface point of the intercept location on the 
                                                  body, in body fixed.*/
      QSharedPointer<AbstractPlate> m_shape; //!< Shape Model for the intercept point.
  
  };

};  // namespace Isis
#endif
