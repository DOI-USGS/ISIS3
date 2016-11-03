#ifndef PixelFOV_h
#define PixelFOV_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/04/29 00:54:15 $
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

