#ifndef RollingShutterCameraDetectorMap_h
#define RollingShutterCameraDetectorMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
