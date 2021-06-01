#ifndef GainFlatField_h
#define GainFlatField_h

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
   * @brief GainFlatField Module - Computes flat field correction for sample
   *
   * This class computes the HiRISE flat field correction component using the A
   * matrix.
   *
   * @ingroup Utility
   *
   * @author 2008-03-04 Kris Becker
   * @history 2009-09-14 Kris Becker Removed temperature components and placed
   *          them in the GainTemperature module.
   * @history 2010-04-16 Kris Becker Modified to used the standardized CSV
   *          reader for the A matrix.
   */
  class GainFlatField : public Module {

    public:
      //  Constructors and Destructor
      GainFlatField() : Module("GainFlatField") { }
      GainFlatField(const HiCalConf &conf) : Module("GainFlatField") {
        init(conf);
      }

      /** Destructor */
      virtual ~GainFlatField() { }

      /**
       * @brief Return statistics A matrix corection
       *
       * @return const Statistics&  Statistics class with all stats
       */
      const Statistics &Stats() const { return (_stats); }

    private:
      std::string _amatrix;
      Statistics _stats;      // Stats Results

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");
        int nsamps = ToInteger(prof("Samples"));

        //  Get parameters from A-Matrix coefficients file
        _data = loadCsv("Flats", conf, prof, nsamps);
        _stats.Reset();
        for ( int i = 0 ; i < _data.dim() ; i++ ) {
          _stats.AddData(_data[i]);
        }

        _history.add("Statistics(Average["+ToString(_stats.Average())+
                     "],StdDev["+ToString(_stats.StandardDeviation())+"])");
        return;
      }

  };

}     // namespace Isis
#endif
