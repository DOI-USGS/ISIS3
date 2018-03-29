// SIP Wrapper to the ISIS3 bundle class
%Include type_conversions.sip


namespace Isis {
  class BundleAdjust : public QObject {
    %TypeHeaderCode
    #include "BundleAdjust.h"
    #include "IException.h"
    %End

  public:
    BundleAdjust(PyObject* settings, const QString &cnetFile, const QString &cubeList, bool printSummary = true) throw(Isis::IException) [(QSharedPointer<Isis::BundleSettings>, QString, QString, bool)];
    %MethodCode
      int sipErr = 0;
      QSharedPointer<Isis::BundleSettings> *wrappedSettings = new QSharedPointer<Isis::BundleSettings>();

      Isis::BundleSettings *settings = (Isis::BundleSettings*) sipConvertToType(a0, sipType_Isis_BundleSettings, NULL, SIP_NOT_NONE, NULL, &sipErr);
      wrappedSettings->reset(settings);
      try {
        sipCpp = new sipIsis_BundleAdjust(*wrappedSettings, *a1, *a2, a3);
      }
      catch (Isis::IException &error) {
          PyErr_SetString(sipException_Isis_IException, error.what());
          return NULL;
      }
    %End

    Isis::BundleSolutionInfo solveCholeskyBR() throw(Isis::IException) ;
   };
};
