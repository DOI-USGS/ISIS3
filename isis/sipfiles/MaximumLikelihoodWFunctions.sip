namespace Isis {

  class MaximumLikelihoodWFunctions {
    %TypeHeaderCode
    #include "MaximumLikelihoodWFunctions.h"
    %End

  public:

    enum Model {
      Huber,
      HuberModified,
      Welsch,
      Chen
    };

    static QString modelToString(Model model) throw(Isis::IException);
    static Isis::MaximumLikelihoodWFunctions::Model stringToModel(QString modelName) throw(Isis::IException);

    MaximumLikelihoodWFunctions() throw(Isis::IException);
    MaximumLikelihoodWFunctions(Model modelSelection) throw(Isis::IException);
    MaximumLikelihoodWFunctions(Model modelSelection, double tweakingConstant) throw(Isis::IException);
    MaximumLikelihoodWFunctions(const Isis::MaximumLikelihoodWFunctions &other) throw(Isis::IException);

    void setModel(Model modelSelection) throw(Isis::IException); // uses default tweaking constant
    void setTweakingConstantDefault() throw(Isis::IException);

    void setModel(Model modelSelection, double tweakingConstant) throw(Isis::IException);
    void setTweakingConstant(double tweakingConstant) throw(Isis::IException);

    Model model() const throw(Isis::IException);
    double tweakingConstant() const throw(Isis::IException);

    // the W functions provide an additional weighting factor W which is used
    // to 're-weight' each observation dynamically during an adjustment, the
    // scalar functions provide access to various flavors of this scalar (as
    // a function of the residual divided by the residuals sigma)

    double sqrtWeightScaler(double residualZScore) throw(Isis::IException); //it is often convient to use square roots of
                                                    //weights when building normals, this function
                                                    // provides the scaler for the square root of
                                                    // the weight directly
    double tweakingConstantQuantile() throw(Isis::IException); // returns which quantile of the residuals is recommended to
                                       // use as the tweaking constant, this varies as a function of
                                       // the model being employed

    QString weightedResidualCutoff() throw(Isis::IException);

    QDataStream &write(QDataStream &stream) const throw(Isis::IException);
    QDataStream &read(QDataStream &stream) throw(Isis::IException);
  };
};
