#ifndef PhotometricFunction_h
#define PhotometricFunction_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IString.h"
#include "Camera.h"
#include "DbProfile.h"
#include "SpecialPixel.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace Isis {

  class PvlObject;
  class Camera;

  /**
   * @brief An abstract implementation of the photometric function
   *
   * This abstract class implements the a generic Photometric function.
   * Child classes are expected to implement the photometry and report methods.
   *
   * @author 2016-08-15 Victor Silva
   *
   * @internal
   *   @history 2016-08-15 - Code adapted from lrowacpho written by Kris Becker
   */

  class PhotometricFunction {

    public:
      PhotometricFunction(PvlObject &pvl, Cube &cube, bool useCamera);
      virtual ~PhotometricFunction();

      void setCamera(Camera *cam);
      static QString algorithmName( const PvlObject &pvl );
      virtual double compute( const double &line, const double &sample, int band = 1, bool useDem = false);
      virtual double photometry( double i, double e, double g, int band = 1 ) const = 0;
      virtual void report( PvlContainer &pvl ) = 0;
      virtual void setMinimumIncidenceAngle( double angle );
      virtual void setMaximumIncidenceAngle( double angle );
      virtual void setMinimumEmissionAngle( double angle );
      virtual void setMaximumEmissionAngle( double angle );
      virtual void setMinimumPhaseAngle( double angle );
      virtual void setMaximumPhaseAngle( double angle );
      virtual double minimumIncidenceAngle();
      virtual double maximumIncidenceAngle();
      virtual double minimumEmissionAngle();
      virtual double maximumEmissionAngle();
      virtual double minimumPhaseAngle();
      virtual double maximumPhaseAngle();


    protected:

      Camera *m_camera;               //<! Camera used for calculating photmetric angles
      double m_iRef;                  //<! Incidence refernce angle
      double m_eRef;                  //<! Emission reference angle
      double m_gRef;                  //<! Phase reference angle
      double m_minimumIncidenceAngle; //<! The minimum incidence angle to perform computations
      double m_maximumIncidenceAngle; //<! The maximum incidence angle to perform computations
      double m_minimumEmissionAngle;  //<! The minimum emission angle to perform computations
      double m_maximumEmissionAngle;  //<! The maximum emission angle to perform computations
      double m_minimumPhaseAngle;     //<! The minimum phase angle to perform computations
      double m_maximumPhaseAngle;     //<! The maximum phase angle to perform computations
      DbProfile m_normProf;           //<! Parameters for the normalization model

      /**
       * @brief Helper template to initialize parameters
       *
       * This template will check the existance of a keyword and extract the value
       * if it exists to the passed parameter (type).  If it doesn't exist, the
       * default values are returned.
       *
       * @param conf Parameter profile container
       * @param keyname Name of keyword to get a value from
       * @param defval Default value it keyword/value doesn't exist
       * @param index Optional index of the value for keyword arrays
       *
       * @return @b T The value of the keyword or the default value if the
       *              keyword is not found.
       */
      template<typename T>
      T ConfKey( const DbProfile &conf, const QString &keyname, const T &defval, int index = 0 ) const {
        if (!conf.exists(keyname)) {
          return (defval);
        }
        if (conf.count(keyname) < index) {
          return (defval);
        }
         return conf.value(keyname, index);
      }
  };
};
#endif
