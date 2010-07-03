/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
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

#include <string>
#include "HrscCamera.h"
#include "VariableLineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "iTime.h"
#include "Statistics.h"

using namespace std;

namespace Isis {
  namespace Mex {
   /**
    * Creates a HrscCamera Camera Model 
    * 
    * @param lab Pvl label from the iamge
    */
    HrscCamera::HrscCamera (Isis::Pvl &lab) : Isis::LineScanCamera(lab) {
      // Setup camera characteristics from instrument and frame kernel
      SetFocalLength();
      SetPixelPitch(0.007);
      InstrumentRotation()->SetFrame(-41210);

      // Get required keywords from instrument group
      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      iTime stime(inst["StartTime"][0]);

      ReadLineRates(lab.Filename());

      // Setup detector map for transform of image pixels to detector position
      new VariableLineScanCameraDetectorMap(this,p_lineRates);
      DetectorMap()->SetDetectorSampleSumming(inst["Summing"]);

      // Setup focal plane map for transform of detector position to
      // focal plane x/y.  This will read the appropriate CCD
      // transformation coefficients from the instrument kernel

      new CameraFocalPlaneMap(this, NaifIkCode());

      string ikernKey = "INS" + iString((int)NaifIkCode())  + "_BORESIGHT_SAMPLE";
      double sampleBoresight = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode())  + "_BORESIGHT_LINE";
      double lineBoresight = GetDouble(ikernKey);

      FocalPlaneMap()->SetDetectorOrigin(sampleBoresight, lineBoresight);

      // Setup distortion map.  This will read the optical distortion
      // coefficients from the instrument kernel
      new CameraDistortionMap(this);

      // Setup the ground and sky map to transform undistorted focal 
      // plane x/y to lat/lon or ra/dec respectively.
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }

    //! Destroys the HiriseCamera object
    HrscCamera::~HrscCamera () {}

    void HrscCamera::ReadLineRates(iString filename) {
      Table timesTable("LineScanTimes", filename);

      if(timesTable.Records() <= 0) {
        std::string msg = "Table [LineScanTimes] in [";
        msg += filename + "] must not be empty";
        throw iException::Message(iException::Pvl, msg, _FILEINFO_);
      }

      for(int i = 0; i < timesTable.Records(); i++) {
        p_lineRates.push_back(LineRateChange((int)timesTable[i][2],(double)timesTable[i][0], timesTable[i][1]));
      }

      if(p_lineRates.size() <= 0) {
        std::string msg = "There is a problem with the data within the Table ";
        msg += "[LineScanTimes] in [" + filename + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }
  }
}

//    H r s c C a m e r a P l u g i n
//
extern "C" Isis::Camera *HrscCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Mex::HrscCamera(lab);
}
