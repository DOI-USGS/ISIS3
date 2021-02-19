#ifndef GainNonLinearity_h
#define GainNonLinearity_h

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
#include "Statistics.h"
#include "IException.h"

namespace Isis {
  /**
   * @brief GainNonLinearity Module - Applies non-linear, line-dependant gain
   *
   * This class computes the line-based non-linearity dependant gain correction.
   * It has a comma separated value (CSV) file in the config file parameter
   * "NonLinearityGain". This file is assumed to contain one column of data: the
   * factor applied to the line average to correct non linear gain contribution.
   *
   * @ingroup Utility
   *
   * @author 2010-05-20 Kris Becker
   * @internal
   */
  class GainNonLinearity : public Module {

    public:
      //  Constructors and Destructor
      GainNonLinearity() : Module("GainNonLinearity") { }
      GainNonLinearity(const HiCalConf &conf) : Module("GainNonLinearity") {
        init(conf);
      }

      /** Destructor */
      virtual ~GainNonLinearity() { }

    private:
      std::string _gainFile;
      double _gainFactor;      // Temperature factor

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");

        //  Get temperature factor
        HiVector factor = loadCsv("NonLinearityGain", conf, prof, 1);
        _gainFactor = factor[0];
        _data = HiVector(1, _gainFactor);

        // History
        _history.add("NonLinearityGainFactor[" + ToString(_gainFactor) + "]");
        return;
      }

  };

}     // namespace Isis
#endif
