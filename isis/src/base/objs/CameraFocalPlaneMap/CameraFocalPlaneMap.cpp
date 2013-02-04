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
#include "CameraFocalPlaneMap.h"

#include <cmath>

#include "Camera.h"
#include "Spice.h"

namespace Isis {
  /** Construct mapping between detectors and focal plane x/y
   *
   * @param parent      parent camera that will use this map
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  CameraFocalPlaneMap::CameraFocalPlaneMap(Camera *parent, const int naifIkCode) {
    Init(parent, naifIkCode);
  }

  /** Construct mapping between detectors and focal plane x/y
   *
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  CameraFocalPlaneMap::CameraFocalPlaneMap(const int naifIkCode) {
    Init(0, naifIkCode);
  }
  CameraFocalPlaneMap::~CameraFocalPlaneMap(){
  }

  void CameraFocalPlaneMap::Init(Camera *parent, const int naifIkCode) {
    p_detectorSampleOrigin = 0.0;
    p_detectorLineOrigin = 0.0;
    p_detectorSampleOffset = 0.0;
    p_detectorLineOffset = 0.0;
    p_camera = parent;

    if (naifIkCode != 0) {
      std::string xkey  = "INS" + Isis::IString(naifIkCode) + "_TRANSX";
      std::string ykey  = "INS" + Isis::IString(naifIkCode) + "_TRANSY";
      std::string ixkey = "INS" + Isis::IString(naifIkCode) + "_ITRANSS";
      std::string iykey = "INS" + Isis::IString(naifIkCode) + "_ITRANSL";
      for (int i = 0; i < 3; ++i) {
        p_transx[i]  = parent->getDouble(xkey, i);
        p_transy[i]  = parent->getDouble(ykey, i);
        p_itranss[i] = parent->getDouble(ixkey, i);
        p_itransl[i] = parent->getDouble(iykey, i);
      }
    }
    else {
      std::string xkey  = "IDEAL_TRANSX";
      std::string ykey  = "IDEAL_TRANSY";
      std::string ixkey = "IDEAL_TRANSS";
      std::string iykey = "IDEAL_TRANSL";
      for (int i = 0; i < 3; ++i) {
        p_transx[i]  = parent->getDouble(xkey, i);
        p_transy[i]  = parent->getDouble(ykey, i);
        p_itranss[i] = parent->getDouble(ixkey, i);
        p_itransl[i] = parent->getDouble(iykey, i);
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
      return (magCoef1 / p_transx[1]);
    }
    else {
      return (magCoef2 / p_transx[2]);
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
      return (magCoef1 / p_transy[1]);
    }
    else {
      return (magCoef2 / p_transy[2]);
    }
  }

    //! Return distorted focal plane x
  double CameraFocalPlaneMap::FocalPlaneX() const {
    return p_focalPlaneX;
  }

  //! Return distorted focal plane y
  double CameraFocalPlaneMap::FocalPlaneY() const {
    return p_focalPlaneY;
  }

  //! Return detector sample
  double CameraFocalPlaneMap::DetectorSample() const {
    return p_detectorSample;
  }

  //! Return detector line
  double CameraFocalPlaneMap::DetectorLine() const {
    return p_detectorLine;
  }

  //! Return centered detector sample
  double CameraFocalPlaneMap::CenteredDetectorSample() const {
    return p_centeredDetectorSample;
  }

  //! Return centered detector line
  double CameraFocalPlaneMap::CenteredDetectorLine() const {
    return p_centeredDetectorLine;
  }

  /** Set the detector origin
   *
   * This is used to set the origin of the detector.  Typically the middle
   * of the detector.  For example, a 512x512 dectector would have the
   * origin at (256.5,256.5).  If not set both are 0.
   *
   * @param sample  detector sample at the origin
   * @param line    detector line at the origin
   */
  void CameraFocalPlaneMap::SetDetectorOrigin(const double sample, const double line) {
    p_detectorSampleOrigin = sample;
    p_detectorLineOrigin = line;
  }

  //! Return detector line origin
  double CameraFocalPlaneMap::DetectorLineOrigin() const {
    return p_detectorLineOrigin;
  }

  //! Return detector sample origin
  double CameraFocalPlaneMap::DetectorSampleOrigin() const {
    return p_detectorSampleOrigin;
  }

  /** Set the detector offset
   *
   * This is used to set the offset between the detector origin and
   * the average location in detector pixels where the image is being
   * viewed.  If not set the offset are both 0.0
   *
   * @param sampleOffset sample offset in pixels
   * @param lineOffset sample offset in lines
   */
  void CameraFocalPlaneMap::SetDetectorOffset(const double sampleOffset,
                                const double lineOffset) {
    p_detectorSampleOffset = sampleOffset;
    p_detectorLineOffset = lineOffset;
  }

  //! Return detector line offset
  double CameraFocalPlaneMap::DetectorLineOffset() const {
    return p_detectorLineOffset;
  }

  //! Return detector sample offset
  double CameraFocalPlaneMap::DetectorSampleOffset() const {
    return p_detectorSampleOffset;
  }

  const double *CameraFocalPlaneMap::TransX() const{
    return p_transx;
  }
  const double *CameraFocalPlaneMap::TransY() const{
    return p_transy;
  }
  const double *CameraFocalPlaneMap::TransS() const{
    return p_itranss;
  }
  const double *CameraFocalPlaneMap::TransL() const{
    return p_itransl;
  }
  //! Convenience method to center detector origin (use when inheriting)
  void CameraFocalPlaneMap::ComputeCentered() {
    p_centeredDetectorSample = p_detectorSample - p_detectorSampleOrigin;
    p_centeredDetectorLine   = p_detectorLine   - p_detectorLineOrigin;
  }

  //! Convenience method to center detector origin (use when inheriting)
  void CameraFocalPlaneMap::ComputeUncentered() {
    p_detectorSample = p_centeredDetectorSample + p_detectorSampleOrigin;
    p_detectorLine   = p_centeredDetectorLine   + p_detectorLineOrigin;
  }

}

