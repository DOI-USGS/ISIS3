#if !defined(DriftCorrect_h)
#define DriftCorrect_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $
 * $Date: 2008/05/23 19:15:17 $
 * $Id: DriftCorrect.h,v 1.4 2008/05/23 19:15:17 kbecker Exp $
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
#include <string>
#include <vector>

#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "HiCalConf.h"
#include "NonLinearLSQ.h"
#include "Component.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief Computes non-linear lsq fit of HiRISE Drift
   * 
   * This class is best used with individual HiRISE images as the number of
   * lines is critical to proper use.  It is best applied by getting the buffer
   * as a reference and applying it during systematic processing.  There really
   * is no direct method to handle line access.
   * 
   * @ingroup Utility
   * 
   * @author 2007-11-07 Kris Becker 
   * @internal 
   *   @history 2008-05-23 Kris Becker Added the ZdOnFailUseLinear option 
   */
  class DriftCorrect : public NonLinearLSQ, public Component {

    public:
    DriftCorrect(const HiCalConf &conf);
    virtual ~DriftCorrect() { }

    /** 
     * @brief Set binning/summing mode
     * 
     * @param bin Summing mode of observatio
     */
    void setBin(int bin) { _timet.setBin(bin); }

    /** 
     * @brief Set scan line time
     * 
     * @param ltime Scan line time
     */
    void setLineTime(double ltime) { _timet.setLineTime(ltime); }

    /** 
     * @brief Returns the size of the data buffer
     *  
     * This is the size of the original data buffer. 
     *  
     * @see nSize()
     * @return int Size of the input buffer
     */
     inline int size() const { return (_data.dim()); }

    /** 
     * @brief Returns the size of the fitted buffer 
     *  
     * \b Important: This returns the size of the buffer being 
     *    fitted and not the size of original data buffer.  This is
     *    a requirement of the NonLinearLSQ class.  USE WITH
     *    CAUTION!
     * 
     * 
     * @return int Size of buffer being fitted
     */
    int nSize()  const { return (_b2.dim()); }

    /** 
     * @brief Number of parameter to be fitted
     *  
     * This is the number of parameters that DriftCorrect needs to 
     * fit.  This functin is a requirement of the NonLinearLSQ 
     * class. 
     * 
     * @return int  Number of parameter to fit
     */
    int nParms() const { return (4); }

    /** Sets the absolute error parameter */
    void setabsErr(double absError) { _absErr = absError; }
    /** Sets the relative error parameter  */
    void setrelErr(double relError) { _relErr = relError; }
    /** Returns the current value of the absolute error */
    double absErr() const { return (_absErr); }
    /** Returns the current value of the relative error */
    double relErr() const { return (_relErr); }

    HiVector Solve(const HiVector &d);
    NLVector guess();
    int checkIteration(const int Iter, const NLVector &fitcoefs, 
                       const NLVector &uncerts, double cplxconj,
                       int Istatus);

    NLVector f_x(const NLVector &a);
    NLMatrix df_x(const NLVector &a);

    /** Returns the Chi-Square value of the fit solution */
    double Chisq() const { return (_chisq); }
    /** Returns the Degrees of Freedom */
    int    DoF()   const { return (nSize() - nParms()); }

    HiVector Yfit() const; 
    HiVector Normalize(const HiVector &v);

  private:
     HiLineTimeEqn _timet;    //  This is the X data set
     HiVector      _data;     //  Typically will be the HiRISE buffer data
     HiVector      _b2;       //  Data buffer used in fitting
     double        _absErr;   //  Absolute error convergence test
     double        _relErr;   //  Relative error convergence test
     double        _maxLog;   //  Maximum log value to constrain
     int           _badLines; //  Exclude lines at end of buffer
     int           _sWidth;   //  Width of guestimate filter
     int           _sIters;   //  Filter interations
     bool          _skipFit;  //  Skip fitting and pass input through
     bool          _useLinFit; // Use linear fit on failure of LM, else Zf
     int           _minLines; //  Minimum number of lines to fit (Default: 100)
     HiVector      _cc;       //  Parameter of 2-D fit
     HiVector      _guess;    //  Initial guestimate of solutions
     HiVector      _coefs;    //  Coefficients of solution
     HiVector      _uncert;   //  Uncertanties
     double        _chisq;    //  ChiSq of NonLinear equation

     HiVector poly_fit(const HiVector &d, const double line0 = 0.0) const;
     virtual void printOn(std::ostream &o) const;
     int goodLines(const HiVector &d) const { return (d.dim() - _badLines); }
     bool gotGoodLines(const HiVector &d) const { 
       return (goodLines(d) >= _minLines);
     }
  };               

}     // namespace Isis
#endif


