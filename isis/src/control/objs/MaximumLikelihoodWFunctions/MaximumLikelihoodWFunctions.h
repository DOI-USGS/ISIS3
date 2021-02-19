#ifndef MaximumLikelihoodWFunctions_h
#define MaximumLikelihoodWFunctions_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

namespace Isis {
  /**
   * @brief Class provides maximum likelihood estimation functions for robust parameter estimation,
   *        e.g. in bundle adjustment.
   *
   * A maximum likelihood estimation W function provides a scheme for 're-weighting' observations so
   * that measures with large residuals have reduced or negligible effect on the solution.  There
   * are many such functions available, a few have been programmed into this class.  See enum Model
   * documentation for specifics of the estimation models.
   *
   * References:  Zhangs,  "Parameter Estimation: A Tutorial with Application to Conic Fitting"
   *              Koch, "Parameter Estimation and Hypothesis Testing in Linear Systems"
   *                    2nd edition, chapter 3.8
   *              Manual of Photogrammetry, 5th edition, chapter 2.2 (particularly 2.2.6)
   *              Chen, "Robust Regression with Projection Based M-estimators"
   *
   * @ingroup ControlNetwork
   * @ingroup Math
   *
   * @author 2012-03-23 Orrin Thomas
   *
   *
   * @internal
   *   @history 2012-03-23 Orrin Thomas - Original Version
   *   @history 2014-06-23 Jeannie Backer - Moved method implementation to cpp file and other ISIS
   *                           coding standards fixes.
   *   @history 2014-07-03 Jeannie Backer - Replace member variable m_PI with Isis constant
   *   @history 2014-07-16 Jeannie Backer - Added enum to QString method and its inverse.
   *   @history 2014-07-23 Jeannie Backer - Added QDataStream >> and << operators and read/write
   *                           methods.
   *   @history 2014-09-19 Jeannie Backer - Removed bugs. Added documentation. Cleaned
   *                           duplicate code.
   *   @history 2018-06-29 Christopher Combs - Added extra HuberModified case to stringToModel().
   *                           Fixes #5446.
   */
  class MaximumLikelihoodWFunctions {
  public:
    /**
     * The supported maximum likelihood estimation models.
     */
    // Each model has an accompannying private method that converts from a
    // resiuduals to a weight scaler.
    enum Model {

      /**  According to Zhang (Parameter Estimation: A Tutorial with application to conic fitting)
       *   "[Huber's] estimator is so satisfactory this is has been recommended for almost all
       *   situations; very rarely has it been found to be inferior to some other function."  Its
       *   one deficiency is the discontinuous second derivative which cause rare diffeculites.
       *   No measures are totally disregarded.
       *    http://research.microsoft.com/en-us/um/people/zhang/Papers/ZhangIVC-97-01.pdf
       */
      Huber,

      /** A modification to Huber's method propsed by William J.J. Rey in Introduction to Robust
       *  and Quasi-Robust Statistical Methods. Springer, Berlin, Heidelberg, 1983.  It has similiar
       *  properties to the Huber, but with a continuous second derivative.  This comes at the cost
       *  of being somewhat more computationally expernsive.  No measures are totally disregarded.
       *    http://research.microsoft.com/en-us/um/people/zhang/Papers/ZhangIVC-97-01.pdf
       */
      HuberModified,

      /** The Welsch method aggresively discounts measures with large resiudals.  Residuals two
       *  times greater than the tweaking constant are all but ignored.  This method can be risky to
       *  use (at least at first) because it does not gaurantee a unique solution.  And if
       *  sufficient measures are effectively 'removed' by the weighting, the system can become
       *  singular.  The manual of photogrammetry recommended using it for clean up after
       *  convergeance or near convergence had been optained with a more stable method
       *  (such as Huber's).
       *    http://research.microsoft.com/en-us/um/people/zhang/Papers/ZhangIVC-97-01.pdf
       */
      Welsch,

      /** The Chen method was found in "Robust Regression with Projection Based M-estimators"  Chen,
       *  et. al.,  though Chen does not take credit as the author.  It was of interest because he
       *  seemed to present its use as expected in systems with large numbers of outliers, and
       *  because of it's unique properties.  It is exceptionally aggresive.  Residuals less than
       *  the tweaking constant generally have MORE influence than in standard least squares
       *  (or any other estimation function I've studied), and residuals larger than the tweaking
       *  function are totaly discounted.
       */
      Chen
    };
    static QString modelToString(Model model);
    static MaximumLikelihoodWFunctions::Model stringToModel(QString modelName);

    MaximumLikelihoodWFunctions();
    MaximumLikelihoodWFunctions(Model modelSelection);
    MaximumLikelihoodWFunctions(Model modelSelection, double tweakingConstant);
    MaximumLikelihoodWFunctions(const MaximumLikelihoodWFunctions &other);
    ~MaximumLikelihoodWFunctions();
    MaximumLikelihoodWFunctions &operator=(const MaximumLikelihoodWFunctions &other);

    void setModel(Model modelSelection); // uses default tweaking constant
    void setTweakingConstantDefault();

    void setModel(Model modelSelection, double tweakingConstant);
    void setTweakingConstant(double tweakingConstant);

    Model model() const;
    double tweakingConstant() const;

    // the W functions provide an additional weighting factor W which is used
    // to 're-weight' each observation dynamically during an adjustment, the
    // scalar functions provide access to various flavors of this scalar (as
    // a function of the residual divided by the residuals sigma)

    double sqrtWeightScaler(double residualZScore); //it is often convient to use square roots of
                                                    //weights when building normals, this function
                                                    // provides the scaler for the square root of
                                                    // the weight directly
    double tweakingConstantQuantile(); // returns which quantile of the residuals is recommended to
                                       // use as the tweaking constant, this varies as a function of
                                       // the model being employed

    QString weightedResidualCutoff();

    QDataStream &write(QDataStream &stream) const;
    QDataStream &read(QDataStream &stream);

  private:
    double weightScaler(double residualZScore); // This directly provides the scaler for the weight
                                                // (instead of the radical weight), thus it provides
                                                // sqrtWeightScaler^2
    double huber(double residualZScore);
    double huberModified(double residualZScore);
    double welsch(double residualZScore);
    double chen(double residualZScore);

   Model m_model; //!< The enumerated value for the maximum likelihood estimation model to be used.
   double m_tweakingConstant; /**< The tweaking constant for the maximum likelihood models.
                                   Default values are available for each model using the method
                                   setTweakingConstantDefault(). This value can also be manually
                                   adjusted using the method setTweakingConstant(). If there is
                                   knowlege of the probility distrubtion of the residuals in an
                                   adjustment, tweakingConstantQuantile() will recommend which
                                   quantile to use as the tweaking constant.*/

 };
  // operators to read/write to/from binary data
  QDataStream &operator<<(QDataStream &stream, const MaximumLikelihoodWFunctions &mlwf);
  QDataStream &operator>>(QDataStream &stream, MaximumLikelihoodWFunctions &mlwf);

  QDataStream &operator<<(QDataStream &stream, const MaximumLikelihoodWFunctions::Model &modelEnum);
  QDataStream &operator>>(QDataStream &stream, MaximumLikelihoodWFunctions::Model &modelEnum);

};// end namespace Isis

#endif
