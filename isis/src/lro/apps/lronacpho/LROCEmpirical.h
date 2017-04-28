#ifndef LROCEmpirical_h
#define LROCEmpirical_h

#include <iostream>
#include <iomanip>
#include <sstream>
#include <iomanip>

#include "Camera.h"
#include "DbProfile.h"
#include "IString.h"
#include "PhotometricFunction.h"
#include "SpecialPixel.h"

namespace Isis {

  class PvlObject;
  class Camera;

  /**
   * @brief An implementation of photometric equation used for LRO NAC Cameras
   *
   * This class implements the LROC Empirical photometric equation used for the LRO NAC
   * Cameras.
   *
   * @author 2016-08-15 Victor Silva
   *
   * @internal
   *   @history 2016-08-15 - Code adapted from lrowacpho written by Kris Becker
   *
   */
  class LROCEmpirical : public PhotometricFunction {
    public:
      LROCEmpirical (PvlObject &pvl, Cube &cube, bool useCamera);
      virtual ~LROCEmpirical () ;

      double photometry(double i, double e, double g, int band = 1) const;
      void report( PvlContainer &pvl );

    private:
      /**
       * Container for band photometric correction parameters
       *
       * @author 2016-08-15 Victor Silva
       * 
       * @internal
       *   @history 2016-08-05 - Code adapted from lrowacpho written by Kris Becker
       */
      struct Parameters {
        Parameters() : a0(0.0), a1(0.0), a2(0.0), a3(0.0),
                       wavelength(0.0), tolerance(0.0),
                       units("Degrees"), phaUnit(1.0), band(0), phoStd(0.0),
                       iProfile(-1) { }
        ~Parameters() { }
        bool IsValid() const { return (iProfile != -1);}
        double a0, a1, a2, a3; //<! Equation parameters
        double wavelength;     //<! Wavelength for correction
        double tolerance;      //<! Wavelength Range/Tolerance
        QString units;         //<! Phase units of equation
        double phaUnit;        //<! 1 for degrees, Pi/180 for radians
        int band;              //<! Cube band parameters
        double phoStd;         //<! Computed photometric std.
        int iProfile;          //<! Profile index of this data

      };

      void init(PvlObject &pvl, Cube &cube);
      double photometry(const Parameters &parms, double i, double e,double g) const;
      Parameters findParameters(const double wavelength) const;
      Parameters extract( const DbProfile &profile) const;

      std::vector<DbProfile> m_profiles; //<! Profiles for all possible wavelengths
      std::vector<Parameters> m_bandpho; //<! Parameters for each band
  };

};
#endif
