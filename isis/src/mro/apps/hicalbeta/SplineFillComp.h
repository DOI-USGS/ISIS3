#if !defined(SplineFillComp_h)
#define SplineFillComp_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $
 * $Date: 2008/11/06 00:08:15 $
 * $Id: SplineFillComp.h,v 1.2 2008/11/06 00:08:15 jwalldren Exp $
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

#include <string>
#include <vector>

#include "iString.h"
#include "Component.h"
#include "NumericalApproximation.h"
#include "SpecialPixel.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief Compute a low pass filter from a Component class content
   * 
   * @ingroup Utility
   * 
   * @author 2007-10-09 Kris Becker 
   * @history 2008-11-05 Jeannie Walldren Replaced references to 
   *  DataInterp class with NumericalApproximation.
   */
  class SplineFillComp : public Component {

    public: 
      //  Constructors and Destructor
      SplineFillComp() : Component("SplineFill"), _filled(0) { }

      SplineFillComp(const Component &c) : 
                     Component("SplineFill", c), _filled(0) {
        fill(c.ref());
        _history.add(formHistory());
      }

      SplineFillComp(const HiVector &v) : 
                     Component("SplineFill"), _filled(0) {
        fill(v);
        _history.add(formHistory());
      }
      SplineFillComp(const HiVector &v, const HiHistory &h) : 
                     Component("SplineFill", h), _filled(0) {
        fill(v);
        _history.add(formHistory());
      }

      /** Destructor */
      virtual ~SplineFillComp() { }

      void Process(const HiVector &v) {
        fill(v);
        _history.clear();
        _history.add(formHistory());
      }

      inline int Filled() const { return (_filled); }

    private:
      int   _filled;         //!< Number values replaced

      std::string formHistory() {
        iString cfilled(_filled);
        return (std::string("SplineFill(Cubic,Filled[" + cfilled + "])"));
      }

      void fill(const HiVector &v) {
        NumericalApproximation spline(NumericalApproximation::CubicNatural);
        for (int i = 0 ; i < v.dim() ; i++) {
          if (!IsSpecial(v[i])) {
            spline.AddData(i, v[i]); 
          }
        }

        //  Compute the spline and fill missing data
        HiVector vout(v.dim());
        _filled = 0;
        for (int j = 0 ; j < v.dim() ; j++) {
          if (IsSpecial(v[j])) {
            vout[j] = spline.Evaluate(j,NumericalApproximation::NearestEndpoint); 
            _filled++;
          }
          else {
            vout[j] = v[j];
          }
        }

        _data = vout;
        return;
      }
  };

}     // namespace Isis
#endif

