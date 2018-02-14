#ifndef PiecewisePolynomial_h
#define PiecewisePolynomial_h
/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/27 07:01:33 $
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

#include "LinearAlgebra.h"
#include "PolynomialUnivariate.h"

namespace Isis {

  /**
   * The PiecewisePolynomial class encapsulates a piecewise-polynomial
   * function. It can be set based on a known function or it can be fit to a
   * data set. Specifically, this class is designed to represent a
   * parameterized space curve such as an object's position over time.
   *
   * @author 2017-03-02 Jesse Mapel
   *
   * @internal
   *   @history 2017-03-02 Jesse Mapel - Original version. References #4650.
   *   @history 2017-03-11 Jesse Mapel - Reordered input data vector, improved
   *                           documentation and error throws and unit test.
   *                           References #4650.
   */
  class PiecewisePolynomial {

    public:
      PiecewisePolynomial();
      PiecewisePolynomial(double minValue, double maxValue,
                          int degree, int dimensions);
      PiecewisePolynomial(const PiecewisePolynomial &other);

      ~PiecewisePolynomial();

      PiecewisePolynomial &operator=(const PiecewisePolynomial &other);

      std::vector<double> evaluate(double value);
      std::vector<double> derivativeVariable(double value);

      void fitPolynomials(const std::vector<double> &values,
                          const std::vector< std::vector<double> > &data,
                          int segments);
      // TODO Is 100 enough samples to default to? JAM
      void refitPolynomials(int segments, int samples = 100);

      int degree() const;
      std::vector< std::vector<double> > coefficients(int segment) const;
      int dimensions() const;
      const std::vector<double> knots() const;
      int segments() const;

      bool isZero() const;

      void setDegree(int degree);
      void setCoefficients(int segment,
                           const std::vector< std::vector<double> > &coefficients);
      void setDimensions(int dimensions);
      void setKnots(std::vector<double> &knots);

      int segmentIndex(double value) const;

    private:
      void validateData(const std::vector<double> &values,
                        const std::vector< std::vector<double> > &data,
                        int segments);
      void computeKnots(const std::vector<double> &values,
                        const std::vector< std::vector<double> > &data,
                        int segments);
      double computeCurvature(const std::vector<double> &localValues,
                              const std::vector<double> &firstPoint,
                              const std::vector<double> &secondPoint,
                              const std::vector<double> &thirdPoint);
      double computeArcLength(const std::vector<double> &firstPoint,
                              const std::vector<double> &secondPoint);
      void computePolynomials(const std::vector<double> &values,
                              const std::vector< std::vector<double> > &data);
      int derivativeCoefficient(int coeffOrder, int derivativeOrder);

      int m_degree; //< The degree of the polynomials.
      int m_dimensions; //< The number of dimensions of the space curve.
      std::vector<double> m_knots; //!< The knots or segment boundaries.
      std::vector< std::vector<PolynomialUnivariate> > m_polynomials;
      /**!< Vector containing vectors of polynomials for each segment.
       *    Each inner vector represents a segment. Each inner vector
       *    has m_dimensions polynomial functions.**/
      
  };
};

#endif
