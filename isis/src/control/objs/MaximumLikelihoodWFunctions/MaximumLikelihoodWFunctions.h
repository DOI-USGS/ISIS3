#ifndef MaximumLikelihoodWFunctions_h
#define MaximumLikelihoodWFunctions_h

/**
 * @file
 * $Revision: 1.14 $
 * $Date: 2009/09/08 17:38:17 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

namespace Isis {
  /**
   * @brief Class provides maximum likelihood estimation functions for robust parameter estimation e.g. in bundle adjustment.
   *
   * A maximum likelihood estimation W function provides a scheme for 're-weighting' observations so that measures with
   * large residuals have reduced or negligible effect on the solution.  There are many such functions available, a few
   * have been programmed into this class.  See enum Model documentation for specifics of the estimation models.
   * References:  Zhangs,  "Parameter Estimation: A Tutorial with Application to Conic Fitting"
   *              Koch, "Parameter Estimation and Hypothesis Testing in Linear Systems" 2nd edition chapter 3.8
   *              Manual of Photogrammetry 5th edition chapter 2.2 (particularly 2.2.6)
   *              Chen "Robust Regression with Projection Based M-estimators"
   *
   * @ingroup ControlNetwork
   *
   * @author 2012-03-23 Orrin Thomas
   *
   *
   * @internal
   *   @history 2012-03-23 Orrin Thomas -- Original Version
   */
  class MaximumLikelihoodWFunctions {
  public:
    /** These are/supported maximum likelihood estimation models,
     *    each has an accompannying private method that converts
     *    from a resiuduals to a weight scaler.
     */
    enum Model {

      /**  According to Zhang (Parameter Estimation: A Tutorial with application to conic fitting) "[Huber's]
       *    estimator is so satisfactory this is has been recommended for almost all situations; very
       *    rarely has it been found to be inferior to some other function."  Its one deficiency is the
       *    discontinuous second derivative which cause rare diffeculites.  No measures are totally disregarded.
       *    http://research.microsoft.com/en-us/um/people/zhang/Papers/ZhangIVC-97-01.pdf
       */
      Huber,     
     
      /** A modification to Huber's method propsed by William J.J. Rey in Introduction to Robust and Quasi-Robust 
       *    Statistical Methods. Springer, Berlin, Heidelberg, 1983.  It has similiar properties to the Huber,
       *    but with a continuous second derivative.  This comes at the cost of being somewhat more computationally
       *    expernsive.  No measures are totally disregarded.
       *    http://research.microsoft.com/en-us/um/people/zhang/Papers/ZhangIVC-97-01.pdf
       */
      HuberModified,  

      /** The Welsch method aggresively discounts measures with large resiudals.  Residuals two times greater
       *    than the tweaking constant are all but ignored.  This method can be risky to use (at least at first)
       *    because it does not gaurantee a unique solution.  And if sufficient measures are effectively 
       *    'removed' by the weighting, the system can become singular.  The manual of photogrammetry recommended 
       *    using it for clean up after convergeance or near convergeance had been optained with a more stable
       *    method (such as Huber's).
       *    http://research.microsoft.com/en-us/um/people/zhang/Papers/ZhangIVC-97-01.pdf
       */
      Welsch,         

      /** The Chen method was found in "Robust Regression with Projection Based M-estimators"  Chen, et. al.,  though
       *    Chen does not take credit as the author.  It was of interest because he seemed to present its use as 
       *    expected in systems with large numbers of outliers, and because of it's unique properties.  It is
       *    exceptionally aggresive.  Residuals less than the tweaking constant generally have MORE influence than 
       *    in standard least squares (or any other estimation function I've studied), and residuals larger than
       *    the tweaking function are totaly discounted.
       */
      Chen            
    };

    MaximumLikelihoodWFunctions() {this->setModel(Huber);}; //choose Model and define the tweaking constant
    MaximumLikelihoodWFunctions(Model modelSelection, double tweakingConstant); //choose Model and define the tweaking constant
    MaximumLikelihoodWFunctions(Model modelSelection);                          //choose Model and and use the default tweaking constant
    ~MaximumLikelihoodWFunctions() {}; //empty destructor
  
    bool setModel(Model modelSelection, double tweakingConstant); //choose Model and define the tweaking constant
    bool setModel(Model modelSelection);  //choose Model and use default tweaking constant
    bool setTweakingConstant(double tweakingConstant); //leave model type unaltered and change tweaking constant
    bool setTweakingConstantDefault();  //reset tweaking constant to the default for the current model

    //the W functions provide an additional weighting factor W which is used to 're-weight' each observation dynamically during an adjustment, the scalar functions provide access to various flavors of this scalar (as a function of the residual divided by the residuals sigma)
    double sqrtWeightScaler(double residualZScore); //it is often convient to use square roots of weights when building normals, this function provides the scaler for the square root of the weight directly
  
    double tweakingConstantQuantile();  //returns which quantile of the residuals is recommended to use as the tweaking constant, this varies as a function of the model being employed

    double tweakingConstant();

    void maximumLikelihoodModel(char *model);
 
    void weightedResidualCutoff(char *cutoff);

  private:
    /** Tweaking constant for the maximum likelihood estimation function
      *   Suggested values are available for each method: setTweakingConstantDefault()
      *   They can also be manually adjusted: setTweakingConstant(double tweakingConstant)
      *   If there is knowlege of the probility distrubtion of the residuals in an adjustement
      *   tweakingConstantQuantile() will recommend what qunatile to use as the tweaking constant;
      */
    double m_c;    //the tweaking constant for the maximum likelihood models

    /** the constant pi, so it only has to calculated one
     */
    double m_PI;   //PI

    /** Which maximum likelihood model is being used
     */   
    Model m_model; //selected maximum likelihood estimation model


    double weightScaler(double residualZScore);     //This directly provides the scaler for the weight (instead of the radical weight), thus it provides sqrtWeightScaler^2

    double huber(double residualZScore);
    double huberModified(double residualZScore);
    double welsch(double residualZScore);
    double chen(double residualZScore);
  };
}// end namespace Isis

#endif
