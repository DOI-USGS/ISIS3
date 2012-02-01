#ifndef _REDUCE_H_
#define _REDUCE_H_

#include "Buffer.h"
#include "Cube.h"
#include "iString.h"
#include "Portal.h"

#include <cmath>

using namespace std;

namespace Isis {
  /**
   * @brief Reduce the pixel dimensions of an image
   *
   * Reduce the pixel dimensions of an image. Has the functionality to reduce 
   * only a sub area in input image to output 
   *
   * @ingroup Geometry
   *
   * @author 1995-11-06 Jeff Anderson
   *
   * @internal
   *   @history 2011-04-15 Sharmila Prasad - Ported the class from "reduce"
   *                           app to base object class.
   *   @history 2011-05-10 Sharmila Prasad - Fixed error while setting input
   *                           bands and moved static members to data members
   *                           in Average class.
   *   @history 2011-05-11 Sharmila Prasad - Use Portal instead of LineMgr to
   *                           read Line & added API setInputBoundary to
   *                           reduce subarea of an image
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *   @history 2012-02-01 Sharmila Prasad - Remove unused band data member
   *                           Fixes #00681
   */
  class Reduce {
  public:
    //! Constructor
    Reduce(Isis::Cube *pInCube,const double sampleScale, const double lineScale);
    
    //! Destructor
    ~Reduce();
    
    //! Create label for the reduced output image
    Isis::PvlGroup  UpdateOutputLabel(Isis::Cube *pOutCube);
    
    //! Parameters to input image sub area
    void setInputBoundary(int startSample, int endSample, 
                          int startLine, int endLine);
    
    protected:
      Isis::Cube *mInCube;        //!< Input image
      double mdSampleScale;       //!< Sample scale
      double mdLineScale;         //!< Line scale
      int miStartSample;          //!< Input start sample
      int miEndSample;            //!< Input end sample
      int miStartLine;            //!< Input start line
      int miEndLine;              //!< Input end line
      double mdLine;              //!< Line index
      int miOutputSamples;        //!< Output Samples
      int miOutputLines;          //!< Output Lines
      int miInputSamples;         //!< Input Samples
      int miInputLines;           //!< Input Lines
      int miInputBands;           //!< Input Bands
      int miBandIndex;            //!< Band Index
      Isis::Portal *m_iPortal;    //!< Input portal
  };


  /**
   * Functor for reduce using near functionality
   *
   * @author 2011-04-15 Sharmila Prasad
   *
   * @internal
   */
  class Nearest : public Isis::Reduce {
    public:
      //! Constructor
      Nearest(Isis::Cube *pInCube, double pdSampleScale, double pdLineScale) 
      :Reduce(pInCube, pdSampleScale, pdLineScale){
      }

      //! Operator () overload 
      void operator() (Isis::Buffer & out);
  };


  /**
   * Functor for reduce using average functionality
   *
   * @author 2011-04-15 Sharmila Prasad
   *
   * @internal
   */
  class Average : public Isis::Reduce {
    public:
      //! Constructor
      Average(Isis::Cube *pInCube, double pdSampleScale, double pdLineScale, 
              double pdValidPer, string psReplaceMode) 
      : Reduce(pInCube, pdSampleScale, pdLineScale){
        mdValidPer    = pdValidPer;
        msReplaceMode = psReplaceMode;
      }

      //! Operator () overload
      void operator() (Isis::Buffer & out);

    private:
      double mdValidPer;   //!< Valid Percentage
      string msReplaceMode;//!< Replace Mode (scale/total)
      double *mdIncTab;
      double *mdSum;
      double *mdNpts;
      double *mdSum2;
      double *mdNpts2;
  };
  
};

#endif
