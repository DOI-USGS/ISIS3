#include "MaximumLikelihoodWFunctions.h"
#include "math.h"
#include "IException.h"
#include "iString.h"
#include <stdio.h>

namespace Isis {

  /**  Sets up a maximumlikelihood estimation function with specified model and tweaking constant
   *  
   *   @param[in] enum Model modelSelection,  the model to be used (see documentation for enum Model)
   *   @param[in] double tweaking constant,  exact meaning varies by model, but generally the larger the value the more influence larger resiudals have on the solution.
   *      As well as possiblely the more measures are included in the solution.
   *   @throws IsisProgrammerError if tweakingConstant <= 0.0
   */
  MaximumLikelihoodWFunctions::MaximumLikelihoodWFunctions(Model modelSelection, double tweakingConstant) {
    //choose Model and define the tweaking constant
    m_PI = acos(-1.0);
    m_model = modelSelection;
    if (tweakingConstant <= 0.0) {
      iString msg = "Maximum likelihood estimation tweaking constants must be > 0.0";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    m_c = tweakingConstant;
  }


  /**  Sets up a maximumlikelihood estimation function with specified model and default tweaking constant
   *  
   *   @param[in] enum Model modelSelection,  the model to be used (see documentation for enum Model)
   */
  MaximumLikelihoodWFunctions::MaximumLikelihoodWFunctions(Model modelSelection) {
    //choose Model and define the tweaking constant
    m_PI = acos(-1.0);
    m_model = modelSelection;
    this->setTweakingConstantDefault();
  }


  /**  Allows the maximum likelihood model to be changed together with the tweaking constant
   *
   *     @param[in] enum Model modelSelection,  the model to be used (see documentation for enum Model)
   *     @param[in] double tweaking constant,  exact meaning varies by model, but generally the larger the value the more influence larger resiudals have on the solution.
   *       As well as possiblely the more measures are included in the solution.
   *   @throws IsisProgrammerError if tweakingConstant <= 0.0
   */
  bool MaximumLikelihoodWFunctions::setModel(Model modelSelection, double tweakingConstant) { 
    //choose Model and define the tweaking constant
    m_model = modelSelection;
    if (tweakingConstant <= 0.0) {
      iString msg = "Maximum likelihood estimation tweaking constants must be > 0.0";
      throw IException(IException::Programmer, msg, _FILEINFO_);
      return false;
    }
    m_c = tweakingConstant;
    return true;
  }


  /**  Allows the maximum likelihood model to be changed together and the default tweaking constant to be set
   *
   *     @Param[in] enum Model modelSelection,  the model to be used (see documentation for enum Model)
   */
  bool MaximumLikelihoodWFunctions::setModel(Model modelSelection) {  
    //choose Model and use default tweaking constant
    m_model = modelSelection;
    this->setTweakingConstantDefault();
    return true;
  }

  void MaximumLikelihoodWFunctions::maximumLikelihoodModel(char *model) {
    switch(m_model) {
    case Huber:
      strcpy(model,"Huber");
      return;  
    case HuberModified:
      strcpy(model,"HuberModified");
      return;
    case Welsch:
      strcpy(model,"Welsch");
      return;  
    case Chen:
      strcpy(model,"Chen");
      return;
    default:
      strcpy(model,"None");
      return;  //default to prevent nonsense from being returned, but the program should never reach this line
    }
  }

  /**  Allows the tweaking constant to be changed without changing the maximum likelihood function
   *
   *   @param[in] double tweaking constant,  exact meaning varies by model, but generally the larger the value the more influence larger resiudals have on the solution.
   *       As well as possiblely the more measures are included in the solution.
   *   @throws IsisProgrammerError if tweakingConstant <= 0.0
   */
  bool MaximumLikelihoodWFunctions::setTweakingConstant(double tweakingConstant) { 
    //leave model type unaltered and change tweaking constant
    if (tweakingConstant <= 0.0) return false;  //the constant must be positive
    if (tweakingConstant <= 0.0) {
      iString msg = "Maximum likelihood estimation tweaking constants must be > 0.0";
      throw IException(IException::Programmer, msg, _FILEINFO_);
      return false;
    }
    m_c = tweakingConstant;
    return true;
  }

  /** Returns the current tweaking constant
   */
  double MaximumLikelihoodWFunctions::tweakingConstant() {
    return m_c;
  }


  void MaximumLikelihoodWFunctions::weightedResidualCutoff(char *cutoff) {
    switch(m_model) {
    case Huber:
      strcpy(cutoff,"N/A");
      return;  
    case HuberModified:
      strcpy(cutoff,"N/A");
      return;
    case Welsch:
      sprintf(cutoff,"%lf",m_c*1.5);
      return;  
    case Chen:
      sprintf(cutoff,"%lf",m_c);
      return;
    default:
      strcpy(cutoff,"None");
      return;  //default to prevent nonsense from being returned, but the program should never reach this line
    }
  }


  /**  This prvoides the scalar for the weight (not the scaler for the square root of the weight, which is generally more useful)
   *
   *     @param[in] double residualZScore, this the residual of a particulare measure in a particular iteration divided by the standard deviation (sigma) of that measure
   *     @return double the scaler adjustment to the weight for the measure  nominal weight = 1 /sigma/sigma and weight' = scaler/sigma/sigma
   */
  double MaximumLikelihoodWFunctions::weightScaler(double residualZScore) {  
    //this is likely the least usefull of the scaler functions but it is provided for completness.  This directly provides the scaler for the weight (instead of the radical weight), thus it provides sqrtWeightScaler^2
    switch(m_model) {
    case Huber:
      return this->huber(residualZScore);  
    case HuberModified:
      return this->huberModified(residualZScore);
    case Welsch:
      return this->welsch(residualZScore);  
    case Chen:
      return this->chen(residualZScore);
    default:
      return 1.0;  //default to prevent nonsense from being returned, but the program should never reach this line
    }
  }


  /**  This provides the scaler to the sqrt of the weight (which is very useful for building normal equations)  
   *
   *     @param[in] double residualZScore, this the residual of a particulare measure in a particular iteration divided by the standard deviation (sigma) of that measure
   *     @return double the scaler adjustment to the sqrt of the weight for the measure  nominal sqrt(weight) = 1 /sigma and sqrt(weight') = scaler/sigma
   */
  double MaximumLikelihoodWFunctions::sqrtWeightScaler(double residualZScore) {
    //it is often convient to use square roots of weights when building normals, this function provides the scaler for the square root of the weight directly
    double scaler = this->weightScaler(residualZScore);
    if (scaler <= 0.0) return 0.0;  //<0 should never happen, but 0.0 may be quite frequent (thus this saves some time)
    return sqrt(scaler);
  }


  /** Huber maximum likelihood estimation function evaluation.  For details see documentation of enum Model
   *
   *     @param[in] double residualZScore, this the residual of a particulare measure in a particular iteration divided by the standard deviation (sigma) of that measure
   *     @return double the scaler adjustment to the weight for the measure  nominal weight = 1 /sigma/sigma and weight' = scaler/sigma/sigma
   */
  double MaximumLikelihoodWFunctions::huber(double residualZScore) {
    //huber weight function
    if ( fabs(residualZScore) < m_c) return 1.0;
    else return m_c/fabs(residualZScore);
  }


  /** Modified Huber maximum likelihood estimation function evaluation.  For details see documentation of enum Model
   *
   *     @param[in] double residualZScore, this the residual of a particulare measure in a particular iteration divided by the standard deviation (sigma) of that measure
   *     @return double the scaler adjustment to the weight for the measure  nominal weight = 1 /sigma/sigma and weight' = scaler/sigma/sigma
   */
  double MaximumLikelihoodWFunctions::huberModified(double residualZScore) {
    //huber modified weight function
    if ( fabs(residualZScore)/m_c < m_PI/2.0) return m_c*(sin(residualZScore/m_c)/residualZScore);
    else return m_c/fabs(residualZScore);
  }


  /** Modified Huber maximum likelihood estimation function evaluation.  For details see documentation of enum Model
   *
   *     @param[in] double residualZScore, this the residual of a particulare measure in a particular iteration divided by the standard deviation (sigma) of that measure
   *     @return double the scaler adjustment to the weight for the measure  nominal weight = 1 /sigma/sigma and weight' = scaler/sigma/sigma
   */
  double MaximumLikelihoodWFunctions::welsch(double residualZScore) {
    //welsch weight function
    return exp(-((residualZScore/m_c)*(residualZScore/m_c)));
  }


  /** Modified Huber maximum likelihood estimation function evaluation.  For details see documentation of enum Model
   *
   *     @param[in] double residualZScore, this the residual of a particulare measure in a particular iteration divided by the standard deviation (sigma) of that measure
   *     @return double the scaler adjustment to the weight for the measure  nominal weight = 1 /sigma/sigma and weight' = scaler/sigma/sigma
   */
  double MaximumLikelihoodWFunctions::chen(double residualZScore) {
    //chen weight function
    if ( fabs(residualZScore) <= m_c) return 6*(m_c*m_c-residualZScore*residualZScore)*(m_c*m_c-residualZScore*residualZScore);
    else return 0.0;
  }


  /**  Sets defualt tweaking constants based on the maximum likelihood estimation model being used
   *
   */
  bool MaximumLikelihoodWFunctions::setTweakingConstantDefault() {
    //default tweaking constants for the various likelihood models
    switch(m_model) {
    case Huber:
      m_c = 1.345;  //"95% asymptotice efficiecy on the standard normal distribution" is obtained with this constant, see Zhang's "Parameter Estimation"
      break;
    case HuberModified:
      m_c = 1.2107; //"95% asymptotice efficiecy on the standard normal distribution" is obtained with this constant, see Zhang's "Parameter Estimation"
      break;
    case Welsch:
      m_c=2.9846; //"95% asymptotice efficiecy on the standard normal distribution" is obtained with this constant, see Zhang's "Parameter Estimation"
      break;
    case Chen:
      m_c=1;  //This is the constant used by Chen in his paper, no specific reason why is stated
      break;
    default:
      m_c=1;  //default, though at the time of writing this should never actually be used
      break;
     }
    return true;
  }


  /**  Suggest a quantile of the probility distribution of the residuals to use as the tweaking constants based on the maximum likelihood estimation model being used
   *
   *     @return double quantile [0,1] the value pretaining to this quantile (in the probility distribution of the residuals) should be used as the tweaking constant
   */
  double MaximumLikelihoodWFunctions::tweakingConstantQuantile() {  
    //returns which quantile of the sqaured residuals is recommended to use as the tweaking constants, this varies as a function of the model being employed
    //desired quantiles for various models,  these parameters are estimated based on inspection of the fucntions and should be tested and revised with experience
    switch(m_model) {
    case Huber:
      return 0.5;  //In this model m_c determines the point at which residuals stop having increased influence on the solution, so after the median all the measures will have the same effect on the solution regardless of magnitude
    case HuberModified:
      return 0.4; //In this model after residualZScore >= m_c*m_PI/2.0 the residuals have the same influence on the solution, 
    case Welsch:
      return 0.7; //at about double m_c the residuals have very little influence
    case Chen:
      return 0.98; //after r > m_c residuals have no influence
    default:
      return 0.5; //default, though at the time of writing this should never actually be used
    }
  }

}//end namespace Isis
