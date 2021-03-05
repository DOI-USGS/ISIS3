#ifndef LoCameraFiducialMap_h
#define LoCameraFiducialMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "PvlGroup.h"

namespace Isis {
  /**
   * @brief Computes map from image coordinates to focal plane based on fiducials
   *
   * The LoCameraFiducialMap class allows for the computation of a transformation
   * from image coordinates (sample,line) to focal plane coordinates (x,y) for
   * either the Lunar Orbiter High Resolution Camera or the Lunar Orbiter Medium
   * resolution camera for any of the last three Lunar Orbiter missions.  The
   * transformation map is an affine transformation defined by values written in
   * the Isis Instrument group labels.
   *
   * This class will load the fiducial sample/line and x/y values from the labels,
   * compute the coefficients of the affine transformation, and place the
   * coefficients in to the data pool.  Typically these values are read from an
   * iak, but for Lunar Orbiter they are frame dependent.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarOrbiter
   *
   * @author 2007-07-17 Debbie A. Cook
   *
   * @internal
   *   @history 2007-07-17 Debbie A. Cook - Original Version
   *   @history 2007-11-01 Debbie A. Cook - Revised to handle medium resolution camera
   *   @history 2008-06-18 Steven Lambright - Fixed documentation
   *   @history 2010-09-23 Debbie A. Cook - Added std before vector declarations
   *                          to avoid confusion with boost vector
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation. Removed Lo
   *                         namespace wrap inside Isis namespace wrap.
   */
  class LoCameraFiducialMap  {
    public:
      LoCameraFiducialMap(PvlGroup &inst, const int naifIkCode);
      //! Destroys LoCameraFiducialMap object.
      ~LoCameraFiducialMap() {};

    private:
      void ReadFiducials(PvlGroup &inst);
      void CreateTrans(int xdir);
      std::vector<double> p_fidSamples; //!< Image sample positions of fiducial map
      std::vector<double> p_fidLines;   //!< Image line positions of fiducial map
      std::vector<double> p_fidXCoords; //!< Focal plane X positions of fiducial map
      std::vector<double> p_fidYCoords; //!< Focal plane Y positions of fiducial map
      int p_naifIkCode;            //!< Naif instrument code

  };
};
#endif
