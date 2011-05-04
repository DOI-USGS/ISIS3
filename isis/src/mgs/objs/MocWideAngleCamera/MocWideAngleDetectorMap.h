#ifndef MocWideAngleDetectorMap_h
#define MocWideAngleDetectorMap_h
/**
 * @file
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
