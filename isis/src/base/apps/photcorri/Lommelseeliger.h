#ifndef Lommelseeliger_h
#define Lommelseeliger_h

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
#include "PhotometricFunction.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Camera.h"
#include "DbProfile.h"
#include "IString.h"
#include "SpecialPixel.h"

namespace Isis {

  class PvlObject;
  class Camera;

  /**
   * An implementation of the Lommel Seeliger photometric function.
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
  class Lommelseeliger: public PhotometricFunction {
    public:
      Lommelseeliger(PvlObject &pvl, 
                     Cube &cube, 
                     bool useCamera);

      virtual ~Lommelseeliger();

      double photometry(double incidenceAngle, 
                        double emissionAngle, 
                        double phaseAngle, 
                        int bandNumber=1) const;

      void report(PvlContainer &pvl);

    private:
      void init(PvlObject &pvl, 
                Cube &cube);

      /**
       * Container for band photometric correction parameters.
       *
       * @author 2016-10-07 Driss Takir
       */
      struct Parameters {
        Parameters() : ALS(0.0), 
                       BETA(0.0), 
                       GAMMA(0.0), 
                       DELTA(0.0), 
                       wavelength(0.0), 
                       tolerance(0.0),
                       units("Degrees"), 
                       phaUnit(1.0), 
                       band(0), 
                       phoStd(0.0),
                       iProfile(-1) {
        }


        ~Parameters() { 
        }


        bool isValid() const { 
          return (iProfile != -1); 
        }


        double ALS;        /**< Lommel Seeliger parameter.*/
        double BETA;       /**< Lommel Seeliger coefficient.*/
        double GAMMA;      /**< Lommel Seeliger coefficient.*/
        double DELTA;      /**< Lommel Seeliger coefficient.*/
        double wavelength; /**< Wavelength for correction.*/
        double tolerance;  /**< Wavelength range or tolerance.*/
        QString units;     /**< Phase units of Hiller equation.*/
        double phaUnit;    /**< 1 for degrees, Pi/180 for radians.*/
        int band;          /**< Cube band parameters.*/
        double phoStd;     /**< Computed photometric standard.*/
        int iProfile;      /**< Profile index of this data.*/
      };

      virtual double photometry(const Parameters &parms, 
                                double incidenceAngle,
                                double emissionAngle, 
                                double phaseAngle) const;

      Parameters findParameters(const double wavelength) const;
      Parameters extract(const DbProfile &profile) const;

      std::vector<DbProfile> m_profiles;      /**< Vector of profiles.*/
      std::vector<Parameters> m_bandpho;      /**< Vector of band photometry parameters.*/

  };

};

#endif

