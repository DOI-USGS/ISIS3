/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessBySample.h"

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
   */
  Isis::Cube *ProcessBySample::SetInputCube(const QString &parameter,
                                            int requirements) {
    int allRequirements = Isis::SpatialMatch | Isis::BandMatchOrOne;
    allRequirements |= requirements;
    return Process::SetInputCube(parameter, allRequirements);
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
  Isis::Cube *ProcessBySample::SetInputCube(const QString &file,
                                            Isis::CubeAttributeInput &att,
                                            int requirements) {
    int allRequirements = Isis::SpatialMatch | Isis::BandMatchOrOne;
    allRequirements |= requirements;
    return Process::SetInputCube(file, att, allRequirements);
  }


  /**
   * This method invokes the process by sample operation over a single input or
   * output cube. It will be an input cube if the method SetInputCube was
   * invoked exactly one time before calling StartProcess. It will be an output
   * cube if the SetOutputCube method was invoked exactly one time. Typically
   * this method can be used to obtain statistics, histograms, or other
   * information from an input cube.
   *
   * @deprecated Please use ProcessCubeInPlace()
   * @param funct (Isis::Buffer &b) Name of your processing function
   */
  void ProcessBySample::StartProcess(void funct(Isis::Buffer &inout)) {
    VerifyCubes(InPlace);
    SetBricks(InPlace);
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by sample operation over exactly one input and
   * one output cube. Typically, this method is used for simple operations such
   * as stretching a cube or applying various operators to a cube (add constant,
   * multiply by constant, etc).
   *
   * @deprecated Please use ProcessCube()
   * @param funct (Isis::Buffer &in, Isis::Buffer &out) Name of your processing
   *                                                    function
   */ 
  void ProcessBySample::StartProcess(void funct(Isis::Buffer &in,
                                                Isis::Buffer &out)) {
    VerifyCubes(InputOutput);
    SetBricks(InputOutput);
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by sample operation over multiple input and
   * output cubes. Typically, this method is used when two input cubes are
   * required for operations like ratios, differences, masking, etc.
   *
   * @deprecated Please use ProcessCubes()
   * @param funct (vector<Isis::Buffer *> &in, vector<Isis::Buffer *> &out) Name
   *                of your processing function
   */
  void ProcessBySample::StartProcess(void funct(std::vector<Isis::Buffer *> &in,
                                                std::vector<Isis::Buffer *> &out)) {
    VerifyCubes(InputOutputList) ;
    SetBricks(InputOutputList);
    ProcessByBrick::StartProcess(funct);
  }


  void ProcessBySample::SetBricks(IOCubes cn){
    switch(cn){
      case InPlace:
        if (InputCubes.size() == 1) {
          SetBrickSize(1, InputCubes[0]->lineCount(), 1);
        }
        else {
          SetBrickSize(1, OutputCubes[0]->lineCount(), 1);
        }
        break;

      case InputOutput:
        SetInputBrickSize(1, InputCubes[0]->lineCount(), 1);
        SetOutputBrickSize(1, OutputCubes[0]->lineCount(), 1);
        break;

      case InputOutputList:
        for(unsigned int i = 0; i < InputCubes.size(); i++) {
          SetInputBrickSize(1, InputCubes[i]->lineCount(), 1, i + 1);
        }
        for(unsigned int i = 0; i < OutputCubes.size(); i++) {
          SetOutputBrickSize(1, OutputCubes[i]->lineCount(), 1, i + 1);
        }
        break;
    }
  }
}
