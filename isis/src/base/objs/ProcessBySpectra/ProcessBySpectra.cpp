/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ProcessBySpectra.h"

#include "Buffer.h"
#include "Cube.h"
#include "IException.h"
#include "Process.h"
#include "ProcessByBrick.h"

using namespace std;
namespace Isis {

/**
   * Opens an input cube specified by the user and verifies requirements are
   * met. This method is overloaded and adds the requirements of
   * ic_base::SpatialMatch which requires all input cubes to have the same
   * number of samples and lines. It also added the requirement
   * ic_base::BandMatchOrOne which forces 2nd, 3rd, 4th, etc input cubes to
   * match the number of bands in the 1st input cube or to have exactly one
   * band. For more information see Process::SetInputCube
   *
   * @return Cube*
   *
   * @param parameter User parameter to obtain file to open. Typically, the
   *                  value is "FROM". For example, the user can specify on the
   *                  command line FROM=myfile.cub and this method will attempt
   *                  to open the cube "myfile.cub" if the parameter was set to
   *                  "FROM".
   *
   * @param requirements See Process::SetInputCube for more information.
   *                     Defaults to 0
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *ProcessBySpectra::SetInputCube(const QString &parameter,
      const int requirements) {
    return ProcessByBrick::SetInputCube(parameter,
                                        Isis::SpatialMatch | requirements);
  }


  /**
   * Opens an input cube specified by the user with cube attributes and
   * requirements. For more information see Process::SetInputCube
   *
   * @return Cube*
   *
   * @param file Name of cube file
   * @param att Cube attributes
   * @param requirements See Process::SetInputCube for more information.
   *                     Defaults to 0
   *
   */
  Isis::Cube *ProcessBySpectra::SetInputCube(const QString &file,
      Isis::CubeAttributeInput &att,
      const int requirements) {
    return ProcessByBrick::SetInputCube(file, att,
                                        Isis::SpatialMatch | requirements);
  }


