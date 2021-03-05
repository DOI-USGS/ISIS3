#ifndef BundleObservation_h
#define BundleObservation_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QStringList>
#include <QVector>

#include "BundleImage.h"
#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "LinearAlgebra.h"

namespace Isis {
  class BundleObservationSolveSettings;
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
   *   @history 2016-10-27 Tyler Wilson - Modified formatBundleOutputString to change N/A to FREE
   *                           in the output under POINTS DETAIL when no lat/lon sigmas were entered.
   *                           Fixes #4317.
   *   @history 2016-11-14 Ken Edmundson - Modified the following...
   *                           -changed adjustedSigma from 0.0 to N/A if error propagation is off
   *                            when writing bundleout.txt OR images.csv.
   *                           -changed sigma default from -1.0 to N/A for position and pointing
   *                            parameters when writing images.csv.
   *   @history 2019-05-14 Tyler Wilson - Added the bundleOutputString(std::ofstream &fpOut,
   *                            bool errorPropagation) function which is called by
   *                            BundleSolutionInfo::outputText(). This function is a refactor of
   *                            the formatBundleOutputString and uses the traditional C function
   *                            sprintf instead of QString arg chaining because it's easier to
   *                            make the output columns align nicely.  Also, it maintains
   *                            consistency with text output in BundleSolutionInfo.
   *   @history 2019-06-03  Tyler Wilson - Deleted the formatBundleOutputString and added the
   *                            functions bundleOutputCSV/bundleOutputFetchData. Combined
   *                            with bundleOutputString these three functions will fulfill
   *                            the same functional role formerly occuped by
   *                            formatBundleOutputString but with reduced code duplication.
   *   @history 2019-08-15  Adam Paquette - Readded the formatBundleOutputString function
   *                            and added deprication warnings to formatBundleOutputString.
   *   @history 2019-09-10  Adam Paquette - Changed how bundleOutputString formats the text
   *                            that is written to the bundleout.txt file.
   *
   */
  class BundleObservation : public QVector<BundleImageQsp> {

    public:
      // default constructor
      BundleObservation();

      // constructor
      BundleObservation(BundleImageQsp image, QString observationNumber, QString instrumentId,
                        BundleTargetBodyQsp bundleTargetBody);

      // copy constructor
      BundleObservation(const BundleObservation &src);

      // destructor
      ~BundleObservation();

      // equals operator
      BundleObservation &operator=(const BundleObservation &src);

      // copy method
      void copy(const BundleObservation &src);

      void append(const BundleImageQsp &value);

      BundleImageQsp imageByCubeSerialNumber(QString cubeSerialNumber);

      bool setSolveSettings(BundleObservationSolveSettings solveSettings);

      void setIndex(int n);
      int index();

      int numberPositionParameters();
      int numberPointingParameters();
      int numberParameters();

      QString instrumentId();

      SpiceRotation *spiceRotation();
      SpicePosition *spicePosition();

      LinearAlgebra::Vector &parameterWeights();
      LinearAlgebra::Vector &parameterCorrections();
      LinearAlgebra::Vector &aprioriSigmas();
      LinearAlgebra::Vector &adjustedSigmas();

      const BundleObservationSolveSettingsQsp solveSettings();

      bool applyParameterCorrections(LinearAlgebra::Vector corrections);
      bool initializeExteriorOrientation();
      void initializeBodyRotation();
      void updateBodyRotation();

      void bundleOutputFetchData(QVector<double> &finalParameterValues,
                            int &nPositionCoefficients, int &nPointingCoefficients,
                            bool &useDefaultPosition, bool &useDefaultPointing,
                            bool &useDefaultTwist);
      void bundleOutputString(std::ostream &fpOut,bool errorPropagation);
      QString bundleOutputCSV(bool errorPropagation);

      QString formatBundleOutputString(bool errorPropagation, bool imageCSV=false);
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
      QMap<QString, BundleImageQsp> m_cubeSerialNumberToBundleImageMap;

      QStringList m_serialNumbers;      //!< List of all cube serial numbers in observation.
      QStringList m_parameterNamesList; //!< List of all cube parameters.
      QStringList m_imageNames;         //!< List of all cube names.

      QString m_instrumentId;      //!< Spacecraft instrument id.

      BundleObservationSolveSettingsQsp m_solveSettings; //!< Solve settings for this observation.

      SpiceRotation *m_instrumentRotation;   //!< Instrument spice rotation (in primary image).
      SpicePosition *m_instrumentPosition;   //!< Instrument spice position (in primary image).
//    SpiceRotation *m_bodyRotation;         //!< Instrument body rotation (in primary image).

      BundleTargetBodyQsp m_bundleTargetBody;       //!< QShared pointer to BundleTargetBody.

    // TODO??? change these to LinearAlgebra vectors...
      LinearAlgebra::Vector m_weights;     //!< Parameter weights.
      //! Cumulative parameter correction vector.
      LinearAlgebra::Vector m_corrections;
      //LinearAlgebra::Vector m_solution;  //!< parameter solution vector.
      //! A posteriori (adjusted) parameter sigmas.
      LinearAlgebra::Vector m_aprioriSigmas;
      //! A posteriori (adjusted) parameter sigmas.
      LinearAlgebra::Vector m_adjustedSigmas;
  };

  //! Typdef for BundleObservation QSharedPointer.
  typedef QSharedPointer<BundleObservation> BundleObservationQsp;
}

#endif // BundleObservation_h
