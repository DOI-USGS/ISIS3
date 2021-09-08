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

#include "Chandrayaan1M3Camera.h"

#include <QString>

#include "Chandrayaan1M3DistortionMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "iTime.h"
#include "IString.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Chandrayaan 1 M3 Camera object using the image labels.
   *  
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *  
   * @author 2013-08-18 Stuart Sides and Tracie Sucharski
   *
   * @internal
   *   @history 2013-08-18 Stuart Sides - Original version.
   */
  Chandrayaan1M3Camera::Chandrayaan1M3Camera(Cube &cube) : LineScanCamera(cube) {
    m_instrumentNameLong = "Moon Mineralogy Mapper";
    m_instrumentNameShort = "M3";
    m_spacecraftNameLong = "Chandrayaan 1";
    m_spacecraftNameShort = "Chan1";

    NaifStatus::CheckErrors();
    // Set up the camera info from ik/iak kernels
    SetFocalLength();
    SetPixelPitch();

    // Get the start time from labels
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockStartCount"];
    double etStart = getClockTime(stime).Et();

    // Get other info from labels
    double csum = inst["SpatialSumming"];
    double lineRate = (double) inst["LineExposureDuration"] / 1000.0;
    //lineRate *= csum;

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap =
      new LineScanCameraDetectorMap(this, etStart, lineRate);
    detectorMap->SetDetectorSampleSumming(csum);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
    QString ikernKey = "INS" + toString((int)naifIkCode()) + "_BORESIGHT_SAMPLE";
    double sampleBoreSight = getDouble(ikernKey);

    ikernKey = "INS" + toString((int)naifIkCode()) + "_BORESIGHT_LINE";
    double lineBoreSight = getDouble(ikernKey);

    focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
    focalMap->SetDetectorOffset(0.0, 0.0);

    QString ppKey("INS" + toString(naifIkCode()) + "_PP");
    QString odKey("INS" + toString(naifIkCode()) + "_OD_K");
    QString decenterKey("INS" + toString(naifIkCode()) + "_DECENTER");


    // Setup distortion map
    new Chandrayaan1M3DistortionMap(this, 
                                    getDouble(ppKey, 0), getDouble(ppKey, 1),
                                    getDouble(odKey, 0), getDouble(odKey, 1), getDouble(odKey, 2),
                                    getDouble(decenterKey, 0), getDouble(decenterKey, 1));

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}


/**
 * This is the function that is called in order to instantiate an Chandrayaan1M3Camera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* Chandrayaan1M3Camera 
 *  
 */
extern "C" Isis::Camera *Chandrayaan1M3CameraPlugin(Isis::Cube &cube) {
  return new Isis::Chandrayaan1M3Camera(cube);
}
