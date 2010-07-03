/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/11/07 23:48:13 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                       

#include <cmath>
#include "AtmosModel.h"
#include "NumericalAtmosApprox.h"
#include "NumericalApproximation.h"
#include "iException.h"
#include "iString.h"

using namespace std;
namespace Isis {
  /**  
   * This variation on the NumericalApproximation method 
   * integrates a specified AtmosModel function rather than an 
   * interpolated function based on a data set. It returns the 
   * integral of the given function from a to b for the given 
   * AtmosModel. Integration is performed by Romberg's method for 
   * Numerical Integration of order 2K, where, e.g., K=2 is 
   * simpson's rule. This is a generalization of the trapezoidal 
   * rule.  Romberg Integration uses a series of refinements on 
   * the extended (or composite) trapezoidal rule. This method 
   * calls a polynomial interpolation (Neville's algorithm) to 
   * extrapolate successive refinements. 
   * 
   * @param am  Pointer to AtmosModel object 
   * @param sub Enumerated value of atmospheric function to be 
   *            integrated
   * @param a  Lower limit of integration
   * @param b  Upper limit of integration
   * @return  @b double Integral approximation of the function 
   *          on the interval (a, b)
   * @throws Isis::iException::Programmer "Failed to converge." 
   * @throws Isis::iException::Programmer "Caught an error" (from 
   *             RefineExtendedTrap(), Constructor, Evaluate(), or
   *             PolynomialNevilleErrorEstimate() method)
   * @see mathworld.wolfram.com/RombergIntegration.html 
   * @see RefineExtendedTrap() 
   * @internal 
   *   @history 1999-08-11 K Teal Thompson - Original version in 
   *            Isis2.
   *   @history 2000-12-29 Randy Kirk - Add absolute error 
   *            tolerance.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in  
   *            AtmosModel class. Original name r8qromb().
   *   @history 2008-11-05 Jeannie Walldren - Renamed, moved to new
   *            class, and changed i/o parameters.
   */
  double NumericalAtmosApprox::RombergsMethod (AtmosModel *am, IntegFunc sub, double a, double b) throw (iException &){
    // This method was derived from an algorithm in the text 
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 4.3 by Flannery, Press, Teukolsky, and Vetterling
    int maxits = 20;       // maximium number of iterations allowed to converge
    double dss = 0;        // error estimate for 
    double h[maxits+1];    // relative stepsizes for trap
    double trap[maxits+1]; // successive trapeziodal approximations
    double epsilon;        // desired fractional accuracy
    double epsilon2;       // desired fractional accuracy
    double ss;             // result

    epsilon = 1.0e-4;
    epsilon2 = 1.0e-6;

    h[0] = 1.0;
    try{
      NumericalApproximation interp(NumericalApproximation::PolynomialNeville);
      for (int i=0; i<maxits; i++) { 
        // i will determine number of trapezoidal partitions of area  
        // under curve for "integration" using refined trapezoidal rule
        trap[i] = RefineExtendedTrap(am,sub,a,b,trap[i],i+1); // validates data here
        if (i >= 4) {
          interp.AddData(5, &h[i-4], &trap[i-4]);
          ss = interp.Evaluate(0.0,NumericalApproximation::Extrapolate);
          dss = interp.PolynomialNevilleErrorEstimate()[0];
          interp.Reset();
          // we work only until our necessary accuracy is achieved
          if (fabs(dss) <= epsilon*fabs(ss)) return ss;
          if (fabs(dss) <= epsilon2) return ss;
        }
        trap[i+1] = trap[i];
        h[i+1] = 0.25 * h[i];
        // This is a key step:  the factor is 0.25d0 even though    
        // the stepsize is decreased by 0.5d0.  This makes the      
        // extraplolation a polynomial in h-squared as allowed      
        // by the equation from Numerical Recipes 4.2.1 pg.132, 
        // not just a polynomial in h.
      }
    }
    catch (iException e){ // catch error from RefineExtendedTrap, Constructor, Evaluate, PolynomialNevilleErrorEstimate
      throw e.Message(e.Type(),
                      "NumericalAtmosApprox::RombergsMethod() - Caught the following error: ",
                      _FILEINFO_);
    }
    throw iException::Message(iException::Programmer, 
                              "NumericalAtmosApprox::RombergsMethod() - Failed to converge in " 
                              + iString(maxits) + " iterations.", 
                              _FILEINFO_);
  }

