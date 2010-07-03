/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:22 $
 * $Id: DriftCorrect.cpp,v 1.1 2009/09/16 03:37:22 kbecker Exp $
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
#include <cmath>
#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include "DriftCorrect.h"
#include "iString.h"
#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiCalConf.h"
#include "LeastSquares.h"
#include "LowPassFilterComp.h"
#include "MultivariateStatistics.h"
#include "iException.h"

using namespace std;

namespace Isis {

  DriftCorrect::DriftCorrect(const HiCalConf &conf) : 
                             NonLinearLSQ(), Component("DriftCorrect") { 
    DbProfile prof = conf.getMatrixProfile();
    _history.add("Profile["+ prof.Name()+"]");
    _timet.setBin(ToInteger(prof("Summing")));
    _timet.setLineTime(ToDouble(prof("ScanExposureDuration")));


    _skipFit = IsEqual(ConfKey(prof, "ZdSkipFit", std::string("TRUE")), "TRUE");
    _useLinFit = IsTrueValue(prof, "ZdOnFailUseLinear");

    _absErr = ConfKey(prof, "AbsoluteError", 1.0E-4);
    _relErr = ConfKey(prof, "RelativeError", 1.0E-4);

    _sWidth = ConfKey(prof, "GuessFilterWidth", 17);
    _sIters = ConfKey(prof, "GuessFilterIterations", 1);

    if ( prof.exists("MaximumIterations") ) {
      setMaxIters(ToInteger(prof("MaximumIterations")));
    }

    _maxLog = ConfKey(prof, "MaximumLog", 709.0);
    _badLines = ToInteger(prof("TrimLines"))/ToInteger(prof("Summing"));
    _minLines = ConfKey(prof,"ZdMinimumLines", 100);

    string histstr = "DriftCorrect(AbsErr[" + ToString(_absErr) +
                          "],RelErr[" + ToString(_relErr) + 
                          "],MaxIter[" + ToString(maxIters()) + "])";
   _history.add(histstr);
  }

  HiVector DriftCorrect::Solve(const HiVector &d) {
    ostringstream hist;
    _data = d;
    if ( _skipFit || (!gotGoodLines(d))) {
      _b2 = _data;
      _coefs = HiVector(4, 0.0);
      _uncert = _coefs;
      _cc = HiVector(2, 0.0);
      _chisq = 0.0;
      if ( !gotGoodLines(d) ) {
        hist << "NotEnoughLines(GoodLines[" << goodLines(d) 
             << "],MinimumLines[" << _minLines << "]);";
      }

      hist << "SkipFit(TRUE: Not using LMFit)";
      _history.add(hist.str());
    }
    else {
      hist << "Fit(";
      _b2 = HiVector(goodLines(_data));
      if ( success(curvefit()) ) {
        _coefs = coefs();
        _uncert = uncert();
        hist << "Solved,#Iters[" << nIterations() << "],ChiSq[" << Chisq()
             << "],DoF[" << DoF() << "])";
        _history.add(hist.str());
        _history.add("a0("+ToString(_coefs[0])+"+-"+ToString(_uncert[0])+")");
        _history.add("a1("+ToString(_coefs[1])+"+-"+ToString(_uncert[1])+")");
        _history.add("a2("+ToString(_coefs[2])+"+-"+ToString(_uncert[2])+")");
        _history.add("a3("+ToString(_coefs[3])+"+-"+ToString(_uncert[3])+")");
      } 
      else {
        //  Punt, fit a straight line to the data
        _cc = poly_fit(d);
        HiVector a(4);
        a[0] = _cc[0];
        a[1] = _cc[1];
        a[2] = 0.0;
        a[3] = 0.0;
        _coefs = a;
  
        hist << "Failed::Reason("<< statusstr() << "),#Iters[" 
             << nIterations() << "])";
        _history.add(hist.str());
        _history.add("a0("+ToString(_coefs[0])+")");
        _history.add("a1("+ToString(_coefs[1])+")");
        _history.add("a2("+ToString(_coefs[2])+")");
        _history.add("a3("+ToString(_coefs[3])+")");
        if ( _useLinFit ) {
          _history.add("OnFailureUse(LinearFit(Zf))");
        }
        else {  
          _skipFit = true;
          _history.add("OnFailureUse(ZfBuffer)");
        }
      }
    }
    return (Yfit());
  }

