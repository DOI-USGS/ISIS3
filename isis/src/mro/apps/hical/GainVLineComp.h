#if !defined(GainVLineComp_h)
#define GainVLineComp_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:22 $
 * $Id: GainVLineComp.h,v 1.1 2009/09/16 03:37:22 kbecker Exp $
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

#include "iException.h"

namespace Isis {

  /**
   * @brief Computes a gain correction for each line
   * 
   * This class computes the HiRISE dark correction component using a 
   * combination of the B matrix, slope/intercept components and temperature 
   * profiles. 
   * 
   * @ingroup Utility
   * 
   * @author 2008-01-10 Kris Becker
   */
  class GainVLineComp : public Component {

    public: 
      //  Constructors and Destructor
      GainVLineComp() : Component("GainVLineComp") { }
      GainVLineComp(const HiCalConf &conf) : Component("GainVLine") {
        init(conf);
      }

      /** Destructor */
      virtual ~GainVLineComp() { }

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
        int skip = ConfKey(prof, "SkipLines", 0);
        _gdfile = conf.getMatrixSource("GainLineCoefficients", prof);
        _coefs = loadCsv(_gdfile, _ccd, _channel, skip);

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


      HiVector loadCsv(const std::string &fname, int ccd, int channel, 
                       int skip)  { 
        Filename csvfile(fname);
        CSVReader csv(csvfile.Expanded(), true, skip);
        CSVReader::CSVIntVector ccds  = csv.convert<int>(csv.getColumn(0));
        CSVReader::CSVIntVector chans = csv.convert<int>(csv.getColumn(1));

        for (int i = 0 ; i < ccds.dim() ; i++) {
          if ( (ccds[i] == ccd) && (chans[i] == channel) ) {
       //  Got it, extract the indicated row and translate
            CSVReader::CSVAxis coefRow = csv.getRow(i);
            HiVector coef(4);
            for (int c = 0 ; c < 4 ; c++) {
              coef[c] = (coefRow[c+2].Trim(" \r\n\t")).ToDouble();
            }
           _history.add("LoadCSV("+ fname +",Ccd[" + ToString(ccd) +
                        "],Channel[" + ToString(channel) +
                        "],Skip["+ ToString(skip) + "],Row[" +
                        ToString(i) + "])") ;
            return (coef);
          }
        }

        //  Did not find the proper row
        std::ostringstream mess;
        mess << "Cannot find Ccd/Channel (" << _ccd << "/" << _channel 
             << ") match in CSV file " << fname;
        throw iException::Message(iException::User, mess.str(), _FILEINFO_);
      }
  };

}     // namespace Isis
#endif

