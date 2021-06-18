#ifndef IsisBundleObservation_h
#define IsisBundleObservation_h

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
#include "BundleObservation.h"
#include "BundleMeasure.h"
#include "SurfacePoint.h"

namespace Isis {
  class BundleObservationSolveSettings;
  class SpicePosition;
  class SpiceRotation;

  /**
   * Class for observations that use ISIS camera models in bundle adjustment
   *
   * @ingroup ControlNetworks
   */
  class IsisBundleObservation : public BundleObservation {

    public:
      // default constructor
      IsisBundleObservation();

      // constructor
      IsisBundleObservation(BundleImageQsp image, QString observationNumber, QString instrumentId,
                        BundleTargetBodyQsp bundleTargetBody);

      // copy constructor
      IsisBundleObservation(const IsisBundleObservation &src);

      // destructor
      ~IsisBundleObservation();

      // equals operator
      IsisBundleObservation &operator=(const IsisBundleObservation &src);

      // copy method
      void copy(const IsisBundleObservation &src);

      virtual bool setSolveSettings(BundleObservationSolveSettings solveSettings);

      int numberPositionParameters();
      int numberPointingParameters();
      int numberParameters();

      SpiceRotation *spiceRotation();
      SpicePosition *spicePosition();

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

      virtual QStringList parameterList();

      bool computeTargetPartials(LinearAlgebra::Matrix &coeffTarget, BundleMeasure &measure, BundleSettingsQsp &bundleSettings, BundleTargetBodyQsp &bundleTargetBody);
      bool computeImagePartials(LinearAlgebra::Matrix &coeffImage, BundleMeasure &measure);
      bool computePoint3DPartials(LinearAlgebra::Matrix &coeffPoint3D, BundleMeasure &measure, SurfacePoint::CoordinateType coordType);
      bool computeRHSPartials(LinearAlgebra::Vector &coeffRHS, BundleMeasure &measure);
      double computeObservationValue(BundleMeasure &measure, double deltaVal);

   private:
      bool initParameterWeights();
      BundleObservationSolveSettingsQsp m_solveSettings; //!< Solve settings for this observation.

      SpiceRotation *m_instrumentRotation; //!< Instrument spice rotation (in primary image).
      SpicePosition *m_instrumentPosition; //!< Instrument spice position (in primary image).

      BundleTargetBodyQsp m_bundleTargetBody; //!< QShared pointer to BundleTargetBody.
  };

  //! Typdef for IsisBundleObservation QSharedPointer.
  typedef QSharedPointer<IsisBundleObservation> IsisBundleObservationQsp;
}

#endif // IsisBundleObservation_h
