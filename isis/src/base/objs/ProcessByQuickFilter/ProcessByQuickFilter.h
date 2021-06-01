#ifndef FilterProcess_h
#define FilterProcess_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Process.h"
#include "Buffer.h"
#include "QuickFilter.h"

namespace Isis {
  /**
   * @brief Process cubes using a Filter Object
   *
   * This class processes an entire cube using an Filter object. That is, it
   * walks a Filter object line-by-line over an input cube. This allows for the
   * development of programs which do spatial filters such as highpass, lowpass,
   * and sharpen. Understanding the Filter class is essential in order to utilize
   * this class. This class expects the user to define an NxM boxcar size. Using
   * that information, a Filter object is created and loaded with the proper cube
   * data in order to walk the NxM boxcar through the entire cube in a very
   * efficient manner. Currently it is required that the following parameters be
   * available in the application XML file:
   *   LINES - Defines the height of the boxcar to convolve over the cube
   *   SAMPLES - Defines the width of the boxcar to convoled over the cube
   *   MINIMUM - Defines the minimum number of pixels in the boxcar in order for
   *             statistics to be computed (see Filter class)
   *   LOW - Defines minimum valid pixel value to be included in statistics
   *         (see Filter class)
   *   HIGH - Defines maximum valid pixel value to be included in statistics
   *          (see Filter class)
   *
   * If you would like to see ProcessByQuickFilter being used in implementation,
   * see sharpen.cpp
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-03-31 Jeff Anderson
   *
   * @internal
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-06-02 Jeff Anderson - Fixed a bug where line unfolding at
   *                           the bottom of the cube was always using band 1
   *   @history 2003-08-28 Jeff Anderson - Added SetFilterParameters method
   *   @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2006-12-15 Jeff Anderson - Fixed bug for images with 1 line
   *   @history 2011-06-27 Jai Rideout and Steven Lambright - Now uses
   *                           FilterCachingAlgorithm
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *   @history 2012-02-24 Steven Lambright - Added ProcessCube()
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to StartProcess
   *                                            function to ensure successful
   *                                            inheritance between Process and its
   *                                            child classes.  References #2215.
   */
  class ProcessByQuickFilter : public Isis::Process {

    public:
      ProcessByQuickFilter();

      using Isis::Process::StartProcess; // make parent functions visable
      virtual void StartProcess(void funct(Isis::Buffer &in, Isis::Buffer &out,
                                   Isis::QuickFilter &filter));
      void ProcessCube(void funct(Isis::Buffer &in, Isis::Buffer &out,
                                  Isis::QuickFilter &filter)) {
        StartProcess(funct);
      }
      void SetFilterParameters(int samples, int lines,
                               double low = -DBL_MAX, double high = DBL_MAX,
                               int minimum = 0);

    private:
      bool p_getParametersFromUser; /**<Flag to indicate whether or not to get
                                        parameters from the user*/
      int p_boxcarSamples;          /**<Number of samples in the boxcar.
                                        Must be odd.*/
      int p_boxcarLines;            /**<Number of lines in the boxcar.
                                        Must be odd.*/
      int p_minimum;                /**<Minimum number of valid pixels in the
                                        sample-by-line boxcar in order for
                                        statistical computations to be valid.
                                        Defaults to 0 */
      double p_low;                 /**<Minimum valid pixel value to include in
                                        statistical computations of the boxcar.
                                        Defaults to DBL_MAX */
      double p_high;                /**<Maximum valid pixel value to include in
                                        statistical computations of the boxcar.
                                         Defaults to DBL_MAX */

      void GetFilterParameters();
  };
};

#endif
