#ifndef _Enlarge_h_
#define _Enlarge_h_

#include "Cube.h"
#include "Transform.h"

namespace Isis {
  /**
   * @brief Enlarge the pixel dimensions of an image
   *
   * Enlarge the pixel dimensions of an image. Has the functionality to enlarge 
   * only a sub area in input image to output 
   *
   * @ingroup Geometry
   *
   * @author 2002-12-13 Stuart Sides
   *
   * @internal
   *  @history 2011-04-14 Sharmila Prasad - Ported the class from "enlarge" app
   *           to base object class with added functionality to enlarge only the
   *           sub area within the input image into the output image
   */
  class Enlarge : public Transform {
    public:
      //! Constructor
      Enlarge(Isis::Cube *pInCube, const double sampleScale, const double lineScale);
    
      //! Set the sub area of input image to enlarge
      void SetInputArea(double pdStartSample, double pdEndSample, 
                        double pdStartLine, double pdEndLine);

      //! Destructor
      ~Enlarge() {};

      //! Implementations for parent's pure virtual members
      //! Convert the requested output samp/line to an input samp/line
      bool Xform(double &inSample, double &inLine,
                 const double outSample, const double outLine);
      
      //! Create label for the enlarged output image
      Isis::PvlGroup  UpdateOutputLabel(Isis::Cube *pOutCube);

      //! Return the output number of samples
      int OutputSamples() const {
        return miOutputSamples;
      }

      //! Return the output number of lines
      int OutputLines() const {
        return miOutputLines;
      }
    
      private:
        Isis::Cube *mInCube; //!< Input image
        int miOutputSamples; //!< Number of samples for output
        int miOutputLines;   //!< Number of lines for output
        double mdSampleScale;//!< Sample scale
        double mdLineScale;  //!< Line scale
        double mdStartSample;//!< Input start sample
        double mdEndSample;  //!< Input end sample
        double mdStartLine;  //!< Input start line
        double mdEndLine;    //!< Input end line
  };
};

#endif

