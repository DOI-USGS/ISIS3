/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/12/07 17:39:26 $
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
#include "PushFrameCameraGroundMap.h"

#include <QDebug>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Distance.h"
#include "Latitude.h"
#include "Longitude.h"
#include "PushFrameCameraDetectorMap.h"
#include "SurfacePoint.h"

namespace Isis {
  /**
   * Compute undistorted focal plane coordinate from ground position
   *
   * @param lat
   * @param lon
   *
   * @return conversion was successful
   */
  bool PushFrameCameraGroundMap::SetGround(NaifContextPtr naif,
                                           const Latitude &lat,
                                           const Longitude &lon) {
    PushFrameCameraDetectorMap *detectorMap = (PushFrameCameraDetectorMap *) p_camera->DetectorMap();

    SurfacePoint surfacePoint(naif, lat, lon, p_camera->LocalRadius(naif, lat, lon));

    // Get ending bounding framelets and distances for iterative loop to minimize the spacecraft distance
    int startFramelet = 1;
    double startDist = FindSpacecraftDistance(naif, 1, surfacePoint);

    int    endFramelet = detectorMap->TotalFramelets();
    double endDist     = FindSpacecraftDistance(naif, endFramelet, surfacePoint);

    bool minimizedSpacecraftDist = false;

    for (int j = 0; j < 30 && !minimizedSpacecraftDist;j++) {
      int deltaX = abs(startFramelet - endFramelet) / 2;

      // start + deltaX = middle framelet.
      //  We're able to optimize this modified binary search
      //  because the 'V' shape -- it's mostly parallel. Meaning,
      //  if the left side is higher than the right, then the
      //  solution is closer to the right. The bias factor will
      //  determine how much closer, and then back off a little so
      //  we dont overshoot it.
      double biasFactor = startDist / endDist;

      if (biasFactor < 1.0) {
        biasFactor = -1.0 / biasFactor;
        biasFactor = -(biasFactor + 1) / biasFactor;

        // The bias is about 50% unsure... sometimes our V is a U
        biasFactor = std::min(biasFactor + 0.50, 0.0);
      }
      else {
        biasFactor = (biasFactor - 1) / biasFactor;

        // The bias is about 50% unsure... sometimes our V is a U
        biasFactor = std::max(biasFactor - 0.50, 0.0);
      }

      int middleFramelet = startFramelet + (int)(deltaX + biasFactor * deltaX);
      double middleDist = FindSpacecraftDistance(naif, middleFramelet, surfacePoint);

      if (startDist > endDist) {
        // This makes sure we don't get stuck halfway between framelets
        if (startFramelet == middleFramelet) middleFramelet++;
        startFramelet = middleFramelet;
        startDist = middleDist;
      }
      else {
        endFramelet = middleFramelet;
        endDist = middleDist;
      }

      if (startFramelet == endFramelet) {
        minimizedSpacecraftDist = true;
      }
    }

    if (!minimizedSpacecraftDist) {
      return false;
    }

    int realFramelet = startFramelet;
    bool frameletEven = (realFramelet % 2 == 0);
    bool timeAscendingFramelets = detectorMap->timeAscendingFramelets();

    // Do we need to find a neighboring framelet? Get the closest (minimize distance)
    if ((timeAscendingFramelets && frameletEven != p_evenFramelets) ||
        (!timeAscendingFramelets && frameletEven == p_evenFramelets)) {
      realFramelet++; // this direction doesnt really matter... it's simply a guess
    }

    int direction = 2;

    double realDist = FindDistance(naif, realFramelet, surfacePoint);
    int    guessFramelet = realFramelet + direction;
    double guessDist     = FindDistance(naif, guessFramelet, surfacePoint);

    if (guessDist > realDist) {
      direction = -1 * direction; // reverse the search direction
      guessFramelet = realFramelet + direction;
      guessDist = FindDistance(naif, guessFramelet, surfacePoint);
    }

    for (int j = 0; (realDist >= guessDist) && (j < 30);j++) {
      realFramelet = guessFramelet;
      realDist = guessDist;

      guessFramelet = realFramelet + direction;
      guessDist = FindDistance(naif, guessFramelet, surfacePoint);

      if (realFramelet <= 0 || realFramelet > detectorMap->TotalFramelets()) {
        return false;
      }
    }

    detectorMap->SetFramelet(naif, realFramelet);

    return CameraGroundMap::SetGround(naif, surfacePoint);
  }


  bool PushFrameCameraGroundMap::SetGround(NaifContextPtr naif, const SurfacePoint &surfacePt) {
    return SetGround(naif, surfacePt.GetLatitude(), surfacePt.GetLongitude());
  }


  /**
   * This method finds the distance from the center of the framelet to the lat,lon.
   *   The distance is only in the line direction and is squared.
   *
   * @param framelet
   * @param lat
   * @param lon
   *
   * @return double Y-Distance squared from center of framelet to lat,lon
   */
  double PushFrameCameraGroundMap::FindDistance(NaifContextPtr naif, int framelet,
      const SurfacePoint &surfacePoint) {
    PushFrameCameraDetectorMap *detectorMap = (PushFrameCameraDetectorMap *) p_camera->DetectorMap();
    CameraDistortionMap *distortionMap = (CameraDistortionMap *) p_camera->DistortionMap();

    detectorMap->SetFramelet(naif, framelet);
    if(!p_camera->Sensor::SetGround(naif, surfacePoint, false)) return DBL_MAX;

    double lookC[3];
    p_camera->Sensor::LookDirection(lookC, naif);
    double ux = p_camera->FocalLength() * lookC[0] / lookC[2];
    double uy = p_camera->FocalLength() * lookC[1] / lookC[2];

    if(!distortionMap->SetUndistortedFocalPlane(ux, uy)) return DBL_MAX;

    double dx = distortionMap->FocalPlaneX();
    double dy = distortionMap->FocalPlaneY();

    CameraFocalPlaneMap *focalMap = p_camera->FocalPlaneMap();
    if(!focalMap->SetFocalPlane(dx, dy)) return DBL_MAX;

    detectorMap->SetDetector(focalMap->DetectorSample(), focalMap->DetectorLine());

    double actualFrameletHeight = detectorMap->frameletHeight() / detectorMap->LineScaleFactor();
    double frameletDeltaY = detectorMap->frameletLine() - (actualFrameletHeight / 2.0);

    return frameletDeltaY * frameletDeltaY;
  }

  /**
   * This method finds the distance from the point on the ground to the
   * spacecraft at the time the specified framelet was taken.
   *
   * @param framelet Which framelet was being captured (determines time)
   * @param lat Latitude of the point on the ground
   * @param lon Longitude of the point on the ground
   *
   * @return double Distance from spacecraft to the lat,lon
   */
  double PushFrameCameraGroundMap::FindSpacecraftDistance(NaifContextPtr naif, int framelet,
      const SurfacePoint &surfacePoint) {
    PushFrameCameraDetectorMap *detectorMap = (PushFrameCameraDetectorMap *) p_camera->DetectorMap();

    detectorMap->SetFramelet(naif, framelet);
    if(!p_camera->Sensor::SetGround(naif, surfacePoint, false)) return DBL_MAX;

    return p_camera->SlantDistance(naif);
  }
}
