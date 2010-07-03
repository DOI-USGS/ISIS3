#if !defined(GruenResult_h)
#define GruenResult_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2009/09/09 23:15:18 $                                                                 
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
#include <iostream>
#include <sstream>                                                                      
#include <iomanip>

#include "GSLUtility.h"
#include "Affine.h"
#include "Constants.h"

namespace Isis {

/**
 * @brief GruenResults contains data derived from the Gruen registration process
 * 
 * This struct container maintains a sizeable volume of data produced by the
 * Gruen class during registration of a point.  It is the return type of the
 * interactive registration algorithm.
 *  
 * @author Kris Becker 
 */
struct GruenResult {
  typedef GSL::GSLUtility::GSLVector GSLVector;
  typedef GSL::GSLUtility::GSLMatrix GSLMatrix;
  enum { NCONSTR = 8 };    //!< Number solution parameters

  /**
   * @brief Default constructor for GruenResult
   */
  GruenResult() { init();   }
  /** Destructor  */
  ~GruenResult() { }


  /** Returns state of solution - final or iterative */
  inline bool IsGood() const { return (isGood); }
  /** Returns the number of paramters/degrees of freedom */
  static int Constraints() { return ((int) NCONSTR); }
  /** Returns the number of iterations - final or iteration */
  inline int Iterations() const { return (nIters); }
  /** Number of points used in current iteration */
  inline int Points() const { return (npts); }

  /** Returns const reference to Gruen Affine/Gain/Shift parameters */
  inline const GSLVector &Alpha() const { return (alpha); }

  /** Reterns the value of the current state of radiometric gain */
  inline double Gain() const { return (alpha[7]); }
  /** Sets initial radiometric gain state */
  void setGain(const double gain) { alpha[7] = gain; }

  /** Returns the value of the current state of radiometric shift */
  inline double Shift() const { return (alpha[6]); }
  /** Sets initial value of radiometric shift state */
  void setShift(const double shift) { alpha[6] = shift; }

  /** Returns the residual pixel error of the current iteration */
  inline double Residuals() const { return (resid); }
  /** Returns the variance of the pixel error of the current iteration */
  inline double Variance() const { 
    return (Residuals() / (double) (Points() - Constraints())); 
  }

  /** Returns the first (largest) eigen value from the Jacobi solution */
  inline double EigenValue1() const { return (eigen[0]); }
  /** Returns the second (smaller) eigen value from the Jacobi solution */
  inline double EigenValue2() const { return (eigen[1]); }
  /** Returns the eigen value (magnitude) for the Jacobi solution */
  inline double Eigen() const { 
    return (std::sqrt((eigen[0]*eigen[0])+(eigen[1]*eigen[1])));
  }   

  /** Store starting sample, line pixel location in image */
  void setStartImage(const double &ssamp, const double &sline) {
    _startSamp = ssamp;
    _startLine = sline;
    return;
  }

  /** Store final starting sample, line pixel location in image */
  void setFinalImage(const double &fsamp, const double &fline) {
    _finalSamp = fsamp;
    _finalLine = fline;
    return;
  }

  /** Return sample error in solution */
  inline double SampleError() const { return (_startSamp - _finalSamp);  }

  /** Return line error in solution */
  inline double LineError() const { return (_startLine - _finalLine);   }

  /** Return the pixel offset error */
  inline double ErrorMagnitude() const {
    return (std::sqrt(SampleError()*SampleError() + LineError()*LineError()));
  }

  /** Sample shift error as determined by Jacobi solution */
  inline double SampleUncertainty() const { return (skmat[0][0]); }
  /** Line shift error as determined by Jacobi solution */
  inline double LineUncertainty() const { return (skmat[1][1]); }
  /** Magnitude of pixel shift error from Jacobi solution */
  inline double Uncertainty() const {
    double sUnc = SampleUncertainty() * SampleUncertainty();
    double lUnc = LineUncertainty() * LineUncertainty();
    return (std::sqrt(sUnc + lUnc)); }


  /** Get the final Chip Affine transform for the solution */
  inline Affine getChipTransform() const { return (_chipTransform); }

  /** Set the final Chip Affine tranform found in the solution */
  void setChipTransform(const Affine &affine) { _chipTransform = affine; }


  /**
   * @brief Accumulation of Gruen adaptive interations results 
   *  
   * The Gruen algorithm iterates over a set of Chips to determine the best 
   * fitting adaptive solution.  At the end of each interation, the solution 
   * must be accumlated to comprise the complete result.  This method makes the 
   * necessary updates to a complete result from a single iteration. 
   * 
   * 
   * @param result Single iteration of a Gruen result
   */
  void update(const GruenResult &result) {
    isGood = result.isGood;
    gerrno = result.gerrno;
    gerrmsg = result.gerrmsg;
    nIters = result.nIters;
    npts = result.npts;
    resid  =  result.resid;
    ata    = result.ata;
    atl    = result.atl;
    kmat  = result.kmat;
    skmat = result.skmat;
    alpha  += result.alpha;
    eigen  = result.eigen;
    return;
  }

  /**
   * @brief Sets final error analysis once the solution converges 
   *  
   * This method makes the appropriate updates to an accumlated Gruen result. 
   * This step is typically recording the error analysis (which is actually done 
   * at each iteration).  These results are used to further apply constraints in 
   * the determination of a "good" Gruen solution. 
   * 
   * 
   * @param result Final iterative Gruen result with error analysis data
   */
  void setErrorAnalysis(const GruenResult &result) {
    isGood = result.isGood;
    gerrno = result.gerrno;
    gerrmsg = result.gerrmsg;
    nIters = result.nIters;
    npts = result.npts;
    resid  =  result.resid;
    ata    = result.ata;
    atl    = result.atl;
    kmat  = result.kmat;
    skmat = result.skmat;
    eigen  = result.eigen;
  }

  //  Intermediate solution variables
  bool        isGood;
  int         gerrno;
  std::string gerrmsg;

  int         nIters;
  BigInt      npts;
  double      resid;

  // Gruen Match algorithm results
  GSLMatrix   ata;
  GSLVector   atl;
  GSLMatrix   kmat;
  GSLMatrix   skmat;
  GSLVector   alpha;
  GSLVector   eigen;   //!< sevals in original code

  Affine     _chipTransform;  //!< Final chip transfrom
  double     _startLine, _startSamp;
  double     _finalLine, _finalSamp;

  /**
   * @brief Initialization of this structure
   * 
   * This sets up variables used in the Gruen algorithm.  It is expected that
   * these parameters be created for immediate use directly in the Gruen class
   * so proper sizes and initial values are carefully maintained.
   */
  void init() {
    isGood = false;
    gerrno = 0;
    gerrmsg = "none";
    nIters = 0;
    npts = 0;
    resid = 0.0;
    ata =  GSLMatrix(NCONSTR, NCONSTR, 0.0);
    atl = GSLVector(NCONSTR, 0.0);
    kmat = GSLMatrix(NCONSTR, NCONSTR, 0.0);
    skmat = GSLMatrix(2, 2, 0.0);
    alpha = GSLVector(NCONSTR, 0.0);
    eigen = GSLVector(2, 0.0);
     
    _chipTransform = Affine();
    _startLine = _startSamp = 0.0;
    _finalLine = _finalSamp = 0.0;

    return;
  }
};

} // namespace Isis
#endif
