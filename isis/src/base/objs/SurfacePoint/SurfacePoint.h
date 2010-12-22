#ifndef SurfacePoint_h
#define SurfacePoint_h
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

#include <vector>
#include <cmath>

#include "boost/numeric/ublas/symmetric.hpp"
#include "boost/numeric/ublas/io.hpp"

#include "Displacement.h"
#include "Distance.h"
#include "Angle.h"

namespace Isis {
  class Latitude;
  class Longitude;

  /**
   * @brief This class defines a body-fixed surface point 
   *  
   * This class is a container for body-fixed surface points.  It provides 
   * methods to set and present the coordinates of surface points in various 
   * usable units to support projection of image points to the ground and 
   * bundle adjustment. 
   *
   * @ingroup Geometry
   *
   * @authors  2010-07-30 Tracie Sucharski, Ken L. Edmunson, and Debbie A. Cook
   * @history  2010-08-25 Debbie A. Cook  Added more error checking and testing
   * @history  2010-09-10 Debbie A. Cook  Made ocentric methods specific to
   *           units of sigmas (degrees or meters)
   * @history  2010-10-04 Debbie A. Cook  Remove using boost to avoid compile
   *           errors throughout Isis3 classes and added boost namespace name to
   *           all uses of matrix
   * @history  2010-10-20 Debbie A. Cook and Steven Lambright  Simplified the 
   *           class by using new Displacement, Distance, Latitude, Longitude,
   *           and Angle objects. 
   *
   * @internal
   * Do we need a bool to make sure a surface point has been set before
   *  computing covariance matrix???? TODO
   */

  class SurfacePoint {
    public:
      // Constructors
//      SurfacePoint(const std::vector <double> radii);
      SurfacePoint();
      SurfacePoint(Latitude lat, Longitude lon, Distance radius);
      SurfacePoint(Latitude lat, Longitude lon, Distance radius,
                   Angle latSigma, Angle lonSigma, Distance radiusSigma);
      SurfacePoint(Latitude lat, Longitude lon, Distance radius,
                   const boost::numeric::ublas::symmetric_matrix
                   <double,boost::numeric::ublas::upper>& covar);
      SurfacePoint(Displacement x, Displacement y, Displacement z);
      SurfacePoint(Displacement x, Displacement y, Displacement z,
                   Distance xSigma, Distance ySigma, Distance zSigma);
      SurfacePoint(Displacement x, Displacement y, Displacement z,
                   const boost::numeric::ublas::symmetric_matrix
                   <double,boost::numeric::ublas::upper>& covar);
      ~SurfacePoint();

// Rectangular loading utilities
      void SetRectangular(Displacement x, Displacement y, Displacement z,
                          Distance xSigma=Distance(),
                          Distance ySigma=Distance(),
                          Distance zSigma=Distance());

      void SetRectangular(Displacement x, Displacement y, Displacement z,
        const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar);

      //! Set surface point and sigmas in rectangular coordinates and convert to planetocentric
      void SetRectangularSigmas(Distance xSigma, Distance ySigma, Distance zSigma);


      void SetRectangularMatrix(
        const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar);

// Spherical loading utilites

      //! Set surface point and covariance matrix in planetocentric coordinates and convert to rectangular
      //! (Latitude, Longitude in degrees, Radius in meters; matrix in radians and radians**2)
      void SetSpherical (Latitude lat, Longitude lon, Distance radius,
                         Angle latSigma=Angle(),
                         Angle lonSigma=Angle(),
                         Distance radiusSigma=Distance());

      void SetSpherical (Latitude lat, Longitude lon, Distance radius,
        const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar);

      void SetSphericalMatrix(
        const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar);

      void SetSphericalSigmas(Angle latSigma, Angle lonSigma, Distance radiusSigma);

      void SetSphericalSigmasDistance(Distance latSigma,
                                      Distance lonSigma,
                                      Distance radiusSigma);

      void SetRadii(Distance majorRadius, Distance minorRadius, Distance
                    polarRadius);

      void ResetLocalRadius(Distance radius);
      bool Valid() const { return p_hasPoint; }

// Output methods
      Displacement GetX() const { return p_x; }
      Displacement GetY() const { return p_y; }
      Displacement GetZ() const { return p_z; }
      Distance GetXSigma() const { return sqrt(p_rectCovar(0,0)); }
      Distance GetYSigma() const { return sqrt(p_rectCovar(1,1)); }
      Distance GetZSigma() const { return sqrt(p_rectCovar(2,2)); }
      boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper> 
        const GetRectangularMatrix() const { return p_rectCovar; }
      Latitude GetLatitude() const; 
      Longitude GetLongitude() const;
      Distance GetLocalRadius() const;
      Angle GetLatSigma()  const{ return sqrt(p_sphereCovar(0,0)); }
      Distance GetLatSigmaDistance() const;
      Angle GetLonSigma() const { return sqrt(p_sphereCovar(1,1)); }
      Distance GetLonSigmaDistance() const;
      Distance GetLocalRadiusSigma() const { return sqrt(p_sphereCovar(2,2)); }
      boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper> 
         GetSphericalMatrix() const { return p_sphereCovar; }

      bool operator==(const SurfacePoint &other) const;

    private:
      void InitCovariance();
      void InitPoint();
      void InitRadii();
      void SetRectangularPoint(Displacement x, Displacement y, Displacement z);
      void SetSphericalPoint(Latitude lat, Longitude lon, Distance radius);

      bool p_hasPoint;
      bool p_hasRadii;
      bool p_hasMatrix;

      Distance p_majorAxis;
      Distance p_minorAxis;
      Distance p_polarAxis;
      Displacement p_x;
      Displacement p_y;
      Displacement p_z;
      boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper> p_rectCovar;  //! 3x3 upper triangular covariance matrix rectangular coordinates
      boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper> p_sphereCovar;  //! 3x3 upper triangular covariance matrix ocentric coordinates
  };
};

#endif

