/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/08/31 15:12:28 $
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

#include <string>
#include "IdealCamera.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include <iomanip>


namespace Isis {
 /**
  * Creates a generic camera model.  That is a camera without
  * optical distortion.  The following information from the label
  * must be available
  * \code  
  *  
  * Group = Instrument
  *   SpacecraftName = IdealSpacecraft
  *   InstrumentId   = IdealCamera
  *   TargetName     = Mars | Moon | etc
  *   StartTime      = YYYY-MM-DDTHH:MM:SS.SSS
  *   StopTime       = YYYY-MM-DDTHH:MM:SS.SSS
  *
  *   EphermisTime = nnnnnnnnnn.sss <second>
  *   ExposureDuration = nnn.nn <milliseconds>
  *
  *   InstrumentType = Framing | Linescan
  *   FocalLength    = nnn.n <millimeters>
  *   PixelPitch      = nn.n <millimeters>
  *   SampleDetectors = nnnn
  *   LineDetectors   = nnnn
  * End_Group 
  *  
  * \endcode
  * Note boresight is assumed to be at the center of the
  * detectors
  *
  * @param lab Pvl label from the image
  * @internal
  * @history 2007-02-12 Debbie A. Cook - Added sign for all the trans parameters
  */
  IdealCamera::IdealCamera (Isis::Pvl &lab) : Isis::Camera(lab) {
    // Get required keywords from instrument group
    Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
    double focalLength      = inst["FocalLength"];
    double pixelPitch       = inst["PixelPitch"];
    double et               = inst["EphemerisTime"];
    double exposureDuration=0.;
    if (inst.HasKeyword("ExposureDuration")) exposureDuration = (double) inst["ExposureDuration"] / 1000.0;
    double sampleDetectors  = inst["SampleDetectors"];
    double lineDetectors    = inst["LineDetectors"];

    int yDependency;
    int xDependency = inst["FocalPlaneXDependency"];
    double xdir,ydir,sdir,ldir;    // Pixel direction

    xdir = 1.;
    if (inst.HasKeyword("TransX")) xdir = inst["TransX"];
    ydir = 1.;
    if (inst.HasKeyword("TransY")) ydir = inst["TransY"];

    if (xDependency == Isis::CameraFocalPlaneMap::Sample) {
      yDependency = Isis::CameraFocalPlaneMap::Line;
      sdir = xdir;
      ldir = ydir;
    }
    else {
      yDependency = Isis::CameraFocalPlaneMap::Sample;
      sdir = ydir;
      ldir = xdir;
    }
    double keyval[3];
    
    // Put the translation coefficients into the Naif kernel pool so the 
    // CameraFocalPlaneClass can find them
    keyval[0] = 0.;
    if (inst.HasKeyword("TransX0")) keyval[0] = inst["TransX0"];
    keyval[xDependency] = pixelPitch*xdir;
    keyval[yDependency] = 0.;
    pdpool_c("IDEAL_TRANSX", 3, keyval);

    keyval[0] = 0.;
    if (inst.HasKeyword("TransY0")) keyval[0] = inst["TransY0"];
    keyval[yDependency] = pixelPitch*ydir;
    keyval[xDependency] = 0.;
    pdpool_c("IDEAL_TRANSY", 3, keyval);

    keyval[0] = 0.;
    if (inst.HasKeyword("TransS0")) keyval[0] = inst["TransS0"];
    keyval[xDependency] = 1/pixelPitch*sdir;
    keyval[yDependency] = 0.;
    pdpool_c("IDEAL_TRANSS", 3, keyval);

    keyval[0] = 0.;
    if (inst.HasKeyword("TransL0")) keyval[0] = inst["TransL0"];
    keyval[yDependency] = 1/pixelPitch*ldir;
    keyval[xDependency] = 0.;
    pdpool_c("IDEAL_TRANSL", 3, keyval);

    // Setup camera characteristics from instrument
    SetFocalLength(focalLength);
    SetPixelPitch(pixelPitch);

    // Create correct camera type
    Isis::iString type = (std::string) inst["InstrumentType"];
    if (type.UpCase() == "FRAMING") {
      p_framing = true;
      new CameraDetectorMap(this);
      CameraFocalPlaneMap *fmap = new CameraFocalPlaneMap(this,0);
      fmap->SetDetectorOrigin(sampleDetectors/2.0+0.5,
                              lineDetectors/2.0+0.5);
      new CameraDistortionMap(this);
      new CameraGroundMap(this);
      new CameraSkyMap(this);

      SetEphemerisTime(et);
      LoadCache();
    }
    else if (type.UpCase() == "LINESCAN") {
      p_framing = false;
      new LineScanCameraDetectorMap(this,et,exposureDuration);
      CameraFocalPlaneMap *fmap = new CameraFocalPlaneMap(this,0);
      fmap->SetDetectorOrigin(sampleDetectors/2.0+0.5,
                              0.0);
      new CameraDistortionMap(this);
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }
    else {
      std::string msg = "Unknown InstrumentType [" +
        (std::string) inst["InstrumentType"] + "]";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  }

  //! Destroys the IdealCamera object
  IdealCamera::~IdealCamera () {}
}

/**
 * @brief External C function for createing the camera plugin
 *
 * This function is used by the CameraFactory to create an instance of the 
 * IdealCamera object. 
 *  
 * @param lab The Isis::Pvl label object used for the information as how to 
 *            create this object.
 */
extern "C" Isis::Camera *IdealCameraPlugin (Isis::Pvl &lab) {
  return new Isis::IdealCamera(lab);
}
