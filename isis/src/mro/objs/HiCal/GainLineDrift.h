#ifndef GainLineDrift_h
#define GainLineDrift_h

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
   * @brief Computes a gain correction for each line (Zg Module)
   *
   * This class computes the HiRISE gain component correction for each line.
   * Time dependant line drift correction also governed by parameters in config
   * file and LineGainDrift coefficients matrix file.
   *
   * @ingroup Utility
   *
   * @author 2008-01-10 Kris Becker
   * @internal
   *   @history 2010-04-16 Kris Becker Modifed to use standardized CSV file
   *            format.
   */
  class GainLineDrift : public Module {

    public:
      //  Constructors and Destructor
      GainLineDrift() : Module("GainLineDrift") { }
      GainLineDrift(const HiCalConf &conf) : Module("GainLineDrift") {
        init(conf);
      }

      /** Destructor */
      virtual ~GainLineDrift() { }

    private:
      std::string _gdfile;
      int _ccd;
      int _channel;
      HiVector _coefs;

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");
        _ccd = CpmmToCcd(ToInteger(prof("CpmmNumber")));
        _channel = ToInteger(prof("ChannelNumber"));

        //  Get parameters from gainVline coefficients file
        _coefs = loadCsv("LineGainDrift", conf, prof, 4);
        _history.add("Coefs["+ToString(_coefs[0])+ ","+
                              ToString(_coefs[1])+ ","+
                              ToString(_coefs[2])+ ","+
                              ToString(_coefs[3])+ "]");

        int bin = ToInteger(prof("Summing"));
        double linetime = ToDouble(prof("ScanExposureDuration"));
        HiLineTimeEqn timet(bin, linetime);
        int nlines = ToInteger(prof("Lines"));

        HiVector gainV(nlines);
        for ( int i = 0 ; i < nlines ; i++ ) {
          double lt = timet(i);
          gainV[i] = _coefs[0] + (_coefs[1] * lt) +
                     _coefs[2] * exp(_coefs[3] * lt);
        }

        _data = gainV;
        return;
      }

  };

}     // namespace Isis
#endif
