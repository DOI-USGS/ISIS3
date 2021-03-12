#ifndef Robinson_h
#define Robinson_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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

