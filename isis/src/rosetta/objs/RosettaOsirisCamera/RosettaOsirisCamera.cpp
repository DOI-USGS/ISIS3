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

#include "RosettaOsirisCamera.h"

#include <QDebug>
#include <QFile>
#include <QString>

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "FileName.h"
#include "NaifStatus.h"
#include "Preference.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a Rosetta Osiris NAC Framing Camera object. 
   *
   * @param cube The image cube.
   *
   * @author Stuart Sides
   * 
   * @internal
   * @history modified by Sasha Brownsberger
   */

  RosettaOsirisCamera::RosettaOsirisCamera(Cube &cube) : FramingCamera(cube) {
    m_instrumentNameLong = "Optical, Spectroscopic, and Infrared Remote Imaging System";
    m_instrumentNameShort = "OSIRIS";
    m_spacecraftNameLong = "Rosetta";
    m_spacecraftNameShort = "Rosetta";

    NaifStatus::CheckErrors();

    // The Osiris focal length is fixed and is designed not to change throught the operational 
    // temperature.  For OSIRIS, the focal length is in mm, so we shouldn't need the unit conversion

    QString ikCode =  toString(naifIkCode());

    QString fl = "INS" + ikCode + "_FOCAL_LENGTH";
    double focalLength = Spice::getDouble(fl);
    SetFocalLength(focalLength);

    // For setting the pixel pitch, the Naif keyword PIXEL_SIZE is used instead of the ISIS
    // default of PIXEL_PITCH, so set the value directly.  Needs to be converted from microns to mm.   
    QString pp = "INS" + ikCode + "_PIXEL_SIZE";

    double pixelPitch = Spice::getDouble(pp);
    pixelPitch /= 1000.0;
    SetPixelPitch(pixelPitch);

    // Setup focal plane map. The class will read data from the instrument addendum kernel to pull
    // out the affine transforms from detector samp,line to focal plane x,y.
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    new CameraDetectorMap(this);
    RosettaOsirisCameraDistortionMap* distortionMap = new RosettaOsirisCameraDistortionMap(this);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    // Setup clock start and stop times.
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString clockStartCount = inst["SpacecraftClockStartCount"];
    double start = getClockTime(clockStartCount).Et();
    // QString clockStopCount = inst["SpacecraftClockStopCount"];
    // double stop = getClockTime(clockStopCount).Et();
    double exposureTime = (double) inst["ExposureDuration"];

    // Setup the distortion map
    PvlGroup &BandBin = lab.findGroup("BandBin", Pvl::Traverse);
    QString filterNumber = BandBin["FilterNumber"];
    initDistortion(ikCode, distortionMap);
    distortionMap->setPixelPitch(pixelPitch);

    // The boresight position depends on the filter. They are all defined as
    // offsets from the middle of the ccd.
    double referenceSample = Spice::getDouble("INS" + ikCode + "_BORESIGHT",0) + 1.0;
    double referenceLine = Spice::getDouble("INS" + ikCode + "_BORESIGHT",1) + 1.0;
    // The offsets in the IAK are based on the S/C frame, not the camera frame
    // For now, do not adjust based on filter. -JAM
//     referenceSample += Spice::getDouble("INS" + ikCode + "_FILTER_" + filterNumber + "_DX");
//     referenceLine += Spice::getDouble("INS" + ikCode + "_FILTER_" + filterNumber + "_DY");
    focalMap->SetDetectorOrigin(referenceSample, referenceLine);
    distortionMap->setBoresight(referenceSample, referenceLine);

    iTime centerTime = start + (exposureTime / 2.0);
    setTime( centerTime ); 

    // Internalize all the NAIF SPICE information into memory.
    LoadCache();
    NaifStatus::CheckErrors();

    return;
  }


  /**
   * Returns the shutter open and close times.  The LORRI camera doesn't use a shutter to start and 
   * end an observation, but this function is being used to get the observation start and end times,
   * so we will simulate a shutter. 
   * 
   * @param exposureDuration ExposureDuration keyword value from the labels,
   *                         converted to seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   */

  /* This should not be an issue with the Osiris cameras, so this can likely be deleted.
     It has been left here just in case something of importance in it was missed.   -Sasha
  */
  pair<iTime, iTime> RosettaOsirisCamera::ShutterOpenCloseTimes(double time,
                                                         double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }


  /**
   * Initialize the distortion map using the paramters from the NAIF SPICE kernels.
   * 
   * @param ikCode The NAIF IK code of the instrument
   * @param[out] distortionMap The distortion map that will be initialized
   */
  void RosettaOsirisCamera::initDistortion(QString ikCode,
                                           RosettaOsirisCameraDistortionMap *distortionMap) {

    // Initialize matrices
    LinearAlgebra::Matrix toUnDistX = LinearAlgebra::zeroMatrix(4, 4);
    LinearAlgebra::Matrix toUnDistY = LinearAlgebra::zeroMatrix(4, 4);

    // Fill matrices from the kernels
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        toUnDistX(i, j) = Spice::getDouble("INS" + ikCode + "_TO_UNDISTORTED_X", 4 * i + j);
        toUnDistY(i, j) = Spice::getDouble("INS" + ikCode + "_TO_UNDISTORTED_Y", 4 * i + j);
      }
    }

    // Save the matrices
    distortionMap->setUnDistortedXMatrix(toUnDistX);
    distortionMap->setUnDistortedYMatrix(toUnDistY);
  }
}

/**
 * This is the function that is called in order to instantiate an OsirisNacCamera
 * object.
 *
 * @param cube The image cube.
 *
 * @return Isis::Camera* OsirisNacCamera
 * @internal
 *   @history 2015-05-21 Sasha Brownsberger - Added documentation.  Removed Lorri
 *            namespace.  Added OsirisNac name.  
 */
extern "C" Isis::Camera *RosettaOsirisCameraPlugin(Isis::Cube &cube) {
  return new Isis::RosettaOsirisCamera(cube);
}
