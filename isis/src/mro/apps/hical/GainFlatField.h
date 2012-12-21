#ifndef GainFlatField_h
#define GainFlatField_h
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
   *
   * @internal
   *   @history 2009-09-14 Kris Becker Removed temperature components and placed 
   *                           them in the GainTemperature module.
   *   @history 2010-04-16 Kris Becker Modified to used the standardized CSV 
   *                           reader for the A matrix.
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
      QString _amatrix;
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

