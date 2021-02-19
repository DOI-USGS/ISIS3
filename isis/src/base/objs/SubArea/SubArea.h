#ifndef SubArea_h
#define SubArea_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include "Pvl.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief  Apply corrections to a cube label for subarea extraction
   *
   * This class is used to apply corrections to a cube label when a subarea
   * has been extracted. It is a base class which will take the cube label from
   * the original cube file along with subarea information and generate a
   * corrected cube label for the output cube file.
   *
   * If you would like to see SubArea being used in implementation, see
   * crop, reduce, enlarge, cropspecial, or pad.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2009-10-02 Janet Barrett
   *
   * @internal
   *  @history 2010-11-21 Mackenzie Boyd - Modified UpdateLabels method so
   *                                       so that the units for Scale and
   *                                       PixelResolution keywords are
   *                                       perserved.
   *  @history 2011-01-24 Steven Lambright - Cameras are still valid when linc
   *                                         and sinc are not equal, so now
   *                                         this class is more likely to not
   *                                         delete the instrument group.
   *  @history 2013-12-27 Janet Barrett - Modified the UpdateLabel method so that
   *                                      it updates the MinimumLatitude, MaximumLatitude,
   *                                      MinimumLongitude, and MaximumLongitude keywords
   *                                      for equatorial cylindrical projections. All other
   *                                      projections will not have these keywords in their
   *                                      labels because it is not possible to recalculate
   *                                      them for curved projections. Fixes #1500.
   */

  class SubArea {
    public:
      SubArea() {};

      ~SubArea() {};

      // Define the subarea
      void SetSubArea(const int orignl, const int origns, const int sl,
                      const int ss, const int el, const int es,
                      const double linc, const double sinc);

      // Create an updated label for a subarea file
      void UpdateLabel(Cube *icube, Cube *ocube, PvlGroup &results);

    private:
      int p_sl;            //!<Starting line of subarea
      int p_ss;            //!<Starting sample of subarea
      int p_el;            //!<Ending line of subarea
      int p_es;            //!<Ending sample of subarea
      int p_nl;            //!<Number of lines in original file
      int p_ns;            //!<Number of samples in original file
      double p_linc;       //!<Line increment for subarea
      double p_sinc;       //!<Sample increment for subarea
  };
};
#endif
