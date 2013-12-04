#ifndef PixelIfov_h
#define PixelIfov_h
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
   * @brief This class defines an instantaneous field of view
   *  
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2013-02-15 Tracie Sucharski
   *
   * @internal
   */

  class PixelIfov {
    public:
      // Constructors
      PixelIfov();
      PixelIfov(const PixelIfov &other);
      ~PixelIfov();

      bool isValid() const;

// Output methods
      QList<QPointF> latLonVertices(Camera &camera) const;
      //QList<QPointF> sampLineVertices() const;

  };
};

#endif

