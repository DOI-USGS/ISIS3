#include "PixelIfov.h"

#include <QDebug>
#include <QList>
#include <QPoint>

#include "Camera.h"
#include "CameraDistortionMap.h"

using namespace std;

namespace Isis {

  /**
   * Constructs an empty PixelIfov object
   *
   */
  PixelIfov::PixelIfov() {
  }


  /**
   * Destroys a PixelIfov object/
   *
   */
  PixelIfov::~PixelIfov() {
  }


  QList<QPointF> PixelIfov::latLonVertices(Camera &camera) const {

    QList<QPointF> vertices;

    QList<QPointF> offsets = camera.PixelIfovOffsets();
    int numVertices = offsets.size();

    double saveLook[3];
    double newLook[3];
    double unitNewLook[3];
    camera.LookDirection(saveLook);
    double focalLength = camera.FocalLength();

    //  For highly distorted instruments, take fpx, fpy (which are undistorted) convert to distorted,
    //  add offsets, undistort.  Only need to worry about if distortion high on a pixel to pixel
    // basis.  If this is done, save samp/line and reset the camera (SetImage).
    double scale = focalLength / saveLook[2];
    for (int i = 0; i < numVertices; i++) {
      double focalPlaneX = saveLook[0] * scale;
      double focalPlaneY = saveLook[1] * scale;
      focalPlaneX += offsets[i].x();
      focalPlaneY += offsets[i].y();
      newLook[0] = focalPlaneX;
      newLook[1] = focalPlaneY;
      newLook[2] = camera.DistortionMap()->UndistortedFocalPlaneZ();
      vhat_c(newLook, unitNewLook);
      if (camera.SetLookDirection(unitNewLook)) {
        vertices.append(QPointF(camera.UniversalLatitude(), camera.UniversalLongitude()));
      }
    }
    //  Reset look direction back to center of pixel
    camera.SetLookDirection(saveLook);
    return vertices;
  }
}
