/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/03/27 06:36:41 $
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

#ifndef IrregularBodyCameraGroundMap_h
#define IrregularBodyCameraGroundMap_h

#include "Camera.h"
#include "CameraDistortionMap.h"
#include "CameraGroundMap.h"
#include "SpicePosition.h"
#include "SurfacePoint.h"

namespace Isis {
  /** 
   * Convert between undistorted focal plane and ground coordinates
   * 
   * This class is derived from CameraGroundMap to support the special case of
   * irregular bodies. Particularly, only the GetXY() method is reimplemented here
   * because it uses the ellipsiod to validate ground point visibility along the
   * look vector to the surface. This is not adequate for most irregular bodies.
   *
   * @ingroup Camera
   *
   * @see CameraGroundMap
   *
   * @author 2018-07-26 UA/OSIRIS-REx IPWG Team 
   *
   * @internal
   *  @history 2018-07-26 UA/OSIRIS-REx IPWG Team  - Developed to support control
   *                         of irregular bodies
   */
  class IrregularBodyCameraGroundMap : public CameraGroundMap {
    public:
      IrregularBodyCameraGroundMap(Camera *parent, const bool isIrregular = true);

      //! Destructor
      virtual ~IrregularBodyCameraGroundMap() {};

      virtual bool GetXY(const SurfacePoint &spoint, double *cudx, double *cudy);

    protected:
      bool   m_isBodyIrregular;   /**! Is the body irregular */

  };
};
#endif
