/**                                                                       
 * @file                                                                  
 * $Revision: 1.11 $
 * $Date: 2009/08/31 15:12:30 $
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
#include "MdisCamera.h"
#include "iString.h"
#include "iException.h"
#include "CameraDistortionMap.h"
#include "TaylorCameraDistortionMap.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"

using namespace std;

namespace Isis {
  namespace Messenger {
    /**
     * @brief Initialize the MDIS camera model for NAC and WAC
     * 
     * This constructor reads the Messenger/MDIS instrument addendum for many of
     * its default parameters.
     * 
     * This camera model does not support subframes of jailbar imaging modes.
     * An exception is thrown in those cases.
     * 
     * @param lab The label provided to initialize the camera model for.
     * 
     * @throws iException:User - when images are subframes or jailbars.
     */
    MdisCamera::MdisCamera (Isis::Pvl &lab) : Isis::FramingCamera(lab) {

      // Set up detector constants
      const int MdisWac(-236800);
//    const int MdisNac(-236820);

      PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);

      // Clarification on MDIS subframe image mode provides us the ability to
      // support this mode now.  The entire MDIS frame is geometrically valid
      // but only portions of the full frame actually contain image data.  The
      // portions outside subframes should be NULL and not interfere in 
      // downstream processing, such as mosaics.
