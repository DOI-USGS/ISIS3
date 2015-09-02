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
#include "MocWideAngleCamera.h"
#include "MocWideAngleDetectorMap.h"
#include "MocWideAngleDistortionMap.h"
#include "MocLabels.h"

#include <QString>

#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "iTime.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  // constructors
  /**
   * Constructor for the Mgs MOC Wide Angle Camera Model
   *
   * @param lab Pvl label from an MOC WAC image.
   *
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Replaced reference
   *          to MocLabels IsWideAngleRed() with MocLabels
   *          WideAngleRed().
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  MocWideAngleCamera::MocWideAngleCamera(Cube &cube) : LineScanCamera(cube) {
    m_instrumentNameLong = "Mars Orbiter Camera Wide Angle";
    m_instrumentNameShort = "MOC-WA";
    m_spacecraftNameLong = "Mars Global Surveyor";
    m_spacecraftNameShort = "MGS";
    
    NaifStatus::CheckErrors();
    // See if we have a moc camera
    Pvl &lab = *cube.label();
    MocLabels *moclab = new MocLabels(cube);
    double lineRate = moclab->LineRate();
    double csum = moclab->CrosstrackSumming();
    double dsum = moclab->DowntrackSumming();
    double ss = moclab->FirstLineSample();
    bool isRed = moclab->WideAngleRed();

    // Set up the camera info from ik/iak kernels
    // LoadEulerMounting();
    SetFocalLength();
    SetPixelPitch();

    if(PixelPitch() == 1) {
      throw IException(IException::User,
                       "Cube file needs to be spiceinit'd with updated iak", _FILEINFO_);
    }
    instrumentRotation()->SetTimeBias(-1.15);

    // Get the start time from labels
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockCount"];
    double etStart = getClockTime(stime).Et();

    // Setup detector map
    MocWideAngleDetectorMap *detectorMap =
      new MocWideAngleDetectorMap(this, etStart, lineRate, moclab);
    detectorMap->SetDetectorSampleSumming(csum);
    detectorMap->SetDetectorLineSumming(dsum);
    detectorMap->SetStartingDetectorSample(ss);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap =
      new CameraFocalPlaneMap(this, naifIkCode());
    if(isRed) {
      focalMap->SetDetectorOrigin(1674.65, 0.0);
      focalMap->SetDetectorOffset(0.0, 6.7785);
    }
    else {
      focalMap->SetDetectorOrigin(1688.58, 0.0);
      focalMap->SetDetectorOffset(0.0, -0.8486);
    }

    // Setup distortion map
    new MocWideAngleDistortionMap(this, isRed);

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
  
  
  /**
   * This method returns the full instrument name.
   *
   * @return QString
   */
  QString MocWideAngleCamera::instrumentNameLong() const {
    return m_instrumentNameLong;
  }
  
  
  /**
   * This method returns the shortened instrument name.
   *
   * @return QString
   */
  QString MocWideAngleCamera::instrumentNameShort() const {
    return m_instrumentNameShort;
  }
  
  
  /**
   * This method returns the full spacecraft name.
   * 
   * @return QString
   */
  QString MocWideAngleCamera::spacecraftNameLong() const {
    return m_spacecraftNameLong;
  }
  
  
  /**
   * This method returns the shortened spacecraft name.
   *
   * @return QString
   */
  QString MocWideAngleCamera::spacecraftNameShort() const {
    return m_spacecraftNameShort;
  }
}


/**
 * This is the function that is called in order to instantiate a
 * MocWideAngleCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* MocWideAngleCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Mgs namespace.
 */
extern "C" Isis::Camera *MocWideAngleCameraPlugin(Isis::Cube &cube) {
  return new Isis::MocWideAngleCamera(cube);
}
