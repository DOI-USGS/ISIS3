#ifndef AbstractBundleObservation_h
#define AbstractBundleObservation_h

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

  /**
   * @brief Class for bundle observations
   */
  class AbstractBundleObservation : public QVector<BundleImageQsp> {

    public:
      // default constructor
      AbstractBundleObservation();

      // constructor
      AbstractBundleObservation(BundleImageQsp image, QString observationNumber, QString instrumentId,
                        BundleTargetBodyQsp bundleTargetBody); // target body and CSM question

      // copy constructor
      AbstractBundleObservation(const AbstractBundleObservation &src);

      // destructor
      virtual ~AbstractBundleObservation();

      // equals operator
      virtual AbstractBundleObservation &operator=(const AbstractBundleObservation &src);

      // copy method
      // not implementedn in BundleObservation either??? 
//      virtual void copy(const AbstractBundleObservation &src);

      virtual void append(const BundleImageQsp &value);

      BundleImageQsp imageByCubeSerialNumber(QString cubeSerialNumber);

      virtual bool setSolveSettings(BundleObservationSolveSettings solveSettings);

      void setIndex(int n);
      int index();

      QString instrumentId();

      LinearAlgebra::Vector &parameterWeights();
      LinearAlgebra::Vector &parameterCorrections();
      LinearAlgebra::Vector &aprioriSigmas();
      LinearAlgebra::Vector &adjustedSigmas();

      virtual const BundleObservationSolveSettingsQsp solveSettings();
      int numberParameters();
      virtual bool applyParameterCorrections(LinearAlgebra::Vector corrections);


      virtual void bundleOutputString(std::ostream &fpOut,bool errorPropagation);
      virtual QString bundleOutputCSV(bool errorPropagation);

      virtual QString formatBundleOutputString(bool errorPropagation, bool imageCSV=false);

      // CAN we have this for both?
      virtual QStringList parameterList();
      virtual QStringList imageNames();

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

  //! Typdef for AbstractBundleObservation QSharedPointer.
  typedef QSharedPointer<AbstractBundleObservation> AbstractBundleObservationQsp;
}

#endif // AbstractBundleObservation_h