  /**
   * This method invokes the process by spectra operation over a single input or
   * output cube. It will be an input cube if the method SetInputCube was
   * invoked exactly one time before calling StartProcess. It will be an output
   * cube if the SetOutputCube method was invoked exactly one time. Typically
   * this method can be used to obtain statistics, histograms, or other
   * information from an input cube.
   *
   * @deprecated Please use ProcessCubeInPlace()
   * @param funct (Isis::Buffer &b) Name of your processing function
   *
   * @throws Isis::iException::Message
   */
  void ProcessBySpectra::StartProcess(void funct(Isis::Buffer &in)) {


    VerifyCubes(InPlace);
    SetBricks(InPlace);
    //SetBrickSizesForProcessCubeInPlace();
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by spectra operation over exactly one input
   * and one output cube. Typically, this method is used for simple operations
   * such as stretching a cube or applying various operators to a cube (add
   * constant, multiply by constant, etc).
   *
   * @deprecated Please use ProcessCube()
   * @param funct (Isis::Buffer &in, Isis::Buffer &out) Name of your processing
   *                                                    function
   *
   * @throws Isis::iException::Message
   */
  void ProcessBySpectra::StartProcess(void
                                      funct(Isis::Buffer &in,
                                            Isis::Buffer &out)) {

      VerifyCubes(InputOutput);
      SetBricks(InputOutput);
    //SetBrickSizesForProcessCube();
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by spectra operation over multiple input
   * and output cubes. Typically, this method is used when two input cubes are
   * required for operations like ratios, differences, masking, etc.
   *
   * @deprecated Please use ProcessCubes()
   * @param funct (vector<Isis::Buffer *> &in, vector<Isis::Buffer *> &out) Name
   *                of your processing function
   *
   * @throws Isis::iException::Message
   */
  void ProcessBySpectra::StartProcess(void funct(std::vector<Isis::Buffer *> &in,
                                      std::vector<Isis::Buffer *> &out)) {
    //SetBrickSizesForProcessCubes();
      VerifyCubes(InputOutputList);
      SetBricks(InputOutputList);
      ProcessByBrick::StartProcess(funct);
  }


  /**
   * Sets the spectra type to one of the following:
   * <UL>
   *   <LI> 0 = PerPixel
   *   <LI> 1 = ByLine
   *   <LI> 2 = BySample
   * </UL>
   * @param type Integer value representing the spectra type.
   */
  void ProcessBySpectra::SetType(const int type) {
    if (type != PerPixel && type != ByLine && type != BySample) {
      string m = "The specified spectra type is invalid";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else p_spectraType = type;
  }


  void ProcessBySpectra::SetBricks(IOCubes cn){




      switch(cn){

        case InPlace:
            int ns, nl, nb;
            if(InputCubes.size() == 1) {

              ns = InputCubes[0]->sampleCount();
              nl = InputCubes[0]->lineCount();
              nb = InputCubes[0]->bandCount();
            }
            else {
              ns = OutputCubes[0]->sampleCount();
              nl = OutputCubes[0]->lineCount();
              nb = OutputCubes[0]->bandCount();
            }

            if(Type() == PerPixel) SetBrickSize(1, 1, nb);

            else if(Type() == ByLine) SetBrickSize(ns, 1, nb);

            else SetBrickSize(1, nl, nb);

          break;

        case InputOutput:

          if(Type() == PerPixel) {
            SetInputBrickSize(1, 1, InputCubes[0]->bandCount());
            SetOutputBrickSize(1, 1, OutputCubes[0]->bandCount());
          }
          else if(Type() == ByLine) {
            SetInputBrickSize(InputCubes[0]->sampleCount(), 1,
                              InputCubes[0]->bandCount());
            SetOutputBrickSize(OutputCubes[0]->sampleCount(), 1,
                               OutputCubes[0]->bandCount());
          }
          else {
            SetInputBrickSize(1, InputCubes[0]->lineCount(),
                              InputCubes[0]->bandCount());
            SetOutputBrickSize(1, OutputCubes[0]->lineCount(),
                               OutputCubes[0]->bandCount());
          }

          break;


        case InputOutputList:

          if(Type() == PerPixel) {
            for(unsigned int i = 0; i < InputCubes.size(); i++) {
              SetInputBrickSize(1, 1, InputCubes[i]->bandCount(), i + 1);
            }
            for(unsigned int i = 0; i < OutputCubes.size(); i++) {
              SetOutputBrickSize(1, 1, OutputCubes[i]->bandCount(), i + 1);
            }
          }
          else if(Type() == ByLine) {
            for(unsigned int i = 0; i < InputCubes.size(); i++) {
              SetInputBrickSize(InputCubes[i]->sampleCount(), 1,
                                InputCubes[i]->bandCount(), i + 1);
            }
            for(unsigned int i = 0; i < OutputCubes.size(); i++) {
              SetOutputBrickSize(OutputCubes[i]->sampleCount(), 1,
                                 OutputCubes[i]->bandCount(), i + 1);
            }
          }
          else {
            for(unsigned int i = 0; i < InputCubes.size(); i++) {
              SetInputBrickSize(1, InputCubes[i]->lineCount(),
                                InputCubes[i]->bandCount(), i + 1);
            }
            for(unsigned int i = 0; i < OutputCubes.size(); i++) {
              SetOutputBrickSize(1, OutputCubes[i]->lineCount(),
                                 OutputCubes[i]->bandCount(), i + 1);
            }
          }

          break;

        }


  }


  /**
   * This is a helper method for StartProcess() and ProcessCubeInPlace().
   */

  /*
  void ProcessBySpectra::SetBrickSizesForProcessCubeInPlace() {
    // Error checks
    if ((InputCubes.size() + OutputCubes.size()) > 1) {
      string m = "You can only specify exactly one input or output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if ((InputCubes.size() + OutputCubes.size()) == 0) {
      string m = "You haven't specified an input or output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    int ns, nl, nb;
    if (InputCubes.size() == 1) {
      ns = InputCubes[0]->sampleCount();
      nl = InputCubes[0]->lineCount();
      nb = InputCubes[0]->bandCount();
    }
    else {
      ns = OutputCubes[0]->sampleCount();
      nl = OutputCubes[0]->lineCount();
      nb = OutputCubes[0]->bandCount();
    }
    if (Type() == PerPixel) SetBrickSize(1, 1, nb);
    else if (Type() == ByLine) SetBrickSize(ns, 1, nb);
    else SetBrickSize(1, nl, nb);
  }

*/
  /**
   * This is a helper method for StartProcess() and ProcessCube().
   */
  /*
  void ProcessBySpectra::SetBrickSizesForProcessCube() {
    // Error checks ... there must be one input and output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    if (Type() == PerPixel) {
      SetInputBrickSize(1, 1, InputCubes[0]->bandCount());
      SetOutputBrickSize(1, 1, OutputCubes[0]->bandCount());
    }
    else if (Type() == ByLine) {
      SetInputBrickSize(InputCubes[0]->sampleCount(), 1,
                        InputCubes[0]->bandCount());
      SetOutputBrickSize(OutputCubes[0]->sampleCount(), 1,
                         OutputCubes[0]->bandCount());
    }
    else {
      SetInputBrickSize(1, InputCubes[0]->lineCount(),
                        InputCubes[0]->bandCount());
      SetOutputBrickSize(1, OutputCubes[0]->lineCount(),
                         OutputCubes[0]->bandCount());
    }
  }

*/
  /**
   * This is a helper method for StartProcess() and ProcessCubes().
   */
  /*
  void ProcessBySpectra::SetBrickSizesForProcessCubes() {
    if (Type() == PerPixel) {
      for (unsigned int i = 0; i < InputCubes.size(); i++) {
        SetInputBrickSize(1, 1, InputCubes[i]->bandCount(), i + 1);
      }
      for (unsigned int i = 0; i < OutputCubes.size(); i++) {
        SetOutputBrickSize(1, 1, OutputCubes[i]->bandCount(), i + 1);
      }
    }
    else if (Type() == ByLine) {
      for (unsigned int i = 0; i < InputCubes.size(); i++) {
        SetInputBrickSize(InputCubes[i]->sampleCount(), 1,
                          InputCubes[i]->bandCount(), i + 1);
      }
      for (unsigned int i = 0; i < OutputCubes.size(); i++) {
        SetOutputBrickSize(OutputCubes[i]->sampleCount(), 1,
                           OutputCubes[i]->bandCount(), i + 1);
      }
    }
    else {
      for (unsigned int i = 0; i < InputCubes.size(); i++) {
        SetInputBrickSize(1, InputCubes[i]->lineCount(),
                          InputCubes[i]->bandCount(), i + 1);
      }
      for (unsigned int i = 0; i < OutputCubes.size(); i++) {
        SetOutputBrickSize(1, OutputCubes[i]->lineCount(),
                           OutputCubes[i]->bandCount(), i + 1);
      }
    }
  }
  */


}
