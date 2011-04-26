#ifndef _REDUCE_H_
#define _REDUCE_H_

#include "Buffer.h"
#include "Cube.h"
#include "iString.h"
#include "LineManager.h"

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
   *  @history 2011-04-15 Sharmila Prasad - Ported the class from "reduce" app
   *           to base object class with added functionality to reduce only the
   *           sub area within the input image into the output image
   */
  class Reduce {
  public:
    //! Constructor
    Reduce(Isis::Cube *pInCube, vector<string>psBands, const double sampleScale, const double lineScale);
    
    //! Destructor
    ~Reduce(){};
    
    //! Create label for the reduced output image
    Isis::PvlGroup  UpdateOutputLabel(Isis::Cube *pOutCube);
    
    protected:
      Isis::Cube *mInCube; //!< Input image
      double mdSampleScale;//!< Sample scale
      double mdLineScale;  //!< Line scale
      double mdStartSample;//!< Input start sample
      double mdEndSample;  //!< Input end sample
      double mdStartLine;  //!< Input start line
      double mdEndLine;    //!< Input end line      
      double mdLine;       //!< Line index
      int miOutputSamples; //!< Output Samples
      int miOutputLines;   //!< Output Lines
      int miInputSamples;  //!< Input Samples
      int miInputLines;    //!< Input Lines
      int miInputBands;    //!< Input Bands
      int miBandIndex;     //!< Band Index
      vector<string>msBands;      //!< Bands list
      Isis::LineManager *mLineMgr;//!< Line Manager
  };
  
  // Functor for reduce using near functionality
  class Nearest : public Isis::Reduce {
    public:
      //! Constructor
      Nearest(Isis::Cube *pInCube, vector<string> psBands, 
              double pdSampleScale, double pdLineScale) 
      :Reduce(pInCube, psBands, pdSampleScale, pdLineScale){
      }
      
      //! Operator () overload 
      void operator() (Isis::Buffer & out);
  };
  
  // Functor for reduce using average functionality
  class Average : public Isis::Reduce {
    public:
      //! Constructor
      Average(Isis::Cube *pInCube, vector<string> psBands, double pdSampleScale,
              double pdLineScale, double pdValidPer, string psReplaceMode) 
      : Reduce(pInCube, psBands, pdSampleScale, pdLineScale){
        mdValidPer    = pdValidPer;
        msReplaceMode = psReplaceMode;
      }

      //! Operator () overload
      void operator() (Isis::Buffer & out);

    private:
      double mdValidPer;   //!< Valid Percentage
      string msReplaceMode;//!< Replace Mode (scale/total)
  };
  
};

#endif
