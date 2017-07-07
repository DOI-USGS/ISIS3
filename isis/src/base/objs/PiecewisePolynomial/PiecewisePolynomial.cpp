#include <cmath>
#include <cfloat>
#include <iterator>

#include "BasisFunction.h"
#include "IException.h"
#include "IString.h"
#include "LeastSquares.h"
#include "PolynomialUnivariate.h"
#include "NumericalApproximation.h"

#include "PiecewisePolynomial.h"

namespace Isis {

  /**
   * Constructs a default PiecewisePolynomial object. Initializes to a single segment
   * of the three dimensional, zero degree, zero polynomial with knots at +/- inf.
   */
  PiecewisePolynomial::PiecewisePolynomial() {
    m_knots.resize(2);
    m_knots[0] = -DBL_MAX;
    m_knots[1] = DBL_MAX;
    m_degree = 0;
    m_dimensions = 3;
    std::vector<PolynomialUnivariate> zeroSegment;
    PolynomialUnivariate zeroPoly(0);
    zeroPoly.SetCoefficients( std::vector<double>(1, 0.0) );
    for (int i = 0 ; i < 3; i++) {
      zeroSegment.push_back(zeroPoly);
    }
    m_polynomials.push_back(zeroSegment);
  }

  /**
   * Constructs a PiecewisePolynomial object. Initializes to a single segment
   * of the zero polynomial.
   * 
   * @param minValue The inclusive minimum value for the PiecewisePolynomial.
   * @param maxValue The exclusive maximum value for the PiecewisePolynomial.
   * @param degree The degree of the polynomials.
   * @param dimensions The number of dimensions of the space curve.
   */
  PiecewisePolynomial::PiecewisePolynomial(double minValue, double maxValue,
                                           int degree, int dimensions) {
    m_knots.resize(2);
    m_knots[0] = minValue;
    m_knots[1] = maxValue;
    m_degree = degree;
    m_dimensions = dimensions;
    std::vector<PolynomialUnivariate> zeroSegment;
    PolynomialUnivariate zeroPoly(degree);
    zeroPoly.SetCoefficients( std::vector<double>(degree + 1, 0.0) );
    for (int i = 0 ; i < dimensions; i++) {
      zeroSegment.push_back(zeroPoly);
    }
    m_polynomials.push_back(zeroSegment);
  }


  /**
   * Copy constructor to make a copy of another PiecewisePolynomial object.
   * 
   * @param other The PiecewisePolynomial object to copy.
   */
  PiecewisePolynomial::PiecewisePolynomial(const PiecewisePolynomial &other)
    : m_degree(other.m_degree),
      m_dimensions(other.m_dimensions),
      m_knots(other.m_knots),
      m_polynomials(other.m_polynomials) {
    
  }


  /**
   * Destroys a PiecewisePolynomial object.
   */
  PiecewisePolynomial::~PiecewisePolynomial() {
    
  }


  /**
   * Assign the state of another PiecewisePolynomial object to this object.
   * 
   * @param other The PiecewisePolynomial object to assign from.
   * 
   * @return @b PiecewisePolynomial& A reference to this after the assignment.
   */
  PiecewisePolynomial &PiecewisePolynomial::operator=(const PiecewisePolynomial &other) {
    m_degree = other.m_degree;
    m_dimensions = other.m_dimensions;
    m_knots = other.m_knots;
    m_polynomials = other.m_polynomials;

    return *this;
  }


