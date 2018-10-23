%Include BundleObservationSolveSettings.sip
%Include MaximumLikelihoodWFunctions.sip

%Include type_conversions.sip

namespace Isis {

  class BundleSettings  {
    %TypeHeaderCode
    #include "BundleSettings.h"
    #include "BundleObservationSolveSettings.h"
    #include <QList>
    %End
  public:

    enum ConvergenceCriteria {
      Sigma0,              /**< The value of sigma0 will be used to determine that the bundle
                                adjustment has converged.*/
      ParameterCorrections /**< All parameter corrections will be used to determine that the
                                bundle adjustment has converged.*/
    };

    // enum MaximumLikelihoodModel {
    //   NoMaximumLikelihoodEstimator,
    //   Huber,
    //   ModifiedHuber,
    //   Welsch,
    //   Chen
    // };

    BundleSettings() throw(Isis::IException);

    void setValidateNetwork(bool) throw(Isis::IException);

    void setSolveOptions(bool solveObservationMode = false,
                         bool updateCubeLabel = false,
                         bool errorPropagation = false,
                         bool solveRadius = false,
                         double globalLatitudeAprioriSigma = Isis::Null,
                         double globalLongitudeAprioriSigma = Isis::Null,
                         double globalRadiusAprioriSigma = Isis::Null)  throw(Isis::IException) /KeywordArgs="Optional"/;

   void setOutlierRejection(bool outlierRejection,
                            double multiplier = 1.0) throw(Isis::IException);

   bool solveObservationMode() const throw(Isis::IException);
   bool solveRadius() const throw(Isis::IException);
   bool updateCubeLabel() const throw(Isis::IException);
   bool errorPropagation() const throw(Isis::IException);
   bool outlierRejection() const throw(Isis::IException);
   double outlierRejectionMultiplier() const throw(Isis::IException);
   double globalLatitudeAprioriSigma() const throw(Isis::IException);
   double globalLongitudeAprioriSigma() const throw(Isis::IException);
   double globalRadiusAprioriSigma() const throw(Isis::IException);

   int numberSolveSettings() const throw(Isis::IException);
   Isis::BundleObservationSolveSettings observationSolveSettings(QString instrumentId) const throw(Isis::IException);
   Isis::BundleObservationSolveSettings observationSolveSettings(int n) const throw(Isis::IException);

   void setObservationSolveOptions(QList<Isis::BundleObservationSolveSettings>);
  //  %MethodCode
  //    Py_ssize_t size = PyList_Size(a0);
  //    QList<Isis::BundleObservationSolveSettings> *cppSettingsList = new QList<Isis::BundleObservationSolveSettings>();
  //    int sipErr = 0;
  //    for(int i = 0; i < size; i++) {
  //      PyObject *pyStat = PyList_GET_ITEM(a0, i);
  //      Isis::BundleObservationSolveSettings *settings = (Isis::BundleObservationSolveSettings*)sipConvertToType(pyStat, sipType_Isis_BundleObservationSolveSettings, NULL, SIP_NOT_NONE, NULL, &sipErr);
  //      cppSettingsList->append(*settings);
  //    }
  //
  //    sipCpp->setObservationSolveOptions(*cppSettingsList);
  //  %End

   SIP_PYLIST maximumLikelihoodEstimatorModels() const throw(Isis::IException);
   %MethodCode // convert QList<QPair> to Python list if tuples
     typedef Isis::MaximumLikelihoodWFunctions::Model Model;
     size_t size = sipCpp->maximumLikelihoodEstimatorModels().size();
     PyObject *l = PyList_New(size);
     for (size_t i = 0; i < size; ++i) {
       // get the two items from the QPair
       Model *model = new Model(sipCpp->maximumLikelihoodEstimatorModels()[i].first);
       double doubl = sipCpp->maximumLikelihoodEstimatorModels()[i].second;

       // Convert the two items to PyObjects
       PyObject *first = sipConvertFromType((void*)(model), sipType_Isis_MaximumLikelihoodWFunctions_Model, NULL);
       PyObject *second = PyFloat_FromDouble(doubl);

       // set the tuple items
       PyObject *tuple = PyTuple_New(2);
       PyTuple_SetItem(tuple, 0, first);
       PyTuple_SetItem(tuple, 1, second);

       // Add new tuple to list
       PyList_SetItem(l, i, tuple);
     }
     return l;
   %End


   int numberTargetBodyParameters() const throw(Isis::IException);
   bool solveTargetBody() const throw(Isis::IException);
   bool solvePoleRA() const throw(Isis::IException);
   bool solvePoleRAVelocity() const throw(Isis::IException);
   bool solvePoleDec() const throw(Isis::IException);
   bool solvePoleDecVelocity() const throw(Isis::IException);
   bool solvePM() const throw(Isis::IException);
   bool solvePMVelocity() const throw(Isis::IException);
   bool solvePMAcceleration() const throw(Isis::IException);
   bool solveTriaxialRadii() const throw(Isis::IException);
   bool solveMeanRadius() const throw(Isis::IException);

  };
};
