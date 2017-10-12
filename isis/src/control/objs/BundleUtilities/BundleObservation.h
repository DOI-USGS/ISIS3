#ifndef BundleObservation_h
#define BundleObservation_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/22 01:35:17 $
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
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QStringList>
#include <QVector>

#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "LinearAlgebra.h"

namespace Isis {
  class BundleImage;
  class BundlePolynomialContinuityConstraint;
  class SpicePosition;
  class SpiceRotation;

  /**
   * @brief Class for bundle observations
   *
   * This class is used for creating a bundle observation. Contained BundleImages are stored as
   * shared pointers, so they will be automatically deleted when all shared pointers are deleted.
   *  
   * @ingroup ControlNetworks
   *
   * @author 2014-07-09 Ken Edmundson
   *
   * @internal
   *   @history 2014-07-09 Ken Edmundson - Original version.
   *   @history 2014-07-16 Jeannie Backer - Replaced QVectors with QLists.
   *   @history 2014-07-17 Kimberly Oyama - Added member variables and accessors for the images and
   *                           parameters in this observation. They will be used for the correlation
   *                           matrix.
   *   @history 2014-07-23 Jeannie Backer - Replaced QVectors with QLists.
   *   @history 2015-02-20 Jeannie Backer - Brought closer to Isis coding standards.
   *   @history 2016-08-03 Jesse Mapel - Changed contained member type to a QSharedPointer.
   *                           Also changed m_solveSettings to a QSharedPointer. Fixes #4150.
   *   @history 2016-08-03 Ian Humphrey - Updated documentation and coding standards. Fixes #4078.
   *   @history 2016-08-10 Jeannie Backer - Replaced boost vector with Isis::LinearAlgebra::Vector.
   *                           References #4163.
   *   @history 2016-08-15 Jesse Mapel - Added a map between cube serial number and contained
   *                           bundle image.  References #4159.
   *   @history 2016-08-23 Ian Humphrey - The applyParameterCorrections() method now throws the
   *                           last exception. Fixes #4153.
   *   @history 2016-10-06 Tyler Wilson - Modified the function formatBundleOutputString so
   *                           that it can be used by BundleSolutionInfo::outputCSVImages()
   *                           function.  Fixes #4314.
   *   @history 2016-10-26 Ian Humphrey - Modified formatBundleOutputString() to provided default
   *                           values for all solve parameters, whether they are being solved for
   *                           or not. Fixes #4464.
   *   @history 2016-10-27 Tyler Wilson Modified formatBundleOutputString to change N/A to FREE 
   *                           in the output under POINTS DETAIL when no lat/lon sigmas were entered.
   *                           Fixes #4317.
   *   @history 2016-11-14 Ken Edmundson Modified the following...
   *                           -changed adjustedSigma from 0.0 to N/A if error propagation is off
   *                            when writing bundleout.txt OR images.csv.
   *                           -changed sigma default from -1.0 to N/A for position and pointing
   *                            parameters when writing images.csv. 
   *   @history 2017-07-14 Ken Edmundson - Added support for piecewise polynomials...
   *                           -m_continuityConstraints member.
   *                           -methods...
   *                               int numberPolynomialPositionSegments
   *                               int numberPolynomialPointingSegments
   *                               int numberPolynomialSegments
   *                               int numberContinuityConstraints
   *                               void applyContinuityConstraints
   *   @history 2017-10-11 Jesse Mapel - Modified output string methods to account for
   *                           piecewise polynomials.
   *   @history 2017-10-12 Jesse Mapel - Initialize exterior orientation will now return false
   *                           if one of the images has a null camera pointer.
   *
   */
  class BundleObservation : public QVector<QSharedPointer<BundleImage> > {

    public:
      // default constructor
      BundleObservation();

      // constructor
      BundleObservation(QSharedPointer<BundleImage> image, QString observationNumber, QString instrumentId,
                        QSharedPointer<BundleTargetBody> bundleTargetBody);

      // copy constructor
      BundleObservation(const BundleObservation &src);

      // destructor
      ~BundleObservation();
      
