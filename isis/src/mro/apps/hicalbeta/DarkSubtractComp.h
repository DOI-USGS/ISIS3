#if !defined(DarkSubtractComp_h)
#define DarkSubtractComp_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $
 * $Date: 2008/06/13 22:28:55 $
 * $Id: DarkSubtractComp.h,v 1.5 2008/06/13 22:28:55 kbecker Exp $
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
#include "Component.h"
#include "Filename.h"
#include "CSVReader.h"
#include "Statistics.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief Computes a complex dark subtraction component
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
   *  
   */
  class DarkSubtractComp : public Component {

    public: 
      //  Constructors and Destructor
      DarkSubtractComp() : Component("DarkSubtract") { }
      DarkSubtractComp(const HiCalConf &conf) : Component("DarkSubtract") {
        init(conf);
      }

      /** Destructor */
      virtual ~DarkSubtractComp() { }

      /** 
       * @brief Return statistics for filtered - raw Buffer
       * 
       * @return const Statistics&  Statistics class with all stats
       */
      const Statistics &Stats() const { return (_stats); }

    private:
      int _tdi;
      int _bin;
      int _ccd;
      int _channel;

      HiVector _BM;
      HiVector _slope;
      HiVector _intercept;
      HiVector _tempProf;

      double _refTemp;

      Statistics _stats;

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");
        _tdi = ToInteger(prof("Tdi"));
        _bin = ToInteger(prof("Summing"));
        int samples = ToInteger(prof("Samples"));

        //  Get B matrix and slope/intercept parameters
        _BM = conf.getMatrix("B", prof);
        _history.add("LoadMatrix(B[" + conf.getMatrixSource("B",prof) +
                     "],Band[" + ToString(conf.getMatrixBand(prof)) + "])");

        int skip = ConfKey(prof, "SkipLines", 0);
        _slope = loadCsv(conf.getMatrixSource("Slope",prof), _tdi, skip);
        _intercept = loadCsv(conf.getMatrixSource("Intercept",prof), _tdi, skip);

        _refTemp = ConfKey(prof, "FpaReferenceTemperature", 21.0);

        //  Smooth/filter if requested
        int width =  ConfKey(prof,"ZbFilterWidth",3);
        int iters =  ConfKey(prof,"ZbFilterIterations",0);
        LowPassFilterComp smooth(width, iters);
        _history.add("Smooth(Width["+ToString(width)+"],Iters["+ToString(iters)+"])");

        //  Set average tempuratures
        double fpa_py_temp = ToDouble(prof("FpaPositiveYTemperature"));
        double fpa_my_temp = ToDouble(prof("FpaNegativeYTemperature"));
        double temp = (fpa_py_temp+fpa_my_temp) / 2.0;
        _history.add("BaseTemperature[" + ToString(temp) + "]");

        //  Filter the slope/intercept
        smooth.Process(_slope);
        _slope = smooth.ref();

        smooth.Process(_intercept);
        _intercept = smooth.ref();

        HiVector t_prof(_slope.dim());
        for (int i = 0 ; i < _slope.dim() ; i++) {
          t_prof[i] = _intercept[i] + _slope[i] * temp;
        }

        _tempProf = rebin(t_prof, samples);
        _history.add("Rebin(T_Profile," + ToString(t_prof.dim()) + "," +
                     ToString(samples) +")");

        HiVector dc(samples);
        double linetime = ToDouble(prof("ScanExposureDuration"));
        double scale = linetime * 1.0E-6 * (_bin*_bin) * 
                       (20.0*103.0/89.0 + _tdi); 
        double baseT = HiTempEqn(_refTemp);
        for (int j = 0 ; j < samples ; j++) {
          dc[j] = _BM[j] * scale * HiTempEqn(_tempProf[j]) / baseT;
        }

        //  Filter it yet again
        smooth.Process(dc);
        _data = smooth.ref();

        //  Compute statistics and record to history
        _stats.Reset();
        for ( int i = 0 ; i < _data.dim() ; i++ ) {
          _stats.AddData(_data[i]);
        }
        _history.add("Statistics(Average["+ToString(_stats.Average())+
                     "],StdDev["+ToString(_stats.StandardDeviation())+"])");
        return;
      }

      HiVector loadCsv(const std::string &fname, int tdi, int skip)  {
        Filename csvfile(fname);
        CSVReader csv(csvfile.Expanded(), true, skip);
        iString myHead = "TDI " + iString(tdi);

        CSVReader::CSVAxis header(csv.getHeader());
        for (int i = 0 ; i < header.dim() ; i++) {
          if (iString::Equal(myHead,iString(header[i]).Trim(" \r\t"))) {
           _history.add("LoadCSV("+ fname +",TDI[" + ToString(tdi) + 
                        "],Skip["+ ToString(skip) + "],Column[" +
                        ToString(i) + "])") ;
           return (convert(csv.getColumn(i)));
          }
        }

        std::ostringstream mess;
        mess << "Column " << myHead <<  " not found in file " << fname
             << "!";
        throw iException::Message(iException::User, mess.str(), _FILEINFO_);
      }

      HiVector convert(const CSVReader::CSVAxis &v) const {
        HiVector newV(v.dim());
        for ( int i = 0 ; i < v.dim() ; i++ ) {
          newV[i] = ToDouble(v[i]);
        }
        return (newV);
      }

      virtual void printOn(std::ostream &o) const {
        o << "#  History = " << _history << std::endl;
        //  Write out the header
        o << std::setw(_fmtWidth)   << "BMatrix"
          << std::setw(_fmtWidth+1) << "RebinTemp"
          << std::setw(_fmtWidth+1) << "Dark\n";

        for (int i = 0 ; i < _data.dim() ; i++) {
          o << formatDbl(_BM[i]) << " "
            << formatDbl(_tempProf[i]) << " "
            << formatDbl(_data[i]) << std::endl;
        }
        return;
      }

  };

}     // namespace Isis
#endif