#if defined(MDIS_SUBFRAMES_UNSUPPORTED)
      int subFrameMode = inst["SubFrameMode"];
      if (subFrameMode != 0) {
        string msg = "Subframe imaging mode is not supported!";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
#endif

      //  According to the MDIS team, this is nothing to be concerned with and
      //  should be treated as other normal observations.  So the test to 
      // disallow it has been effectively removed 2007-09-05 (KJB).
#if defined(MDIS_JAILBARS_UNSUPPORTED)
      int jailBars = inst["JailBars"];
      if (jailBars != 0) {
        string msg = "Jail bar observations are not currently supported!";
        throw iException::Message(iException::Programmer,msg, _FILEINFO_);
      }
#endif

      //  Determine filter number.  Only conditional code required for
      //  NAC and WAC support!
      int filterNumber(0);    //  Default appropriate for MDIS-NAC
      if (NaifIkCode() == MdisWac) {
        PvlGroup &bandBin = lab.FindGroup("BandBin", Pvl::Traverse);
        filterNumber = bandBin["Number"];
      }

      //  Set up instrument and filter code strings
      iString ikCode((int) NaifIkCode());
      int fnCode(NaifIkCode() - filterNumber);
      iString filterCode(fnCode);
      std::string ikernKey;

      // Fetch the frame translations from the instrument kernels
      ikernKey = "INS" + ikCode + "_REFERENCE_FRAME";
      iString baseFrame = GetString(ikernKey);

      ikernKey = "INS" + filterCode + "_FRAME";
      iString ikFrame = GetString(ikernKey);

      // Set up the camera info from ik/iak kernels

      //  Turns out (2008-01-17) the WAC has different focal lengths for
      // each filter.  Added to the instrument kernel (IAK) on this date.
      double focalLength = GetDouble("INS" + filterCode + "_FOCAL_LENGTH");
      SetFocalLength(focalLength);

      SetPixelPitch();

// Removed by Jeff Anderson.  The refactor of the SPICE class
// uses frames always so this is no longer needed
//      LoadFrameMounting(baseFrame, ikFrame, false);

      // Get the start time from labels as the starting image time plus half
      // the exposure duration (in <MS>) to get pointing attitude.
      //  !!NOTE:  The ephemeris time MUST be set prior to creating the
      //           cache (CreateCache) because the kernels are all unloaded
      //           after the cache is done and this operation will fail!!
      string stime = inst["SpacecraftClockCount"];
      double expTime = inst["ExposureDuration"];

      SpiceDouble etStart;
      scs2e_c (NaifSpkCode(),stime.c_str(),&etStart);

      //  Setup camera detector map
      CameraDetectorMap *detMap = new CameraDetectorMap(this);

      // Setup focal plane map, and detector origin for the instrument that
      // may have a filter (WAC only!).
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,fnCode);

      //  Retrieve boresight location from instrument kernel (IK) (addendum?)
      ikernKey = "INS" + ikCode + "_BORESIGHT_SAMPLE";
      double sampleBoreSight = GetDouble(ikernKey);

      ikernKey = "INS" + ikCode + "_BORESIGHT_LINE";
      double lineBoreSight = GetDouble(ikernKey);

      //  Apply the boresight
      focalMap->SetDetectorOrigin(sampleBoreSight,lineBoreSight);

      // Determine summing.  MDIS has two sources of summing or binning.  
      // One is performed in the FPU and the in the MP, post-observation,
      // on-board after coming out of the FPGAs, where the FPU binning is
      // performed.  The FPU binning was programmed incorrectly and the
      // actual pixels from the detector are peculiar.  Hence, I have
      // designed this camera model such that the offsets can be managed
      // external to the code.  See the MDIS instrument kernel addendum
      // in $ISIS3DATA/messenger/kernels/iak/mdisAddendum???.ti for the
      // offsets for *each* detector.  Note that an offset is only applied 
      // when FPU binning is performed.
      int fpuBinMode   = inst["FpuBinningMode"];
      int pixelBinMode = inst["PixelBinningMode"];

      int summing(1 + (1 * pixelBinMode));
      //  FPU binning was performed, retrieve the FPU binning offsets and
      //  apply them to the focal plane mapping.
      if (fpuBinMode == 1) {
        ikernKey = "INS" + ikCode + "_FPUBIN_START_SAMPLE";
        double fpuStartingSample = GetDouble(ikernKey);
        detMap->SetStartingDetectorSample(fpuStartingSample);

        ikernKey = "INS" + ikCode + "_FPUBIN_START_LINE";
        double fpuStartingLine = GetDouble(ikernKey);
        detMap->SetStartingDetectorLine(fpuStartingLine);

        summing *= 2;
      }

      //  Set summing/binning modes as an accumulation of FPU and MP binning.
      detMap->SetDetectorLineSumming(summing);
      detMap->SetDetectorSampleSumming(summing);

      // Setup distortion map.  As of 2007/12/06, we now have an actual model.
      // Note that this model supports distinct distortion for each WAC filter.
      // See $ISIS3DATA/messenger/kernels/iak/mdisAddendumXXX.ti or possibly
      // $ISIS3DATA/messenger/kernels/ik/msgr_mdis_vXXX.ti for the *_OD_K
      // parameters.
      // NAC has a new implementation of its distortion contributed by
      // Scott Turner and Lillian Nguyen at JHUAPL.
      if (NaifIkCode() == MdisWac) {
        CameraDistortionMap *distortionMap = new CameraDistortionMap(this);
        distortionMap->SetDistortion(fnCode);
      }
      else {  // Camera is the NAC, use new distortion model
        try {
          TaylorCameraDistortionMap *distortionMap = new TaylorCameraDistortionMap(this); 
          distortionMap->SetDistortion(NaifIkCode());
        } catch (iException &ie) {
          string msg = "New MDIS/NAC distortion model invalidates previous "
                       "SPICE - you must rerun spiceinit to get new kernels";
          ie.Message(iException::User, msg, _FILEINFO_);
          throw;
        }
      }

      // Setup the ground and sky map
      new CameraGroundMap(this);
      new CameraSkyMap(this);

      // Create a cache and grab spice info since it does not change for
      // a framing camera (fixed spacecraft position and pointing) after,
      // of course applying the gimble offset which is handled in the SPICE
      // kernels (thank you!).  Note this was done automagically in the
      // SetEpheremisTime call above.  IMPORTANT that it be done prior to
      // creating the cache since all kernels are unloaded, essentially
      // clearing the pool and whacking the frames definitions, required to
      double obsTime(etStart + ((expTime / 2.0) / 1000.0));
      SetEphemerisTime(obsTime);
      LoadCache();
    }    
  }
}

/**
 * This is the function that is called in order to instantiate a MdisCamera 
 * object. 
 * 
 * @param lab Cube labels
 * 
 * @return Isis::Camera* MdisCamera
 */
extern "C" Isis::Camera *MdisCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Messenger::MdisCamera(lab);
}
