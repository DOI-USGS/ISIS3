/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/03/07 17:57:27 $
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

#ifndef RollingShutterCameraDetectorMap_h
#define RollingShutterCameraDetectorMap_h

#include "CameraDetectorMap.h"

#include <utility>
#include <vector>

namespace Isis {
  class Camera;

  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a rolling shutter camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2018-04-02 Makayla Shepherd and Ian Humphrey
   */
  class RollingShutterCameraDetectorMap : public CameraDetectorMap {
    public:

      RollingShutterCameraDetectorMap(Camera *parent,
                                      std::vector<double> times,
                                      std::vector<double> sampleCoeffs,
                                      std::vector<double> lineCoeffs);

      virtual ~RollingShutterCameraDetectorMap();

      virtual bool SetParent(const double sample,
                             const double line);

      virtual bool SetParent(const double sample,
                             const double line,
                             const double deltaT);

      virtual bool SetDetector(const double sample,
                               const double line);

      std::pair<double, double> applyJitter(const double sample,
                                            const double line);
      std::pair<double, double> removeJitter(const double sample,
                                             const double line);

    private:

      /**  List of normalized [-1, 1] readout times for all the lines in the input image. */
      std::vector<double> m_times;
      /**
       * List of coefficients for the n-order polynomial characterizing the jitter in the sample
       * direction.
       */
      std::vector<double> m_sampleCoeffs;
      /**
       * List of coefficients for the n-order polynomial characterizing the jitter in the line
       * direction.
       */
      std::vector<double> m_lineCoeffs;
  };
};
#endif
