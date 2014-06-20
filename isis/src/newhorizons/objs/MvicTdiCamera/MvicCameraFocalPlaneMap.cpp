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
#include "MvicCameraFocalPlaneMap.h"

#include <cmath>

#include <QDebug>

#include "Camera.h"
#include "Spice.h"

namespace Isis {
  /** Construct mapping between detectors and focal plane x/y
   *
   * @param parent      parent camera that will use this map
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  MvicCameraFocalPlaneMap::MvicCameraFocalPlaneMap(Camera *parent, const int naifIkCode) :
                           CameraFocalPlaneMap(parent, naifIkCode) {
    
  }



  /** Construct mapping between detectors and focal plane x/y
   *
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  MvicCameraFocalPlaneMap::MvicCameraFocalPlaneMap(const int naifIkCode) :
                           CameraFocalPlaneMap(naifIkCode) {
    
  }



  MvicCameraFocalPlaneMap::~MvicCameraFocalPlaneMap(){
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
  bool MvicCameraFocalPlaneMap::SetFocalPlane(const double dx, const double dy) {
    qDebug()<<"MvicCameraFocalPlaneMap::SetFocalPlane";
    qDebug()<<"     dx: dy = "<<dx<<" : "<<dy;
    //  X-Y is actually Z-Y.  Need to rotate -90 into the X-Y plane
    SpiceDouble zy[3];
    SpiceDouble xy[3];
    zy[0] = 0;
    zy[1] = dy;
    zy[2] = dx;

    SpiceDouble rotAngle = -90 * rpd_c();
    rotvec_c(zy, rotAngle, 2, xy);
//  qDebug()<<"     after rotation xy[0]: xy[1]: xy[2] = "<<QString::number(xy[0],'f')<<" : "<<QString::number(xy[1],'f')<< " : "<<QString::number(xy[2],'f');

    p_focalPlaneX = xy[0];
    p_focalPlaneY = xy[2];

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
  bool MvicCameraFocalPlaneMap::SetDetector(const double sample, const double line) {
//  qDebug()<<"MvicCameraFocalPlaneMap::SetDetector";
//  qDebug()<<"     samp: line = "<<sample<<" : "<<line;
    p_detectorSample = sample;
    p_detectorLine = line;
    ComputeCentered();
    p_focalPlaneX = p_transx[0] + (p_transx[1] * p_centeredDetectorSample)
                    + (p_transx[2] * p_centeredDetectorLine) ;
    p_focalPlaneY = p_transy[0] + (p_transy[1] * p_centeredDetectorSample)
                    + (p_transy[2] * p_centeredDetectorLine) ;
//  qDebug()<<"     focalPlaneX : focalPlaneY = "<<QString::number(p_focalPlaneX,'f')<<" : "<<QString::number(p_focalPlaneY,'f');

    //  X-Y needs to be rotated to Z-Y to match the instrument boresight.  Need to rotate 90
    // into the X-Y plane
    SpiceDouble zy[3];
    SpiceDouble xy[3];
//  xy[0] = p_focalPlaneX;
//  xy[1] = p_focalPlaneY;
//  xy[2] = 0;
// experiment to see what the outcome from a rotation would be
    xy[0] = 1.5;
    xy[1] = 2.7;
    xy[2] = 3.9;
//Convert from x-y to z-y
    SpiceDouble rotAngle = 90 * rpd_c();
    rotvec_c(xy, rotAngle, 2, zy);
    qDebug()<<"     after rotation zy[0]: zy[1]: zy[2] = "<<QString::number(zy[0],'f')<<" : "<<QString::number(zy[1],'f')<< " : "<<QString::number(zy[2],'f');
//
//  p_focalPlaneX = -zy[2];
//  p_focalPlaneY = zy[1];
//  qDebug()<<"     focalPlaneX = "<<p_focalPlaneX<<"   focalPlaneY = "<<p_focalPlaneY;
    return true;
  }
}

