#include "BundlePolynomialContinuityConstraint.h"

// Qt Library
#include <QDebug>

// Isis Library
#include "BundleObservationSolveSettings.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

// boost lib
//#include <boost/lexical_cast.hpp>
//#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Default constructor
   *
   */
  BundlePolynomialContinuityConstraint::BundlePolynomialContinuityConstraint() {
    m_numberCkCoefficients = 0;
    m_numberSpkCoefficients = 0;
    m_numberSpkSegments = 0;
    m_numberCkSegments = 0;
    m_numberSpkBoundaries = 0;
    m_numberCkBoundaries = 0;
    m_numberParameters = 0;
    m_numberConstraintEquations = 0;
  }


  /**
   * Constructor
   *
   * @param parentObservation parent BundleObservation
   */
  BundlePolynomialContinuityConstraint::
      BundlePolynomialContinuityConstraint(BundleObservationQsp parentObservation) {

    m_parentObservation = parentObservation;

    // initialize variables
    m_numberCkCoefficients = 0;
    m_numberSpkCoefficients = 0;
    m_numberCkSegments = 0;
    m_numberSpkSegments = 0;
    m_numberSpkBoundaries = 0;
    m_numberCkBoundaries = 0;
    m_numberSegmentParameters = 0;
    m_numberParameters = 0;
    m_numberConstraintEquations = 0;

    BundleObservationSolveSettingsQsp solveSettings = m_parentObservation->solveSettings();

    int nSpkParameters = 0;
    int nCkParameters = 0;

    if (solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      m_numberSpkCoefficients = solveSettings->numberCameraPositionCoefficientsSolved();
      m_numberSpkSegments = solveSettings->numberSpkPolySegments();
      m_numberSpkBoundaries = m_numberSpkSegments - 1;

      nSpkParameters = 3 * m_numberSpkCoefficients;

      // knots contain scaled time
      m_spkKnots = parentObservation->spicePosition()->scaledPolynomialKnots();

      // remove end knots, leaving only knots at segment boundaries
      if (m_spkKnots.size() > 2) {
        m_spkKnots.pop_back();
        m_spkKnots.erase(m_spkKnots.begin());
      }
    }

    if (solveSettings->instrumentPointingSolveOption() !=
        BundleObservationSolveSettings::NoPointingFactors) {

      m_numberCkCoefficients = solveSettings->numberCameraAngleCoefficientsSolved();
      m_numberCkSegments = solveSettings->numberCkPolySegments();
      m_numberCkBoundaries = m_numberCkSegments - 1;

      nCkParameters = 2 * m_numberCkCoefficients;
      if (solveSettings->solveTwist()) {
        nCkParameters += m_numberCkCoefficients;
      }

      // knots contain scaled time
      m_ckKnots = parentObservation->spiceRotation()->scaledPolynomialKnots();

      // remove end knots, leaving only knots at segment boundaries
      if (m_ckKnots.size() > 2) {
        m_ckKnots.pop_back();
        m_ckKnots.erase(m_ckKnots.begin());
      }
    }

    if (nSpkParameters > 0) {
      m_numberConstraintEquations = m_numberSpkBoundaries * (m_numberSpkCoefficients-1) * 3.0;
    }
    if (nCkParameters > 0) {
      if (m_parentObservation->solveSettings()->solveTwist()) {
        m_numberConstraintEquations += m_numberCkBoundaries * (m_numberCkCoefficients-1) * 3.0;
      }
      else {
        m_numberConstraintEquations += m_numberCkBoundaries * (m_numberCkCoefficients-1) * 2.0;
      }
    }

    m_numberParameters = m_numberSpkSegments * nSpkParameters + m_numberCkSegments * nCkParameters;

    m_numberSpkSegmentParameters = nSpkParameters;
    m_numberCkSegmentParameters = nCkParameters;

    constructMatrices();
  }


  /**
   * Destructor
   */
  BundlePolynomialContinuityConstraint::~BundlePolynomialContinuityConstraint() {
  }


  /**
   * Copy constructor
   *
   * Constructs a BundlePolynomialContinuityConstraint from another.
   *
   * @param src Source BundlePolynomialContinuityConstraint to copy.
   */
  BundlePolynomialContinuityConstraint::
      BundlePolynomialContinuityConstraint(const BundlePolynomialContinuityConstraint &src) {
    m_parentObservation = src.m_parentObservation;

    m_numberSegmentParameters = src.m_numberSegmentParameters;
    m_numberParameters = src.m_numberParameters;
    m_numberConstraintEquations = src.m_numberConstraintEquations;

    m_ckKnots = src.m_ckKnots;
    m_spkKnots = src.m_spkKnots;
    m_numberCkCoefficients = src.m_numberCkCoefficients;
    m_numberSpkCoefficients = src.m_numberSpkCoefficients;
    m_numberCkSegments = src.m_numberCkSegments;
    m_numberSpkSegments = src.m_numberSpkSegments;
    m_numberCkBoundaries = src.m_numberCkBoundaries;
    m_numberSpkBoundaries = src.m_numberSpkBoundaries;
    m_numberCkSegmentParameters = src.m_numberCkSegmentParameters;
    m_numberSpkSegmentParameters = src.m_numberSpkSegmentParameters;

    m_designMatrix = src.m_designMatrix;
    m_normalsSpkMatrix = src.m_normalsSpkMatrix;
    m_normalsCkMatrix = src.m_normalsCkMatrix;
    m_rightHandSide = src.m_rightHandSide;
    m_omcVector = src.m_omcVector;
  }


  /**
   * Assignment operator
   *
   * Assigns state of this BundlePolynomialContinuityConstraint from another.
   *
   * @param src Source BundlePolynomialContinuityConstraint to assign state from.
   *
   * @return    BundlePolynomialContinuityConstraint& Returns reference to this
   *            BundlePolynomialContinuityConstraint.
   */
  BundlePolynomialContinuityConstraint
      &BundlePolynomialContinuityConstraint::
      operator=(const BundlePolynomialContinuityConstraint &src) {

    // Prevent self assignment
    if (this != &src) {
      m_parentObservation = src.m_parentObservation;

      m_numberSegmentParameters = src.m_numberSegmentParameters;
      m_numberParameters = src.m_numberParameters;
      m_numberConstraintEquations = src.m_numberConstraintEquations;

      m_ckKnots = src.m_ckKnots;
      m_spkKnots = src.m_spkKnots;
      m_numberCkCoefficients = src.m_numberCkCoefficients;
      m_numberSpkCoefficients = src.m_numberSpkCoefficients;
      m_numberCkSegments = src.m_numberCkSegments;
      m_numberSpkSegments = src.m_numberSpkSegments;
      m_numberCkBoundaries = src.m_numberCkBoundaries;
      m_numberSpkBoundaries = src.m_numberSpkBoundaries;
      m_numberCkSegmentParameters = src.m_numberCkSegmentParameters;
      m_numberSpkSegmentParameters = src.m_numberSpkSegmentParameters;

      m_designMatrix = src.m_designMatrix;
      m_normalsSpkMatrix = src.m_normalsSpkMatrix;
      m_normalsCkMatrix = src.m_normalsCkMatrix;
      m_rightHandSide = src.m_rightHandSide;
      m_omcVector = src.m_omcVector;
    }

    return *this;
  }


  /**
   * Returns number of spk segments in piecewise polynomial.
   *
   * @return int Returns number of spk segments in piecewise polynomial.
   */
  int BundlePolynomialContinuityConstraint::numberSpkSegments() const {
    return m_numberSpkSegments;
  }


  /**
   * Returns number of ck segments in piecewise polynomial.
   *
   * @return int Returns number of ck segments in piecewise polynomial.
   */
  int BundlePolynomialContinuityConstraint::numberCkSegments() const {
    return m_numberCkSegments;
  }


  /**
   * Returns number of spk coefficients in piecewise polynomial
   *
   * @return int Returns number of spk coefficients in piecewise polynomial
   */
  int BundlePolynomialContinuityConstraint::numberSpkCoefficients() const {
    return m_numberSpkCoefficients;
  }


  /**
   * Returns number of ck coefficients in piecewise polynomial
   *
   * @return int Returns number of ck coefficients in piecewise polynomial
   */
  int BundlePolynomialContinuityConstraint::numberCkCoefficients() const {
    return m_numberCkCoefficients;
  }


  /**
   * Returns number of continuity constraint equations
   *
   * @return int Number of continuity constraint equations
   */
  int BundlePolynomialContinuityConstraint::numberConstraintEquations() const {
    return m_numberConstraintEquations;
  }


  /**
   * Returns matrix with contribution to position portion of bundle adjustment normal equations from
   * continuity constraints.
   *
   * @return SparseBlockMatrix Matrix with contribution to position portion of bundle adjustment
   *                           normal equations from continuity constraints.
   */
  SparseBlockMatrix &BundlePolynomialContinuityConstraint::normalsSpkMatrix() {
    return m_normalsSpkMatrix;
  }


  /**
   * Returns matrix with contribution to pointing portion of bundle adjustment normal equations from
   * continuity constraints.
   *
   * @return SparseBlockMatrix Matrix with contribution to pointing portion of bundle adjustment
   *                           normal equations from continuity constraints.
   */
  SparseBlockMatrix &BundlePolynomialContinuityConstraint::normalsCkMatrix() {
    return m_normalsCkMatrix;
  }


  /**
   * Returns vector with contribution to bundle adjustment normal equations right hand side from
   * continuity constraints.
   *
   * @return LinearAlgebra::Vector Vector with contribution to bundle adjustment normal equations
   *                               right hand side from continuity constraints.
   */
  LinearAlgebra::Vector
      &BundlePolynomialContinuityConstraint::rightHandSideVector() {
    return m_rightHandSide;
  }

  /**
   * Constructs m_normalsSpkMatrix and m_normalsCkMatrix, m_rightHandSide vector, m_designMatrix,
   * and m_omcVector (or observed - computed vector).
   *
   * @todo need more documentation on technical aspects of approach.
   */
  void BundlePolynomialContinuityConstraint::constructMatrices() {

    if (m_numberConstraintEquations <= 0 )
      return;

    // initialize size of right hand side vector
    // the values in this vector are updated each iteration, but vector size will not change
    m_rightHandSide.resize(m_numberParameters);
    m_rightHandSide.clear();

    // initialize size of design matrix
    // this will not change throughout the bundle adjustment
    m_designMatrix.resize(m_numberConstraintEquations, m_numberParameters, false);
    m_designMatrix.clear();

    // initialize size of "observed - computed" vector
    m_omcVector.resize(m_numberConstraintEquations);

    int designRow=0;

    // spk (position) contribution
    if (m_numberSpkSegments > 1 && m_numberConstraintEquations > 0 && m_numberSpkCoefficients > 1)
      positionContinuity(designRow);

    // ck (pointing) contribution
    if (m_numberCkSegments > 1 && m_numberConstraintEquations > 0 && m_numberCkCoefficients > 1)
      pointingContinuity(designRow);

    int numPositionParameters = m_parentObservation->numberPositionParametersPerSegment();
    int numPointingParameters = m_parentObservation->numberPointingParametersPerSegment();

    // initialize and fill position blocks in m_normalsSpkMatrix
    if (m_numberSpkSegments > 1 && m_numberConstraintEquations > 0 && m_numberSpkCoefficients > 1) {
      m_normalsSpkMatrix.setNumberOfColumns(m_numberSpkSegments);

      for (int i = 0; i < m_numberSpkSegments; i++) {
        m_normalsSpkMatrix.insertMatrixBlock(i, i, numPositionParameters, numPositionParameters);
        LinearAlgebra::Matrix *block = m_normalsSpkMatrix.getBlock(i, i);

        matrix_range<LinearAlgebra::MatrixCompressed>
            mr1 (m_designMatrix, range (0, m_designMatrix.size1()),
                range (i*numPositionParameters, (i+1)*numPositionParameters));

        *block += prod(trans(mr1), mr1);

        if (i > 0) {
          m_normalsSpkMatrix.insertMatrixBlock(i, i-1, numPositionParameters, numPositionParameters);
          block = m_normalsSpkMatrix.getBlock(i, i-1);

          matrix_range<LinearAlgebra::MatrixCompressed>
              mr2 (m_designMatrix, range (0, m_designMatrix.size1()),
                  range ((i-1)*numPositionParameters, i*numPositionParameters));

          *block += prod(trans(mr2), mr1);
        }
      }
    }

    // initialize and fill pointing blocks
    if (m_numberCkSegments > 1 && m_numberConstraintEquations > 0 && m_numberCkCoefficients > 1) {
      m_normalsCkMatrix.setNumberOfColumns(m_numberCkSegments);

      int t = numPositionParameters * m_numberSpkSegments;

      for (int i = 0; i < m_numberCkSegments; i++) {
        m_normalsCkMatrix.insertMatrixBlock(i, i, numPointingParameters, numPointingParameters);
        LinearAlgebra::Matrix *block = m_normalsCkMatrix.getBlock(i, i);

        matrix_range<LinearAlgebra::MatrixCompressed>
            mr1 (m_designMatrix, range (0, m_designMatrix.size1()),
                range (t+i*numPointingParameters, t+(i+1)*+numPointingParameters));

        *block += prod(trans(mr1), mr1);

        if (i > 0) {
          m_normalsCkMatrix.insertMatrixBlock(i, i-1, numPointingParameters, numPointingParameters);
          block = m_normalsCkMatrix.getBlock(i, i-1);

          matrix_range<LinearAlgebra::MatrixCompressed>
              mr2 (m_designMatrix, range (0, m_designMatrix.size1()),
                  range (t+(i-1)*numPointingParameters, t+i*numPointingParameters));

          *block += prod(trans(mr2), mr1);
        }
      }
    }

    // initialize right hand side vector
    updateRightHandSide();
  }


  /**
   * Constructs portion of m_designMatrix relative to position continuity constraints.
   *
   * @param designRow Index of current row of design matrix to fill.
   *
   * @todo need more documentation on technical aspects of approach.
   */
  void BundlePolynomialContinuityConstraint::positionContinuity(int &designRow) {
    LinearAlgebra::Vector partials(m_numberParameters);
    LinearAlgebra::Vector segment1Partials; // segment1 contribution
    LinearAlgebra::Vector segment2Partials; // segment1 contribution

    segment1Partials.resize(m_numberSpkCoefficients);
    segment2Partials.resize(m_numberSpkCoefficients);

    // 0-order continuity
    // {1,t,t^2}, {-1,-t,-t^2} if 2nd order polynomial; {1,t}, {-1,-t} if 1st order polynomial

    double t, t2;

    for (int i = 0; i < m_numberSpkBoundaries; i++) {
      int segment1Start = m_numberSpkSegmentParameters*i;
      int segment2Start = segment1Start+m_numberSpkSegmentParameters;

      // time (t) and time-squared (t2)
      t = m_spkKnots[i];
      t2 = t*t;

      segment1Partials(0) = 1.0;
      segment1Partials(1) = t;
      if (m_numberSpkCoefficients == 3)
        segment1Partials(2) = t2;

      segment2Partials(0) = -1.0;
      segment2Partials(1) = -t;
      if (m_numberSpkCoefficients == 3)
        segment2Partials(2) = -t2;

      // zero order in X, Y, and Z
      for (int j = 0; j < 3; j++) {
        partials.clear();
        std::copy(segment1Partials.begin(),segment1Partials.end(),
                  partials.begin()+segment1Start+m_numberSpkCoefficients*j);
        std::copy(segment2Partials.begin(),segment2Partials.end(),
                  partials.begin()+segment2Start+m_numberSpkCoefficients*j);

        partials *= 1.0e+5; // multiply by square root of weight

        row(m_designMatrix,designRow) = partials;
        designRow++;
      }
    }

    if (m_numberSpkCoefficients == 3) {

    // 1st-order continuity
    // {0,1,2t}, {0,-1,-2t} if 2nd order polynomial; {0,1}, {0,-1} if 1st order polynomial

      segment1Partials(0) = 0.0;
      segment1Partials(1) = 1.0;
      segment2Partials(0) = 0.0;
      segment2Partials(1) = -1.0;

      for (int i = 0; i < m_numberSpkBoundaries; i++) {
        int segment1Start = m_numberSpkSegmentParameters*i;
        int segment2Start = segment1Start+m_numberSpkSegmentParameters;

        // time (t)
        t = m_spkKnots[i];

        if (m_numberSpkCoefficients == 3) {
          segment1Partials(2) = 2.0*t;
          segment2Partials(2) = -2.0*t;
        }

        // 1st order in X, Y, and Z
        for (int j = 0; j < 3; j++) {
          partials.clear();
          std::copy(segment1Partials.begin(),segment1Partials.end(),
                    partials.begin()+segment1Start+m_numberSpkCoefficients*j);
          std::copy(segment2Partials.begin(),segment2Partials.end(),
                    partials.begin()+segment2Start+m_numberSpkCoefficients*j);

          partials *= 1.0e+5; // multiply by square root of weight

          row(m_designMatrix,designRow) = partials;
          designRow++;
        }
      }
    }
  }


  /**
   * Constructs portion of m_designMatrix relative to pointing continuity constraints.
   *
   * @param designRow Index of current row of design matrix to fill.
   *
   * @todo need more documentation on technical aspects of approach.
   */
  void BundlePolynomialContinuityConstraint::pointingContinuity(int &designRow) {
    LinearAlgebra::Vector partials(m_numberParameters);
    LinearAlgebra::Vector segment1Partials; // segment1 contribution
    LinearAlgebra::Vector segment2Partials; // segment1 contribution

    segment1Partials.resize(m_numberCkCoefficients);
    segment2Partials.resize(m_numberCkCoefficients);

    bool solveTwist = m_parentObservation->solveSettings()->solveTwist();

    // 0-order continuity
    // {1,t,t^2}, {-1,-t,-t^2} if 2nd order polynomial; {1,t}, {-1,-t} if 1st order polynomial

    double t, t2;

    for (int i = 0; i < m_numberCkBoundaries; i++) {
      int segment1Start
          = (m_numberSpkSegmentParameters*m_numberSpkSegments) + m_numberCkSegmentParameters*i;
      int segment2Start = segment1Start+m_numberCkSegmentParameters;

      // time (t) and time-squared (t2)
      t = m_ckKnots[i];
      t2 = t*t;

      segment1Partials(0) = 1.0;
      segment1Partials(1) = t;
      if (m_numberCkCoefficients == 3)
        segment1Partials(2) = t2;

      segment2Partials(0) = -1.0;
      segment2Partials(1) = -t;
      if (m_numberCkCoefficients == 3)
        segment2Partials(2) = -t2;

      // zero order in RA, DEC, and TWIST
      for (int j = 0; j < 3; j++) {
        if (j == 2 && solveTwist == false)
          break;

        partials.clear();
        std::copy(segment1Partials.begin(),segment1Partials.end(),
                  partials.begin()+segment1Start+m_numberCkCoefficients*j);
        std::copy(segment2Partials.begin(),segment2Partials.end(),
                  partials.begin()+segment2Start+m_numberCkCoefficients*j);

        partials *= 1.0e+5; // multiply by square root of weight

        row(m_designMatrix,designRow) = partials;
        designRow++;
      }
    }

    if (m_numberCkCoefficients == 3) {

      // 1st-order continuity
      // {0,1,2t}, {0,-1,-2t} if 2nd order polynomial; {0,1}, {0,-1} if 1st order polynomial

      segment1Partials(0) = 0.0;
      segment1Partials(1) = 1.0;
      segment2Partials(0) = 0.0;
      segment2Partials(1) = -1.0;

      for (int i = 0; i < m_numberCkBoundaries; i++) {
        int segment1Start
            = (m_numberSpkSegmentParameters*m_numberSpkSegments) + m_numberCkSegmentParameters*i;
        int segment2Start = segment1Start+m_numberCkSegmentParameters;

        // time (t)
        t = m_ckKnots[i];

        if (m_numberCkCoefficients == 3) {
          segment1Partials(2) = 2.0*t;
          segment2Partials(2) = -2.0*t;
        }

        // 1st order in RA, DEC, and TWIST
        for (int j = 0; j < 3; j++) {
          if (j == 2 && solveTwist == false)
            break;

          partials.clear();
          std::copy(segment1Partials.begin(),segment1Partials.end(),
                    partials.begin()+segment1Start+m_numberCkCoefficients*j);
          std::copy(segment2Partials.begin(),segment2Partials.end(),
                    partials.begin()+segment2Start+m_numberCkCoefficients*j);

          partials *= 1.0e+5; // multiply by square root of weight

          row(m_designMatrix,designRow) = partials;
          designRow++;
        }
      }
    }
  }


  /**
   * Updates right hand side vector after parameters have been updated at each iteration.
   *
   * @todo need more documentation on technical aspects of approach.
   */
  void BundlePolynomialContinuityConstraint::updateRightHandSide() {

    SpicePosition *position = m_parentObservation->spicePosition();
    SpiceRotation *rotation = m_parentObservation->spiceRotation();

    int designRow = 0;

    bool solveTwist = m_parentObservation->solveSettings()->solveTwist();

    // clear "observed - computed" (omc) and "right-hand side" vectors
//  LinearAlgebra::Vector omcVector(m_numberConstraintEquations);
    m_omcVector.clear();

    m_rightHandSide.clear();

    double t, t2;

    if (m_numberSpkSegments > 1) {
      std::vector<double> seg1CoefX(m_numberSpkCoefficients);
      std::vector<double> seg1CoefY(m_numberSpkCoefficients);
      std::vector<double> seg1CoefZ(m_numberSpkCoefficients);
      std::vector<double> seg2CoefX(m_numberSpkCoefficients);
      std::vector<double> seg2CoefY(m_numberSpkCoefficients);
      std::vector<double> seg2CoefZ(m_numberSpkCoefficients);

      // loop over segment boundaries for zero order in X,Y,Z
      for (int i = 0; i < m_numberSpkBoundaries; i++) {

        position->GetPolynomial(seg1CoefX, seg1CoefY, seg1CoefZ, i);
        position->GetPolynomial(seg2CoefX, seg2CoefY, seg2CoefZ, i+1);

        // TODO: if 1st order, should we be using 3 coefficients here?

        // time (t) and time-squared (t2)
        t = m_spkKnots[i];
        t2 = t*t;

        // 0 order in X
        m_omcVector(designRow) = -seg1CoefX[0] - seg1CoefX[1]*t + seg2CoefX[0] + seg2CoefX[1]*t;
        if (m_numberSpkCoefficients == 3) {
          m_omcVector(designRow) = m_omcVector(designRow) - seg1CoefX[2]*t2 + seg2CoefX[2]*t2;
        }
        designRow++;
        // 0 order in Y
        m_omcVector(designRow) = -seg1CoefY[0] - seg1CoefY[1]*t + seg2CoefY[0] + seg2CoefY[1]*t;
        if (m_numberSpkCoefficients == 3) {
          m_omcVector(designRow) = m_omcVector(designRow) - seg1CoefY[2]*t2 + seg2CoefY[2]*t2;
        }
        designRow++;
        // 0 order in Z
        m_omcVector(designRow) = -seg1CoefZ[0] - seg1CoefZ[1]*t + seg2CoefZ[0] + seg2CoefZ[1]*t;
        if (m_numberSpkCoefficients == 3) {
          m_omcVector(designRow) = m_omcVector(designRow) - seg1CoefZ[2]*t2 + seg2CoefZ[2]*t2;
        }
        designRow++;
      }

      if (m_numberSpkCoefficients == 3) {

        // loop over segment boundaries for 1st order in X,Y,Z
        for (int i = 0; i < m_numberSpkBoundaries; i++) {

          position->GetPolynomial(seg1CoefX, seg1CoefY, seg1CoefZ, i);
          position->GetPolynomial(seg2CoefX, seg2CoefY, seg2CoefZ, i+1);

          // time (t)
          t = m_spkKnots[i];

          // 1st order in X
          m_omcVector(designRow) = -seg1CoefX[1] + seg2CoefX[1];
          if (m_numberSpkCoefficients == 3) {
            m_omcVector(designRow) = m_omcVector(designRow) - 2.0*seg1CoefX[2]*t + 2.0*seg2CoefX[2]*t;
          }
          designRow++;
          // 1st order in Y
          m_omcVector(designRow) = -seg1CoefY[1] + seg2CoefY[1];
          if (m_numberSpkCoefficients == 3) {
            m_omcVector(designRow) = m_omcVector(designRow) - 2.0*seg1CoefY[2]*t + 2.0*seg2CoefY[2]*t;
          }
          designRow++;
          // 1st order in Z
          m_omcVector(designRow) = -seg1CoefZ[1] + seg2CoefZ[1];
          if (m_numberSpkCoefficients == 3) {
            m_omcVector(designRow) = m_omcVector(designRow) - 2.0*seg1CoefZ[2]*t + 2.0*seg2CoefZ[2]*t;
          }
          designRow++;
        }
      }
    }

    if (m_numberCkSegments > 1) {
      std::vector<double> seg1CoefRA(m_numberCkCoefficients);
      std::vector<double> seg1CoefDEC(m_numberCkCoefficients);
      std::vector<double> seg1CoefTWIST(m_numberCkCoefficients);
      std::vector<double> seg2CoefRA(m_numberCkCoefficients);
      std::vector<double> seg2CoefDEC(m_numberCkCoefficients);
      std::vector<double> seg2CoefTWIST(m_numberCkCoefficients);

      // loop over segment boundaries for zero order in RA,DEC,TWIST
      for (int i = 0; i < m_numberCkBoundaries; i++) {

        rotation->GetPolynomial(seg1CoefRA, seg1CoefDEC, seg1CoefTWIST, i);
        rotation->GetPolynomial(seg2CoefRA, seg2CoefDEC, seg2CoefTWIST, i+1);

        // TODO: if 1st order, should we be using 3 coefficients here?

        // time (t) and time-squared (t2)
        t = m_ckKnots[i];
        t2 = t*t;

        // 0 order in RA
        m_omcVector(designRow) = -seg1CoefRA[0] - seg1CoefRA[1]*t + seg2CoefRA[0] + seg2CoefRA[1]*t;
        if (m_numberCkCoefficients == 3) {
          m_omcVector(designRow) = m_omcVector(designRow) - seg1CoefRA[2]*t2 + seg2CoefRA[2]*t2;
        }
        designRow++;
        // 0 order in DEC
        m_omcVector(designRow)
            = -seg1CoefDEC[0] - seg1CoefDEC[1]*t + seg2CoefDEC[0] + seg2CoefDEC[1]*t;
        if (m_numberCkCoefficients == 3) {
          m_omcVector(designRow) = m_omcVector(designRow) - seg1CoefDEC[2]*t2 + seg2CoefDEC[2]*t2;
        }
        designRow++;
        if (solveTwist) {
          // 0 order in TWIST
          m_omcVector(designRow)
              = -seg1CoefTWIST[0] - seg1CoefTWIST[1]*t + seg2CoefTWIST[0] + seg2CoefTWIST[1]*t;
          if (m_numberCkCoefficients == 3) {
            m_omcVector(designRow) = m_omcVector(designRow) - seg1CoefTWIST[2]*t2 + seg2CoefTWIST[2]*t2;
          }
          designRow++;
        }
      }

      if (m_numberCkCoefficients == 3) {

      // loop over segment boundaries for 1st order in RA,DEC,TWIST
        for (int i = 0; i < m_numberCkBoundaries; i++) {

          rotation->GetPolynomial(seg1CoefRA, seg1CoefDEC, seg1CoefTWIST, i);
          rotation->GetPolynomial(seg2CoefRA, seg2CoefDEC, seg2CoefTWIST, i+1);

          // time (t)
          t = m_ckKnots[i];

          // 1st order in RA
          m_omcVector(designRow) = -seg1CoefRA[1] + seg2CoefRA[1];
          if (m_numberCkCoefficients == 3) {
            m_omcVector(designRow) = m_omcVector(designRow) - 2.0*seg1CoefRA[2]*t + 2.0*seg2CoefRA[2]*t;
          }
          designRow++;
          // 1st order in DEC
          m_omcVector(designRow) = -seg1CoefDEC[1] + seg2CoefDEC[1];
          if (m_numberCkCoefficients == 3) {
            m_omcVector(designRow) = m_omcVector(designRow) - 2.0*seg1CoefDEC[2]*t + 2.0*seg2CoefDEC[2]*t;
          }
          designRow++;
          if (solveTwist) {
            // 1st order in TWIST
            m_omcVector(designRow) = -seg1CoefTWIST[1] + seg2CoefTWIST[1];
            if (m_numberCkCoefficients == 3) {
              m_omcVector(designRow)
                  = m_omcVector(designRow) - 2.0*seg1CoefTWIST[2]*t + 2.0*seg2CoefTWIST[2]*t;
            }
            designRow++;
          }
        }
      }
    }

    // NOTE: 1.0e+5 is the square root of the weight applied to the constraint equations
    // the design matrix has been premultipled by the square root of the weight
    // we aren't premultiplying the omcVector so we have access to its raw values to output
    // to the bundleout.txt file as the deltas between 0 and 1st order functions at segment
    // boundaries
    m_rightHandSide = prod(trans(m_designMatrix),m_omcVector*1.0e+5);
  }

  /**
   * Creates and returns formatted QString summarizing continuity constraints for output to
     bundleout.txt file.
   *
   * @return QString Returns formatted QString summarizing continuity constraints for output to
   *                 bundleout.txt file.
   */
  QString BundlePolynomialContinuityConstraint::formatBundleOutputString() {
    QString finalqStr = "";
    QString qStr = "";

    int index = 0;

    if (m_numberSpkBoundaries > 0) {
      qStr = QString("\nContinuity Constraints\n======================\n\n"
                     "Position Segments/Boundaries: %1/%2\n"
                     "         0-order Constraints: %3\n").
                     arg(m_numberSpkSegments).
                     arg(m_numberSpkBoundaries).
                     arg(3*m_numberSpkBoundaries);

      // loop over segment boundaries for 0-order constraints
      for (int i = 0; i < m_numberSpkBoundaries; i++) {
        qStr += QString("            Bndry %1 dX/dY/dZ: %2/%3/%4\n").
                       arg(i+1).
                       arg(m_omcVector(index), 5, 'e', 1).
                       arg(m_omcVector(index+1), 5, 'e', 1).
                       arg(m_omcVector(index+2), 5, 'e', 1);
        index += 3;
      }

      if (m_numberSpkCoefficients > 2) {
        qStr += QString("       1st-order Constraints: %1\n").
                        arg(3*m_numberSpkBoundaries);
        // loop over segment boundaries for 1st-order constraints
        for (int i = 0; i < m_numberSpkBoundaries; i++) {
          qStr += QString("            Bndry %2 dX/dY/dZ: %3/%4/%5\n").
                         arg(i+1).
                         arg(m_omcVector(index), 5, 'e', 1).
                         arg(m_omcVector(index+1), 5, 'e', 1).
                         arg(m_omcVector(index+2), 5, 'e', 1);
          index += 3;
        }
      }
    }

    finalqStr += qStr;

    if (m_numberCkBoundaries > 0) {
      if (!m_parentObservation->solveSettings()->solveTwist()) {
        qStr = QString("\nPointing Segments/Boundaries: %1/%2\n"
                       "         0-order Constraints: %3\n").
                       arg(m_numberCkSegments).
                       arg(m_numberCkBoundaries).
                       arg(2*m_numberCkBoundaries);

        // loop over segment boundaries for 0-order constraints
        for (int i = 0; i < m_numberCkBoundaries; i++) {

          qStr += QString("            Bndry %1 dRa/dDec: %2/%3\n").
                         arg(i+1).
                         arg(m_omcVector(index), 5, 'e', 1).
                         arg(m_omcVector(index+1), 5, 'e', 1);
          index += 2;
        }

        if (m_numberCkCoefficients > 2) {
          qStr += QString("       1st-order Constraints: %1\n").
                          arg(2*m_numberCkBoundaries);
          // loop over segment boundaries for 1st-order constraints
          for (int i = 0; i < m_numberCkBoundaries; i++) {
            qStr += QString("            Bndry %2 dRa/dDec: %3/%4\n").
                           arg(i+1).
                           arg(m_omcVector(index), 5, 'e', 1).
                           arg(m_omcVector(index+1), 5, 'e', 1);
            index += 2;
          }
        }
      }
      else {
        qStr = QString("\nPointing Segments/Boundaries: %1/%2\n"
                       "         0-order Constraints: %3\n").
                       arg(m_numberCkSegments).
                       arg(m_numberCkBoundaries).
                       arg(3*m_numberCkBoundaries);

        // loop over segment boundaries for 0-order constraints
        for (int i = 0; i < m_numberCkBoundaries; i++) {

          qStr += QString("       Bndry %1 dRa/dDec/dTwi: %2/%3/%4\n").
                         arg(i+1).
                         arg(m_omcVector(index), 5, 'e', 1).
                         arg(m_omcVector(index+1), 5, 'e', 1).
                         arg(m_omcVector(index+2), 5, 'e', 1);
          index += 3;
        }

        if (m_numberCkCoefficients > 2) {
          qStr += QString("       1st-order Constraints: %1\n").
                          arg(3*m_numberCkBoundaries);
          // loop over segment boundaries for 1st-order constraints
          for (int i = 0; i < m_numberCkBoundaries; i++) {
            qStr += QString("       Bndry %2 dRa/dDec/dTwi: %3/%4/%5\n").
                           arg(i+1).
                           arg(m_omcVector(index), 5, 'e', 1).
                           arg(m_omcVector(index+1), 5, 'e', 1).
                           arg(m_omcVector(index+2), 5, 'e', 1);
            index += 3;
          }
        }
      }
    }

    finalqStr += qStr;

    return finalqStr;
  }
}






