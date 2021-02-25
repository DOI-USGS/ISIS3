/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>
#include "ThemisVisDistortionMap.h"


using namespace std;

namespace Isis {
  /**
   * Constructs a Distortion Map for the Themis Vis Camera
   *
   * @param parent Pointer to the parent Camera object
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.  Fixed
   *                          documentation.
   */
  ThemisVisDistortionMap::ThemisVisDistortionMap(Camera *parent) :
    CameraDistortionMap(parent, 1.0) {
    // Set necessary constant values

    // The IR pixel pitch is used by VIS processing because optical distortion corrections are in
    // terms of IR pixels. Note that the pixel pitch for the instrument currently being processed
    // is stored in ISIS2's ccd.mmpp.
    // from IR_PIXEL_PITCH in ISIS2's thm_parameters.def.N
    p_irPixelPitch = 0.05; // in meters per pixel
    p_visPixelPitch = 0.009;// in meters per pixel

    // This value is computed from IR_BAND_FIRST_ROW[5] and
    // IR_BAND_LAST_ROW[5] in ISIS2's thm_parameters.def.N
    p_ir_b5_effectiveDetectorLine = (95.0 + 110.0) / 2.0;

    // from IR_BORESIGHT_LINE in ISIS2's thm_parameters.def.N
    p_irBoreLine = 109.5;
  }



  ThemisVisDistortionMap::~ThemisVisDistortionMap() {
  }



  /**
   * Sets the focal plane value for the distortion map
   *
   * @param dx The focal plane x value
   * @param dy The focal plane y value
   *
   * @return bool Returns true if the set was successful and false if it was
   *              not
   */
//  Optical distortion correction for VIS from lev1u_m01_thm_linesamp_to_pointing() in lev1u_m01_thm_routines.c */

//  bool ThemisVisDistortionMap::SetFocalPlane(const double dx,
//                                             const double dy) {
//     // first set the distorted x/y, in meters
//     p_focalPlaneX = dx;
//     p_focalPlaneY = dy;
//
//
//     // Polynomial coefficients for doing optical distortion corrections for the X and Y directions.
//     // These are for going from pointing to samp/line.
//     double vis_od_icx[] = { -4.02919e-5, 0.0, 0.0 };// VIS optical distortion inverse coefficients for x
//     double vis_od_icy[]  = { -0.0176112, -0.00718251, 5.52591e-5 };
//
//     // Vertical offset from boresight in IR pixels - positive is DOWN */
//     double jp =      p_focalPlaneY  / p_irPixelPitch;
//
//     // IR distortion in vertical direction (pixels) */
//     // Note that (-jp) is used here because the orientation of positive displacement from the boresight
//     // in the Y direction assumed for the correction parameters is opposite of the positive direction
//     // for j.
//     double deltajp =      vis_od_icy[0] + (     vis_od_icy[1] * (-jp)) + (     vis_od_icy[2] * (-jp) * (-jp));
//
//     // Horizontal offset from boresight in IR pixels */
//     double ip =      p_focalPlaneX  / p_irPixelPitch;
//
//     // IR stretch factor
//     double cb1 =      vis_od_icx[0] * ((-jp) +       p_irBoreLine -    ir_b5_effective_detector_line - deltajp);
//
//     // Focal plane horizontal position (in VIS pixels) with the distortion removed
//     p_undistortedFocalPlaneX = ip *    p_irPixelPitch   * (1.0 + (cb1 / (1.0 - cb1)));
//     p_undistortedFocalPlaneY = (jp - deltajp) *    p_irPixelPitch;
//    return true;
//  }

  /**
   * Sets the undistorted focal plane value for the distortion map
   *
   * @param ux The undistorted focal plane x value
   * @param uy The undistorted focal plane y value
   *
   * @return bool Returns true if the set was successful and false if it was
   *              not
   */
  bool ThemisVisDistortionMap::SetUndistortedFocalPlane(const double ux,
      const double uy) {
    // See the optical distortion correction for VIS section in the
    // lev1u_m01_thm_vis_pointing_to_linesamp() routine in lev1u_m01_thm_routines.c

    // Set the focal plane coordinates, in pixels
    //     X direction is perpendicular to the along - track direction;
    //     Y direction is parallel to along-track direction
    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    // Constant values
    // Polynomial coefficients for doing optical distortion corrections for the X and Y directions.
    // These are for going from samp/line to pointing. Given the X and Y components of the angle
    // between the boresight and the look vector for a given detector sample/line coordinate,
    // these factors are used to compute a detector sample coordinate that will give a look
    // vector corresponding to where the given look vector (detector sample/line) is
    // really looking on the planet
    // (values read from VIS_OD_CX and VIS_OD_CY[N] in thm_parameters.def.7.2, last updated 03/2003)
    double vis_od_cx[] = { -4.02919e-5, 0.0, 0.0 }; // VIS optical distortion coefficients for x
    double vis_od_cy[] = { -0.0178649, -0.00727843, 5.65278e-5 };

    // Calculate necessary intermediate values

    // compute IR pixels(lines) relative to boresight
    double j = p_undistortedFocalPlaneY / p_irPixelPitch;

    // IR distortion in Y (pixels)
    // Note that (-j) is used here because the orientation of positive displacement from the boresight in the Y
    // direction assumed for the correction parameters is opposite of the positive direction for j.
    double deltaj = vis_od_cy[0] + ( vis_od_cy[1] * (-j)) + ( vis_od_cy[2] * (-j) * (-j));

    // IR stretch factor in X direction
    // Note that (-j) is used here because the orientation of positive displacement from the boresight in the Y
    // direction assumed for the correction parameters is opposite of the positive direction for j.
    double cb1 = vis_od_cx[0] * ((-j) + p_irBoreLine - p_ir_b5_effectiveDetectorLine);

    // Corrected X location relative to boresight in focal plane of ideal camera (meters)
    // Focal plane coordinates, in meters
    p_focalPlaneX = p_undistortedFocalPlaneX * (1.0 + cb1);
    p_focalPlaneY = p_irPixelPitch * (j + deltaj);

    return true;
  }



