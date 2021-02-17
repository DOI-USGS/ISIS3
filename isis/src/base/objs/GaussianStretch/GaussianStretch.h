#ifndef GaussianStretch_h
#define GaussianStretch_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Statistics.h"
#include "Histogram.h"
#include "Stretch.h"
#include "IException.h"
#include "Constants.h"

namespace Isis {
  /**
   * @brief Gaussian stretch class
   *
   * This class is used to stretch the input histogram to a
   * gaussian distribution with the specified mean and standard
   * deviation.
   *
   * @ingroup Utility
   *
   * @author 2006-05-25 Jacob Danton
   *
   * @internal
   *   @history 2006-05-25 Jacob Danton Original Version
   *   @history 2006-10-28 Stuart Sides Fixed stretch pair ordering
   *   @history 2008-09-09 Steven Lambright Fixed stretch pair ordering again;
   *            this fix does not solve our problem but makes our tests work and
   *            isn't wrong.
   */
  class GaussianStretch : public Isis::Statistics {
    public:
      GaussianStretch(Histogram &histogram, const double mean = 0.0, const double standardDeviation = 1.0) ;
      ~GaussianStretch() {};

      double Map(const double value) const;
    private:
      //! Value of the mean
      Stretch p_stretch;
  };
};

#endif
