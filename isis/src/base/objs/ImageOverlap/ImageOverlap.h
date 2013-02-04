#ifndef ImageOverlap_h
#define ImageOverlap_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/25 22:49:35 $
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
 *   $ISISROOT/doc/documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <vector>

#include <QString>

#include <geos/geom/MultiPolygon.h>

namespace Isis {

  /**
   * @brief Individual overlap container
   *
   * Hold information about a single area of overlap. This includes the
   * serial numbers of each cube that overlap this area and the polygon
   * that defines this area.
   *
   * @ingroup ControlNetworks
   *
   * @author  2005-08-03 Stuart Sides
   *
   * @internal
   *   @history 2005-08-03 Stuart Sides Original version
   *   @history 2007-11-02 Tracie Sucharski, Added HasSN method
   *   @history 2008-08-18 Steven Lambright - Updated to work with geos3.0.0
   *           instead of geos2. Mostly namespace changes.
   *   @history 2008-11-03 Steven Lambright - Added the Read and Write methods
   *   @history 2010-05-25 Steven Lambright - Made HasAnySameSerialNumber and
   *                                          HasSerialNumber const
   *
   */

  class ImageOverlap {
    public:
      ImageOverlap();
      ImageOverlap(QString serialNumber, geos::geom::MultiPolygon &polygon);
      ImageOverlap(std::istream &inputStream);

      virtual ~ImageOverlap();

      // Set a new polygon
      virtual void SetPolygon(const geos::geom::MultiPolygon &polygon);
      virtual void SetPolygon(const geos::geom::MultiPolygon *polygon);

      // Add a serial number
      void Add(QString &sn);

      // Return the number of serial numbers in this overlap area
      int Size() const {
        return p_serialNumbers.size();
      };

      // Return the ith serial number
      QString operator[](int index) const {
        return p_serialNumbers[index];
      };

      // Return the polygon
      const geos::geom::MultiPolygon *Polygon() const {
        return p_polygon;
      };

      // Return the area of the polygon
      virtual double Area();

      // Compare the serial numbers with another ImageOverlap
      bool HasAnySameSerialNumber(ImageOverlap &other) const;

      // Does serial number exist in this ImageOverlap
      bool HasSerialNumber(QString &sn) const;

      void Write(std::ostream &outputStream);
    private:

      std::vector<QString> p_serialNumbers;
      geos::geom::MultiPolygon *p_polygon;

      void Init();

  };
};

#endif
