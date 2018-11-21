#ifndef Hillier_h
#define Hillier_h
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
   * @brief An implementation of the Hillier photometric function
   *
   * This class implements the Hillier-Buratti-Hill photometric
   * equation as outline in thier paper "Multispectral Photometry
   * of the Moon and Absolute Calibration of the Clementine UV/VIS
   * Camera", published in Icaris v141, pg. 205-255 (1999).
   *
   * @author  2010-02-15 Kris Becker
   *
   */
  class Hillier : public PhotometricFunction{
    public:
      /**
       * @brief Create Hilier photometric object
       *
       */
      Hillier (PvlObject &pvl, Cube &cube, bool useCamera) : PhotometricFunction(pvl, cube, useCamera) {init(pvl, cube);};

      //! Destructor
      virtual ~Hillier() {};

      double photometry(double i, double e, double g, int band = 1) const;
      void report(PvlContainer &pvl);

    private:
      /**
       * @brief Container for band photometric correction parameters
       *
       * @author Kris Becker - 2/21/2010
       */
      struct Parameters {
        Parameters() : b0(0.0), b1(0.0), a0(0.0), a1(0.0), a2(0.0), a3(0.0),
                       a4(0.0), wavelength(0.0), tolerance(0.0),
                       units("Degrees"), phaUnit(1.0), band(0), phoStd(0.0),
                       iProfile(-1) { }
        ~Parameters() { }
        bool IsValid() const { return (iProfile != -1); }
        double b0, b1, a0, a1, a2, a3, a4;  //<! Hillier parameters
        double wavelength;                  //<! Wavelength for correction
        double tolerance;                   //<! Wavelenght Range/Tolerance
        QString units;                      //<! Phase units of Hiller eq.
        double phaUnit;  // 1 for degrees, Pi/180 for radians
        int band;                           //<! Cube band parameters
        double phoStd;                      //<! Computed photometric std.
        int iProfile;                       //<! Profile index of this data
      };

      std::vector<DbProfile> m_profiles;
      std::vector<Parameters> m_bandpho;

      void init(PvlObject &pvl, Cube &cube);

      virtual double photometry(const Parameters &parms, double i, double e,double g) const;

      Parameters findParameters(const double wavelength) const;
      Parameters extract(const DbProfile &profile) const;
  };

};

#endif
