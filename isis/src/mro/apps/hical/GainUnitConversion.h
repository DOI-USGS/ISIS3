#if !defined(GainUnitConversion_h)
#define GainUnitConversion_h
/**                                                                       
 * @file                                                                  
 * $Revision$
 * $Date$
 * $Id$
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
#include <cmath>
#include <string>
#include <vector>

#include "iString.h"
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiCalConf.h"
#include "Module.h"
#include "Filename.h"

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
      GainUnitConversion(HiCalConf &conf, const std::string &units) : 
                 Module("GainUnitConversion"),  _units(units) {
        init(conf);
      }

      /** Destructor */
      virtual ~GainUnitConversion() { }

    private:
      std::string   _units;

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

