#ifndef NirsDetectorMap_h
#define NirsDetectorMap_h
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

#include "CameraDetectorMap.h"

namespace Isis {
  /**
   * @brief The detector map class for the Hayabusa NIRS camera.
   * 
   * The detector map class to allow for exposure duration storage and retrieval
   * in the Hayabusa NIRS camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa
   *
   * @author 2017-01-04 Jesse Mapel
   *
   * @internal
   *   @history 2017-01-04 Jesse Mapel - Original version. Fixes #4576.
   */
  class NirsDetectorMap : public CameraDetectorMap {
    public:
      NirsDetectorMap(double exposureDuration, Camera *parent);

      ~NirsDetectorMap();

      void setExposureDuration(double exposureDuration);

      virtual double exposureDuration(const double sample,
                                      const double line,
                                      const int band) const;
    protected:
      double m_exposureDuration; //!< The total time for the observation
  };
};
#endif