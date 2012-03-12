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

#include "IdealCamera.h"

#include <iomanip>
#include <string>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "iString.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"
#include "Pvl.h"

using namespace std;
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
   *   @history 2007-02-12 Debbie A. Cook - Added sign for all the trans
   *           parameters
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  IdealCamera::IdealCamera(Pvl &lab) : Camera(lab) {
    NaifStatus::CheckErrors();
    // Get required keywords from instrument group
    PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);
    double focalLength      = inst["FocalLength"];
    double pixelPitch       = inst["PixelPitch"];
    double et               = inst["EphemerisTime"];
    double exposureDuration = 0.;
    if(inst.HasKeyword("ExposureDuration")) exposureDuration = (double) inst["ExposureDuration"] / 1000.0;
    double sampleDetectors  = inst["SampleDetectors"];
    double lineDetectors    = inst["LineDetectors"];

    int yDependency;
    int xDependency = inst["FocalPlaneXDependency"];
    double xdir, ydir, sdir, ldir; // Pixel direction

    xdir = 1.;
    if(inst.HasKeyword("TransX")) xdir = inst["TransX"];
    ydir = 1.;
    if(inst.HasKeyword("TransY")) ydir = inst["TransY"];

    if(xDependency == CameraFocalPlaneMap::Sample) {
      yDependency = CameraFocalPlaneMap::Line;
      sdir = xdir;
      ldir = ydir;
    }
    else {
      yDependency = CameraFocalPlaneMap::Sample;
      sdir = ydir;
      ldir = xdir;
    }
    double keyval[3];

    // Put the translation coefficients into the Naif kernel pool so the
    // CameraFocalPlaneClass can find them
    keyval[0] = 0.;
    if(inst.HasKeyword("TransX0")) keyval[0] = inst["TransX0"];
    keyval[xDependency] = pixelPitch * xdir;
    keyval[yDependency] = 0.;
    pdpool_c("IDEAL_TRANSX", 3, keyval);

    keyval[0] = 0.;
    if(inst.HasKeyword("TransY0")) keyval[0] = inst["TransY0"];
    keyval[yDependency] = pixelPitch * ydir;
    keyval[xDependency] = 0.;
    pdpool_c("IDEAL_TRANSY", 3, keyval);

    keyval[0] = 0.;
    if(inst.HasKeyword("TransS0")) keyval[0] = inst["TransS0"];
    keyval[xDependency] = 1 / pixelPitch * sdir;
    keyval[yDependency] = 0.;
    pdpool_c("IDEAL_TRANSS", 3, keyval);

    keyval[0] = 0.;
    if(inst.HasKeyword("TransL0")) keyval[0] = inst["TransL0"];
    keyval[yDependency] = 1 / pixelPitch * ldir;
    keyval[xDependency] = 0.;
    pdpool_c("IDEAL_TRANSL", 3, keyval);

    // Setup camera characteristics from instrument
    SetFocalLength(focalLength);
    SetPixelPitch(pixelPitch);

    // Create correct camera type
    iString type = (string) inst["InstrumentType"];
    if(type.UpCase() == "FRAMING") {
      p_framing = true;
      new CameraDetectorMap(this);
      CameraFocalPlaneMap *fmap = new CameraFocalPlaneMap(this, 0);
      fmap->SetDetectorOrigin(sampleDetectors / 2.0 + 0.5,
                              lineDetectors / 2.0 + 0.5);
      new CameraDistortionMap(this);
      new CameraGroundMap(this);
      new CameraSkyMap(this);

      SetTime(et);
      LoadCache();
    }
    else if(type.UpCase() == "LINESCAN") {
      p_framing = false;
      new LineScanCameraDetectorMap(this, et, exposureDuration);
      CameraFocalPlaneMap *fmap = new CameraFocalPlaneMap(this, 0);
      fmap->SetDetectorOrigin(sampleDetectors / 2.0 + 0.5,
                              0.0);
      new CameraDistortionMap(this);
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
      NaifStatus::CheckErrors();
    }
    else {
      string msg = "Unknown InstrumentType [" +
                        (string) inst["InstrumentType"] + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  //! Destroys the IdealCamera object
  IdealCamera::~IdealCamera() {}


  /**
   * CK frame ID.  This is an overridden method for the Camera class pure
   * virtual method. It will always throw an error for Ideal Camera models.
   * @throw IException - "No CK Frame ID for Ideal Camera class."
   * @return @b int
   */
  int IdealCamera::CkFrameId() const {
    string msg = "No CK Frame ID for Ideal Camera class";
    throw IException(IException::User, msg, _FILEINFO_);
  }


  /**
   * CK Reference ID.  This is an overridden method for the Camera class pure
   * virtual method. It will always throw an error for Ideal Camera models.
   * @throw IException - "No CK Reference ID for Ideal Camera class."
   * @return @b int
   */
  int IdealCamera::CkReferenceId() const {
    string msg = "No CK Reference ID for Ideal Camera class";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  /**
   * SPK Target ID.  This is an overridden method for the Camera class pure
   * virtual method. It will always throw an error for Ideal Camera models.
   * @throw IException - "No SPK Target ID for Ideal Camera class."
   * @return @b int
   */
  int IdealCamera::SpkTargetId() const {
    string msg = "No SPK Target ID for Ideal Camera class";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  /**
   * SPK Center ID.  This is an overridden method for the Camera class pure
   * virtual method. It will always throw an error for Ideal Camera models.
   * @throw IException - "No SPK Center ID for Ideal Camera class."
   * @return @b int
   */
  int IdealCamera::SpkCenterId() const {
    string msg = "No SPK Center ID for Ideal Camera class";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  /**
   * SPK Reference ID.  This is an overridden method for the Camera class pure
   * virtual method. It will always throw an error for Ideal Camera models.
   * @throw IException - "No SPK Reference ID for Ideal Camera class."
   * @return @b int
   */
  int IdealCamera::SpkReferenceId() const {
    string msg = "No SPK Reference ID for Ideal Camera class";
    throw IException(IException::User, msg, _FILEINFO_);
  }
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
extern "C" Isis::Camera *IdealCameraPlugin(Isis::Pvl &lab) {
  return new Isis::IdealCamera(lab);
}
