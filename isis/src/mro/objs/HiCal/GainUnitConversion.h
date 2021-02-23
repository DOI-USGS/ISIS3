#ifndef GainUnitConversion_h
#define GainUnitConversion_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cmath>
#include <string>
#include <vector>

#include "IString.h"
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiCalConf.h"
#include "Module.h"
#include "FileName.h"

#include "IException.h"

namespace Isis {

  /**
   * @brief Computes units parameters for HiRISE data calibration (Ziof Module)
   *
   * This class computes the HiRISE parameters necessary to derive the user
   * selected calibration units of the image data.
   *
   * @ingroup Utility
   *
   * @author 2010-04-19 Kris Becker
   * @internal
   *   @history 2010-10-28 Kris Becker Renamed parameters replacing "Ziof" with
   *            "GainUnitConversion".
   */
  class GainUnitConversion : public Module {

    public:
      //  Constructors and Destructor
      GainUnitConversion() : Module("GainUnitConversion"), _units("DN") { }
      GainUnitConversion(HiCalConf &conf, const QString &units) :
                 Module("GainUnitConversion"),  _units(units) {
        init(conf);
      }

      /** Destructor */
      virtual ~GainUnitConversion() { }

    private:
      QString   _units;

      void init(HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");

        double sed = ToDouble(prof("ScanExposureDuration"));  // units = us
        if ( IsEqual(_units, "IOF") ) {
          //  Add solar I/F correction parameters
          double au = conf.sunDistanceAU();
          _history.add("SunDist[" + ToString(au) + " (AU)]");
          double suncorr =  1.5 / au;
          suncorr *= suncorr;

          double zbin  = ToDouble(prof("GainUnitConversionBinFactor"));
          _history.add("GainUnitConversionBinFactor[" + ToString(zbin) + "]");

          double ztemp = getTempDepGain(conf, prof);
          _history.add("ScanExposureDuration[" + ToString(sed) + "]");
          double ziof = (zbin * ztemp) * (sed * 1.0e-6)  * suncorr;
          _data = HiVector(1, ziof);

          _history.add("I/F_Factor[" + ToString(ziof) + "]");
          _history.add("Units[I/F]");
        }
        else if (  IsEqual(_units, "DN/US") ) {
          // Ziof is a divisor in calibration equation
          double ziof = sed;
          _data = HiVector(1, ziof);
          _history.add("ScanExposureDuration[" + ToString(sed) + "]");
          _history.add("DN/uS_Factor[" + ToString(ziof) + "]");
          _history.add("Units[DNs/microsecond]");
        }
        else {
          // Units are already in DN
          double ziof = 1.0;
          _data = HiVector(1, ziof);
          _history.add("DN_Factor[" + ToString(ziof) + "]");
          _history.add("Units[DN]");
        }

        return;
      }

      /**
       * @brief Compute CalFact, CCD QE, Temperature I/F dependancy
       *
       * @author Kris Becker - 4/28/2010
       *
       * @param prof Profile providing parameters
       *
       * @return double Dependancy factor
       */
      double getTempDepGain(const HiCalConf &conf, const DbProfile &prof) {
        double zgain = ToDouble(prof("FilterGainCorrection"));
        _history.add("FilterGainCorrection[" + ToString(zgain) + "]");

        double fpa_py_temp = ToDouble(prof("FpaPositiveYTemperature"));
        double fpa_my_temp = ToDouble(prof("FpaNegativeYTemperature"));
        double T = (fpa_py_temp+fpa_my_temp) / 2.0;
        _history.add("T(AveFpa_YTemp)[" + ToString(T) + "]");

        double baseT = ToDouble(prof("IoverFbasetemperature"));
        _history.add("IoverFbasetemperature[" + ToString(baseT) + "]");

        double QEpcntC = ToDouble(prof("QEpercentincreaseperC"));
        _history.add("QEpercentincreaseperC[" + ToString(QEpcntC) + "]");

        double absGainTdi = ToDouble(prof("AbsGain_TDI128"));
        _history.add("AbsGain_TDI128[" + ToString(absGainTdi) + "]");

        double QETD = zgain * ( 1.0 + (T - baseT) * QEpcntC * absGainTdi);
        _history.add("CalFactQETempDep[" + ToString(QETD) + "]");
        return (QETD);
      }


  };

}     // namespace Isis
#endif
