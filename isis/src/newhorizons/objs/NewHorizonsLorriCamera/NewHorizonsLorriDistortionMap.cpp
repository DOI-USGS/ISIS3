#include "NewHorizonsLorriDistortionMap.h"
#include "CameraFocalPlaneMap.h"

using namespace std;

namespace Isis {
  NewHorizonsLorriDistortionMap::NewHorizonsLorriDistortionMap(Camera *parent, double k1, double zDirection) : CameraDistortionMap(parent, zDirection) {
    p_k1 = k1;
  }

  bool NewHorizonsLorriDistortionMap::SetUndistortedFocalPlane(const double ux, const double uy) {
    double offsetSqrd;

    p_undistortedFocalPlaneX = ux;
    p_undistortedFocalPlaneY = uy;

    offsetSqrd = ux * ux + uy * uy;

    p_focalPlaneX = ux * (1.0 + p_k1 * offsetSqrd);
    p_focalPlaneY = uy * (1.0 + p_k1 * offsetSqrd);
    return true;
  }

  bool NewHorizonsLorriDistortionMap::SetFocalPlane(const double dx, const double dy) {
    double offsetSqrd;
    int    numAttempts;
    double delta;
    bool    done;

    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    /****************************************************************************
    * Pre-loop intializations
    ****************************************************************************/

    numAttempts = 1;
    delta = 0.00001;

    offsetSqrd = dy * dy + dx * dx;
    double guess_dx, guess_dy;
    double guess_ux, guess_uy;

    /****************************************************************************
    * Loop ...
    ****************************************************************************/
    do {
      guess_ux = dx / (1.0 + p_k1 * offsetSqrd);
      guess_uy = dy / (1.0 + p_k1 * offsetSqrd);

      offsetSqrd = guess_uy * guess_uy + guess_ux * guess_ux;

      guess_dx = guess_ux * (1.0 + p_k1 * offsetSqrd);
      guess_dy = guess_uy * (1.0 + p_k1 * offsetSqrd);

      done = true;
      if(abs(guess_dy - dy) > delta) {
        done = false;
      }

      if(abs(guess_dx - dx) > delta) {
        done = false;
      }

      /* Not converging so bomb */
      numAttempts++;
      if(numAttempts > 20) {
        return false;
      }
    }
    while(!done);

    /****************************************************************************
    * Sucess ...
    ****************************************************************************/

    p_undistortedFocalPlaneX = guess_ux;
    p_undistortedFocalPlaneY = guess_uy;
    return true;
  }
}
