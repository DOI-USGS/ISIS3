#ifndef BundlePolynomialContinuityConstraint_h
#define BundlePolynomialContinuityConstraint_h
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
// Qt Library
#include <QSharedPointer>

// Isis Library
#include "BundleConstraint.h"
#include "BundleObservation.h"
#include "LinearAlgebra.h"

namespace Isis {
  /**
   * @brief Implements 0, 1, and 2-order piecewise polynomial continuity constraints for bundle
   * adjustment.
   *
   * @ingroup ControlNetworks
   *
   * @author 2017-03-03 Ken Edmundson
   *
   * @internal
   *   @history 2017-03-03 Ken Edmundson - Original version.
   */
  class BundlePolynomialContinuityConstraint : public BundleConstraint {
    public:
      // constructors
      BundlePolynomialContinuityConstraint();
      BundlePolynomialContinuityConstraint(BundleObservationQsp parentObservation);

      // copy constructor
      BundlePolynomialContinuityConstraint(const BundlePolynomialContinuityConstraint &src);

      // destructor
      ~BundlePolynomialContinuityConstraint();

      // Assignment operator
      BundlePolynomialContinuityConstraint &operator=
          (const BundlePolynomialContinuityConstraint &src);

      int numberSpkSegments() const;
      int numberCkSegments() const;
      int numberSpkCoefficients() const;
      int numberCkCoefficients() const;
      int numberConstraintEquations() const;

      void updateRightHandSide();
      LinearAlgebra::MatrixUpperTriangular &normalsMatrix();
      LinearAlgebra::Vector &rightHandSideVector();

      QString formatBundleOutputString();

    private:
      void constructMatrices();
      void positionContinuity(int &designRow);
      void pointingContinuity(int &designRow);

      BundleObservationQsp m_parentObservation;                //! parent BundleObservation

      // spk related members
      std::vector<double> m_spkKnots;                          //! scaled spk boundary times
      int m_numberSpkCoefficients;                             //! # coefficients
      int m_numberSpkSegments;                                 //! # segments
      int m_numberSpkBoundaries;                               //! # segment boundaries
      int m_numberSpkSegmentParameters;

      // ck related members
      std::vector<double> m_ckKnots;                           //! scaled ck boundary times
      int m_numberCkCoefficients;                              //! # coefficients
      int m_numberCkSegments;                                  //! # segments
      int m_numberCkBoundaries;                                //! # segment boundaries
      int m_numberCkSegmentParameters;

      int m_numberSegmentParameters;                           //! TODO: # parameters per segment
      int m_numberParameters;                                  //! TODO: # parameters
      int m_numberConstraintEquations;                         //! # constraint equations
      LinearAlgebra::MatrixCompressed m_designMatrix;          //! design matrix
      LinearAlgebra::MatrixUpperTriangular m_normalsMatrix;    //! normals matrix contribution
      LinearAlgebra::Vector m_rightHandSide;                   //! right hand side of normals
      LinearAlgebra::Vector m_omcVector;                       //! observed minus corrected vector
  };

  //! Typdef for BundlePolynomialContinuityConstraint QSharedPointer.
  typedef QSharedPointer<BundlePolynomialContinuityConstraint> BundlePolynomialContinuityConstraintQsp;
};

#endif
