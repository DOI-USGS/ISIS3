#ifndef PhotometricFunction_h
#define PhotometricFunction_h
/**
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

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "DbProfile.h"
#include "IString.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

namespace Isis {

  class PvlObject;
  class Camera;

  /**
   * Implementation of photometric correction model.
   *
   * Note: This photometric model was adopted by the OSIRIS-REx
   * project, based on the paper of "Takir et al. (2015): 
   * Photometric Models of Disk-integrated Observations of the 
   * OSIRIS-REx target Asteroid (101955) Bennu, Icarus, 252, 
   * 393-399." The code for this class was adapted from code 
   * originally written by Kris Becker for the LRO mission. 
   *  
   * @author 2016-10-09 Driss Takir 
   * @internal 
   *   @history 2016-10-09 Driss Takir - Original Version.
   *   @history 2016-12-27 Jeannie Backer - Fixed coding standards. References #4570. 
   */
  class PhotometricFunction {
    public:
      PhotometricFunction( PvlObject &pvl, 
                           Cube &cube, 
                           bool useCamera);

      virtual ~PhotometricFunction();

      virtual double photometry(double incidenceAngle,
                                double emissionAngle, 
                                double phaseAngle, 
                                int bandNumber = 1) const = 0;

      virtual double compute(const double &line, 
                             const double &sample, 
                             int band = 1, 
                             bool useDem = false) const;

      void operator()(Buffer &in, 
                      Buffer &out) const;

      static QString algorithmName(const PvlObject &pvl);

      void setCamera(Camera *cam);
      void useDem(bool useDem);
      void setIncidence(double angle);
      void setIncidenceReference(double angle);
      void setMinimumIncidenceAngle(double angle);
      void setMaximumIncidenceAngle(double angle);
      void setEmission(double angle);
      void setEmissionReference(double angle);
      void setMinimumEmissionAngle(double angle);
      void setMaximumEmissionAngle(double angle);
      void setPhase(double angle);
      void setPhaseReference(double angle);
      void setMinimumPhaseAngle(double angle);
      void setMaximumPhaseAngle(double angle);        

      bool useDem() const;
      double incidence() const;
      double incidenceReference() const;
      double minimumIncidenceAngle() const;
      double maximumIncidenceAngle() const;
      double emission() const;
      double emissionReference() const;
      double minimumEmissionAngle() const;
      double maximumEmissionAngle() const;
      double phase() const;
      double phaseReference() const;
      double minimumPhaseAngle() const;
      double maximumPhaseAngle() const;
      DbProfile normalProfile() const;

      virtual void report(PvlContainer &pvl) = 0;
 
    protected:

      /**
        * Helper method to initialize parameter.
        *
        * This method will check the existance of a keyword and extract the 
        * value, if it exists, to the passed parameter (type).  If it doesn't 
        * exist, the default values is returned. 
        *
        * @param T Templated variable type
        * @param conf Parameter profile container
        * @param keyname Name of keyword to get a value from
        * @param defval Default value it keyword/value doesn't exist
        * @param index Optional index of the value for keyword arrays
        *
        * @return T Return type
        *
        * @author 2016-10-07 Driss Takir
         */
      template<typename T> T ConfKey(const DbProfile &conf, 
                                     const QString &keyname, 
                                     const T &defval, 
                                     int index = 0 ) const {

        if (!conf.exists(keyname)) {
          return defval;
        }
        if (conf.count(keyname) < index) {
          return defval;
        }
        return conf.value(keyname, index);
      }
    private:
      double m_incRef; /**< Incidence reference angle found in the PVL under IncRef.*/
      double m_emaRef; /**< Emission reference angle found in the PVL under EmaRef.*/
      double m_phaRef; /**< Phase reference angle found in the PVL as PhaRef.*/

      Camera *m_camera;   /**< Camera associated with the given cube, if used.*/
      bool m_useDem;      /**< Indicates whether DEM will be used.*/
      double m_incidence; /**< The user provided incidence angle.*/
      double m_emission;  /**< The user provided emission angle.*/
      double m_phase;     /**< The user provided phase angle.*/


      double m_minimumIncidenceAngle; /**< The user provided minimum incidence angle.*/
      double m_maximumIncidenceAngle; /**< The user provided maximum incidence angle.*/
      double m_minimumEmissionAngle;  /**< The user provided minimum emission angle.*/
      double m_maximumEmissionAngle;  /**< The user provided maximum emission angle.*/
      double m_minimumPhaseAngle;     /**< The user provided minimum phase angle.*/
      double m_maximumPhaseAngle;     /**< The user provided maximum phase angle.*/

      DbProfile m_normProf;           /**< Parameter file container.*/

  };


  /** 
   * Implement templatized MIN fumnction.
   */
  template<typename T> inline T MIN( const T &A, const T &B ) {
    if (A < B) {
      return A;
    }
    else {
      return B;
    }
  }


  /** 
   * Implement templatized MAX function.
   */
  template<typename T> inline T MAX( const T &A, const T &B ) {
    if (A > B) {
      return A;
    }
    else {
      return B;
    }
  }

};

#endif

