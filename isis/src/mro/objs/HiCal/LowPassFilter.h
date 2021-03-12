#ifndef LowPassFilter_h
#define LowPassFilter_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                      

#include <string>
#include <vector>

#include "IString.h"
#include "HiCalUtil.h"
#include "Module.h"
#include "QuickFilter.h"
#include "IException.h"

namespace Isis {

  /**
   * @brief Compute a low pass filter from a Module class content
   *
   * @ingroup Utility
   *
   * @author 2007-10-09 Kris Becker
   */
  class LowPassFilter : public Module {

    public:
      //  Constructors and Destructor
      LowPassFilter() : Module("LowPassFilter"), _width(3),
                            _iterations(1) { }
      LowPassFilter(int width, int iterations = 1) :
        Module("LowPassFilter"), _width(width), _iterations(iterations) { }

      LowPassFilter(const Module &c, int width = 3, int iterations = 1) :
                        Module("LowPassFilter", c), _width(width),
                        _iterations(iterations) {
        _data = filterIterator(c.ref(), _width, _iterations);
        _history.add(formHistory());
      }

      LowPassFilter(const HiVector &v, const HiHistory &h,
                        int width = 3, int iterations = 1) :
                        Module("LowPassFilter", h), _width(width),
                        _iterations(iterations) {
        _data = filterIterator(v, _width, _iterations);
        _history.add(formHistory());
      }

      /** Destructor */
      virtual ~LowPassFilter() { }

      void Process(const HiVector &v) {
        _data = filterIterator(v, _width, _iterations);
        _history.clear();
        _history.add(formHistory());
      }

      inline int Width() const { return (_width); }

    private:
      int   _width;         //!< Filter width
      int   _iterations;    // Number iterations to apply filter

      QString formHistory() {
        return (QString("LowPassFilter(Width[" + ToString(_width) +
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
