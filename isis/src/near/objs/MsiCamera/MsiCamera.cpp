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
#include "MsiCamera.h"

#include <QString>

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "Plugin.h"
#include "RadialDistortionMap.h"

using namespace std;
namespace Isis {
  /**
   * Constructs an MsiCamera object using the image labels.
   *
   * @param lab Pvl label from a Cassini ISS Narrow Angle Camera image.
   *
   * @internal
   *   @history 2013-03-03 Jeannie Walldren - Original Version. This version
   *                           does not correct for temperature. If the
   *                           temperature-dependent ik is used, the code at the
   *                           end of this file should replace the
   *                           LoadCache() call in the constructor.
   */
  MsiCamera::MsiCamera(Cube &cube) : FramingCamera(cube) {
    m_instrumentNameLong = "Multi-Spectral Imager";
    m_instrumentNameShort = "MSI";
    m_spacecraftNameLong = "Near Earth Asteroid Rendezvous";
    m_spacecraftNameShort = "NEAR";
    
    Pvl &lab = *cube.label();
    NaifStatus::CheckErrors();
    SetFocalLength();
    SetPixelPitch();
    NaifStatus::CheckErrors();

    // Get the start time in et
    PvlGroup inst = lab.findGroup("Instrument", Pvl::Traverse);

    double et = iTime((QString)inst["StartTime"]).Et();

    // divide exposure duration keyword value by 1000 to convert to seconds
    double exposureDuration = ((double) inst["ExposureDuration"]) / 1000.0;
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(et, exposureDuration);

    //correct time for center of exposure duration
    iTime centerTime = shuttertimes.first.Et() + exposureDuration / 2.0;

    // Setup detector map.  These images are full summing.
    new CameraDetectorMap(this);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    QString sample = "INS" + toString(naifIkCode()) + "_BORESIGHT_SAMPLE";
    QString line = "INS" + toString(naifIkCode()) + "_BORESIGHT_LINE";
    focalMap->SetDetectorOrigin(Spice::getDouble(sample), Spice::getDouble(line));

    // Setup distortion map
    double k1 = Spice::getDouble("INS" + toString(naifIkCode()) + "_K1");
    new RadialDistortionMap(this, k1, 1); 

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);
    setTime(centerTime);
    // Note: If the temperature-dependent instrument kernel is used,  
    // the following LoadCache() command should be replaced with the
    // commented lines at the end of this file.
    LoadCache();
    NaifStatus::CheckErrors();
  }

  //! Destroys the MsiCamera object.
  MsiCamera::~MsiCamera(){}

  /**
   * Returns the shutter open and close times.  The user should pass in the
   * ExposureDuration keyword value, converted from milliseconds to seconds, and
   * the StartTime keyword value, converted to ephemeris time. 
   *  
   *  
   * The StartTime keyword value from the labels represents the shutter open ???
   * time of the exposure. 
   *  
   * This method uses the FramingCamera class implementation, returning the 
   * given time value as the shutter open and the sum of the time value and 
   * exposure duration as the shutter close. 
   *
   * @param exposureDuration ExposureDuration keyword value from the labels,
   *                         converted to seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */
  pair<iTime, iTime> MsiCamera::ShutterOpenCloseTimes(double time,
                                                      double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }

  /**
   * CK frame ID - Instrument Code from spacit run on CK For more details,
   * read the Camera class documentation for this method. 
   * @return @b int The appropriate instrument code for the "Camera-matrix" 
   *         Kernel Frame ID
   *  
   * @see Camera 
   */
  int MsiCamera::CkFrameId() const { return -93000; }

  /** 
   * CK Reference ID - Reference Frame value for J2000 from spacit run on 
   * CK. For more details, read the Camera class documentation for this 
   * method. 
   * 
   * @return @b int The appropriate instrument code for the "Camera-matrix"
   *         Kernel Reference ID
   */
  int MsiCamera::CkReferenceId() const { return 1; }

  /** 
   * SPK Target ID - Target Body value for NEAR Shoemaker from spacit run 
   * on SPK. For more details, read the Camera class documentation for this
   * method. 
   *  
   * @return @b int The appropriate instrument code for the Spacecraft 
   *         Kernel Center ID
   */