  /** 
   * This variation on the NumericalApproximation method 
   * integrates a specified AtmosModel function rather than an 
   * interpolated function based on a data set. This routine 
   * computes the @a n<sup>th</sup> stage of refinement of an 
   * extended trapezoidal rule. This method is used by 
   * RombergsMethod() to integrate. When called with @a n = 1, the
   * method returns the crudest estimate of the integral. 
   * Subsequent calls with @a n = 2, 3, ... (in sequential order)
   * will improve the accuracy by adding 
   * 2<sup>@a n-2</sup> additional interior points. This method
   * can be used to integrate by the extended trapeziodal rule if 
   * you know the number of steps you want to take. 
   *  
   * @param am  Pointer to AtmosModel object 
   * @param sub Enumerated value of atmospheric function to be 
   *            integrated
   * @param a  Lower limit of integration
   * @param b  Upper limit of integration
   * @param s  Previous value of refinement
   * @param n  Number of partitions to use when integrating
   * @return  @b double Integral (refined) approximation of the function 
   *          on the interval (a, b)
   * @throws Isis::iException::Programmer "Caught an error" (from 
   *             InrFunc2Bint() or OutrFunc2Bint() method) 
   * @see RombergsMethod()
   * @internal 
   *   @history 1999-08-11 K Teal Thompson - Original version.
   *   @history 2007-02-20 Janet Barrett - Imported to Isis3 in AtmosModel class. Original name r8trapzd().
   *   @history 2008-11-05 Jeannie Walldren - Renamed, moved to new
   *          class, and changed i/o parameters.
   */
  double NumericalAtmosApprox::RefineExtendedTrap(AtmosModel *am, IntegFunc sub, double a, double b, double s, unsigned int n) throw (iException &){
    // This method was derived from an algorithm in the text 
    // Numerical Recipes in C: The Art of Scientific Computing
    // Section 4.2 by Flannery, Press, Teukolsky, and Vetterling
    try{
      if (n == 1) {
        double begin;
        double end;
        if (sub == InnerFunction) {
          begin = InrFunc2Bint(am,a);
          end = InrFunc2Bint(am,b);
        }
        else {
          begin = OutrFunc2Bint(am,a);
          end = OutrFunc2Bint(am,b);
        }
        return (0.5 * (b - a) * (begin + end));
      }
      else {
        int it;
        double delta,tnm,x,sum;
        it = (int)(pow(2.0,(double)(n-2)));
        tnm = it;
        delta = (b - a) / tnm; // spacing of the points to be added
        x = a + 0.5 * delta;
        sum = 0.0;
        for (int i=0; i<it; i++) {
          if (sub == InnerFunction) {
            sum = sum + InrFunc2Bint(am,x);
          }
          else {
            sum = sum + OutrFunc2Bint(am,x);
          }
          x = x + delta;
        }
        return (0.5 * (s + (b - a) * sum / tnm));// replace s with refined value
      }
    }
    catch (iException e){ // catch exception from Evaluate()
      throw e.Message(e.Type(),
                      "NumericalAtmosApprox::RefineExtendedTrap() - Caught the following error: ",
                      _FILEINFO_);
    }
  }

  /** 
   *  This function is the outer integrand over mu at
   * specified phi. Outer function to be integrated. 
   *
   * @param am  Pointer to AtmosModel object 
   * @param phi  Angle at which the function will be integrated 
   * @return @b double Value of the function evaluated at the
   *         given @a phi 
   * @internal 
   *   @history 1999-03-15 Randy Kirk - Original version in Isis2.
   *   @history 2000-12-29 Randy Kirk - Modified /hide_inc/ so phi
   *            gets passed, etc.
   *   @history 2007-07-20 Janet Barrett - Imported to Isis3 in 
   *            AtmosModel class.
   *   @history 2008-11-05 Jeannie Walldren - Moved to new class and
   *            replaced Isis::PI with PI since this is in Isis
   *            namespace.
   *
   */
  double NumericalAtmosApprox::OutrFunc2Bint(AtmosModel *am, double phi) {
    double result;
    NumericalAtmosApprox::IntegFunc sub;
    sub = NumericalAtmosApprox::InnerFunction;

    am->p_atmosPhi = phi;
    am->p_atmosCosphi = cos((PI/180.0)*phi);

    NumericalAtmosApprox qromb;
    try{
      result = qromb.RombergsMethod(am,sub,1.0e-6,1.0);
      return result;
    }
    catch (iException e){ // catch exception from RombergsMethod()
      throw e.Message(e.Type(),
                      "NumericalAtmosApprox::OutrFunc2Bint() - Caught the following error: ",
                      _FILEINFO_);
    }
  }

