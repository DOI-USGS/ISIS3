#ifndef RosettaOsirisCameraDistortionMap_h
#define RosettaOsirisCameraDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "CameraDistortionMap.h"

#include "LinearAlgebra.h"

namespace Isis {
  /**
   * Distortion map for converting between undistorted focal plane and
   * distorted focal plane coordinates for the Rosetta OSIRIS NAC and WAC.
   *
   * The distortion models are defined by pixelspace polynomials. The
   * polynomials use zero-based pixel space with the origin at the top left
   * corner of the image, so the input focal plane coordinates are converted to
   * pixel coordinates using the boresight location and pixel pitch. After
   * computation, they are converted back into focal plane coordinates by the
   * inverse process.
   *
   * Given a set of distorted pixel coordinates (dx, dy), the undistorted pixel
   * coordinates (ux, uy) are computed as:
   * @f[ (ux, uy) = F(dx, dy) = ( \sum_{i=0}^3 \sum_{j=0}^3 C_{i,j}^x dx^i dy^j,
   * \sum_{i=0}^3 \sum_{j=0}^3 C_{i,j}^y dx^i dy^j) @f] where @f$ C_{i,j}^y @f$
   * and @f$ C_{i,j}^y @f$ are the @f$ (i,j)^{\text{th}} @f$ coefficients of
   * the @f$ x @f$ and @f$ y @f$ polynomials respectively.
   *
   * Given a set of undistorted pixel coordinates (ux, uy), Newton's method is
   * used to find the distorted coordinates (dx, dy) within a tolerance of
   * @f$ 10^{-7} @f$ pixels.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Rosetta
   *
   * @see RosettOsirisCamera
   *
   * @author 2016-10-31 Jesse Mapel and Jeannie Backer
   *
   * @internal
   *   @history 2016-10-31 Jesse Mapel and Jeannie Backer - Original Version
   *   @history 2017-06-02 Jesse Mapel - Converted from using inverse matrix to
   *                                     a Newton's method approach when computing
   *                                     distorted coordinates from undistorted
   *                                     coordinates.
   */
  class RosettaOsirisCameraDistortionMap : public CameraDistortionMap {
    public:
      RosettaOsirisCameraDistortionMap(Camera *parent);

      //! Destroys the RosettaOsirisCameraDistortionMap object
      virtual ~RosettaOsirisCameraDistortionMap() { }

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      void setUnDistortedXMatrix(LinearAlgebra::Matrix xMat);
      void setUnDistortedYMatrix(LinearAlgebra::Matrix yMat);
      void setBoresight(double sample, double line);
      void setPixelPitch(double pitch);

    private:
      double focalXToLine(double x);
      double focalYToSample(double y);
      double lineToFocalX(double line);
      double sampleToFocalY(double sample);

      LinearAlgebra::Matrix m_toUnDistortedX; /**< Matrix for computing
                                                   undistorted X coordinates. */
      LinearAlgebra::Matrix m_toUnDistortedY; /**< Matrix for computing
                                                   undistorted Y coordinates. */

      double m_boresightSample; /**< Camera boresight sample coordinate for
                                     converting focal plane coordinates to
                                     pixel coordinates. */
      double m_boresightLine;   /**< Camera boresight line coordinate for
                                     converting focal plane coordinates to
                                     pixel coordinates. */
      double m_pixelPitch;      /**< Camera pixel pitch for converting focal
                                     plane coordinates to pixel coordinates. */

  };
};
#endif
