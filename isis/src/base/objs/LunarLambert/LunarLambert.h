#ifndef LunarLambert_h
#define LunarLambert_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Lunar (Lommel-Seeliger)-Lambert law photometric model
   * Derive model albedo for Lunar (Lommel-Seeliger)-Lambert law.
   * Phase independent and calculated analytically.
   * Lommel-Seeliger law:
   *
   * Reflectance=1/(1+cos(E)/cos(I))
   *
   * Where: E=the angle between the observer and the slope normal
   *       I=the angle between the sun and the slope normal
   *
   * @author 1999-01-08 Randy Kirk
   *
   * @internal
   *  @history 2007-07-31 Steven Lambright - Moved PhotoL from base PhotoModel class
   *                       to this child.
   */
  class LunarLambert : public PhotoModel {
    public:
      LunarLambert(Pvl &pvl);
      virtual ~LunarLambert() {};

      void SetPhotoL(const double l);

      //! Return photometric L value
//      inline double PhotoL() const {
//        return p_photoL;
//      };

    protected:
      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);
      
    private:
//      double p_photoL;

  };
};

#endif
