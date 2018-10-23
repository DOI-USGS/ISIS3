namespace Isis {
  class BundleControlPoint {

    %TypeHeaderCode
    #include "BundleControlPoint.h"
    #include <QSharedPointer>
    %End

    public:

      SIP_PYTYPE measures() throw(Isis::IException);
      %MethodCode
        size_t size = sipCpp->size();

        PyObject *l = PyList_New(size);
        for (size_t i = 0; i < size; ++i) {
          Isis::BundleMeasure* cppMeasure = sipCpp->at(i).data();

          PyObject *pyMeasure = sipConvertFromType((void*)(cppMeasure), sipType_Isis_BundleMeasure, NULL);
          PyList_SetItem(l, i, pyMeasure);
        }

        return l;

      %End

      // BundleControlPoint(ControlPoint *point); // default constructor
      BundleControlPoint(const Isis::BundleControlPoint &src) throw(Isis::IException);
      ~BundleControlPoint();

      // copy
      void copy(const Isis::BundleControlPoint &src) throw(Isis::IException);

      // mutators
      // QSharedPointer<Isis::BundleMeasure> addMeasure(ControlMeasure *controlMeasure);
      void computeResiduals() throw(Isis::IException);
      void setNumberOfRejectedMeasures(int numRejected) throw(Isis::IException);
      void setRejected(bool reject) throw(Isis::IException);
      void setWeights(const QSharedPointer<Isis::BundleSettings> settings, double metersToRadians) throw(Isis::IException);
      void zeroNumberOfRejectedMeasures() throw(Isis::IException);

      // accessors
      bool isRejected() const throw(Isis::IException);
      int numberOfMeasures() const throw(Isis::IException);
      int numberOfRejectedMeasures() const throw(Isis::IException);
      double residualRms() const throw(Isis::IException);
      QString id() const throw(Isis::IException);
      Isis::ControlPoint::PointType type() const throw(Isis::IException);

      // string format methods
      QString formatBundleOutputSummaryString(bool errorPropagation) const throw(Isis::IException);
      QString formatBundleOutputDetailString(bool errorPropagation, double RTM, bool solveRadius=false) throw(Isis::IException) /KeywordArgs="Optional"/;
      QString formatValue(double value, int fieldWidth, int precision) throw(Isis::IException);
      QString formatAprioriSigmaString(int type, int fieldWidth, int precision, bool solveRadius=false) throw(Isis::IException) /KeywordArgs="Optional"/;
      QString formatLatitudeAprioriSigmaString(int fieldWidth, int precision) const throw(Isis::IException);
      QString formatLongitudeAprioriSigmaString(int fieldWidth, int precision) const throw(Isis::IException);
      QString formatRadiusAprioriSigmaString(int fieldWidth, int precision, bool solveRadius=false) throw(Isis::IException) /KeywordArgs="Optional"/;
      QString formatAdjustedSigmaString(int type, int fieldWidth, int precision,
                                        bool errorPropagation) const throw(Isis::IException);
      QString formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                bool errorPropagation) const throw(Isis::IException);
      QString formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                 bool errorPropagation) const throw(Isis::IException);
      QString formatRadiusAdjustedSigmaString(int fieldWidth, int precision,
                                              bool errorPropagation) const throw(Isis::IException);

  };
};
