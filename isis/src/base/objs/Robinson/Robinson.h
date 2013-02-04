#ifndef Robinson_h
#define Robinson_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
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

#include "TProjection.h"

#include <QList>
#include <QString>

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief Robinson Map Projection
   *
   * This class provides methods for the forward and inverse equations of the
   * Robinson map projection (for a sphere). 
   *  
   *  
   * The Robinson projection is a psuedo-cylindrical projection.  It is neither an equal-area or 
   * conformal.  The meridians become increasingly curved farther from the 
   * central meridian, however less curved than other psuedo-cylindrical projections.  The poles 
   * are stretched into long straight lines, 0.5322 times as long as the equator.  Parallels are 
   * straight parallel lines, equally spaced between 38 degrees north and south. Beyond 38 degrees, 
   * space between parallels decreases.  Distortions are small between 45 degrees north and south, 
   * and within 45 degrees of the central meridian. 
   *  
   * The code was converted to C++ from the Fortran version given in John p. Snyders's paper, 
   * "The Robinson Projection - A Computation Algorithm". There are no analytical formulas for this 
   * projection.  The projection is defined by values in a table indexed by latitude.  A 
   * second-order interpolation is used for latitudes between table values.  The interpolation used 
   * is Stirling's central difference formulat, using first and second differences only. 
   *  
   * This class inherits Projection and provides the two virtual methods SetGround (forward) and 
   * SetCoordinate (inverse) and a third virtual method, XYRange, for obtaining projection 
   * coordinate coverage for a latitude/longitude window. 
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *
   * @ingroup MapProjection
   *
   * @author 2012-12-20 Tracie Sucharski
   *
   * @internal
   */

  class Robinson : public TProjection {
    public:
      Robinson(Pvl &label, bool allowDefaults = false);
      ~Robinson();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      QList<double> m_pr;
      QList<double> m_xlr;

      double m_centerLongitude; //!< The center longitude for the map projection
  };
};

#endif

