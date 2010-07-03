#if !defined(FlatFieldComp_h)
#define FlatFieldComp_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $
 * $Date: 2009/09/15 21:56:44 $
 * $Id: FlatFieldComp.h,v 1.4 2009/09/15 21:56:44 kbecker Exp $
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
   * @brief Za Module - Computes flat field correction for each sample (column)
   * 
   * This class computes the HiRISE flat field correction component using the A
   * matrix.
   * 
   * @ingroup Utility
   * 
   * @author 2008-03-04 Kris Becker 
   * @history 2009-09-14 Kris Becker Removed temperature components and placed 
   *          them in the Zt (TempGainCorrect) module.
   */
  class FlatFieldComp : public Component {

    public: 
      //  Constructors and Destructor
      FlatFieldComp() : Component("FlatFieldComp") { }
      FlatFieldComp(const HiCalConf &conf) : Component("FlatField") {
        init(conf);
      }

      /** Destructor */
      virtual ~FlatFieldComp() { }

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

        //  Get parameters from gainVline coefficients file
        _amatrix = conf.getMatrixSource("A", prof);
        _data = conf.getMatrix("A",prof);
        _history.add("LoadMatrix(A[" + _amatrix +
                    "],Band[" + ToString(conf.getMatrixBand(prof)) + "])");

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

