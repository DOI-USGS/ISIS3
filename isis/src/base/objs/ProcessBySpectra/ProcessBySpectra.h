#ifndef ProcessBySpectra_h
#define ProcessBySpectra_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessByBrick.h"
#include "Buffer.h"

namespace Isis {
  /**
   * @brief Process cubes by spectra
   *
   * This class allows a programmer to develop a program which process input and
   * output cubes sequentially by spectra. That is, receive the input data for
   * spectra one, manipulate the data, and pass back the data for output spectra
   * one. Then proceed to the spectra two and so one. This class is derived from
   * the ProcessByBrick class which give many functions for setting up input and
   * output cubes.
   *
   *
   * @ingroup HighLevelCubeIO
   *
   * @author  2006-03-27 Jacob Danton
   *
   * @internal
   *   @history 2006-03-27 Jacob Danton - Original Version
   *   @history 2006-08-07 Tracie Sucharski, Fixed bug in StartProcess with
   *                           a single input buffer.  Error checks and set-up
   *                           of brick was not being done correctly.
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                            $temporary variable instead of /tmp directory.
   *                            Added some documentation to methods.
   *
   */
  class ProcessBySpectra : public Isis::ProcessByBrick {
    private:
      int p_spectraType; /**< Spectra type: valid values are 0 (PerPixel),
                              1 (ByLine), or 2 (BySample)*/

    public:
      /**
       * Constructs ProcessBySpectra object using specified spectra type.
       * <UL>
       *   <LI> 0 = PerPixel
       *   <LI> 1 = ByLine
       *   <LI> 2 = BySample
       * </UL>
       *
       * @param type Spectra type to be given the object. Defaults to PerPixel
       *             (0)
       */
      ProcessBySpectra(const int type = PerPixel): ProcessByBrick() {
        SetType(type);
      };

      using Isis::ProcessByBrick::SetInputCube; // Make parent functions visable
      Isis::Cube *SetInputCube(const QString &parameter,
                               const int requirements = 0);
      Isis::Cube *SetInputCube(const QString &file,
                               Isis::CubeAttributeInput &att,
                               const int requirements = 0);

      void SetType(const int type);

      /**
       * Returns the spectra type:
       * <UL>
       *   <LI> 0 = PerPixel
       *   <LI> 1 = ByLine
       *   <LI> 2 = BySample
       * </UL>
       * @return @b int Integer value representing the spectra type.
       */
      int Type() {
        return p_spectraType;
      };

      void StartProcess(void funct(Isis::Buffer &in));

      void StartProcess(void funct(Isis::Buffer &in, Isis::Buffer &out));

      void StartProcess(void funct(std::vector<Isis::Buffer *> &in,
                                   std::vector<Isis::Buffer *> &out));


      /**
       * @see ProcessByBrick::ProcessCubeInPlace()
       * @param funct
       * @param threaded
       */
      template <typename Functor>
      void ProcessCubeInPlace(const Functor & funct, bool threaded = true) {
        //SetBrickSizesForProcessCubeInPlace();
          VerifyCubes(InPlace);
          SetBricks(InPlace);
        ProcessByBrick::ProcessCubeInPlace(funct, threaded);
      }

      /**
       * @see ProcessByBrick::ProcessCube()
       * @param funct
       * @param threaded
       */
      template <typename Functor>
      void ProcessCube(const Functor & funct, bool threaded = true) {
        //SetBrickSizesForProcessCube();
          VerifyCubes(InputOutput);
          SetBricks(InputOutput);
          ProcessByBrick::ProcessCube(funct, threaded);
      }

      /**
       * @see ProcessByBrick::ProcessCubes()
       * @param funct
       * @param threaded
       */
      template <typename Functor>
      void ProcessCubes(const Functor & funct, bool threaded = true) {
          VerifyCubes(InputOutputList);
          SetBricks(InputOutputList);
        //SetBrickSizesForProcessCubes();
        ProcessByBrick::ProcessCubes(funct, threaded);
      }

      static const int PerPixel = 0; //!< PerPixel spectra type (equal to 0)
      static const int ByLine = 1;   //!< ByLine spectra type (equal to 1)
      static const int BySample = 2; //!< BySample spectra type (equal to 2)

    private:

      void SetBricks(IOCubes cn);
      void SetBrickSizesForProcessCubeInPlace();
      void SetBrickSizesForProcessCube();
      void SetBrickSizesForProcessCubes();
  };
};

#endif