  /**
   * Evaluates the PiecewisePolynomial at a given value.
   * 
   * @param value The value to evaluate at.
   * 
   * @return @b std::vector<double> The output values.
   */
  std::vector<double> PiecewisePolynomial::evaluate(double value) {
    try {
      int knotIndex = segmentIndex(value);

      std::vector<double> output( dimensions() );
      for (int i = 0; i < dimensions(); i++) {
        output[i] = m_polynomials[knotIndex][i].Evaluate(value);
      }
      return output;
    }
    catch (IException &e) {
      QString msg = "Failed evaluating piecewise polynomial at value ["
                    + toString(value) + "].";
      throw IException(e, IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Calculates the derivatives with respect to the variable at a given value.
   * 
   * @param value The value to compute the derivatives at.
   * 
   * @return @b std::vector<double> The derivative of each dimensions with
   *                                respect to the variable.
   */
  std::vector<double> PiecewisePolynomial::derivativeVariable(double value) {
    int knotIndex = segmentIndex(value);

    std::vector<double> output( dimensions() );
    for (int i = 0; i < dimensions(); i++) {
      output[i] = m_polynomials[knotIndex][i].DerivativeVar(value);
    }
    return output;
  }


  /**
   * Returns the index of the segment that a given value belongs to.
   * If the value is less than the minimum value or greater than
   * the maximum value, then an error is thrown.
   * 
   * @param value The value to find the segment for.
   * 
   * @return @b int The zero-based index of the segment.
   * 
   * @throws IException IException::Programmer "Value is not within valid range"
   * 
   * @TODO Do we really want to throw an error?  Maybe we should just return
   *       the first/last index? JAM.
   */
  int PiecewisePolynomial::segmentIndex(double value) {
    if ( value < m_knots.front() - 1e-10 || value > m_knots.back() + 1e-10 ) {
      QString msg = "Value [" + toString(value) + "] is not within valid range [" +
                    toString( m_knots.front() ) + ", " + toString( m_knots.back() ) +
                    "].";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if ( value == m_knots.back() ) {
      return m_knots.size() - 2;
    }
    else if ( value == m_knots.front() ) {
      return 0;
    }

    std::vector<double>::const_iterator knotIt;
    knotIt = std::upper_bound(m_knots.begin(), m_knots.end(), value);
    int knotIndex = std::distance<std::vector<double>::const_iterator>(m_knots.begin(), knotIt);
    return knotIndex - 1;
  }


  /**
   * @brief Compute knots and fit polynomials to a data set.
   * 
   * Given a set of data, fit a set of polynomials and internalize them.
   * This function will first compute knot locations based on the arc length
   * between data points and curvature at each data point. Next it will fit
   * polynomials for each dimension and each segment with continuity conditions
   * at the knots.
   * 
   * @see PiecewisePolynomial::computeKnots for a more technical description of
   *      the knot computation process.
   * 
   * @see PiecewisePolynomial::computePolynomials for a more technical
   *      description of the polynomial fitting process.
   * 
   * @param values The vector of parameter values cooresponding to each data
   *               point.
   * @param data A vector of data points to fit polynomials over.
   * @param segments The number of segments to create.
   */
  void PiecewisePolynomial::fitPolynomials(const std::vector<double> &values,
                                           const std::vector< std::vector<double> > &data,
                                           int segments) {
    validateData(values, data, segments);
    computeKnots(values, data, segments);
    computePolynomials(values, data);
  }


  /**
   * @brief Check if input data for polynomial fitting is valid.
   * 
   * Checks the following:
   * <ul>
   * <li>The input values are sorted</li>
   * <li>There are the same number of input values and data points</li>
   * <li>The data points have the correct dimensions</li>
   * <li>There are sufficient data points to solve for polynomials</li>
   * </ul>
   * 
   * @param values The vector of parameter values cooresponding to each data
   *               point.
   * @param data A vector of data points to fit polynomials over.
   * @param segmentCount The number of segments to create.
   * 
   * @throws IException::Programmer "The number of input values and data points
   *                                 do not match."
   * @throws IException::Programmer "The number of data points is insufficient
   *                                 to fit polynomials."
   * @throws IException::Programmer "Input values are not sorted in ascending
   *                                 order."
   * @throws IException::Programmer "Data point has the incorrect number of
   *                                 dimensions."
   */
  void PiecewisePolynomial::validateData(const std::vector<double> &values,
                                         const std::vector< std::vector<double> > &data,
                                         int segmentCount) {
    if ( values.size() != data.size() ) {
      QString msg = "The number of input values [" + toString( (int)values.size() ) +
                    "] and data points [" + toString( (int)data.size() ) + "] do not match.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Without continuity conditions, we need 1 data point per coefficient
    int numPointsNeeded = segmentCount * (degree() + 1) * dimensions();

    // The continuity conditions reduce the number of data points needed.
    numPointsNeeded -= ( segmentCount - 1 ) * std::min( 3, degree() );

    int numObservations = data.size() * dimensions();
    if ( numObservations < numPointsNeeded ) {
      QString msg = "The number of data points [" + toString( (int)data.size() ) +
                    "] is insufficient to fit polynomials. at least [" +
                    toString( numPointsNeeded ) + "] data points are required.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    for (int i = 0; i < (int)values.size() - 1; i++) {
      if ( values[i + 1] < values[i] ) {
        QString msg = "Input values are not sorted in ascending order.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    for (int i = 0; i < (int)data.size(); i++) {
      if ( (int)data[i].size() != dimensions() ) {
        QString msg = "Data point number [" + toString( i + 1 ) +
                      "] has the incorrect number of dimensions [" +
                      toString( (int)data[i].size() ) +
                      "]. Expected [" + toString( dimensions() )  + "] dimensions.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
  }


  /**
   * @brief Computes knot locations for a data set.
   * 
   * <p>
   * Computes knot locations based on the curvature of a data set. The
   * first and last values, with some padding, are used as the first and last
   * knots. The interior knots are evenly distributed based on the integral of
   * the curvature with respect to the arc length of the piecewise linear space
   * curve between data points.
   * </p>
   * 
   * <p>
   * <b>The data set must be sorted in increasing order based on the parameter
   * values.</b>
   * </p>
   * 
   * TODO Full citation
   * <p>
   * This method is based on the paper <i>Data Point Selection for Piecewise
   * Linear Curve Approximation</i> by Bernd Hamann and Jiann-Liang Chen. Another
   * more detailed reference is <i>Knot Placement for B-Spline Curve Approximation</i>
   * by Anshuman Razdan.
   * </p>
   * 
   * @param values The vector of parameter values cooresponding to each data
   *               point. These are expected to be sorted.
   * @param data A vector of data points to fit polynomials over. The paremter
   *             value for each data point must be the entry in values at the
   *             same index.
   * @param segments The number of segments to create. this number plus one
   *                 knots will be computed. Must be greater than 0.
   * 
   * @throws IException::Programmer "Segment count must be greater than 0."
   */
  void PiecewisePolynomial::computeKnots(const std::vector<double> &values,
                                         const std::vector< std::vector<double> > &data,
                                         int segments) {
    if ( segments < 1 ) {
      QString msg = "Segment count [" + toString(segments) + "] must be greater than 0.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // If there is only a single point, then use -inf to inf
    if ( values.size() == 1 ) {
      std::vector<double> newKnots;
      newKnots.push_back(-DBL_MAX);
      newKnots.push_back(DBL_MAX);
      setKnots(newKnots);
      return;
    }

    // =========================
    // = Setup storage vectors =
    // =========================

    int candidateCount = values.size();
    std::vector<double> curvatures(candidateCount, 0);
    std::vector<double> arcLengths(candidateCount - 1);
    std::vector<double> candidateWeights(candidateCount, 0);
    // We cannot compute curvature at the last data point, so we only integrate
    // from the first data point to the second to last data point.
    std::vector<double> cumulativeWeights(candidateCount - 1, 0);
    double totalWeight = 0;

    // ====================================================
    // = Compute the curvatures, arc lengths, and weights =
    // ====================================================

    // Compute the length of the first arc,
    // the other arc lengths will be computed in the loop.

    arcLengths[0] = computeArcLength(data[0], data[1]);
    for (int i = 1; i < candidateCount - 1; i++) {
      std::vector<double> localValues(values.begin() + i - 1,
                                      values.begin() + i + 2);
      curvatures[i] = computeCurvature(localValues,
                                       data[i-1],
                                       data[i],
                                       data[i+1]);
      arcLengths[i] = computeArcLength(data[i],
                                       data[i+1]);
      candidateWeights[i] = curvatures[i] * ( arcLengths[i-1] + arcLengths[i] ) / 2;
      totalWeight += candidateWeights[i];
      cumulativeWeights[i] = totalWeight;
    }

    // ===========================
    // = Calculate the new knots =
    // ===========================

    double segmentWeight = totalWeight / segments;
    // We always take the first and last times plus padding for the knots.
    std::vector<double> newKnots(segments + 1);
    newKnots.front() = values.front();
    newKnots.back() = values.back();
    for (int i = 1; i < segments; i++) {
      // Find the data point before the knot
      std::vector<double>::iterator timeIt;
      timeIt = std::upper_bound( cumulativeWeights.begin(),
                                 cumulativeWeights.end(),
                                 segmentWeight*i );
      int preKnotIndex = std::distance( cumulativeWeights.begin(), timeIt ) - 1;
      // Interpolate the actual knot location.
      double remainingWeight = segmentWeight*i - cumulativeWeights[preKnotIndex];
      // The following will not perform division by zero. There are two ways a
      // point can have a candidate weight of 0. First, if the arc length on
      // either side is 0. In this case the curvature calculation will fail
      // because there is not enough information available. Second, if the
      // curvature is 0. in this case, the cumulative weight at that point will
      // be the same as the previous point. So, the upper_bound algorithm will
      // never return a point with a candidate weight of 0.
      double ratio = remainingWeight / candidateWeights[preKnotIndex+1];
      newKnots[i] = ratio * values[preKnotIndex+1] +
                    (1 - ratio) * values[preKnotIndex];
    }

    // Save the new knot locations and reset the polynomials to zero polynomials.
    setKnots(newKnots);
  }


  /**
   * @brief Computes the curvature for a point.
   * 
   * <p>
   * If the data is one dimensionsal, then the second derivative of the
   * polynomial fit through the three points is returned. Otherwise, this
   * method computes the curvature based on the circle passing through the
   * three points. The actual calculation is done by leveraging the fact that
   * the radius of the circle is equal to the product of the distances between
   * the points and the area of the triangle with the points as its vertices. A
   * brief proof of this is outlined below.
   * </p>
   * 
   * <p>
   * Consider the points @f$a,b,c@f$ and the triangle they form,
   * @f$\triangle abc@f$. Let @f$A@f$ be the length of the side opposite
   * @f$a@f$, @f$B@f$ be the length of the side opposite @f$b@f$, @f$C@f$ be
   * the length of the side opposite @f$c@f$, @f$K@f$ be the area of
   * @f$\triangle abc@f$, and @f$R@f$ be the circumradius of @f$\triangle abc@f$.
   * Then, by the law of sines,
   * @f[ \sin (a) = \frac{A}{2R} @f]
   * Substituting into the equation
   * @f$K = \frac{1}{2}BC\sin(a)@f$, we get
   * @f[ K = \frac{ABC}{4R} @f]
   * Hence,
   * @f[ \frac{1}{R} = \frac{4K}{ABC} @f]
   * </p>
   * 
   * @param localValues The parameter values at the three points, in order.
   * @param firstPoint The first data point.
   * @param secondPoint The second data point.
   * @param thirdPoint The third data point.
   * 
   * @return @b double The curvature at the center point.
   */
  double PiecewisePolynomial::computeCurvature(const std::vector<double> &localValues,
                                               const std::vector<double> &firstPoint,
                                               const std::vector<double> &secondPoint,
                                               const std::vector<double> &thirdPoint) {
    int dataDimensions = firstPoint.size();
    if ( dataDimensions < 1 ) {
      QString msg = "Input data dimensions [" + toString(dataDimensions) +
                    "] must be greater than 0.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // If the data is 1 dimensionsal, use the magnitude of the second derivative instead.
    if (dataDimensions == 1) {
      NumericalApproximation polynomial(NumericalApproximation::Polynomial);
      polynomial.AddData( localValues[0], firstPoint[0] );
      polynomial.AddData( localValues[1], secondPoint[0] );
      polynomial.AddData( localValues[2], thirdPoint[0] );
      return fabs( polynomial.GslSecondDerivative( localValues[1] ) );
    }
    
    // Extract the three points
    LinearAlgebra::Vector a(dataDimensions);
    LinearAlgebra::Vector b(dataDimensions);
    LinearAlgebra::Vector c(dataDimensions);
    std::copy(firstPoint.begin(),  firstPoint.end(),  a.begin());
    std::copy(secondPoint.begin(), secondPoint.end(), b.begin());
    std::copy(thirdPoint.begin(),  thirdPoint.end(),  c.begin());

    // Compute the distance between points and sort them
    std::vector<double> distances;
    distances.push_back( LinearAlgebra::magnitude( LinearAlgebra::subtract(a,b) ) );
    distances.push_back( LinearAlgebra::magnitude( LinearAlgebra::subtract(a,c) ) );
    distances.push_back( LinearAlgebra::magnitude( LinearAlgebra::subtract(c,b) ) );
    std::sort( distances.begin(), distances.end() );

    // If two points are very close together then the triangle is degenerate and
    // we cannot compute curvature.
    if (distances[0] < 1.0e-15) {
      QString msg = "Cannot compute curvature. The triangle between points is degenerate.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Modified Heron's formula to avoid square root of negatives when area is close to 0
    double heronProd = ( distances[2] + (distances[1] + distances[0]) ) *
                       ( distances[0] - (distances[2] - distances[1]) ) *
                       ( distances[0] + (distances[2] - distances[1]) ) *
                       ( distances[2] + (distances[1] - distances[0]) );
    // The area is actuall sqrt(heronProd) / 4, but we need to multiply this by 4 later
    // so just remove the division by 4.
    double fourTimesArea = std::sqrt( fabs(heronProd) );

    // Compute the product of the distances.
    double distanceProd = distances[0] * distances[1] * distances[2];

    // Compute the curvature
    return fourTimesArea / distanceProd;
  }


  /**
   * Computes the linear arc length (distance) between two points.
   * 
   * @param firstPoint The first point.
   * @param secondPoint The first point.
   * 
   * @return @b double The linear arc length between the two points.
   */
  double PiecewisePolynomial::computeArcLength(const std::vector<double> &firstPoint,
                                               const std::vector<double> &secondPoint) {

    LinearAlgebra::Vector firstVector( firstPoint.size() );
    LinearAlgebra::Vector SecondVector( secondPoint.size() );
    std::copy(firstPoint.begin(),  firstPoint.end(),  firstVector.begin());
    std::copy(secondPoint.begin(),  secondPoint.end(),  SecondVector.begin());

    return LinearAlgebra::magnitude( LinearAlgebra::subtract(firstVector, SecondVector) );
  }


  /**
   * @brief Fits polynomials to a given data set.
   * 
   * Fits polynomials for each segment to a given data set. A least squares
   * solution is computed for each segment. Additionally, continuity up to
   * order min(degree - 1, 2) is enforced at each interior knot. The first and
   * last knots are free.
   * 
   * @param values The vector of parameter values cooresponding to each data
   *               point. These are expected to be sorted.
   * @param data A vector of data points to fit polynomials over. The paremter
   *             value for each data point must be the entry in values at the
   *             same index.
   */
  void PiecewisePolynomial::computePolynomials(const std::vector<double> &values,
                                               const std::vector< std::vector<double> > &data) {

    /** Create the LeastSquares object to find the coefficients
    The coefficients are ordered based on the following:

    All of the coefficients for each segment are adjacent
    All of the coefficients for each dimensions of each segment are adjacent
    The coefficients are ordered in increasing order

    For a two dimensional, 2nd order piecewise polynomial with two segments,
    f(t)= ( c_1_x + b_1_x * t + a_1_x * t^2, c_1_y + b_1_y * t + a_1_y * t^2 ); if t < knotValue
    f(t)= ( c_2_x + b_2_x * t + a_2_x * t^2, c_2_y + b_2_y * t + a_2_y * t^2 ); if t >= knotValue,
    the coefficients would be ordered as follows:
    c_1_x, b_1_x, a_1_x, c_1_y, b_1_y, a_1_y, c_2_x, b_2_x, a_2_x, c_2_y, b_2_y, a_2_y
    */

    // ===========================================
    // = Setup constants and LeastSquares object =
    // ===========================================

    int numSegments = segments();
    int numDimensions = dimensions();
    int numCoeffPerDimension = degree() + 1;
    int numObservations = data.size();
    int numCoefficients = numCoeffPerDimension * numDimensions * numSegments;
    // Attempt to enforce up to 2nd order
    // If the polynomial degree is less than 3, enforce degree - 1
    int continuityOrder = std::min( degree() - 1, 2 );
    int numLSQObservations = numObservations * numDimensions +
                             (continuityOrder + 1) * (numSegments - 1) * numDimensions;
    BasisFunction PolyFunc("PiecewisePolynomial", numCoefficients, numCoefficients);
    LeastSquares LSQ(PolyFunc, true, numLSQObservations, numCoefficients, false);

    // ===================================
    // = Add the data point observations =
    // ===================================

    // Loop through observations
    for (int i = 0; i < numObservations; i++) {

      // Find the segment that the observation belongs to.
      int segIndex = segmentIndex( values[i] );

      // Loop through the dimensions
      for (int j = 0; j < numDimensions; j++) {

        std::vector<double> inputData(numCoefficients, 0.0);
        int startIndex = segIndex * numDimensions * numCoeffPerDimension +
                         j * numCoeffPerDimension;
        // Set the first coefficient to 1 to avoid pow(0, 0)
        inputData[startIndex] = 1;
        for (int k = 1; k < numCoeffPerDimension; k++) {
          inputData[startIndex + k] = pow(values[i], k);
        }
        LSQ.AddKnown( inputData, data[i][j] );

      }

    }

    // =================================
    // = Add the continuity conditions =
    // =================================

    // Loop through segments
    for (int i = 0; i < numSegments - 1; i ++) {

      double knotValue = m_knots[i + 1];

      // Loop through dimensions
      for (int j = 0; j < numDimensions; j++) {

        // Calculate where to fill input data
        int currentSegmentStart = i * numDimensions * numCoeffPerDimension +
                                  j * numCoeffPerDimension;
        int nextSegmentStart = (i + 1) * numDimensions * numCoeffPerDimension +
                               j * numCoeffPerDimension;

        // Loop through continuity conditions
        for (int k = 0; k <= continuityOrder; k++) {
          std::vector<double> inputData(numCoefficients, 0.0);
          for (int h = 0; h < numCoeffPerDimension; h++) {
            if (h == k) {
              inputData[currentSegmentStart + h] = derivativeCoefficient(h, k);
              inputData[nextSegmentStart + h] = - derivativeCoefficient(h, k);
            }
            else if (h > k) {
              inputData[currentSegmentStart + h] = derivativeCoefficient(h, k) *
                                                   pow(knotValue, h-k);
              inputData[nextSegmentStart + h] = - derivativeCoefficient(h, k) *
                                                  pow(knotValue, h-k);
            }
          }
          LSQ.AddKnown(inputData, 0.0, 1e10);
        }

      }

    }

    // ====================
    // = Solve the system =
    // ====================

    LSQ.Solve(LeastSquares::SPARSE);

    // ============================
    // = Extract the coefficients =
    // ============================

    // Loop through segments
    for (int i = 0; i < numSegments; i++) {

      int segmentOffset = i * numDimensions * numCoeffPerDimension;

      // Loop through dimensions
      for (int j = 0; j < numDimensions; j++) {

        int dimensionsOffset = j * numCoeffPerDimension;
        std::vector<double> polyCoefficienst(numCoeffPerDimension);
        //Loop through coefficients
        for (int k = 0; k < numCoeffPerDimension; k++) {
          polyCoefficienst[k] = PolyFunc.Coefficient(segmentOffset + dimensionsOffset + k);
        }
        m_polynomials[i][j].SetCoefficients(polyCoefficienst);
      }

    }

  }


  /**
   * Helper function for computePolynomials to compute the accumulated
   * coefficient to multiply a power of the time by for the continuity
   * conditions.
   * 
   * @param coeffOrder The order of the coefficient in the polynomial.
   * @param derivativeOrder The order of the derivative being taken.
   * 
   * @return @b int The coefficient to multiply by.
   */
  int PiecewisePolynomial::derivativeCoefficient(int coeffOrder, int derivativeOrder) {
    int coeff = 1;
    for (int i = 0; i < derivativeOrder; i++) {
      coeff *= (coeffOrder - i );
    }
    return coeff;
  }


  /**
   * Return the degree of the polynomials.
   * 
   * @return @b int The degree of the polynomials.
   */
  int PiecewisePolynomial::degree() const {
    return m_degree;
  }


  /**
   * Return the coefficients for the polynomials in a given segment.
   * 
   * @param segment The segment to access the coefficients for.
   * 
   * @return @b std::vector<std::vector<double>> A vector of vectors containing
   *                                             the coefficients for each
   *                                             dimensions.
   */
  std::vector< std::vector<double> > PiecewisePolynomial::coefficients(int segment) const {
    std::vector< std::vector<double> > coefficientsVector;
    for (int i = 0 ; i < dimensions(); i++) {
      std::vector<double> coordinateCoefficients;
      for (int j = 0; j < m_polynomials[segment][i].Coefficients(); j++) {
        coordinateCoefficients.push_back( m_polynomials[segment][i].Coefficient(j) );
      }
      coefficientsVector.push_back(coordinateCoefficients);
    }
    return coefficientsVector;
  }


  /**
   * Return the number of dimensions of the space curve.
   * 
   * @return @b int The number of dimensions of the space curve.
   */
  int PiecewisePolynomial::dimensions() const {
    return m_dimensions;
  }


  /**
   * Return the vector of knots that mark the boundaries between segments.
   * 
   * @return @b std::vector<double> The vector of knots.
   */
  const std::vector<double> PiecewisePolynomial::knots() const {
    return m_knots;
  }


  /**
   * Return the number of segments in the PiecewisePolynomial.
   * 
   * @return @b int The number of segments.
   */
  int PiecewisePolynomial::segments() const {
    return m_knots.size() - 1;
  }


  /**
   * Sets the degree of the polynomials. All polynomials will be reset to the
   * zero polynomial.
   * 
   * @param degree The new degree of the polynomials.
   * 
   * @throws IException::Programmer "Input degree must be greater than or equal to 0."
   */
  void PiecewisePolynomial::setDegree(int degree) {
    if ( degree < 0 ) {
      QString msg = "Input degree [" + toString(degree) +
                    "] must be greater than or equal to 0.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    //Save the degree
    m_degree = degree;

    //Reinitialize the polynomials
    std::vector<PolynomialUnivariate> coordinateVector;
    for (int i = 0; i < dimensions(); i++) {
      coordinateVector.push_back( PolynomialUnivariate(degree) );
    }
    m_polynomials.clear();
    for (int i = 0; i < segments(); i++) {
      m_polynomials.push_back(coordinateVector);
    }
  }


  /**
   * Sets the coefficients for the polynomials of a segments.
   * 
   * @param coefficients A vector of vectors containing the coefficients for each dimensions.
   * 
   * @throws IException::Programmer "Segment index is invalid."
   * @throws IException::Programmer "Invalid number of dimensions."
   * @throws IException::Programmer "Invalid number of coefficients for dimension."
   */
  void PiecewisePolynomial::setCoefficients(int segment,
                                      const std::vector< std::vector<double> > &coefficients) {
    if ( segment < 0 || segment >= segments() ) {
      QString msg = "Segment index [" + toString(segment) + "] is invalid. "
                    "Valid segment indices are between [0] and [" + toString(segments() - 1) +
                    "] inclusive.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if ( (int)coefficients.size() != dimensions() ) {
      QString msg = "Invalid number of dimensions [" +
                    toString( (int)coefficients.size() ) + "]. Expected [" +
                    toString( dimensions() ) + "] dimensions.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    for (int i = 0; i < dimensions(); i++) {
      if ( (int)coefficients[i].size() != degree() + 1) {
        QString msg = "Invalid number of coefficients [" +
                      toString( (int)coefficients[i].size() ) +
                      "] for dimension number [" + toString(i + 1) +
                      "]. Expected [" + toString( degree() + 1 ) +
                      "] coefficients.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      m_polynomials[segment][i].SetCoefficients( coefficients[i] );
    }
  }


  /**
   * Sets the dimensions of the polynomials. All polynomials will be reset to
   * the zero polynomial.
   * 
   * @param dimensions The new dimensions of the polynomials.
   * 
   * @throws IException::Programmer "Input dimensions must be greater than 0."
   */
  void PiecewisePolynomial::setDimensions(int dimensions) {
    if ( dimensions < 1 ) {
      QString msg = "Input dimensions [" + toString(dimensions) + "] must be greater than 0.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    //Save the dimensions
    m_dimensions = dimensions;

    //Reinitialize the polynomials
    std::vector<PolynomialUnivariate> zeroSegment;
    PolynomialUnivariate zeroPoly( degree() );
    zeroPoly.SetCoefficients( std::vector<double>(degree() + 1, 0.0) );
    for (int i = 0 ; i < dimensions; i++) {
      zeroSegment.push_back(zeroPoly);
    }
    m_polynomials.clear();
    m_polynomials.resize(segments(), zeroSegment);
  }


  /**
   * Sets the knots of the PiecewisePolynomial. The input vector does not
   * have to be sorted.
   * 
   * @param knots A vector of the knots.
   * 
   * @throws IException::Programmer "Invalid number of knots.
   *                                 At least 2 knots must be specified."
   */
  void PiecewisePolynomial::setKnots(std::vector<double> &knots) {
    if ( knots.size() < 2 ) {
      QString msg = "Invalid number of knots [" + toString( (int)knots.size() ) +
                    "]. At least 2 knots must be specified.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    m_polynomials.clear();    
    m_knots = knots;
    std::sort( m_knots.begin(), m_knots.end() );
    setDimensions( dimensions() );
  }

}
