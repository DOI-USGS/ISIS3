/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2009/08/31 15:12:31 $                                                                 
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
#include "HiriseCamera.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include <iomanip>


namespace Isis {
  namespace Mro {


   /**
    * Creates a Hirise Camera Model 
    * 
    * @param lab Pvl label from the iamge
    */
    HiriseCamera::HiriseCamera (Isis::Pvl &lab) : Isis::LineScanCamera(lab) {
      // Setup camera characteristics from instrument and frame kernel
      SetFocalLength();
      SetPixelPitch();
      //LoadFrameMounting("MRO_SPACECRAFT", "MRO_HIRISE_OPTICAL_AXIS");
      InstrumentRotation()->SetFrame(-74690);

      // Get required keywords from instrument group
      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      int tdiMode = inst["Tdi"];
      double binMode = inst["Summing"];
      int chan = inst["ChannelNumber"];
      int cpmm = inst["CpmmNumber"];
      double deltaLineTimerCount = inst["DeltaLineTimerCount"];
      std::string stime = inst["SpacecraftClockStartCount"];

      // Convert CPMM number to CCD number
      static int cpmm2ccd[] = {0,1,2,3,12,4,10,11,5,13,6,7,8,9};
      int ccd = cpmm2ccd[cpmm];

      // Compute the line rate, convert to seconds, and multiply by the
      // downtrack summing
      double unBinnedRate = (74.0 + (deltaLineTimerCount/16.0)) / 1000000.0;
      double lineRate = unBinnedRate * binMode;

      // Convert the spacecraft clock count to ephemeris time
      SpiceDouble et;
      // The -74999 is the code to select the transformation from
      // high-precision MRO SCLK to ET
      scs2e_c (-74999,stime.c_str(),&et);
      // Adjust the start time so that it is the effective time for
      // the first line in the image file.  Note that on 2006-03-29, this
      // time is now subtracted as opposed to adding it.  The computed start
      // time in the EDR is at the first serial line.
      et -= unBinnedRate * (((double) tdiMode/2.0) - 0.5);
      // Effective observation
      // time for all the TDI lines used for the
      // first line before doing binning
      et += unBinnedRate * (((double) binMode/2.0) - 0.5);
      // Effective observation time of the first line
      // in the image file, which is possibly binned

      // Compute effective line number within the CCD (in pixels) for the
      // given TDI mode.
      //   This is the "centered" 0-based line number, where line 0 is the
      //   center of the detector array and line numbers decrease going
      //   towards the serial readout.  Line number +64 sees a spot
      //   on the ground before line number 0 or -64.
      double ccdLine_c = -64.0 + ((double) tdiMode / 2.0);

      // Setup detector map for transform of image pixels to detector position
      //      CameraDetectorMap *detectorMap = 
      //        new LineScanCameraDetectorMap(this,et,lineRate);
      LineScanCameraDetectorMap *detectorMap = 
        new LineScanCameraDetectorMap(this,et,lineRate);
      detectorMap->SetDetectorSampleSumming(binMode);
      detectorMap->SetDetectorLineSumming(binMode);
      if (chan == 0) {
        detectorMap->SetStartingDetectorSample(1025.0);
      }

      // Setup focal plane map for transform of detector position to
      // focal plane x/y.  This will read the appropriate CCD
      // transformation coefficients from the instrument kernel
      CameraFocalPlaneMap *focalMap = 
        new CameraFocalPlaneMap(this,-74600-ccd);
      focalMap->SetDetectorOrigin(1024.5,0.0);
      focalMap->SetDetectorOffset(0.0,ccdLine_c);

      // Setup distortion map.  This will read the optical distortion
      // coefficients from the instrument kernel
      CameraDistortionMap *distortionMap = new CameraDistortionMap(this);
      distortionMap->SetDistortion(NaifIkCode());

      // Setup the ground and sky map to transform undistorted focal 
      // plane x/y to lat/lon or ra/dec respectively.
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }
    
    //! Destroys the HiriseCamera object
    HiriseCamera::~HiriseCamera () {}
  }
}

//    H i r i s e C a m e r a P l u g i n
//
extern "C" Isis::Camera *HiriseCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Mro::HiriseCamera(lab);
}
