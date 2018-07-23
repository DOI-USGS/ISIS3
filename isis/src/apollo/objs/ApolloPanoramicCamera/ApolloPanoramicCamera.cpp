/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/06/17 18:59:11 $
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
#include "ApolloPanoramicCamera.h"
//#include "ApolloPanIO.h"
#include "ApolloPanoramicCameraFiducialMap.h"
#include "ApolloPanoramicDetectorMap.h"

#include <QString>

#include "Affine.h"

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
//#include "SampleScanCameraDetectorMap.h"
#include "SampleScanCameraGroundMap.h"
#include "SampleScanCameraSkyMap.h"
#include "NaifStatus.h"
//#include "VariableSampleScanCameraDetectorMap.h"
//#include "PvlGroup.h"
//#include "PvlKeyword.h"

using namespace std;
namespace Isis {
  /**
   * Constructs the Apollo Panoramic Camera model object from the labels.
   *
   * This constructor uses the Pvl labels for Apollo Panoramic Camera Images.
   *
   * @param lab Pvl label from an Apollo Panoramic Camera image.
   * @internal
   *   @history 2016-09-12 Ken Edmundson - Original version, sort of. Orrin Thomas did the original
   *                       original. The camera model has changed significantly since.
   *
   */
  ApolloPanoramicCamera::ApolloPanoramicCamera(Isis::Cube &cube) : Isis::SampleScanCamera(cube) {
    NaifStatus::CheckErrors();

    m_instrumentNameLong = "Panoramic Camera";
    m_instrumentNameShort = "Pan";

    // Apollo15 Pan naif code = -915230
    if (naifIkCode() == -915230) {
      m_spacecraftNameLong = "Apollo 15";
      m_spacecraftNameShort = "Apollo15";
      m_ckFrameId = -915230;
    }
    // Apollo16 Pan naif code = -916230
    else if (naifIkCode() == -916230) {
      m_spacecraftNameLong = "Apollo 16";
      m_spacecraftNameShort = "Apollo16";
      m_ckFrameId = -916230;
    }
    // Apollo17 Pan naif code = -917230
    else if (naifIkCode() == -917230) {
      m_spacecraftNameLong = "Apollo 17";
      m_spacecraftNameShort = "Apollo17";
      m_ckFrameId = -917230;
    }
    else {
      QString msg = "File does not appear to be an Apollo Panoramic image";
      msg += QString::number(naifIkCode());
      msg += " is not a supported instrument kernel code for Apollo.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Get the Instrument label information needed to define the camera for this frame
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString spacecraft = (QString)inst["SpacecraftName"];
    QString instId = (QString)inst["InstrumentId"];

    // read focal length and pixel pitch from the instrument kernel
    SetFocalLength();
    SetPixelPitch();

    ReadSampleRates(lab.fileName());

    SpiceDouble etStart = iTime((QString)inst["StartTime"]).Et();
    setTime(etStart);

    // Setup fiducial mark affine transformation (in pixels)
    PvlGroup &fiducials = lab.findGroup("Fiducials", Pvl::Traverse);
    ApolloPanoramicCameraFiducialMap fid(fiducials, naifIkCode());

    Affine* fiducialMap = fid.CreateTrans();

    // Setup detector map for transform of parent image pixels to "detector" pixels
    new VariableSampleScanCameraDetectorMap(this, p_sampleRates, fiducialMap);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

//    focalMap->SetDetectorOrigin(114374.916541692, 11490.0);
    focalMap->SetDetectorOrigin(0.0, 11490.0);
    focalMap->SetDetectorOffset(0.0, 0.0);

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
//    double sampleBoreSight = 0.0;  //Presently no NAIF keywords for this sensor
//    double lineBoreSight = 0.0;  //Presently no NAIF keywords for this sensor

    // Setup distortion map
    new CameraDistortionMap(this, -1.0);

    //Setup the ground and sky map
    new SampleScanCameraGroundMap(this);
    new SampleScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();

    delete fiducialMap;
  }

  /**
   * @param filename
   */
  void ApolloPanoramicCamera::ReadSampleRates(QString filename) {
    Table timesTable("SampleScanTimes", filename);

    if(timesTable.Records() <= 0) {
      QString msg = "Table [SampleScanTimes] in [";
      msg += filename + "] must not be empty";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    for(int i = 0; i < timesTable.Records(); i++) {
      p_sampleRates.push_back(SampleRateChange((int)timesTable[i][2],
                                           (double)timesTable[i][0],
                                           timesTable[i][1]));
    }

    if(p_sampleRates.size() <= 0) {
      QString msg = "There is a problem with the data within the Table ";
      msg += "[SampleScanTimes] in [" + filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }

}// end Isis namespace


/**
 * This is the function that is called in order to instantiate an
 * ApolloPanoramicCamera object.
 *
 * @param lab Cube labels
 *
 */
extern "C" Isis::Camera *ApolloPanoramicCameraPlugin(Isis::Cube &cube) {
   return new Isis::ApolloPanoramicCamera(cube);
}
