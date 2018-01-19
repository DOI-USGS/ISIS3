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

#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFactory.h"
#include "CameraFocalPlaneMap.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for ApolloPanoramicCameraFiducialMap..." << endl;

  try {

    cout << "Testing fiducial fit on full Apollo 15 Panoramic AS15-P-0116 tile 5 (39795 x 25448)" << endl;
    Cube tile5("/work/projects/progteam/kedmundson/Apollo/PAN/AS15/testCubes/trimmed/AS15-P-0116_0005_TRIM.cub", "r");
    Camera *cam = CameraFactory::Create(tile5);

    cout << "FileName: " << FileName(tile5.fileName()).name() << endl;
    cout << "CK Frame: " << cam->instrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(9);

    // Test four corners detector to distorted focal plane
    cout << "testing four corners detector to distorted focal plane..." << endl << endl;
    
    // upper left pixel
    double samp = 1.0;
    double line = 1.0;
    double knownUpperLeftX = 63.011779675;
    double knownUpperLeftY = 148.840431351;

    // convert from detector to distorted focal plane
    if(cam->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = cam->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = cam->FocalPlaneMap()->FocalPlaneY();
      
      cout << "Upper Left" << endl << setprecision(0) << "  (" << samp << " , " << line
           << ") => (Xfp, Yfp) = (" << setprecision(16) << focalPlaneX << " , " << focalPlaneY
           << ")" << endl;
	   
      if(fabs(focalPlaneX - knownUpperLeftX) < 1E-9 &&
          fabs(focalPlaneY - knownUpperLeftY) < 1E-9) {
        cout << "Upper Left OK" << endl << endl;
      }
      else {
        cout << setprecision(16) << "Upper Left X off by: " << focalPlaneX - knownUpperLeftX << endl;
        cout << setprecision(16) << "Upper Left Y off by: " << focalPlaneY - knownUpperLeftY << endl;
      }
    }

    // upper right pixel
    samp = cam->Samples();
    double knownUpperRightX = 62.786897000;
    double knownUpperRightY = -51.221066454;

    // convert from detector to distorted focal plane
    if(cam->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = cam->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = cam->FocalPlaneMap()->FocalPlaneY();
      cout << "Upper Right" << endl << "  (1, " << setprecision(0) << samp << ") => (Xfp, Yfp) = (" << setprecision(16) << focalPlaneX << " , " << focalPlaneY << ")" << endl;

      if(fabs(focalPlaneX - knownUpperRightX) < 1E-9 &&
          fabs(focalPlaneY - knownUpperRightY) < 1E-9) {
        cout << "Upper Right OK" << endl << endl;
      }
      else {
        cout << setprecision(16) << "Upper Right X off by: " << focalPlaneX - knownUpperRightX << endl;
        cout << setprecision(16) << "Upper Right Y off by: " << focalPlaneY - knownUpperRightY << endl;
      }
    }

    // lower left pixel
    samp = 1.0;
    line = cam->Lines();
    double knownLowerLeftX = -61.5404808995972417;
    double knownLowerLeftY = 148.8119557628731684;

    // convert from detector to distorted focal plane
    if(cam->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = cam->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = cam->FocalPlaneMap()->FocalPlaneY();
      cout << "Lower Left" << endl << "  (1, " << setprecision(0) << samp << ") => (Xfp, Yfp) = (" << setprecision(16) << focalPlaneX << " , " << focalPlaneY << ")" << endl;

      if(fabs(focalPlaneX - knownLowerLeftX) < 1E-9 &&
          fabs(focalPlaneY - knownLowerLeftY) < 1E-9) {
        cout << "Lower Left OK" << endl << endl;
      }
      else {
        cout << setprecision(16) << "Lower Left X off by: " << focalPlaneX - knownLowerLeftX << endl;
        cout << setprecision(16) << "Lower Left Y off by: " << focalPlaneY - knownLowerLeftY << endl;
      }
    }

    // lower right pixel
    samp = cam->Samples();
    line = cam->Lines();
    double knownLowerRightX = -61.7653635743449598;
    double knownLowerRightY = -51.2495420416903897;

    // convert from detector to distorted focal plane
    if(cam->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = cam->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = cam->FocalPlaneMap()->FocalPlaneY();
      cout << "Lower Right" << endl << "  (1, " << setprecision(0) << samp << ") => (Xfp, Yfp) = (" << setprecision(16) << focalPlaneX << " , " << focalPlaneY << ")" << endl;

      if(fabs(focalPlaneX - knownLowerRightX) < 1E-9 &&
          fabs(focalPlaneY - knownLowerRightY) < 1E-9) {
        cout << "Lower Right OK" << endl << endl;
      }
      else {
        cout << setprecision(16) << "Lower Right X off by: " << focalPlaneX - knownLowerRightX << endl;
        cout << setprecision(16) << "Lower Right Y off by: " << focalPlaneY - knownLowerRightY << endl;
      }
      cout  << endl << "-------------------------------------" << endl;
    }
    
    // Test four corners distorted focal plane to detector
    cout << "testing four corners distorted focal plane to detector..." << endl << endl;
    
    // upper left pixel
    double distortedFocalPlaneX = 63.011779675;
    double distortedFocalPlaneY = 148.840431351;
    double knownUpperLeftSample = 1.0;
    double knownUpperLeftLine = 1.0;

    // convert from detector to distorted focal plane
    if(cam->DistortionMap()->SetUndistortedFocalPlane(distortedFocalPlaneX, distortedFocalPlaneY)) {
      double sample = cam->DetectorMap()->ParentSample();
      double line = cam->DetectorMap()->ParentLine();
      
      cout << "Upper Left" << endl << setprecision(16) << "  (" << distortedFocalPlaneX << " , "
           << distortedFocalPlaneY
           << ") => (sample, line) = (" << setprecision(16) << sample << " , " << line
           << ")" << endl;
	   
      if(fabs(sample - knownUpperLeftSample) < 1E-9 &&
          fabs(line - knownUpperLeftLine) < 1E-9) {
        cout << "Upper Left OK" << endl << endl;
      }
      else {
        cout << setprecision(16) << "Upper Left sample off by: " << sample - knownUpperLeftSample << endl;
        cout << setprecision(16) << "Upper Left line off by: " << line - knownUpperLeftLine << endl;
      }
    }
    
    
    delete cam;
  }
