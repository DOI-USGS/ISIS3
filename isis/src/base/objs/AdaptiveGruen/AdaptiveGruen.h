#ifndef AdaptiveGruen_h
#define AdaptiveGruen_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Gruen.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Gruen (adaptive) pattern matching
   *
   * The AdaptiveGruen pattern/search chip registration algorithm is derived
   * from the Gruen class.  It is adaptive in that it uses an Affine transform
   * to load the subsearch chip from the search chip.  The Affine transform is
   * iteratively minimized to converge on an cummulative affine solution that
   * best matches the pattern chip.
   *
   * @ingroup PatternMatching
   *
   * @see Gruen AutoReg MinimumGruen
   *
   * @author 2009-09-09 Kris Becker
   * 
   * @internal
   *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
   *
   * @internal
   */
  class AdaptiveGruen : public Gruen {
    public:
      /**
       * @brief Construct a AdaptiveGruen search algorithm
       *
       * This will construct an AdaptiveGruen search algorithm.  It is
       * recommended that you use a AutoRegFactory class as opposed to this
       * constructor
       *
       * @param pvl  A Pvl object that contains a valid automatic registration
       * definition
       */
      AdaptiveGruen(Pvl &pvl) : Gruen(pvl) { }

      /** Destructor for AdaptiveGruen */
      virtual ~AdaptiveGruen() {}

      /**
       * AdaptiveGruen is adaptive
       * 
       * @return bool Always True
       */
      virtual bool IsAdaptive() {
        return (true);
      }

    protected:
      /** 
       * Return name of Algorithm 
       * 
       * @return QString The name of the algorithm
       */
      virtual QString AlgorithmName() const {
        return ("AdaptiveGruen");
      }


  };
};

#endif
