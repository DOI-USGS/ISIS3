/**
 * @file
 * $Revision: 1.0 $
 * $Date: 2009/05/27 12:08:01 $
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

#include "VoyagerCamera.h"

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "FileName.h"
#include "IString.h"
#include "iTime.h"
#include "naif/SpiceUsr.h"
#include "NaifStatus.h"
#include "ReseauDistortionMap.h"
#include "Spice.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a Voyager Camera Model using the image labels.  The constructor
   * determines the pixel pitch, focal length, kernels and reseaus, and sets up
   * the focal plane map, detector origin, ground map and sky map. As required
   * for all framing cameras, the start and end exposure times are set in this
   * constructor.
   *
   * @param lab Pvl label from a Voyager image.
   *
   * @throw iException::User - "File does not appear to be a Voyager image.
   *        Invalid InstrumentId."
   * @throw iException::User - "File does not appear to be a Voyager image.
   *        Invalid SpacecraftName."
   *
   * @author 2010-07-19 Mackenzie Boyd
   *
   * @internal
   *   @history 2010-07-19 Mackenzie Boyd - Original Version
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check. Updated
   *                          documentation. Added call to
   *                          ShutterOpenCloseTimes() method.
   */
  VoyagerCamera::VoyagerCamera (Pvl &lab) : FramingCamera(lab) {
    NaifStatus::CheckErrors();

    // Set the pixel pitch
    SetPixelPitch();
    SetFocalLength();
    // Find out what camera is being used, and set the focal length, altinstcode,
    // and camera
    PvlGroup inst = lab.findGroup ("Instrument",Pvl::Traverse);
    QString spacecraft = (QString)inst["SpacecraftName"];
    QString instId = (QString)inst["InstrumentId"];

    QString reseauFileName = "";

    // These set up which kernel and other files to access,
    if (spacecraft == "VOYAGER_1") {
      p_ckFrameId = -31100;
      p_spkTargetId = -31;

      reseauFileName += "1/reseaus/vg1";

      if (instId == "NARROW_ANGLE_CAMERA") {
        reseauFileName += "na";
      }
      else if (instId == "WIDE_ANGLE_CAMERA") {
        reseauFileName += "wa";
      }
      else {
        QString msg = "File does not appear to be a Voyager image. InstrumentId ["
            + instId + "] is invalid Voyager value.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else if (spacecraft == "VOYAGER_2") {
      p_ckFrameId = -32100;
      p_spkTargetId = -32;

      reseauFileName += "2/reseaus/vg2";

      if (instId == "NARROW_ANGLE_CAMERA") {
        reseauFileName += "na";
      }
      else if (instId == "WIDE_ANGLE_CAMERA") {
        reseauFileName += "wa";
      }
      else {
        QString msg = "File does not appear to be a Voyager image. InstrumentId ["
            + instId + "] is invalid Voyager value.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else {
      QString msg = "File does not appear to be a Voyager image. SpacecraftName ["
          + spacecraft + "] is invalid Voyager value.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    new CameraDetectorMap(this);

    // Setup focal plane map, and detector origin
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
    focalMap->SetDetectorOrigin(500.0, 500.0);

    // Master reseau location file
    reseauFileName = "$voyager" + reseauFileName + "MasterReseaus.pvl";
    FileName masterReseaus(reseauFileName);
    try {
      new ReseauDistortionMap(this, lab, masterReseaus.expanded());
    } catch (IException &e) {
      e.print();
    }

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    // StartTime is the most accurate time available because in voy2isis the
    // StartTime is modified to be highly accurate.
    // exposure duration keyword value is measured in seconds
    double exposureDuration = inst["ExposureDuration"];
    iTime startTime;
    startTime.setUtc((QString)inst["StartTime"]);

    // set the start (shutter open) and end (shutter close) times for the image
    /*****************************************************************************
     * AS NOTED IN ISIS2 PROGRAM lev1u_vgr_routines.c:
     * StartTime (FDS count) from the labels calculated to correspond the true spacecraft
     * clock count for the frame.  The true spacecraft clock count is readout
     * time of the frame, which occurred 2 seconds after shutter close.
     *****************************************************************************/
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(startTime.Et(),
                                                            exposureDuration);

    // add half the exposure duration to the start time to get the center if the image
    iTime centerTime = shuttertimes.first.Et() + exposureDuration / 2.0;
    setTime(centerTime);

    LoadCache();
    NaifStatus::CheckErrors();
  }

  /**
   * Returns the shutter open and close times. The user should pass in the
   * ExposureDuration keyword value and the StartTime keyword value, converted
   * to ephemeris time. The StartTime keyword value from the labels represents
   * the true spacecraft clock count. This is the readout time of the frame,
   * which occurred 2 seconds after the shutter close. To find the end time of
   * the exposure, 2 seconds are subtracted from the time input parameter. To
   * find the start time of the exposure, the exposure duration is subtracted
   * from the end time. This method overrides the FramingCamera class method.
   *
   * @param exposureDuration ExposureDuration keyword value from the labels, in
   *                         seconds.
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
  pair<iTime, iTime> VoyagerCamera::ShutterOpenCloseTimes(double time,
                                                          double exposureDuration) {
    pair<iTime, iTime> shuttertimes;
    // To get shutter end (close) time, subtract 2 seconds from the StartTime keyword value
    shuttertimes.second = time - 2;
    // To get shutter start (open) time, take off the exposure duration from the end time.
    shuttertimes.first = shuttertimes.second.Et() - exposureDuration;
    return shuttertimes;
  }
}

/**
 * This is the function that is called in order to instantiate a VoyagerCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* VoyagerCamera
 * @author 2010-07-19 Mackenzie Boyd
 * @internal
 *   @history 2010-07-19 Mackenzie Boyd - Original Version
 */
extern "C" Isis::Camera *VoyagerCameraPlugin(Isis::Pvl &lab) {
  return new Isis::VoyagerCamera(lab);
}
