#ifndef GainLineDrift_h
#define GainLineDrift_h
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
      QString _gdfile;
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

