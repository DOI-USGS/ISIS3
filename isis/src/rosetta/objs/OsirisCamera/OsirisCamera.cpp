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

#include "OsirisCamera.h"

#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a Rosetta Osiris NAC Framing Camera object. 
   *
   * @param lab Pvl label from a Osiris Nac Framing Camera image.
   *
   * @original author Stuart Sides
   * 
   * @modified by Sasha Brownsberger
   *
   * @internal
   */

  OsirisCamera::OsirisCamera(Cube &cube) : FramingCamera(cube) {
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
    // cout << "Setting up FocalPlaneMap...\n";
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    // The boresight position recorded in the IK is zero-based and therefore needs to be adjusted 
    // for ISIS
    // Don't know if this is true for OSIRIS. For now, we'll keep as is and see if things look off -Sasha
    double boresightSample = Spice::getDouble("INS" + ikCode + "_BORESIGHT",0) + 1.0;
    double boresightLine = Spice::getDouble("INS" + ikCode + "_BORESIGHT",1) + 1.0;
    focalMap->SetDetectorOrigin(boresightSample,boresightLine); //Presumably, don't need to worry about z (?)

    new CameraDetectorMap(this);
    new CameraDistortionMap(this);

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
  pair<iTime, iTime> OsirisCamera::ShutterOpenCloseTimes(double time,
                                                         double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }
}

/**
 * This is the function that is called in order to instantiate an OsirisNacCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* OsirisNacCamera
 * @internal
 *   @history 2015-05-21 Sasha Brownsberger - Added documentation.  Removed Lorri
 *            namespace.  Added OsirisNac name.  
 */
extern "C" Isis::Camera *OsirisCameraPlugin(Isis::Cube &cube) {
  return new Isis::OsirisCamera(cube);
}
