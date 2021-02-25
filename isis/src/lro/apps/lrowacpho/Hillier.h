#ifndef Hillier_h
#define Hillier_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
