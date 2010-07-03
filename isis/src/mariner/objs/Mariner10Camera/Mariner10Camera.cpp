/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/06/29 18:16:39 $
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

#include "Mariner10Camera.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "ReseauDistortionMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "iString.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"
#include "Filename.h"

#include <iostream>
#include <iomanip>


using namespace std;
namespace Isis {
  /**
   * Creates a Mariner10 Camera Model
   *
   * @param lab Pvl Label from the image
   *
   * @throws Isis::iException::User - The file does not appear to be a mariner10
   *                                            image
   */
  Mariner10Camera::Mariner10Camera(Pvl &lab) : FramingCamera(lab) {

    //  Turn off the aberration corrections for instrument position object
    InstrumentPosition()->SetAberrationCorrection("NONE");
    InstrumentRotation()->SetFrame(-76000);

    // Set camera parameters
    SetFocalLength();
    SetPixelPitch();

    PvlGroup inst = lab.FindGroup("Instrument", Pvl::Traverse);
    // Get utc start time
    string stime = inst["StartTime"];
    double et;
    utc2et_c(stime.c_str(), &et);
    //std::cout<<std::setw(12)<<std::fixed<<std::setprecision(8)<<"et = "<<et<<std::endl;

    double sclk;
    sce2c_c(-76, et, &sclk);
    //std::cout<<std::setw(12)<<std::fixed<<std::setprecision(8)<<"sclk = "<<sclk<<std::endl;

    sce2t_c(-76, et, &sclk);
    //std::cout<<std::setw(12)<<std::fixed<<std::setprecision(8)<<"sclk = "<<sclk<<std::endl;

    char ssclk[25];
    sce2s_c(-76, et, 25, ssclk);
    //std::cout<<std::setw(25)<<std::fixed<<std::setprecision(8)<<"ssclk = "<<ssclk<<std::endl;

    // Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map, and detector origin
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, NaifIkCode());

    iString ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_SAMPLE";
    double sampleBoresight = GetDouble(ikernKey);
    ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_LINE";
    double lineBoresight = GetDouble(ikernKey);

    focalMap->SetDetectorOrigin(sampleBoresight, lineBoresight);

    // Setup distortion map which is dependent on encounter, use start time
    // MOON:  1973-11-08T03:16:26.350
    iString spacecraft = (string)inst["SpacecraftName"];
    iString instId = (string)inst["InstrumentId"];
    string cam;
    if(instId == "M10_VIDICON_A") {
      cam = "a";
    }
    else if(instId == "M10_VIDICON_B") {
      cam = "b";
    }
    else {
      string msg = "File does not appear to be a Mariner10 image";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    const string fname = Filename("$mariner10/reseaus/mar10" + cam
                                  + "MasterReseaus.pvl").Expanded();

    try {
      new ReseauDistortionMap(this, lab, fname);
    }
    catch(iException &e) {
      std::string msg = "Unable to create distortion map.";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    SetEphemerisTime(et);
    LoadCache();
  }
} // end namespace isis

extern "C" Isis::Camera *Mariner10CameraPlugin(Isis::Pvl &lab) {
  return new Isis::Mariner10Camera(lab);
}
