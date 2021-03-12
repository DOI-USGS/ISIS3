#ifndef LunarLambertMcEwen_h
#define LunarLambertMcEwen_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Moonpr photometric model
   *  Computes normalized albedo for the Moon,
   *  normalized to 0 degrees emission angle and
   *  30 degrees illumination and phase angles.
   *
   * @author 1995-11-27 Alfred McEwen
   *
   * @internal
   */
  class LunarLambertMcEwen : public PhotoModel {
    public:
      LunarLambertMcEwen(Pvl &pvl);
      virtual ~LunarLambertMcEwen() {};

    protected:
      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

    private:
      double p_photoM1;
      double p_photoM2;
      double p_photoM3;
      double p_photoR30;
  };
};

#endif
