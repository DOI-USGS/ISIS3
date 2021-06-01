#ifndef ProcessBySample_h
#define ProcessBySample_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Buffer.h"
#include "ProcessByBrick.h"

namespace Isis {
  /**
   * @brief Process cubes by sample
   *
   * This class allows a programmer to develop a program which process input and
   * output cubes sequentially by sample. That is, receive the input data for
   * sample one, manipulate the data, and pass back the data for output sample
   * one. Then proceed to the sample two and so one. This class is derived from
   * the Process class which give many functions for setting up input and output
   * cubes.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author  2006-03-27 Jacob Danton
   *
   * @internal
   *   @history 2006-03-27 Jacob Danton - Original Version
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *                           Added some documentation to methods.
   *   @history 2012-02-22 Steven Lambright - Updated to have functorized and
   *                           threaded StartProcess equivalents.
   *   @history 2017-02-17 JP Bonn - Formatting changes, removed commented out
   *                           code.
   */
  class ProcessBySample : public Isis::ProcessByBrick {
    public:
      ProcessBySample(): ProcessByBrick() {
        SetWrap(true);
      };

      using Isis::ProcessByBrick::SetInputCube;
      Isis::Cube *SetInputCube(const QString &parameter,
                               int requirements = 0);
      Isis::Cube *SetInputCube(const QString &file,
                               Isis::CubeAttributeInput &att,
                               int requirements = 0);

      void StartProcess(void funct(Isis::Buffer &inout));
      void StartProcess(void funct(Isis::Buffer &in, Isis::Buffer &out));
      void StartProcess(void funct(std::vector<Isis::Buffer *> &in,
                                   std::vector<Isis::Buffer *> &out));

      /**
       * @see ProcessByBrick::ProcessCubeInPlace()
       * @param funct The processing function or functor which does your
       *              desired calculations.
       * @param threaded True if multi-threading is supported, false otherwise.
       */
      template <typename Functor>
      void ProcessCubeInPlace(const Functor & funct, bool threaded = true) {
        VerifyCubes(InPlace);
        SetBricks(InPlace);
        //SetBrickSizesForProcessCubeInPlace();
        ProcessByBrick::ProcessCubeInPlace(funct, threaded);
      }


      /**
       * @see ProcessByBrick::ProcessCube()
       * @param funct The processing function or functor which does your
       *              desired calculations.
       * @param threaded True if multi-threading is supported, false otherwise.
       */
      template <typename Functor>
      void ProcessCube(const Functor & funct, bool threaded = true) {
        VerifyCubes(InputOutput);
        SetBricks(InputOutput);
        ProcessByBrick::ProcessCube(funct, threaded);
      }


      /**
       * @see ProcessByBrick::ProcessCubes()
       * @param funct The processing function or functor which does your
       *              desired calculations.
       * @param threaded True if multi-threading is supported, false otherwise.
       */
      template <typename Functor>
      void ProcessCubes(const Functor & funct, bool threaded = true) {
        VerifyCubes(InputOutputList);
        SetBricks(InputOutputList);
        ProcessByBrick::ProcessCubes(funct, threaded);
      }

    private:
      void SetBricks(IOCubes cn);
  };
};

#endif
