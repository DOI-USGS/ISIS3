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

#include "NewHorizonsLeisaCamera.h"
#include "NaifStatus.h"
#include "iTime.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "CameraFocalPlaneMap.h"
#include "LineScanCameraDetectorMap.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a New Horizons LEISA LineScanCamera object. LEISA is technically a frame
   * type camera, but it has the etalon filer in front of it exposing each line of a frame to have a
   * different wavelength, so we treat it like a linescan camera. Each band of the ISIS cube is made 
   * by combining all the corresponding frame line numbers into that band (i.e., All the line number 
   * 1s from each frame in an observatoin are combined into band 1, all line number 2s are put into 
   * band 2, and so on) 
   * 
   * @param something Description
   *
   * @author Kristin Berry
   * @internal
   */
  NewHorizonsLeisaCamera::NewHorizonsLeisaCamera(Cube &cube) : LineScanCamera(cube) {

    // Override the SPICE error process for SPICE calls 
    NaifStatus::CheckErrors(); 

    SetFocalLength();
    SetPixelPitch();

    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString expDuration = inst["ExposureDuration"];

    QString stime = inst["SpacecraftClockStartCount"];
    double m_etStart = getClockTime(stime).Et(); 
    
    // The line rate is set to the time between each frame since we are treating LEASA as a linescan    
    double m_lineRate = expDuration.toDouble();

    // The detector map tells us how to convert from image coordinates to
    // detector coordinates. In our case, a (sample,line) to a (sample,time)
    new LineScanCameraDetectorMap(this, m_etStart, m_lineRate);

    // The focal plane map tells us how to go from detector position
    // to focal plane x/y (distorted).  That is, (sample,time) to (x,y) and back.
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
    focalMap->SetDetectorOrigin(128.5, 128.5);

    // Use the defualt no correction distortion map. 
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();  

    // Check to see if there were any SPICE errors
    NaifStatus::CheckErrors();  
  }

}

/**
 * This is the function that is called in order to instantiate a 
 * NewHorizonsLeisaCamera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* NewHorizonsLeisaCamera
 * 
 */
extern "C" Isis::Camera *NewHorizonsLeisaCameraPlugin(Isis::Cube &cube) {
  return new Isis::NewHorizonsLeisaCamera(cube);
}

