#ifndef Intercept_h
#define Intercept_h
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
