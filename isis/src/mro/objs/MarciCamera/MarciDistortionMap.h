#ifndef MarciDistortionMap_h
#define MarciDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @see MarciCamera
   *
   * @author 2008-08-22 Steven Lambright
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Updated documentation.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   */
  class MarciDistortionMap : public CameraDistortionMap {
    public:
      MarciDistortionMap(Camera *parent, int naifIkCode);

      //! Destructor
      virtual ~MarciDistortionMap() {};

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      void SetFilter(int filter) {
        p_filter = filter;
      }

    private:
      double GuessDx(double uX);
      int p_filter;
  };
};
#endif
