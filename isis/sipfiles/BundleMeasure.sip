%Include type_conversions.sip

namespace Isis {
  class BundleMeasure /NoDefaultCtors/ {
    %TypeHeaderCode
      #include "BundleMeasure.h"
    %End

    public:

      bool isRejected() const throw(Isis::IException);
      // Camera *camera() const;
      // Isis::BundleControlPoint *parentControlPoint();
      QSharedPointer<Isis::BundleImage> parentBundleImage() throw(Isis::IException);
      // QSharedPointer<BundleObservation> parentBundleObservation();
      // const QSharedPointer<BundleObservationSolveSettings> observationSolveSettings();

      double sample() const throw(Isis::IException);
      double sampleResidual() const throw(Isis::IException);
      double line() const throw(Isis::IException);
      double lineResidual() const throw(Isis::IException);
      double residualMagnitude() const throw(Isis::IException);
      QString cubeSerialNumber() const throw(Isis::IException);
      double focalPlaneComputedX() const throw(Isis::IException);
      double focalPlaneComputedY() const throw(Isis::IException);
      double focalPlaneMeasuredX() const throw(Isis::IException);
      double focalPlaneMeasuredY() const throw(Isis::IException);
      int observationIndex() const throw(Isis::IException);
  };
};
