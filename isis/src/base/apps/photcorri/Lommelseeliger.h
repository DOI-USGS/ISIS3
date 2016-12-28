#ifndef Lommelseeliger_h
#define Lommelseeliger_h

/* This program was originally written by Kris Becker for LRO mission and has been modified by Driss Takir (USGS) to 
     * incorporate the four photometric models (Lommel-Seeliger, 
     * Rolo, Minnaert, and McEwen) adopted by the OSIRIS-REx 
     * project. 
     * Build 3.0- 8/15/2016 
*/ 
/**
 * @file
 * $Revision$
 * $Date$
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

#include <iostream>
#include <sstream>
#include <iomanip>

#include "PhotometricFunction.h"
#include "IString.h"
#include "Camera.h"
#include "DbProfile.h"
#include "SpecialPixel.h"

namespace Isis {

  class PvlObject;
  class Camera;

  /**
   * @brief An implementation of the Lommel Seeliger photometric 
   *        function
   * 
   * adopted by the OSIRIS-REx project, based on the paper of 
   * "Takir et al. (2015): Photometric Models of Disk-integrated 
   * Observations of the OSIRIS-REx target Asteroid (101955) 
   * Bennu, Icarus, 252, 393-399." 
   * 
   *
   * @author  Driss Takir- 10/9/16
   *
   */
  class Lommelseeliger: public PhotometricFunction{
    public:
      /**
       * @brief Create Lommel Seeliger photometric object
       *
       */
      Lommelseeliger (PvlObject &pvl, Cube &cube, bool useCamera) : PhotometricFunction(pvl, cube, useCamera) {init(pvl, cube);};

      //! Destructor
      virtual ~Lommelseeliger() {};

      double photometry(double i, double e, double g, int band = 1) const;
      void Report(PvlContainer &pvl);

    private:
      /**
       * @brief Container for band photometric correction parameters
       *
       * @author Driss Takir - 10/9/2016
       */
      struct Parameters {
        Parameters() : ALS(0.0), BETA(0.0), GAMMA(0.0), DELTA(0.0), wavelength(0.0), tolerance(0.0),
                       units("Degrees"), phaUnit(1.0), band(0), phoStd(0.0),
                       iProfile(-1) { }
        ~Parameters() { }
        bool IsValid() const { return (iProfile != -1); }
        double ALS, BETA, GAMMA, DELTA;  //<! Lommel Seeliger parameters
        double wavelength;                  //<! Wavelength for correction
        double tolerance;                   //<! Wavelenght Range/Tolerance
        QString units;                      //<! Phase units of Hiller eq.
        double phaUnit;  // 1 for degrees, Pi/180 for radians
        int band;                           //<! Cube band parameters
        double phoStd;                      //<! Computed photometric std.
        int iProfile;                       //<! Profile index of this data
      };

      std::vector<DbProfile> _profiles;
      std::vector<Parameters> _bandpho;

      void init(PvlObject &pvl, Cube &cube);

      virtual double photometry(const Parameters &parms, double i, double e,double g) const;

      Parameters findParameters(const double wavelength) const;
      Parameters extract(const DbProfile &profile) const;
  };

};

#endif