  /**
   * Sets the focal plane value for the distortion map
   *
   * @param dx The focal plane x value
   * @param dy The focal plane y value
   *
   * @return bool Returns true if the set was successful and false if it was
   *              not
   */
  bool ThemisVisDistortionMap::SetFocalPlane(const double dx,
                                             const double dy) {
    // VIS optical distortion inverse coefficients for x and y, respectively
    double vis_od_cx[] = { -4.02919e-5, 0.0, 0.0 };
    double vis_od_cy[] = { -0.0178649, -0.00727843, 5.65278e-5 };
    int    numAttempts;
    double delta;
    bool   done = false;

    // set the focal plane coordinates, in meters relative to boresight
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    numAttempts = 1;
    delta = 0.00001;

    double dx_guess, dy_guess;
    double ux_guess, uy_guess;

    // "solve" undistorted focal plane equations for ux and uy to get beginning offset
    double xOffset  = -dy / p_irPixelPitch * vis_od_cx[0]
                      + vis_od_cx[0] * p_irBoreLine
                      - vis_od_cx[0] * p_ir_b5_effectiveDetectorLine;
    double yOffset  =  p_irPixelPitch * vis_od_cy[0] / dy
                      - vis_od_cy[1]
                      + dy / p_irPixelPitch * vis_od_cy[2];

    while (!done) {
    // In this loop, we will use the current offset to make a guess at the
    // undistorted focal plane coordinate that corresponds to the known
    // distorted focal plane coordinate.
    //
    // The offset is updated using the undistored guess.
    //
    // Each undistorted guess will be tested by reversing the equation used to
    // compute the current undistorted guess to get a corresponding distorted
    // coordinate.
    //
    // If this distorted coordinate that corresponds to the current undistorted
    // guess is close enough to the known distorted coordinate, then we will
    // accept the corresponding undistorted coordinate as our solution and end
    // the loop.
      // guesses are based on the known distorted x/y values and the offset
      ux_guess = dx / (1.0 + xOffset);
      uy_guess = dy / (1.0 + yOffset);

      // offset is updated with each undistored x and y guess
      xOffset = -uy_guess / p_irPixelPitch * vis_od_cx[0]
                + vis_od_cx[0] * p_irBoreLine
                - vis_od_cx[0] * p_ir_b5_effectiveDetectorLine;
      yOffset =  p_irPixelPitch * vis_od_cy[0] / uy_guess
                - vis_od_cy[1]
                + uy_guess / p_irPixelPitch * vis_od_cy[2];

      // find the distorted x/y corresponding to the undistorted x/y guess
      dx_guess = ux_guess * (1.0 + xOffset);
      dy_guess = uy_guess * (1.0 + yOffset);

      // if the  distorted (x,y) corresponding to the undistorted (x,y) guesses
      //  are close enough to the known distorted (x,y), then we are done...
      if ((abs(dy_guess - dy) < delta) && (abs(dx_guess - dx) < delta)) {
        done = true;
      }

      // if we exceed the number of allowed attempts, return that this method failed
      numAttempts++;
      if (numAttempts > 20) {
        return false;
      }
    }

    // use the undistorted x/y whose corresponding distorted x/y are both close enough to the known
    // distorted x/y
    p_undistortedFocalPlaneX = ux_guess;
    p_undistortedFocalPlaneY = uy_guess;
    return true;

  }


}
//svn commit -m "PROG: Documented existing optical distortion algorithms in ThemisVisDistortionMap using information from ISIS2 lev1u_m01_thm_routines.c."  src/odyssey/objs/ThemisVisCamera/ThemisVisDistortionMap.cpp src/odyssey/objs/ThemisVisCamera/ThemisVisDistortionMap.h
