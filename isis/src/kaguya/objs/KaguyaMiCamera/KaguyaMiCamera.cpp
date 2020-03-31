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
#include "KaguyaMiCamera.h"

#include <iomanip>

#include <QString>

#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "KaguyaMiCameraDistortionMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the Kaguya MI Camera Model
   *
   * @param lab Pvl Label to create the camera model from
   *
   * @internal 
   *   @history 2012-06-14 Orrin Thomas - original version
   */
  KaguyaMiCamera::KaguyaMiCamera(Cube &cube) : LineScanCamera(cube) {
    m_spacecraftNameLong = "Kaguya";
    m_spacecraftNameShort = "Kaguya";

    int ikCode = naifIkCode();

    // https://darts.isas.jaxa.jp/pub/spice/SELENE/kernels/ik/SEL_MI_V01.TI
    // MI-VIS instrument kernel codes -131331 through -131335
    if (ikCode <= -131331 && ikCode >= -131335) {
      m_instrumentNameLong = "Multi Band Imager Visible";
      m_instrumentNameShort = "MI-VIS";
    }
    // MI-NIR instrument kernel codes -131341 through -131344
    else if (ikCode <= -131341 && ikCode >= -131344) {
      m_instrumentNameLong = "Multi Band Imager Infrared";
      m_instrumentNameShort = "MI-NIR";
    }
    else {
      QString msg = QString::number(ikCode);
      msg += " is not a supported instrument kernel code for Kaguya.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    
    NaifStatus::CheckErrors();
    // Set up the camera info from ik/iak kernels

    SetFocalLength();
    //Kaguya IK kernal uses INS-131???_PIXEL_SIZE instead of PIXEL_PITCH
    QString ikernKey = "INS" + toString(naifIkCode()) + "_PIXEL_SIZE";
    SetPixelPitch(getDouble(ikernKey));
 

    // Get the start time from labels
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = (QString)inst["StartTime"];
    SpiceDouble etStart=0;

    if(stime != "NULL") {
      etStart = iTime(stime).Et();
    }
    else {
      //TODO throw an error if "StartTime" keyword is absent
    }

    NaifStatus::CheckErrors();


    // Get other info from labels
    double lineRate = (double) inst["CorrectedSamplingInterval"] / 1000.0;
    setTime(etStart);

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, etStart, lineRate);
    detectorMap->SetDetectorSampleSumming(1.0);
    detectorMap->SetStartingDetectorSample(1.0);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
    // Retrieve boresight location from instrument kernel (IK) (addendum?)
    ikernKey = "INS" + toString(naifIkCode()) + "_CENTER";
    double sampleBoreSight = getDouble(ikernKey,0);
    double lineBoreSight = getDouble(ikernKey,1)-1.0;

    focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
    focalMap->SetDetectorOffset(0.0, 0.0);


    KaguyaMiCameraDistortionMap *distMap = new KaguyaMiCameraDistortionMap(this);
    distMap->SetDistortion(naifIkCode());

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();

    NaifStatus::CheckErrors();
  }
}

/**
 * This is the function that is called in order to instantiate a
 * KaguyaMi object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* KaguyaMiCamera
 * @internal
 *   @history 2012-06-14 Orrin Thomas - original version
 */
extern "C" Isis::Camera *KaguyaMiCameraPlugin(Isis::Cube &cube) {
  return new Isis::KaguyaMiCamera(cube);
}
