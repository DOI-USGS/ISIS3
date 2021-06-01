#ifndef _Enlarge_h_
#define _Enlarge_h_
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Transform.h"

namespace Isis {
  class Cube;
  class PvlGroup;
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
   *   @history 2011-04-14 Sharmila Prasad - Ported the class from "enlarge" app
   *                           to base object class with added functionality to
   *                           enlarge only the sub area within the input image
   *                           into the output image
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   */
  class Enlarge : public Transform {
    public:
      // Constructor
      Enlarge(Cube *pInCube, const double sampleScale, const double lineScale);
    
      // Set the sub area of input image to enlarge
      void SetInputArea(double pdStartSample, double pdEndSample, 
                        double pdStartLine, double pdEndLine);

      //! Destructoys the Enlarge object.
      ~Enlarge() {};

      // Implementations for parent's pure virtual members
      // Convert the requested output samp/line to an input samp/line
      bool Xform(double &inSample, double &inLine,
                 const double outSample, const double outLine);
      
      // Create label for the enlarged output image
      PvlGroup  UpdateOutputLabel(Cube *pOutCube);

      /**
       * Return the output number of samples 
       * @return @b int - Number of samples in output 
       */
      int OutputSamples() const {
        return miOutputSamples;
      }

      /**
       * Return the output number of lines 
       * @return @b int - Number of lines in output
       */      
      int OutputLines() const {
        return miOutputLines;
      }
    
      private:
        Cube *mInCube;        //!< Input image
        int miOutputSamples;  //!< Number of samples for output
        int miOutputLines;    //!< Number of lines for output
        double mdSampleScale; //!< Sample scale
        double mdLineScale;   //!< Line scale
        double mdStartSample; //!< Input start sample
        double mdEndSample;   //!< Input end sample
        double mdStartLine;   //!< Input start line
        double mdEndLine;     //!< Input end line
  };
};

#endif