  /**
   * Inner function to be integrated. This function is the inner 
   * integrand with all its parameters except cos(ema) hidden. For
   * atmSwitch=0 this integrand is mu times the photometric angle, 
   * giving the hemispheric albedo for the outer integral. 
   * atmSwitch of 1, 2, 3, give the 3 integrals over the 
   * atmospheric single- particle phase function used in the 
   * Hapke/Henyey-Greenstein atmospheric model. 
   *  
   * @param am  Pointer to AtmosModel object 
   * @param mu  Angle at which the function will be integrated 
   * @return @b double Value of the function evaluated at the 
   *         given @a mu 
   * @throws Isis::iException::Programmer "Invalid value of
   *             atmospheric switch used as argument to this
   *             function"
   * @internal 
   *   @history 1999-03-15 Randy Kirk - Original version in Isis2. 
   *   @history 2000-07-07 Randy Kirk - Add other integrals besides 
   *            Ah.
   *   @history 2000-12-29 Randy Kirk - Modified /hide_inc/ so phi
   *            gets passed, etc. Moved factors to outside
   *            integration.
   *   @history 2007-07-20 Janet Barrett - Imported to Isis3 in AtmosModel class.
   *   @history 2008-11-05 Jeannie Walldren - Moved to new class and
   *            replaced Isis::PI with PI since this is in Isis
   *            namespace.
   */
  double NumericalAtmosApprox::InrFunc2Bint(AtmosModel *am, double mu) {
    double ema;    //pass in mu, get emission angle
    double sine;   //sin(ema)
    double alpha; 
    double phase;  //angle between sun and camera
    double result;
    double phasefn;//Henyey-Greenstein phase fn
    double xx;     //temp var to calculate emunot, emu
    double emunot; //exp(-tau/munot)
    double emu;    //exp(-tau/mu)
    double tfac;   //factor that occurs in the integrals for transmission

    //	calculate the phase angle
    //	also calculate any of the other redundant parameters
    ema = acos(mu) * (180.0/PI);
    sine = sin(ema*(PI/180.0));
    if ((am->p_atmosAtmSwitch == 0) || (am->p_atmosAtmSwitch == 2)) { // Reflection phase <= 90
      alpha = am->p_atmosSini * sine * am->p_atmosCosphi +
          am->p_atmosMunot * mu;
    }
    else { // Transmission phase <= 90
      alpha = am->p_atmosSini * sine * am->p_atmosCosphi -
          am->p_atmosMunot * mu;
    }
    phase = acos(alpha) * (180.0/PI);
    //	now evaluate the integrand; all needed parameters
    //	have been hidden separately and passed to it in COMMON.
    if (am->p_atmosAtmSwitch == 0) {
      // integrand for hemispheric albedo
      result = mu * am->p_atmosPM->CalcSurfAlbedo(phase,am->p_atmosInc,ema);
    }
    else {
      phasefn = (1.0 - am->AtmosHga() * am->AtmosHga()) /pow((1.0+2.0*am->AtmosHga()*alpha+am->AtmosHga()*am->AtmosHga()),1.5);
      xx = -am->AtmosTau() / max(am->p_atmosMunot,1.0e-30);
      if (xx < -69.0) {
        emunot = 0.0;
      }
      else if (xx > 69.0) {
        emunot = 1.0e30;
      }
      else {
        emunot = exp(xx);
      }
      xx = -am->AtmosTau() / max(mu,1.0e-30);

      if (xx < -69.0) {
        emu = 0.0;
      }
      else if (xx > 69.0) {
        emu = 1.0e30;
      }
      else {
        emu = exp(xx);
      }
      if (mu == am->p_atmosMunot) {
        tfac = am->AtmosTau() * emunot / (am->p_atmosMunot * am->p_atmosMunot);
      }
      else {
        tfac = (emunot - emu) / (am->p_atmosMunot - mu);
      }
      if (am->p_atmosAtmSwitch == 1) {
        result = mu * (phasefn - 1.0) * tfac;
      }
      else if (am->p_atmosAtmSwitch == 2) {
        result = am->p_atmosMunot * mu * (phasefn - 1.0) * (1.0 - emunot * emu) / (am->p_atmosMunot + mu);
      }
      else if (am->p_atmosAtmSwitch == 3) {
        result = -sine * am->p_atmosCosphi * (phasefn - 1.0) * tfac;
      }
      else {
        string msg = "NumericalAtmosApprox::InrFunc2Bint() - Invalid value of atmospheric ";
        msg += "switch used as argument to this function";
        throw iException::Message(iException::Programmer,msg,_FILEINFO_);
      }
    }
    return result;
  }
}//end NumericalAtmosApprox.cpp
