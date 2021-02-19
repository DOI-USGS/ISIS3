#ifndef LommelSeeliger_h
#define LommelSeeliger_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PhotoModel.h"

namespace Isis {
  class Pvl;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class LommelSeeliger : public PhotoModel {
    public:
      LommelSeeliger(Pvl &pvl) : PhotoModel(pvl) {};
      virtual ~LommelSeeliger() {};

    protected:
      virtual double PhotoModelAlgorithm(double phase, double incidence,
                                         double emission);

  };
};

#endif