      // equals operator
      BundleObservation &operator=(const BundleObservation &src);
      
      // copy method
      void copy(const BundleObservation &src);

      void append(const QSharedPointer<BundleImage> &value);

      QSharedPointer<BundleImage> imageByCubeSerialNumber(QString cubeSerialNumber);
      
      bool setSolveSettings(BundleObservationSolveSettings solveSettings);

      void setContinuityConstraints(QSharedPointer<BundlePolynomialContinuityConstraint> polyConstraints);
      
      void setIndex(int n);
      int index();
      
      int numberPositionParameters();
      int numberPointingParameters();
      int numberParameters();

      int numberPolynomialPositionSegments();
      int numberPolynomialPointingSegments();
      int numberPolynomialSegments();

      int numberContinuityConstraints() const;

      QString instrumentId();
      
      SpiceRotation *spiceRotation();
      SpicePosition *spicePosition();
     
      LinearAlgebra::Vector &parameterWeights();
      LinearAlgebra::Vector &parameterCorrections();
      LinearAlgebra::Vector &aprioriSigmas();
      LinearAlgebra::Vector &adjustedSigmas();

      const BundleObservationSolveSettingsQsp solveSettings();

      void computePartials(LinearAlgebra::Matrix &coeffImage);
      bool applyParameterCorrections(LinearAlgebra::Vector corrections);      
      void applyContinuityConstraints(LinearAlgebra::Matrix *diagonalBlock,
                                      LinearAlgebra::Vector &rhs);

      LinearAlgebra::MatrixUpperTriangular &continuityContraintMatrix();
      LinearAlgebra::Vector &continuityRHS();

      bool initializeExteriorOrientation();
      void initializeBodyRotation();
      void updateBodyRotation();

      QString formatPositionSegmentHeader(int segmentIndex);
      QString formatPositionOutputString(int segmentIndex,
                                         bool errorPropagation, bool imageCSV=false);
      QString formatPointingSegmentHeader(int segmentIndex);
      QString formatPointingOutputString(int segmentIndex,
                                         bool errorPropagation, bool imageCSV=false);
      QString formatBundleContinuityConstraintString();
      QStringList parameterList();
      QStringList imageNames();

    private:
      bool initParameterWeights();

    private:
      QString m_observationNumber; /**< This is typically equivalent to serial number
                                        except in the case of "observation mode" (e.g.
                                        Lunar Orbiter) where for each image in the
                                        observation, the observation number is the serial number
                                        augmented with an additional integer. **/

      int m_index; //!< Index of this observation.

      //! Map between cube serial number and BundleImage pointers.
      QMap<QString, QSharedPointer<BundleImage> > m_cubeSerialNumberToBundleImageMap;

      QStringList m_serialNumbers;      //!< List of all cube serial numbers in observation.
      QStringList m_parameterNamesList; //!< List of all cube parameters.
      QStringList m_imageNames;         //!< List of all cube names.

      QString m_instrumentId;           //!< Spacecraft instrument id.

      //!< Solve settings for this observation.
      QSharedPointer<BundleObservationSolveSettings> m_solveSettings;

      //!< Continuity constraints
      QSharedPointer<BundlePolynomialContinuityConstraint> m_continuityConstraints;

      SpiceRotation *m_instrumentRotation;   //!< Instrument spice rotation (primary image).
      SpicePosition *m_instrumentPosition;   //!< Instrument spice position (primary image).

      //!< QShared pointer to BundleTargetBody.
      QSharedPointer<BundleTargetBody> m_bundleTargetBody;

      LinearAlgebra::Vector m_weights;        //!< Parameter weights.
      LinearAlgebra::Vector m_corrections;    //!< Cumulative parameter correction vector.
      LinearAlgebra::Vector m_aprioriSigmas;  //!< A posteriori (adjusted) parameter sigmas.
      LinearAlgebra::Vector m_adjustedSigmas; //!< A posteriori (adjusted) parameter sigmas.
  };

  //! Typdef for BundleObservation QSharedPointer.
  typedef QSharedPointer<BundleObservation> BundleObservationQsp;
}

#endif // BundleObservation_h
