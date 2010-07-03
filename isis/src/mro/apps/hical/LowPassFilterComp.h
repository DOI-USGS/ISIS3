#if !defined(LowPassFilterComp_h)
#define LowPassFilterComp_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:23 $
 * $Id: LowPassFilterComp.h,v 1.1 2009/09/16 03:37:23 kbecker Exp $
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
#include "HiCalUtil.h"
#include "Component.h"
#include "QuickFilter.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief Compute a low pass filter from a Component class content
   * 
   * @ingroup Utility
   * 
   * @author 2007-10-09 Kris Becker
   */
  class LowPassFilterComp : public Component {

    public: 
      //  Constructors and Destructor
      LowPassFilterComp() : Component("LowPassFilter"), _width(3), 
                            _iterations(1) { }
      LowPassFilterComp(int width, int iterations = 1) : 
        Component("LowPassFilter"), _width(width), _iterations(iterations) { }

      LowPassFilterComp(const Component &c, int width = 3, int iterations = 1) : 
                        Component("LowPassFilter", c), _width(width), 
                        _iterations(iterations) {
        _data = filterIterator(c.ref(), _width, _iterations);
        _history.add(formHistory());
      }

      LowPassFilterComp(const HiVector &v, const HiHistory &h,
                        int width = 3, int iterations = 1) : 
                        Component("LowPassFilter", h), _width(width), 
                        _iterations(iterations) {
        _data = filterIterator(v, _width, _iterations);
        _history.add(formHistory());
      }

      /** Destructor */
      virtual ~LowPassFilterComp() { }

      void Process(const HiVector &v) {
        _data = filterIterator(v, _width, _iterations);
        _history.clear();
        _history.add(formHistory());
      }

      inline int Width() const { return (_width); }

    private:
      int   _width;         //!< Filter width
      int   _iterations;    // Number iterations to apply filter

      std::string formHistory() {
        return (std::string("LowPassFilter(Width[" + ToString(_width) + 
                            "],Iters["+ToString(_iterations)+"])"));
      }

      HiVector filterIterator(const HiVector &v, int width, int iterations) {
        HiVector vout(v.copy());
        for (int i = 0 ; i < iterations; i++) {
          vout = filter(vout,width);
        }
        return (vout);
      }

      HiVector filter(const HiVector &v, int width) {
        QuickFilter lowpass(v.dim(), width, 1);
        lowpass.AddLine(&v[0]);
        HiVector vout(v.dim());
        for (int i = 0 ; i < v.dim() ; i ++) {
          vout[i] = lowpass.Average(i);
        }
        return (vout);
      }
  };

}     // namespace Isis
#endif

