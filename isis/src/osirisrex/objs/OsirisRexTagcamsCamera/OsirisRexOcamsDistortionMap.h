/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/02/21 16:04:33 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#ifndef OsirisRexOcamsDistortionMap_h
#define OsirisRexOcamsDistortionMap_h

#include <QSharedPointer>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates for OSIRIS REx's cameras.
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2017-08-21 Kristin Berry and Jeannie Backer
   *
   * @internal
   *   @history 2017-08-21 Kristin Berry and Jeannie Backer - Original Version
   *
   */
  class OsirisRexOcamsDistortionMap : public CameraDistortionMap {
    public:
      OsirisRexOcamsDistortionMap(Camera *parent, double zDirection = 1.0);

      virtual ~OsirisRexOcamsDistortionMap();

      virtual void SetDistortion(int naifIkCode);

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);

    protected: 
      double m_pixelPitch; //!< The pixel pitch for OCAMS.
      double m_detectorOriginSample; //!< The origin of the detector's sample coordinate.
      double m_detectorOriginLine; //!< The origin of the detector's line coordinate.
      double m_distortionOriginSample; //!< The distortion's origin sample coordinate.
      double m_distortionOriginLine; //!< The distortion's origin line coordinate. 
      double p_tolerance;            //!< Convergence tolerance
      bool   p_debug;                //!< Debug the model

      QSharedPointer<CameraFocalPlaneMap> m_focalMap;  // Local focal plane map
  };
};
#endif
