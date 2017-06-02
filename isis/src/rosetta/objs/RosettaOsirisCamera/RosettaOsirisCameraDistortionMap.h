#ifndef RosettaOsirisCameraDistortionMap_h
#define RosettaOsirisCameraDistortionMap_h
/** 
 * @file 
 *  
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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