  /** 
   * @brief Compute the initial guess of the fit
   *  
   * This method provides the non-linear fit with an initial guess of the 
   * solution.  It involves a linear fit to the latter half of the data to 
   * provide the first two coefficents, the difference of the averages of the 
   * residuals at both ends of the data set and 5 times the last line time as 
   * the final (fourth) element...a bit involved really. 
   * 
   * @return NLVector  4-element vector of the initial guess coefficients
   */
  NonLinearLSQ::NLVector DriftCorrect::guess()  {
    int n = _data.dim();
    int nb = n - _badLines;

    HiVector b1 = _data.subarray(0, nb-1);
    LowPassFilterComp gfilter(b1, _history, _sWidth, _sIters);

    int nb2 = nb/2;
    _b2 = gfilter.ref();
    HiVector cc = poly_fit(_b2.subarray(nb2,_b2.dim()-1), nb2-1);

    //  Compute the 3rd term guess by getting the average of the residual
    //  at both ends of the data set.
    Statistics s;

    //  Get the head of the data set
    int n0 = MIN(nb, 20);
    for ( int k = 0 ; k < n0 ; k++ ) {
      double d = _b2[k] - (cc[0] + cc[1] * _timet(k));
      s.AddData(&d, 1);
    }
    double head = s.Average();

    //  Get the tail of the data set
    s.Reset();
    n0 = (int) (0.9 * nb);
    for ( int l = n0 ; l < nb ; l++ ) {
      double d = _b2[l] - (cc[0] + cc[1] * _timet(l));
      s.AddData(&d, 1);
    }
    double tail = s.Average();

    //  Populate the guess with the results
    NLVector g(4, 0.0);
    g[0] = cc[0];
    g[1] = cc[1];
    g[2] = head-tail;
    g[3] = -5.0/_timet(nb-1);
    _guess = g;
    _history.add("Guess["+ToString(_guess[0])+ ","+
                          ToString(_guess[1])+ ","+
                          ToString(_guess[2])+ ","+
                          ToString(_guess[3])+ "]");
    return (g); 
  }

  int DriftCorrect::checkIteration(const int Iter, const NLVector &fitcoefs, 
                     const NLVector &uncerts, double cplxconj,
                     int Istatus) { 
      _chisq = pow(cplxconj, 2.0);
      return (Istatus);
  }

  NonLinearLSQ::NLVector DriftCorrect::f_x(const NLVector &a) {
    double a0 = a[0];
    double a1 = a[1];
    double a2 = a[2];
    double a3 = a[3];

    int n = _b2.dim();
    NLVector f(n);
    for (int i = 0 ; i < n ; i++) {
        double lt = _timet(i);
        double et = a3 * lt;
        double Yi = a0 + (a1 * lt) + a2 * exp(MIN(et,_maxLog));
        f[i] = (Yi - _b2[i]);
    }
    return (f);
  }

  NonLinearLSQ::NLMatrix DriftCorrect::df_x(const NLVector &a) {
    // double a0 = a[0];
    // double a1 = a[1];
    double a2 = a[2];
    double a3 = a[3];

    int n = _b2.dim();
    NLMatrix J(n, 4);
    for (int i = 0; i < n; i++) {
      double lt = _timet(i);
      double et = a3 * lt;
      double p0 = exp (MIN(et,_maxLog));
      J[i][0] = 1.0;
      J[i][1] = lt;
      J[i][2] = p0;
      J[i][3] = a2 * lt * p0;
    }
    return (J);
  }

  HiVector DriftCorrect::Yfit() const {
    if ( _skipFit || (!gotGoodLines(_data))) {
      return (_data.copy());
    }
    else {
      HiVector dcorr(_data.dim());
      HiVector a = _coefs;
      for ( int i = 0 ; i < dcorr.dim() ; i++ ) {
        double lt = _timet(i);
        dcorr[i] = a[0] + (a[1] * lt) + a[2] * exp(a[3] * lt);
      }
      return (dcorr);
    }
  }

  HiVector DriftCorrect::Normalize(const HiVector &v) {
    HiVector vNorm(v.dim());
    double v0 = v[0];
    for ( int i = 0 ; i < v.dim() ; i++ ) {
      vNorm[i] = v[i] - v0;
    }
    _history.add("Normalize[" + ToString(v0) + "]");
    return (vNorm);
  }

  HiVector DriftCorrect::poly_fit(const HiVector &d, const double line0) const {
    //  Needs a linear fit to latter half of data
    MultivariateStatistics fit;
    int n = d.dim();
    for ( int i = 0 ; i < n ; i++ ) {
      double t = _timet(line0+i);
      fit.AddData(&t, &d[i], 1);
    }
    NLVector cc(2);
    fit.LinearRegression(cc[0], cc[1]);
    return (cc);
  }

  void DriftCorrect::printOn(std::ostream &o) const {
    o << "#  History = " << _history << endl;
    //  Write out the header
    o << setw(_fmtWidth) << "Line"
      << setw(_fmtWidth+1) << "Time"
      << setw(_fmtWidth+1) << "Data"
      << setw(_fmtWidth+1) << "Fit\n";

    HiVector fit = Yfit();
    for (int i = 0 ; i < _data.dim() ; i++) {
      o << formatDbl(i) << " " 
        << formatDbl(_timet(i)) << " "
        << formatDbl(_data[i]) << " " 
        << formatDbl(fit[i]) << endl;
    }
    return;
  }

}     // namespace Isis


