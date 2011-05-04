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
#include "CameraFactory.h"
#include "CameraFocalPlaneMap.h"
#include "Filename.h"
#include "iException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for LoCameraFiducialMap..." << endl;

  try {

    cout << "Testing medium fiducial fit on a 100 x 100 cropped frame" << endl;
    Pvl pm("$lo/testData/3083_med_raw.cub");
    Camera *camm = CameraFactory::Create(pm);

    cout << "Filename: " << Filename(pm.Filename()).Name() << endl;
    cout << "CK Frame: " << camm->InstrumentRotation()->Frame() << endl << endl;
    cout.setf(std::ios::fixed);
    cout << setprecision(9);

    // Test all four corners to make sure the conversions are correct
    // Upper left pixel
    double samp = 1.0;
    double line = 1.0;
    double knownUpperLeftX = 37.38041454316298;
    double knownUpperLeftY = -31.62938549926507;

    // Now Convert from detector to distorted focal plane
    if(camm->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camm->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camm->FocalPlaneMap()->FocalPlaneY();
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
    samp = camm->Samples();
    double knownUpperRightX = 36.6926459512808;
    double knownUpperRightY = -31.62980917422247;

    // Now Convert from detector to distorted focal plane
    if(camm->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camm->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camm->FocalPlaneMap()->FocalPlaneY();

      if(fabs(focalPlaneX - knownUpperRightX) < 1E-9 &&
          fabs(focalPlaneY - knownUpperRightY) < 1E-9) {
        cout << "Upper right corner OK" << endl;
      }
      else {
        cout << setprecision(16) << "Upper right X off by: " << focalPlaneX - knownUpperRightX << endl;
        cout << setprecision(16) << "Upper right Y off by: " << focalPlaneY - knownUpperRightY << endl;
      }
    }

    // Lower left pixel
    samp = 1.0;
    line = camm->Lines();
    double knownLowerLeftX = 37.37931706369603;
    double knownLowerLeftY = -30.94212343336155;

    // Now Convert from detector to distorted focal plane
    if(camm->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camm->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camm->FocalPlaneMap()->FocalPlaneY();
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
    samp = camm->Samples();
    line = camm->Lines();
    double knownLowerRightX = 36.69154847181385;
    double knownLowerRightY = -30.94254710831895;

    // Now Convert from detector to distorted focal plane
    if(camm->FocalPlaneMap()->SetDetector(samp, line)) {
      double focalPlaneX = camm->FocalPlaneMap()->FocalPlaneX();
      double focalPlaneY = camm->FocalPlaneMap()->FocalPlaneY();
      if(fabs(focalPlaneX - knownLowerRightX) < 1E-9 &&
          fabs(focalPlaneY - knownLowerRightY) < 1E-9) {
        cout << "Lower right corner OK" << endl;
      }
      else {
        cout << setprecision(16) << "Lower right X off by: " << focalPlaneX - knownLowerRightX << endl;
        cout << setprecision(16) << "Lower right Y off by: " << focalPlaneY - knownLowerRightY << endl;
      }
      cout  << endl << "-------------------------------------" << endl;
    }

    cout << "Testing high fiducial fit on a full 34530 x 8750 frame" << endl;
    Pvl ph("$lo/testData/4164H_Full_mirror.cub");
    Camera *camh = CameraFactory::Create(ph);

    cout << "Filename: " << Filename(ph.Filename()).Name() << endl;
    cout << "CK Frame: " << camh->InstrumentRotation()->Frame() << endl << endl;
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

  catch(iException &e) {
    e.Report();
  }
}