/*
    cout << "Testing high fiducial fit on a full 34530 x 8750 frame" << endl;
    Cube ch("$lo/testData/4164H_Full_mirror.cub", "r");
    Camera *camh = CameraFactory::Create(ch);

    cout << "FileName: " << FileName(ch.fileName()).name() << endl;
    cout << "CK Frame: " << camh->instrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(9);

    // Test all four corners and center pixel to make sure the conversions are correct
    // Upper left pixel
    samp = 1.0;
    line = 1.0;
    knownUpperLeftX = 125.8894771096694;
    knownUpperLeftY = -31.23569821243375;

    // Now Convert from detector to distorted focal plane
    if(camh->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camh->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camh->FocalPlaneMap()->FocalPlaneY();
      if(fabs(focalPlaneX - knownUpperLeftX) < 1E-9 &&
          fabs(focalPlaneY - knownUpperLeftY) < 1E-9) {
        cout << "Upper left corner OK" << endl;
      }
      else {
        cout << setprecision(16) << "Upper left X off by: " << focalPlaneX - knownUpperLeftX << endl;
        cout << setprecision(16) << "Upper left Y off by: " << focalPlaneY - knownUpperLeftY << endl;
      }
    }

    // Upper right pixel
    samp = camh->Samples();
    knownUpperRightX = -114.13710918057000754;
    knownUpperRightY = -31.21483142132193;

    // Now Convert from detector to distorted focal plane
    if(camh->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camh->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camh->FocalPlaneMap()->FocalPlaneY();

      if(fabs(focalPlaneX - knownUpperRightX) < 1E-9 &&
          fabs(focalPlaneY - knownUpperRightY) < 1E-9) {
        cout << "Upper right corner OK" << endl;
      }
      else {
        cout << setprecision(18) << "Upper right X off by: " << focalPlaneX - knownUpperRightX << endl;
        cout << setprecision(18) << "Upper right Y off by: " << focalPlaneY - knownUpperRightY << endl;
      }
    }

    // Lower left pixel
    samp = 1.0;
    line = camh->Lines();
    knownLowerLeftX = 125.9405233275204;
    knownLowerLeftY = 29.52307388869981;

    // Now Convert from detector to distorted focal plane
    if(camh->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camh->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camh->FocalPlaneMap()->FocalPlaneY();
      if(fabs(focalPlaneX - knownLowerLeftX) < 1E-9 &&
          fabs(focalPlaneY - knownLowerLeftY) < 1E-9) {
        cout << "Lower left corner OK" << endl;
      }
      else {
        cout << setprecision(16) << "Lower left X off by: " << focalPlaneX - knownLowerLeftX << endl;
        cout << setprecision(16) << "Lower left Y off by: " << focalPlaneY - knownLowerLeftY << endl;
      }
    }

    // Lower right pixel
    samp = camh->Samples();
    line = camh->Lines();
    knownLowerRightX = -114.0860629627191;
    knownLowerRightY = 29.54394067981162;

    // Now Convert from detector to distorted focal plane
    if(camh->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camh->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camh->FocalPlaneMap()->FocalPlaneY();
      if(fabs(focalPlaneX - knownLowerRightX) < 1E-9 &&
          fabs(focalPlaneY - knownLowerRightY) < 1E-9) {
        cout << "Lower right corner OK" << endl;
      }
      else {
        cout << setprecision(16) << "Lower right X off by: " << focalPlaneX - knownLowerRightX << endl;
        cout << setprecision(16) << "Lower right Y off by: " << focalPlaneY - knownLowerRightY << endl;
      }
    }

    // Center pixel
    samp = camh->Samples() / 2;
    line = camh->Lines() / 2;
    double knownCenterX = 5.90517988076963;//119.9912428434887;
    double knownCenterY = -0.8493513951460194;//-30.39329207495764;

    // Now Convert from detector to distorted focal plane
    if(camh->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camh->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camh->FocalPlaneMap()->FocalPlaneY();
      if(fabs(focalPlaneX - knownCenterX) < 1E-9 &&
          fabs(focalPlaneY - knownCenterY) < 1E-9) {
        cout << "Center corner OK" << endl;
      }
      else {
        cout << setprecision(16) << "Center X off by: " << focalPlaneX - knownCenterX << endl;
        cout << setprecision(16) << "Center Y off by: " << focalPlaneY - knownCenterY << endl;
      }
    }

    delete camh;
  }
*/
  catch(IException &e) {
    e.print();
  }
}

