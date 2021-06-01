#ifndef ProcessByLine_h
#define ProcessByLine_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessByBrick.h"
#include "Buffer.h"
namespace Isis {
  class Cube;
  class CubeAttributeInput;

  /**
   * @brief Process cubes by line
   *
   * This class allows a programmer to develop a program which process input and
   * output cubes sequentially by line. That is, receive the input data for line
   * one, manipulate the data, and pass back the data for output line one. Then
   * proceed to the line two and so one. This class is derived from the Process
   * class which give many functions for setting up input and output cubes.
   *
   * Here is an example of how to use ProcessByLine
   * @code
   *  using namespace std;
   *  #include "Isis.h"
   *  #include "ProcessByLine.h"
   *  #include "SpecialPixel.h"
   *  void mirror (Buffer &in, Buffer &out);
   *  void IsisMain() {
   *    // We will be processing by line
   *    ProcessByLine p; // Setup the input and output cubes
   *    p.SetInputCube("FROM");
   *    p.SetOutputCube ("TO");
   *    // Start the processing
   *    p.StartProcess(mirror);
   *    p.EndProcess();
   *  } .
   *  // Line processing routine
   *  void mirror (Buffer &in, Buffer &out) {
   *    // Loop and flip pixels in the line.
   *    int index = in.size() - 1;
   *    for (int i=0; i<in.size(); i++) {
   *      out[i] = in[index - i];
   *    }
   *  }
   * @endcode
   * If you would like to see ProcessByLine being used in implementation with
   * multiple input cubes, see ratio.cpp
   *
   * @ingroup HighLevelCubeIO
   *
   * @author  2003-02-13 Jeff Anderson
   *
   * @internal
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-08-29 Jeff Anderson - Fixed bug in StartProcess method for
   *                           multiple inputs/outputs which require cubes to have
   *                           the same number of lines and samples.
   *   @history 2004-07-15 Jeff Anderson - Modified to allow for a different
   *                           number of samples in the output cube(s) than are
   *                           in the input cube(s). This facilitates the ability
   *                           to scale or concatenate in the sample direction.
   *   @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2011-04-22 Sharmila Prasad - Extended StartProcess functionality
   *                           to be able to be called from any Object class by
   *                           using Functors
   *   @history 2011-05-07 Sharmila Prasad - 1. Added API SetInputCube(Cube*) to
   *                           take opened cube 2. Edited StartProcess using
   *                           Functors take reference to Functors
   *   @history 2006-03-29 Jacob Danton Rewrote code to extend ProcessByBrick
   *                           class.
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *   @history 2015-09-25 Tyler Wilson  - Combined SetBrickSizesForProcessCube,
   *                           SetBrickSizesForProcessCubeInPlace,
   *                           and SetBrickSizesForProcessCubes
   *                           into one
   *                           function:SetBricks(CubeNum) which
   *                           takes in the enumerated date type
   *                           enum as an argument (CubeNum =
   *                           {InPlace, InputOutput,
   *                           InputOutputList}). Also moved the
   *                           verification of cubes out of the
   *                           SetBrick functions and into
   *                           ProcessByBrick::
   *                           VerifyCubes(CubeNum) in parent
   *                           class.
   *   @history 2017-04-13 Kaj Williams - Fixed a minor typo in
   *                           the API documentation. Fixed a few
   *                           source code formatting issues.
   */
  class ProcessByLine : public Isis::ProcessByBrick {

    public:
      ProcessByLine(): ProcessByBrick() {
        SetWrap(true);
      };

      using Isis::ProcessByBrick::SetInputCube; // make parents SetInputCube visable
      Isis::Cube *SetInputCube(const QString &parameter,
                               const int requirements = 0);
      Isis::Cube *SetInputCube(const QString &file,
                               Isis::CubeAttributeInput &att,
                               const int requirements = 0);

      void StartProcess(void funct(Isis::Buffer &inout));
      virtual void StartProcess(std::function<void(Isis::Buffer &in)> funct );
      void StartProcess(std::function<void(Buffer &in, Buffer &out)> funct);

      void StartProcess(void funct(Isis::Buffer &in, Isis::Buffer &out));
      void StartProcess(void
                        funct(std::vector<Isis::Buffer *> &in,
                              std::vector<Isis::Buffer *> &out));

      void SetBricks(IOCubes cn);

      /**
       * Same functionality as StartProcess(void funct(Isis::Buffer &inout))
       * using Functors. The Functor operator(), takes the parameter
       * (Isis::Buffer &)
       *
       * @author 2011-04-22 Sharmila Prasad
       *
       * @param funct - Functor with overloaded operator()(Isis::Buffer &)
       * @param threaded @see ProcessByBrick::ProcessCubeInPlace()
       */
      template <typename Functor>
      void ProcessCubeInPlace(const Functor & funct, bool threaded = true) {
        VerifyCubes(InPlace);
        SetBricks(InPlace);
        ProcessByBrick::ProcessCubeInPlace(funct, threaded);
      }


      /**
       * Same functionality as
       * StartProcess(void funct(Isis::Buffer &in, Isis::Buffer &out)) using
       * Functors. The Functor operator(), takes parameters
       * (Isis::Buffer &, Isis::Buffer &)
       *
       * @author 2011-04-22 Sharmila Prasad
       *
       * @param funct - Functor with overloaded
       *                operator()(Isis::Buffer &, Isis::Buffer &)
       * @param threaded @see ProcessByBrick::ProcessCube()
       */
      template <typename Functor>
      void ProcessCube(const Functor & funct, bool threaded = true) {
        VerifyCubes(InputOutput);
        SetBricks(InputOutput);
        ProcessByBrick::ProcessCube(funct, threaded);
      }


      /**
       * Same functionality as StartProcess(std::vector<Isis::Buffer *> &in,
       * std::vector<Isis::Buffer *> &out) using Functors. The Functor operator(),
       * takes parameters (std::vector<Isis::Buffer *> &,
       * std::vector<Isis::Buffer *> &)
       *
       * @author 2011-04-22 Sharmila Prasad
       *
       * @param funct - Functor with overloaded operator()
       *              (Istd::vector<Isis::Buffer *> &,
       *                std::vector<Isis::Buffer *> &)
       * @param threaded @see ProcessByBrick::ProcessCubes()
       */
      template <typename Functor>
      void ProcessCubes(const Functor & funct, bool threaded = true) {
        VerifyCubes(InputOutputList);
        SetBricks(InputOutputList);
        ProcessByBrick::ProcessCubes(funct, threaded);
      }
  };
};

#endif
