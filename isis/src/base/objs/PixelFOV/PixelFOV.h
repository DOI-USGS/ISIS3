#ifndef PixelFOV_h
#define PixelFOV_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QList>
#include <QPointF>

namespace Isis {
  class Camera;

  /**
   * @brief This class defines a field of view
   *  
   * This class defines a field of view for a given camera.  Field of views are returned as point
   * clouds defining the boundary of the field of view. The number of vertices is instrment
   * dependent. The default  Camera::PixelIfovOffsets assumes a square pixel and simply returns
   * the offsets of the four corner vertices in microns from the current pixel center. If the
   * instruments pixels are not square, the Camera::PixelIfov will  need to be implemented in the
   * the instrument's camera model.  For an example, see VimsCamera::PixelIfov().
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2013-02-15 Tracie Sucharski
   *
   * @internal
   *   @history 2013-02-15 Tracie Sucharski - Original version of PixelIfov class.
   *   @history 2016-10-24 Jesse Mapel - Renamed and expanded functionality of PixelIfov to allow
   *                           for both instantaneous and full field of views. References #4476.
   *   @history 2016-10-24 Jeannie Backer - Improved coding standards. References #4476.
   */

  class PixelFOV {
    public:
      // Constructors
      PixelFOV();
      PixelFOV(const PixelFOV &other);
      ~PixelFOV();

      QList< QList<QPointF> > latLonVertices(Camera &camera,
                                             const double sample,
                                             const double line,
                                             const int numIfovs = 1) const;

    private:
      QList<QPointF> instantaneousFov(Camera &camera) const;
      QList<QPointF> envelope(QList<QPointF> vertices) const;
      QList< QList<QPointF> > splitIfov(QList<QPointF> vertices) const;

  };
};

#endif

