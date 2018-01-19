#include "ApolloPanoramicCameraFiducialMap.h"

#include "Affine.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"

namespace Isis {
  /** Constructs mapping between Apollo Panoramic Camera detectors and focal cylinder x/y.
   *  This method sets the x-axis direction to -1 if the NAIF IK code is even
   *  and to 1 if the code is odd.
   *
   * @param inst  Instrument group from the Pvl labels
   * @param naifIkCode  Naif code of the Apollo Panoramic instrument for reading coefficients
   *
   */
  ApolloPanoramicCameraFiducialMap::ApolloPanoramicCameraFiducialMap(PvlGroup &fiducials,
                                                                     const int naifIkCode) {
    // Get the Instrument label information needed to define the fiducial map for this frame
    p_naifIkCode = naifIkCode;
    ReadFiducials(fiducials);

    // Set the x-axis direction.
    int xdir;
    if(naifIkCode % 2 == 0) {
      xdir = -1;
    }
    else {
      xdir = 1;
    }
//    CreateTrans(xdir);
  }


  /**
   *
   * Reads the fiducials from the instrument group of the labels
   * @param inst Instrument group from the Pvl labels
   *
   * @throws IException::User - "Unable to read fiducial mapping from cube
   *             labels - Input cube must be processed in Isis 2 through
   *             lofixlabel and converted to Isis 3 with pds2isis"
   */
  void ApolloPanoramicCameraFiducialMap::ReadFiducials(PvlGroup &fiducials) {

    // Try to read the fiducials from the labels
    try {
      // Fiducial mapping to define the Focal Plane map
      PvlKeyword &measuredSamples = fiducials["Sample"];
      PvlKeyword &measuredLines = fiducials["Line"];
      PvlKeyword &calibratedLines = fiducials["XCoordinates"];
      PvlKeyword &calibratedSamples = fiducials["YCoordinates"];
      for(int i = 0; i < measuredSamples.size(); i++) {
        m_fidMeasuredSamples.push_back(toDouble(measuredSamples[i]));
        m_fidMeasuredLines.push_back(toDouble(measuredLines[i]));
        m_fidCalibratedLines.push_back(toDouble(calibratedLines[i]));
        m_fidCalibratedSamples.push_back(toDouble(calibratedSamples[i]));
      }
    }
    catch(IException &e) {
      string msg = "Unable to read fiducial mapping from cube labels";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Creates focal plane affine transform.
   * @param xdir The x-axis direction.
   *
   *
   * @throws IException::User - "Unable to create fiducial map."
   */
  Affine* ApolloPanoramicCameraFiducialMap::CreateTrans() {
    Affine *fptrans = new Affine();

    try {
      fptrans->Solve(&m_fidMeasuredSamples[0], (double *) &m_fidMeasuredLines[0],
                     (double *) &m_fidCalibratedSamples[0], (double *) &m_fidCalibratedLines[0],
                     m_fidMeasuredSamples.size());
    }
    catch(IException &e) {
      string msg = "Unable to create fiducial map.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    return fptrans;
  }
}
