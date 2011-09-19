#if !defined(GainNonLinearity_h)
#define GainNonLinearity_h
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
#include "Statistics.h"
#include "iException.h"

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

