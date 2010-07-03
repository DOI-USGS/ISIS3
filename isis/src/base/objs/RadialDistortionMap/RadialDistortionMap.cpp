#include "RadialDistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;

namespace Isis {
  RadialDistortionMap::RadialDistortionMap (Camera *parent, double k1, double zDirection) : CameraDistortionMap(parent, zDirection) {
    p_k1 = k1;
  }

  // Compute undistorted focal plane x/y.  
  bool RadialDistortionMap::SetFocalPlane (const double dx, const double dy) {
    double offsetSqrd;

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    offsetSqrd = dx * dx + dy * dy;

    p_undistortedFocalPlaneX = dx * (1.0 + p_k1 * offsetSqrd);
    p_undistortedFocalPlaneY = dy * (1.0 + p_k1 * offsetSqrd);

    return true;
  }

  bool RadialDistortionMap::SetUndistortedFocalPlane (const double ux, const double uy) {
    double offsetSqrd;
    int    numAttempts;
    double delta;
    bool    done;

    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

  /****************************************************************************
  * Pre-loop intializations
  ****************************************************************************/
    
    numAttempts = 1;
    delta = 0.00001;

    offsetSqrd = uy * uy + ux * ux;
    double guess_dx, guess_dy;
    double guess_ux, guess_uy;

  /****************************************************************************
  * Loop ...
  ****************************************************************************/
    do {
      /* Guess a distorted line/samp */
      guess_dx = ux / (1.0 + p_k1 * offsetSqrd);
      guess_dy = uy / (1.0 + p_k1 * offsetSqrd);
  
      /* Use the guess to calculate a corrected line/samp */
      offsetSqrd = guess_dy * guess_dy + guess_dx * guess_dx;
  
      guess_ux = guess_dx * (1.0 + p_k1 * offsetSqrd);
      guess_uy = guess_dy * (1.0 + p_k1 * offsetSqrd);

      /* If guessed corrected line/samp match the input line/samp we're done*/
      done = true;
      if (abs(guess_uy - uy) > delta) {
        done = false;
      }

      if (abs(guess_ux - ux) > delta) {
        done = false;
      }

      /* Not converging so bomb */
      numAttempts++;
      if (numAttempts > 20) {
        return false;
      }
    } while (!done);

    /****************************************************************************
    * Sucess ...
    ****************************************************************************/

    p_focalPlaneX = guess_dx;
    p_focalPlaneY = guess_dy;
    return true;
  }
}
