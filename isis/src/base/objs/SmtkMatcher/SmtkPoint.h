#ifndef SmtkPoint_h
#define SmtkPoint_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                   

#include "Camera.h"
#include "GruenTypes.h"
#include "ControlPoint.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * @brief Container for a point and its geometry
   * 
   * @author 2011-06-04 Kris Becker
   *
   * @internal
   */
  class PointGeometry {
    public:
      PointGeometry() : m_point(), m_geom() {  }
      PointGeometry(const Coordinate &pnt, 
                    const Coordinate &geom = Coordinate()) : 
                    m_point(pnt), m_geom(geom) {    }
      ~PointGeometry() {  }

      inline bool isValid() const {
        return ( m_point.isValid() && m_geom.isValid() );
      }

      inline const Coordinate &getPoint() const { return (m_point);   }
      inline const Coordinate &getGeometry() const { return (m_geom); }

      Coordinate m_point;
      Coordinate m_geom;
  };

  /**                                                                       
   * @brief Container for SMTK match points 
   *  
   * This container maintains the state of a SMTK point candidate.  It will 
   * hold all the neccessary information to complete stereo processing and 
   * generation of other SMTK points. 
   *  
   * @author 2011-05-24 Kris Becker
   * 
   * @internal
   */
  class SmtkPoint {
    public:
      SmtkPoint() : m_matchpt(), m_regpnt(), m_geom(), m_registered(false),
                    m_isValid(false) { }
      SmtkPoint(const PointPair &point, const PointPair &geom = PointPair()) :
                m_matchpt(point), 
                m_regpnt(PointGeometry(point.getRight(), geom.getRight())), 
                m_geom(geom), m_registered(false), m_isValid(false)  { } 
      SmtkPoint(const MatchPoint &mpt, const PointGeometry &regpnt,
                const PointPair &geom) : m_matchpt(mpt), m_regpnt(regpnt),
                                         m_geom(geom), 
                                         m_registered(mpt.isValid()), 
                                         m_isValid(false) { } 
      ~SmtkPoint() { }
  
      /** Indicates the smtk portion of the point is valid */
      inline bool isValid() const { return (m_isValid); }
  
      /** Returns goodness of the fit registration */
      inline double GoodnessOfFit() const { return (m_matchpt.getEigen()); }
  
      /**
       * @brief Get initial left and right point pair
       *  
       * This method returns the points used in the registration of the point. 
       * The right point contains the origin of registration and @b NOT the 
       * registered point. 
       *  
       * @see getRight() 
       * 
       * @author Kris Becker - 6/4/2011
       * 
       * @return const PointPair& Returns left/right point pair
       */
      inline const PointPair &getPoints() const {
        return (m_matchpt.m_point);
      }
  
      /** Return left and right point geometry */
      inline const PointPair &getGeometry() const {
        return (m_geom);
      }

      /** Returns the left point */
      inline const Coordinate &getLeft() const {
        return (getPoints().getLeft());
      }

      /**
       * @brief Returns the @b registered right coordinate 
       *  
       * Use this method to get the @b registered right point coordinate.  It 
       * should be the one used to compute the stereo match. 
       *  
       * @author Kris Becker - 6/4/2011
       * 
       * @return const Coordinate& Coordinate of the right registered point
       */
      inline const Coordinate &getRight() const {
        return (m_regpnt.getPoint());
      }
  
      /** Returns the affine transform and radiometic results */
      inline const AffineRadio &getAffine() const {
        return (m_matchpt.m_affine);
      }

      /**
       * @brief Returns registration status 
       *  
       * If this method returns true, then the point set has been registered by 
       * the Gruen algorithm.  If false, it is not registered.   This could be 
       * because the point was unsuccessfully registered (see isValid()) or it
       * was cloned from a registered point. 
       * 
       * @author Kris Becker - 5/26/2011
       * 
       * @return bool True if point has been registered, otherwise false.
       */
      inline bool isRegistered() const { return (m_registered);  }
  
      MatchPoint    m_matchpt;
      PointGeometry m_regpnt;
      PointPair     m_geom;
      bool          m_registered;
      bool          m_isValid;
    };
};

#endif
