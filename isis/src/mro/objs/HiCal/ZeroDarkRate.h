#ifndef ZeroDarkRate_h
#define ZeroDarkRate_h

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
#include "LoadCSV.h"
#include "LowPassFilter.h"
#include "Statistics.h"
#include "IException.h"

namespace Isis {

  /**
   * @brief Computes a complex dark subtraction component (ZeroDarkRate module)
   *
   * This class computes the HiRISE dark correction component using a
   * combination of the B matrix, slope/intercept components and temperature
   * profiles.
   *
   * @ingroup Utility
   *
   * @author 2008-01-10 Kris Becker
   * @internal
   * @history 2008-06-13 Kris Becker - Added PrintOn method to produce more
   *          detailed data dump;  Added computation of statistics
   * @history 2010-04-16 Kris Becker - Implemented standardized access to CSV
   *          files.
   * @history 2010-10-28 Kris Becker Renamed parameters replacing "Zb" with
   *            "ZeroDarkRate"
   * @history 2021-02-28 Moses Milazzo Copied "ZeroDarkRate.h" to new file 
   * 		"ZeroDarkRate.h" to add new dark current correction model.
   * 		This model no longer uses the B matrices and instead uses
   * 		a temperature-dependent exponential model to calculate
   * 		the correction on a per-image basis.
   * 		dc_rate = a * exp(b*FPA_T) + c
   *
   */
  class ZeroDarkRate : public Module {

    public:
      //  Constructors and Destructor
      ZeroDarkRate() : Module("ZeroDarkRate") { }
      ZeroDarkRate(const HiCalConf &conf) : Module("ZeroDarkRate") {
        init(conf);
      }

      /** Destructor */
      virtual ~ZeroDarkRate() { }

      /**
       * @brief Return statistics for filtered - raw Buffer
       *
       * @return const Statistics&  Statistics class with all stats
       */
      const Statistics &Stats() const { return (_stats); }

    private:
      int _tdi;
      int _bin;

      /**
       * The coefficients are stored in a csv text file as a 3-column, 1024/bin row
       * matrix. 
       * Each coefficient has 1024/bin elements, so must be treated as a vector.
       */
      HiVector _coeff_a;
      HiVector _coeff_b;
      HiVector _coeff_c;
      HiVector _adc_set;
      //HiVector _coeffs; // I'm not sure if this is needed yet.

      Statistics _stats;

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");
        _tdi = ToInteger(prof("Tdi"));
        _bin = ToInteger(prof("Summing"));
        int samples = ToInteger(prof("Samples"));

	// Cause this file not to compile so that someone who knows what they're doing
	// can make this next part work.
	DEBUG
	// Load the coefficients 
	// I don't really know how loadCsv works. The CSV files for this module are named:
	// file: DarkRate_CCD_Ch_TDI${tdi}_BIN{$binning}_hical_????.csv
	// Example:
	// DarkRate_RED1_1_TDI64_BIN2_hical_0002.csv
	// The format of the file is as follows:
	// There are three header lines, with a '#' sign as a comment:
	// # Number of files used to generate these values = 40 
	// # exponential equation: DC_Rate = a * exp(b * FPA Temperature) + c 
	// # a, b, c 
	// Then the coefficients begin.
	// Three columns (a, b, c), and 1024/binning rows
	// 2.483618177203812394e+00,2.255885064806690821e-01,5.617339162650616345e+03
	// I don't know how loadCsv can be used to load this into memory.
	// Later in this file, I assume I have three coefficient variables:
	// _coeff_a, _coeff_b, and _coeff_c
	// Each of these variables is a vector with 1024/binning elements.
        _coeffs = loadCsv("DarkRate", conf, prof, samples);
	_coeff_a = _coeffs[0];
	_coeff_b = _coeffs[1];
	_coeff_c = _coeffs[2];

        //  Set average FPA temperature
        double fpa_py_temp = ToDouble(prof("FpaPositiveYTemperature"));
        double fpa_my_temp = ToDouble(prof("FpaNegativeYTemperature"));
        double temp = (fpa_py_temp+fpa_my_temp) / 2.0;
        _history.add("BaseTemperature[" + ToString(temp) + "]");

	// Calculate the dark rate for each column.
        HiVector dc(samples);
        for (int j = 0 ; j < samples ; j++) {
          dc[j] = _coeff_a[j] * exp(_coeff_b[j]*temp) + _coeff_c[j];
        }

        //  Compute statistics and record to history
        _stats.Reset();
        for ( int i = 0 ; i < _data.dim() ; i++ ) {
          _stats.AddData(_data[i]);
        }
        _history.add("Statistics(Average["+ToString(_stats.Average())+
                     "],StdDev["+ToString(_stats.StandardDeviation())+"])");
        return;
      }


      /** Virtualized data dump method */
      virtual void printOn(std::ostream &o) const {
        o << "#  History = " << _history << std::endl;
        //  Write out the header
        o << std::setw(_fmtWidth+1) << "FPA_Temperature\n";
          << std::setw(_fmtWidth+1) << "ZeroDarkRate\n";

        for (int i = 0 ; i < _data.dim() ; i++) {
          o << formatDbl(_temp) << " "
            << formatDbl(_data[i]) << std::endl;
        }
        return;
      }

  };

}     // namespace Isis
#endif
