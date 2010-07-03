#if !defined(DriftBuffer_h)
#define DriftBuffer_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:22 $
 * $Id: DriftBuffer.h,v 1.1 2009/09/16 03:37:22 kbecker Exp $
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
#include <iostream>
#include <sstream>


#include "iString.h"
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiCalConf.h"
#include "Component.h"
#include "SplineFillComp.h"
#include "LowPassFilterComp.h"
#include "Statistics.h"
#include "SpecialPixel.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief Loads and processes Buffer calibration data
   * 
   * This class loads and processes the Buffer data from a HiRISE image for 
   * drift correction purposes.   Additional processing may occur in subsequent 
   * modules. 
   *  
   * @ingroup Utility
   * 
   * @author 2008-06-10 Kris Becker
   */
  class DriftBuffer : public Component {

    public: 
      //  Constructors and Destructor
      DriftBuffer() : Component("DriftBuffer") { }
      DriftBuffer(HiCalData &cal, const HiCalConf &conf) : 
                  Component("DriftBuffer") { 
        init(cal, conf);
      }

      /** Destructor */
      virtual ~DriftBuffer() { }

      /** 
       * @brief Return statistics for filtered - raw Buffer
       * 
       * @return const Statistics&  Statistics class with all stats
       */
      const Statistics &Stats() const { return (_stats); }

    private:
      HiVector   _buffer;
      Statistics _stats;

      void init(HiCalData &cal, const HiCalConf &conf) {
        DbProfile prof = conf.getMatrixProfile();
        _history.clear();
        _history.add("Profile["+ prof.Name()+"]");

        int samp0 = ConfKey(prof,"ZfFirstSample",0);
        int sampN = ConfKey(prof,"ZfLastSample",11);
        _buffer = averageSamples(cal.getBuffer(), samp0, sampN);
        _history.add("AveCols(Buffer["+ToString(samp0)+","+ToString(sampN)+"])");

        //  Smooth/filter the averages
        LowPassFilterComp bufter(_buffer, _history,
                          ConfKey(prof,"ZfFilterWidth",201),
                          ConfKey(prof,"ZfFilterIterations",2));
        //  If need be, fill the data with a cubic spline
        SplineFillComp spline(bufter);
        _data = spline.ref();
        _history = spline.History();

        //  Compute statistics and record to history
        _stats.Reset();
        for ( int i = 0 ; i < _data.dim() ; i++ ) {
          // Spline guarantees _data is non-null!
          if ( !IsSpecial(_buffer[i]) ) {
            _stats.AddData(_data[i] - _buffer[i]);
          }
        }
        _history.add("Statistics(Average["+ToString(_stats.Average())+
                     "],StdDev["+ToString(_stats.StandardDeviation())+"])"); 
        return;
      }

      virtual void printOn(std::ostream &o) const {
        o << "#  History = " << _history << std::endl;
        //  Write out the header
        o << std::setw(_fmtWidth)   << "RawBuffer"
          << std::setw(_fmtWidth+1) << "Filtered\n";

        for (int i = 0 ; i < _data.dim() ; i++) {
          o << formatDbl(_buffer[i]) << " "
            << formatDbl(_data[i]) << std::endl;
        }
        return;
      }

  };

}     // namespace Isis
#endif

