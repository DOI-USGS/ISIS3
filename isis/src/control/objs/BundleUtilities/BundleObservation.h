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
#include "BundleSettings.h"
#include "BundleTargetBody.h"
#include "LinearAlgebra.h"
#include "BundleMeasure.h"
#include "SurfacePoint.h"

namespace Isis {

  /**
   * @brief Abstract base class for an observation in bundle adjustment.
   *
   * This class is the abstract base class that represents an observation in bundle
   * adjustment. It handles operations related to the images in the observation,
   * parameter corrections, and parameter uncertainties. It defines the interface
   * for choosing observation parameters via BundleObservationSolveSettings. The
   * partial computation apis required by the bundle adjustment are also defined
   * but must be implemented by sub-classes.
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
      virtual ~BundleObservation();

      // equals operator
      virtual BundleObservation &operator=(const BundleObservation &src);

      // copy method

      void copy(const BundleObservation &src);

      virtual bool setSolveSettings(BundleObservationSolveSettings solveSettings) = 0;

      virtual void append(const BundleImageQsp &value);

      BundleImageQsp imageByCubeSerialNumber(QString cubeSerialNumber);

      void setIndex(int n);
      int index();

      QString instrumentId();

      double vtpv();

      virtual LinearAlgebra::Vector &parameterWeights();
      virtual LinearAlgebra::Vector &parameterCorrections();
      virtual LinearAlgebra::Vector &aprioriSigmas();
      virtual LinearAlgebra::Vector &adjustedSigmas();


      virtual const BundleObservationSolveSettingsQsp solveSettings() = 0;
      virtual int numberParameters() = 0;
      virtual bool applyParameterCorrections(LinearAlgebra::Vector corrections) = 0;

      virtual void bundleOutputString(std::ostream &fpOut,bool errorPropagation) = 0;
      virtual QString bundleOutputCSV(bool errorPropagation) = 0;

      virtual QStringList parameterList() = 0;
      virtual QStringList imageNames();

      virtual bool computeTargetPartials(LinearAlgebra::Matrix &coeffTarget, BundleMeasure &measure,
                                         BundleSettingsQsp &bundleSettings, BundleTargetBodyQsp &bundleTargetBody) = 0;
      virtual bool computeImagePartials(LinearAlgebra::Matrix &coeffImage, BundleMeasure &measure) = 0;
      virtual bool computePoint3DPartials(LinearAlgebra::Matrix &coeffPoint3D, BundleMeasure &measure, SurfacePoint::CoordinateType coordType = SurfacePoint::Rectangular) = 0;
      virtual bool computeRHSPartials(LinearAlgebra::Vector &coeffRHS, BundleMeasure &measure) = 0;
      virtual double computeObservationValue(BundleMeasure &measure, double deltaVal) = 0;

    protected:
      QString m_observationNumber; /**< The shared portion of the serial numbers of
                                        all images in the observation. **/
      int m_index; //!< Index of this observation in the set of observations.
      //! Map between cube serial number and BundleImage pointers.
      QMap<QString, BundleImageQsp> m_cubeSerialNumberToBundleImageMap;
      QStringList m_serialNumbers;      //!< List of all cube serial numbers in observation.
      QStringList m_imageNames;         //!< List of all cube names.
      QString m_instrumentId;           //!< Spacecraft instrument id.

      LinearAlgebra::Vector m_weights;     //!< Parameter weights.
      //! Cumulative parameter correction vector.
      LinearAlgebra::Vector m_corrections;
      //! A posteriori (adjusted) parameter sigmas.
      LinearAlgebra::Vector m_aprioriSigmas;
      //! A posteriori (adjusted) parameter sigmas.
      LinearAlgebra::Vector m_adjustedSigmas;
  };

  //! Typdef for BundleObservation QSharedPointer.
  typedef QSharedPointer<BundleObservation> BundleObservationQsp;
}

#endif // BundleObservation_h
