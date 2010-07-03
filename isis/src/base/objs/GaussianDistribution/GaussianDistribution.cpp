/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/07/09 16:58:08 $                                                                 
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
#include "GaussianDistribution.h"
#include "Message.h"
#include <string>
#include <iostream>

using namespace std;
namespace Isis {
  /**
   * Constructs a gaussian distribution object.
   * 
   * @param mean The Distribution's mean
   * @param standardDeviation The Distribution's standard deviation
   */
  GaussianDistribution::GaussianDistribution (const double mean, const double standardDeviation) {
    p_mean = mean;
    p_stdev = standardDeviation;
  }

  /**
   * Computes and returns the probability of the specified value
   * on the gaussian distribution.
   * 
   * @param value The input value
   * 
   * @returns The PDF evaluated at the specified value
   */
  double GaussianDistribution::Probability (const double value) {
    // P(x) = 1/(sqrt(2*pi)*sigma)*e^(-1/2*((x-mu)/sigma)^2)
    return std::exp(-0.5*std::pow((value-p_mean)/p_stdev, 2))/(std::sqrt(2*PI)*p_stdev);
  }

  /**
   * Computes and returns the cumulative distribution up to the
   * specified value on the gaussian distribution.
   * 
   * @param value the input value
   * 
   * @returns The CDF evaluated at the specified value
   */
  double GaussianDistribution::CumulativeDistribution (const double value) {
    // returns for the two extremes
    if (value == DBL_MIN) {
      return 0.0;
    } else if (value == DBL_MAX) {
      return 1.0;
    }

    // normalize the value and calculate the area under the 
    //  pdf's curve.
    double x = (value-p_mean)/p_stdev;
    double sum = 0.0,
    pre = -1.0;
    double fact = 1.0;  // fact = n!
    // use taylor series to compute the area to machine precesion
    //  i.e. if an iteration has no impact on the sum, none of the following
    //  ones will since they are monotonically decreasing
    for (int n=0; pre != sum; n++) {
      pre = sum;
      // the nth term is x^(2n+1)/( (2n+1)*n!*(-2)^n )
      sum += std::pow(x, 2*n+1)/(fact*(2*n+1)*std::pow(-2.0, n));
      fact *= n+1;
    }

    // return the percentage (100% based)
    return 50.0 + 100.0/std::sqrt(2.0*PI)*sum;
  }

  /**
  * Computes and returns the inverse cumulative distribution
  * evaluated at the specified percentage value on the gaussian
  * distribution.
  *
  *@see "Rational Chebyshev Approximations for the Error Function
  *     - W. J. Cody
   *@see
   *     http://home.online.no/~pjacklam/notes/invnorm/#The_distribution_function
  * 
  * @param percent The input percentage value
  * @returns The ICDF evaluated at the specified percentage value
  */
  double GaussianDistribution::InverseCumulativeDistribution (const double percent) {
    // the cutoff values used in the ICDF algorithm
    static double lowCutoff = 2.425;
    static double highCutoff = 97.575;

    if ((percent < 0.0) || (percent > 100.0)) {
      string m = "Argument percent outside of the range 0 to 100 in";
      m += " [GaussianDistribution::InverseCumulativeDistribution]";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    // for information on the following algorithm, go to the website
    //  specified above

    double x;

    if (percent == 0.0) {
      return DBL_MIN;
    } else if (percent == 100.0) {
      return DBL_MAX;
    }

    if (percent < lowCutoff) {
      double q = std::sqrt(-2.0*std::log(percent/100.0));
      x = C(q)/D(q);
    } else if (percent < highCutoff) {
      double q = percent/100.0 - 0.5,
      r = q*q;
      x = A(r)*q/B(r);
    } else {
      double q = std::sqrt(-2.0*std::log(1.0 - percent/100.0));
      x = -1.0*C(q)/D(q);
    }

    // refine the calculated value using one iteration of Halley's method
    //   for full machine precision
    double e = CumulativeDistribution(p_stdev*x + p_mean) - percent; // the error amount
    double u = e*std::sqrt(2.0*PI)*std::exp(-0.5*x*x);
    x = x - u/(1 + 0.5*x*u);

    return p_stdev*x + p_mean;
  }

  // The following methods are used to compute the ICDF
  //  see the InverseCumulativeDistribution method for
  //  more information

  double GaussianDistribution::A (const double x) {
    static const double a[6] = { -39.69683028665376,
      220.9460984245205,
      -275.9285104469687,
      138.3577518672690,
      -30.66479806614716,
      2.506628277459239};

    return((((a[0]*x + a[1])*x + a[2])*x + a[3])*x + a[4])*x + a[5];
  }

  double GaussianDistribution::B (const double x) {
    static const double b[6] = { -54.47609879822406,
      161.5858368580409,
      -155.6989798598866,
      66.80131188771972,
      -13.28068155288572,
      1.0};

    return((((b[0]*x + b[1])*x + b[2])*x + b[3])*x + b[4])*x + b[5];
  }

  double GaussianDistribution::C (const double x) {
    static const double c[6] = { -0.007784894002430293,
      -0.3223964580411365,
      -2.400758277161838,
      -2.549732539343734,
      4.374664141464968,
      2.938163982698783};

    return((((c[0]*x + c[1])*x + c[2])*x + c[3])*x + c[4])*x + c[5];
  }

  double GaussianDistribution::D (const double x) {
    static const double d[5] = { 0.007784695709041462,
      0.3224671290700398,
      2.445134137142996,
      3.754408661907416,
      1.0};

    return(((d[0]*x + d[1])*x + d[2])*x + d[3])*x + d[4];
  }
}
