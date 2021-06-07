namespace Isis {
  class BundleImage /NoDefaultCtors/ {
  %TypeHeaderCode
    #include "BundleImage.h"
  %End
  %InstanceCode
    sipCpp = new Isis::BundleImage(NULL, "", "");
  %End
  public:


    // QSharedPointer<IsisBundleObservation> parentObservation();
    QString serialNumber() throw(Isis::IException);
    QString fileName() throw(Isis::IException);
  };
};
