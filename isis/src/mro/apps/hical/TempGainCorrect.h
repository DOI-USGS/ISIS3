#if !defined(TempGainCorrect_h)
#define TempGainCorrect_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:23 $
 * $Id: TempGainCorrect.h,v 1.1 2009/09/16 03:37:23 kbecker Exp $
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
   * @brief Zt Module - Applies temperature-dependant gain correction (column)
   *  
   * This class computes the temperature dependant gain correction.  It has a 
   * comma separated value (CSV) file in the config file parameter 
   * "FpaTemperatureFactorFile".  This file is assumed to contain three columns 
   * of data:  column 0: CCD identifier (Ex:  RED0), column 1: FPA factor for 
   * channel 0, and column 2: FPA factor for channel 1.  It should have 14 rows, 
   * 1 for each HiRISE CCD. 
   * 
   * @ingroup Utility
   * 
   * @author 2009-09-14 Kris Becker
   */
  class TempGainCorrect : public Component {

    public: 
      //  Constructors and Destructor
      TempGainCorrect() : Component("TempGainCorrect") { }
      TempGainCorrect(const HiCalConf &conf) : Component("TempGainCorrect") {
        init(conf);
      }

      /** Destructor */
      virtual ~TempGainCorrect() { }

    private:
      std::string _fpaFile;
      double _refTemp;        // Reference temperature
      double _fpaFactor;      // Temperature factor
      double _baseT;          // Base temperature

      void init(const HiCalConf &conf) {
        _history.clear();
        DbProfile prof = conf.getMatrixProfile();
        _history.add("Profile["+ prof.Name()+"]");

        //  Get parameters from gainVline coefficients file
        _fpaFile = conf.getMatrixSource("FpaTemperatureFactorFile", prof);
        _fpaFactor = fetchFactor(_fpaFile, prof);

        //  Get temperature parameters
        _refTemp = ConfKey(prof, "FpaReferenceTemperature", 21.0);

        double fpa_py_temp = ToDouble(prof("FpaPositiveYTemperature"));
        double fpa_my_temp = ToDouble(prof("FpaNegativeYTemperature"));


        double FPA_temp = (fpa_py_temp+fpa_my_temp) / 2.0;
        double _baseT = 1.0 + (_fpaFactor * (FPA_temp - _refTemp));

        //  Create data
        int nsamps = ToInteger(prof("Samples"));
        _data = HiVector(nsamps, _baseT);

        // History
        _history.add("FpaTemperatureFactor[" + ToString(_fpaFactor) + "]");
        _history.add("FpaAverageTemperature[" + ToString(FPA_temp) + "]");
        _history.add("FpaReferenceTemperature[" + ToString(_refTemp) + "]");
        _history.add("Correction[" + ToString(_baseT) + "]");
        return;
      }

      double fetchFactor(const std::string &fname, const DbProfile &prof)  { 

        //  Determine file and load characteristics
        int skip = ConfKey(prof, "FpaTemperatureFactorSkipLines", 0);
        bool header = IsTrueValue(prof, "FpaTemperatureFactorHeader");
        int nSkipped = skip + ((header) ? 1 : 0);


        // Crack the file
        Filename csvfile(fname);
        CSVReader csv(csvfile.Expanded(), header, skip);

        // Set row/column targets
        std::string ccdId = prof("Filter") + prof("Ccd");
        int channel = ToInteger(prof("Channel"));

        // Find the proper row
        for (int i = 0 ; i < csv.rows() ; i++) {
          CSVReader::CSVAxis cRow = csv.getRow(i);
          iString csvCcdId =  cRow[0];
          if (!csvCcdId.empty()) {
            if (IsEqual(ccdId, csvCcdId.Trim(" \r\n\t"))) {
              iString cfactor =  cRow[channel+1];
              if (!cfactor.empty()) {
                double factor = cfactor.Trim(" \r\n\t").ToDouble();
                _history.add("LoadCSV("+ fname +",Ccd[" + ToString(ccdId) +
                             "],Channel[" + ToString(channel) +
                             "],Skip["+ ToString(skip) + "],Row[" +
                             ToString(i+1) + "])") ;
                return (factor);
              }
              else {
                std::ostringstream mess;
                mess << "Bad (empty) value in row " << nSkipped+i+1 
                     << ", column " << channel+2
                     << " in CSV file " << fname;
                throw iException::Message(iException::User, mess.str(), _FILEINFO_);
              }
            }
          }
        }

        //  Did not find the proper row
        std::ostringstream mess;
        mess << "Cannot find Ccd/Channel (" << ccdId << "/" << channel 
             << ") match in CSV file " << fname;
        throw iException::Message(iException::User, mess.str(), _FILEINFO_);
      }

  };

}     // namespace Isis
#endif

