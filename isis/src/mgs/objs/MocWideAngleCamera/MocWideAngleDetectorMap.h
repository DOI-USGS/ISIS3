#ifndef MocWideAngleDetectorMap_h
#define MocWideAngleDetectorMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCameraDetectorMap.h"
#include "MocLabels.h"

namespace Isis {
  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a the Moc wide angle
   * camera. It is needed to handle variable summing modes
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsGlobalSurveyor
   *
   * @see Camera
   *
   * @author 2005-02-01 Jeff Anderson
   * @internal
   *   @history 2005-02-01 Jeff Anderson - Original version
   *   @history 2011-05-03 Jeannie Walldren - Removed Mgs namespace wrap.
   */
  class MocWideAngleDetectorMap : public LineScanCameraDetectorMap {
    public:
      /**
       * Construct a detector map for line scan cameras
       *
       * @param parent The parent Camera Model
       * @param etStart   starting ephemeris time in seconds
       *                  at the top of the first line
       * @param lineRate  the time in seconds between lines
       * @param moclab The moc labels to use for the camera creation
       *
       */
      MocWideAngleDetectorMap(Camera *parent, const double etStart,
                              const double lineRate, MocLabels *moclab) :
        LineScanCameraDetectorMap(parent, etStart, lineRate) {
        p_moclab = moclab;
      }

      //! Destructor
      virtual ~MocWideAngleDetectorMap() {};

      virtual bool SetParent(const double sample, const double line);

      virtual bool SetDetector(const double sample, const double line);

    private:
      MocLabels *p_moclab;
  };
};
#endif
