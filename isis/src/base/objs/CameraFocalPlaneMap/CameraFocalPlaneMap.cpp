/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2008/06/17 16:10:40 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "Spice.h"
#include "CameraFocalPlaneMap.h"
#include <cmath>

namespace Isis {
  /** Construct mapping between detectors and focal plane x/y
   *
   * @param parent      parent camera that will use this map
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  CameraFocalPlaneMap::CameraFocalPlaneMap(Camera *parent, const int naifIkCode) {
    Init(parent,naifIkCode);
  }

  /** Construct mapping between detectors and focal plane x/y
   *
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  CameraFocalPlaneMap::CameraFocalPlaneMap(const int naifIkCode) {
    Init(0,naifIkCode);
  }

  void CameraFocalPlaneMap::Init(Camera *parent, const int naifIkCode) {
    p_detectorSampleOrigin = 0.0;
    p_detectorLineOrigin = 0.0;
    p_detectorSampleOffset = 0.0;
    p_detectorLineOffset = 0.0;
    p_camera = parent;

    if (naifIkCode != 0) {
      std::string xkey  = "INS" + Isis::iString(naifIkCode) + "_TRANSX";
      std::string ykey  = "INS" + Isis::iString(naifIkCode) + "_TRANSY";
      std::string ixkey = "INS" + Isis::iString(naifIkCode) + "_ITRANSS";
      std::string iykey = "INS" + Isis::iString(naifIkCode) + "_ITRANSL";
      for (int i = 0; i < 3; ++i) {
        p_transx[i]  = Spice::GetDouble(xkey, i);
        p_transy[i]  = Spice::GetDouble(ykey, i);
        p_itranss[i] = Spice::GetDouble(ixkey, i);
        p_itransl[i] = Spice::GetDouble(iykey, i);
      }
    }
    else {
       std::string xkey  = "IDEAL_TRANSX";
       std::string ykey  = "IDEAL_TRANSY";
       std::string ixkey = "IDEAL_TRANSS";
       std::string iykey = "IDEAL_TRANSL";
       for (int i = 0; i < 3; ++i) {
         p_transx[i]  = Spice::GetDouble(xkey, i);
         p_transy[i]  = Spice::GetDouble(ykey, i);
         p_itranss[i] = Spice::GetDouble(ixkey, i);
         p_itransl[i] = Spice::GetDouble(iykey, i);
       }
    }

    if (parent != 0) {
      p_camera->SetFocalPlaneMap(this);
    }
  }

  /** Compute detector position from focal plane coordinate
   *
   * This method will compute both the centered and normal detector position
   * given a distorted focal plane coordinate.
   *
   * @param dx distorted focal plane x in millimeters
   * @param dy distorted focal plane y in millimeters
   *
   * @return conversion was successful
   */
  bool CameraFocalPlaneMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;
    p_focalPlaneY = dy;

    p_centeredDetectorSample = p_itranss[0] + (p_itranss[1] * dx)
                                            + (p_itranss[2] * dy);
    p_centeredDetectorLine   = p_itransl[0] + (p_itransl[1] * dx)
                                            + (p_itransl[2] * dy);
    ComputeUncentered();
    return true;
  }

  /** Compute distorted focal plane coordinate from detector position
   *
   * This method will compute both the distorted focal plane x/y and centered
   * detector position given a detector position
   *
   * @param sample undistorted focal plane x in millimeters
   * @param line undistorted focal plane y in millimeters
   *
   * @return conversion was successful
   */
  bool CameraFocalPlaneMap::SetDetector(const double sample, const double line) {
    p_detectorSample = sample;
    p_detectorLine = line;
    ComputeCentered();
    p_focalPlaneX = p_transx[0] + (p_transx[1] * p_centeredDetectorSample)
                                + (p_transx[2] * p_centeredDetectorLine) ;
    p_focalPlaneY = p_transy[0] + (p_transy[1] * p_centeredDetectorSample)
                                + (p_transy[2] * p_centeredDetectorLine) ;
    return true;
  }

  /** Return the focal plane x dependency variable
   *
   * This method returns the image variable (sample or line) on
   * which the focal plane x depends.
   * 
   * @return dependency variable
   */
//  CameraFocalPlaneMap::FocalPlaneXDependencyType CameraFocalPlaneMap::FocalPlaneXDependency() {
  int CameraFocalPlaneMap::FocalPlaneXDependency() {
    if (p_transx[1] > p_transx[2]) {
      return Sample;
    }
    else {
      return Line;
    }
  }


  /** Return the sign of the p_transx coefficient with the greatest magnitude
   *
   * This method returns a +1. or -1. based on the sign of the p_transx 
   * coefficient with the greatest magnitude.  Only p_transx[1] and
   * p_transx[2] are compared since p_transx[0] is used as a constant in the 
   * affine transformation.
   * 
   * @return sign of most significant coefficient
   */
  double CameraFocalPlaneMap::SignMostSigX()  {
    double magCoef1 = fabs(p_transx[1]);
    double magCoef2 = fabs(p_transx[2]);

    if (magCoef1 > magCoef2) {
      return (magCoef1/p_transx[1]);
    }
    else {
      return (magCoef2/p_transx[2]);
    }
  }



  /** Return the sign of the p_transy coefficient with the greatest magnitude
   *
   * This method returns a +1 or -1 based on the sign of the p_transy 
   * coefficient with the greatest magnitude.  Only p_transy[1] and
   * p_transy[2] are compared since p_transy[0] is used as a constant in the 
   * affine transformation.
   * 
   * @return sign of most significant coefficient
   */
  double CameraFocalPlaneMap::SignMostSigY()  {
    double magCoef1 = fabs(p_transy[1]);
    double magCoef2 = fabs(p_transy[2]);

    if (magCoef1 > magCoef2) {
      return (magCoef1/p_transy[1]);
    }
    else {
      return (magCoef2/p_transy[2]);
    }
  }

}

