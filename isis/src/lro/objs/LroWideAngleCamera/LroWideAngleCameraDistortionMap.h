#ifndef LroWideAngleCameraDistortionMap_h
#define LroWideAngleCameraDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QVector>
#include "CameraDistortionMap.h"

namespace Isis {
  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @see LroWideAngleCamera
   *
   * @author 2008-08-22 Steven Lambright
   *
   * @internal
   *   @history 2009-11-19 Kris Becker - Changed the convergence tolerance
   *            from 1/10,000 of a pixel to 1/100 of a pixel
   *   @history 2010-05-05 Ken Edmundson - Corrected distorted and undistorted
   *            computations;  Fix requires coefficients in the
   *            lro_instruments_v??.ti to be negative (essentially matches
   *            what is reported in the calibration document); removed the
   *            GuessDx method as it was not used; updated the UV boresight in
   *            the IK based upon analysis of the VIS and UV.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *            coding standards. References #972.
   *   @history 2011-08-30 Kris Becker - Implemented new decentering distortion
   *            model.  This becomes version 3 of the camera model
   *   @history 2012-03-06 Kris Becker - Added distortion model tolerance parameter
   *   @history 2013-03-07 Kris Becker - Modified to implement new distortion
   *            model with three terms and allow for band independant
   *            distortions.
   */
  class LroWideAngleCameraDistortionMap : public CameraDistortionMap {
    public:
      LroWideAngleCameraDistortionMap(Camera *parent, int naifIkCode);

      //! Destroys the LroWideAngleCameraDistortionMap object
      virtual ~LroWideAngleCameraDistortionMap() { }

      void addFilter(int naifIkCode);
      void setBand(int vband);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      QVector<std::vector<double> > m_odkFilters;

  };
};
#endif