//  int MsiCamera::SpkTargetId() const { return -93; }

  /** 
   * SPK Reference ID - Reference Frame value for J2000 from spacit run on SPK. 
   * For more details, read the Camera class documentation for this method. 
   *  
   * @return @b int The appropriate instrument code for the Spacecraft 
   *         Kernel Reference ID
   */
  int MsiCamera::SpkReferenceId() const { return 1; }
  
  
   /**
   * This method returns the full instrument name.
   *
   * @return QString
   */
  QString MsiCamera::instrumentNameLong() const {
    return m_instrumentNameLong;
  }
  
  
  /**
   * This method returns the shortened instrument name.
   *
   * @return QString
   */
  QString MsiCamera::instrumentNameShort() const {
    return m_instrumentNameShort;
  }
  
  
  /**
   * This method returns the full spacecraft name.
   * 
   * @return QString
   */
  QString MsiCamera::spacecraftNameLong() const {
    return m_spacecraftNameLong;
  }
  
  
  /**
   * This method returns the shortened spacecraft name.
   *
   * @return QString
   */
  QString MsiCamera::spacecraftNameShort() const {
    return m_spacecraftNameShort;
  }
}

/**
 * This is the function that is called in order to instantiate a MsiCamera
 * object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* MsiCamera
 */
extern "C" Isis::Camera *MsiCameraPlugin(Isis::Cube &cube) {
  return new Isis::MsiCamera(cube);
}

// Comment: 2013-03-08 Jeannie Backer - The following code will adjust the
//                         camera model using the temperature dependent
//                         instrument kernel. Currently, we have decided not to
//                         implement this adjustment since it does not appear to
//                         have been applied in ISIS2.  (Code exists for this in
//                         ISIS2, but without SCLK or CK, it could not have been
//                         applied).

//     if (instrumentRotation()->IsCached()) {
//       LoadCache();
//     }
//     else {
//       double toCelcius = -273.15;
//       SpiceDouble tempCelcius = toDouble(inst["DpuDeckTemperature"][0])+toCelcius;
//       ConstSpiceChar *nearMsiTimeDependentFrame;
//       iTime originalTempDependencyDate("1999-12-20T00:00:00");
// 
//       if (centerTime.Et() <= originalTempDependencyDate.Et() ) {
//         nearMsiTimeDependentFrame = "NEAR_MSI_TEMP_DEP0000";
//       }
//       else {
//         nearMsiTimeDependentFrame = "NEAR_MSI_TEMP_DEP0001";
//       }
// 
//       // get transformation matrix based on temperature, from spacecraft to MSI
//       SpiceDouble sc2msiMatrix[3][3];
//       pxform_c("J2000", nearMsiTimeDependentFrame,tempCelcius, sc2msiMatrix);
//       NaifStatus::CheckErrors();
//       // get current TC matrix, from j2000 to spacecraft
//       vector<double> originalCJ = instrumentRotation()->TimeBasedMatrix();
//       // get new CJ matrix based on temperatureBasedRotation x originalTC
//       // now we have transformation from j2000 to msi
//       vector<double> newCJ(9);
//       mxm_c(sc2msiMatrix, (SpiceDouble( *)[3]) &originalCJ[0], 
//             (SpiceDouble( *)[3]) &newCJ[0]);
//       NaifStatus::CheckErrors();
//       // mxmg_c(sc2msiMatrix, (SpiceDouble( *)[3]) &originalTC[0], 
//       //        6, 6, 6, (SpiceDouble( *)[3]) &newCJ[0]);
//       // set new CJ matrix
//       instrumentRotation()->SetTimeBasedMatrix(newCJ);
//       LoadCache();
//     }

