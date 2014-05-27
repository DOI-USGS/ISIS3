#ifndef LroWideAngleCameraFocalPlaneMap_h
#define LroWideAngleCameraFocalPlaneMap_h
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

#include <QVector>
#include "CameraFocalPlaneMap.h"

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
   * @author 2013-03-07 Kris Becker
   *
   * @internal
   *   @history 2013-03-07 Kris Becker - Implements new focal plane map allowing
   *            for band independent distortions.
   */
  class LroWideAngleCameraFocalPlaneMap : public CameraFocalPlaneMap {
    public:
      LroWideAngleCameraFocalPlaneMap(Camera *parent, int naifIkCode);

      //! Destroys the LroWideAngleCameraFocalPlaneMap object
      virtual ~LroWideAngleCameraFocalPlaneMap() { }

      void addFilter(int naifIkCode);
      void setBand(int vband);

    private:
      struct TranslationParameters {
        double m_transx[3];
        double m_transy[3];
        double m_itranss[3];
        double m_itransl[3];
      };
      QVector<TranslationParameters> m_transparms;
  };
}
#endif
