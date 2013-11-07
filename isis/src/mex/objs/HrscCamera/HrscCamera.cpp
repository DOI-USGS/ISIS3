/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/08/31 15:12:30 $
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

#include "HrscCamera.h"

#include <string>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "iTime.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"
#include "Statistics.h"
#include "VariableLineScanCameraDetectorMap.h"

using namespace std;
namespace Isis {
  /**
   * Creates a HrscCamera Camera Model
   *
   * @param lab Pvl label from the iamge
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  HrscCamera::HrscCamera(Cube &cube) : LineScanCamera(cube) {
    NaifStatus::CheckErrors();
    // Setup camera characteristics from instrument and frame kernel
    SetFocalLength();
    SetPixelPitch(0.007);
    instrumentRotation()->SetFrame(-41210);

    // Get required keywords from instrument group
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);

    ReadLineRates(lab.fileName());

    // Setup detector map for transform of image pixels to detector position
    new VariableLineScanCameraDetectorMap(this, p_lineRates);
    DetectorMap()->SetDetectorSampleSumming(inst["Summing"]);

    // Setup focal plane map for transform of detector position to
    // focal plane x/y.  This will read the appropriate CCD
    // transformation coefficients from the instrument kernel

    new CameraFocalPlaneMap(this, naifIkCode());

    QString ikernKey = "INS" + toString(naifIkCode())  + "_BORESIGHT_SAMPLE";
    double sampleBoresight = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode())  + "_BORESIGHT_LINE";
    double lineBoresight = getDouble(ikernKey);

    FocalPlaneMap()->SetDetectorOrigin(sampleBoresight, lineBoresight);

    // Setup distortion map.  This will read the optical distortion
    // coefficients from the instrument kernel
    new CameraDistortionMap(this);

    // Setup the ground and sky map to transform undistorted focal
    // plane x/y to lat/lon or ra/dec respectively.
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }

  //! Destroys the HiriseCamera object
  HrscCamera::~HrscCamera() {}


  /**
   * @param filename
   */
  void HrscCamera::ReadLineRates(QString filename) {
    Table timesTable("LineScanTimes", filename);

    if(timesTable.Records() <= 0) {
      QString msg = "Table [LineScanTimes] in [";
      msg += filename + "] must not be empty";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    for(int i = 0; i < timesTable.Records(); i++) {
      p_lineRates.push_back(LineRateChange((int)timesTable[i][2],
                                           (double)timesTable[i][0],
                                           timesTable[i][1]));
    }

    if(p_lineRates.size() <= 0) {
      QString msg = "There is a problem with the data within the Table ";
      msg += "[LineScanTimes] in [" + filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }
}


/**
 * This is the function that is called in order to instantiate a
 * HrscCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* HrscCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Mex namespace.
 */
extern "C" Isis::Camera *HrscCameraPlugin(Isis::Cube &cube) {
  return new Isis::HrscCamera(cube);
}
