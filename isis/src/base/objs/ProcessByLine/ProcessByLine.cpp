/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ProcessByLine.h"

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
  Isis::Cube *ProcessByLine::SetInputCube(const QString &parameter,
                                          const int requirements) {
    int allRequirements = Isis::SpatialMatch | Isis::BandMatchOrOne;
    allRequirements |= requirements;
    return Process::SetInputCube(parameter, allRequirements);
  }


  /**
   * Opens an input cube file specified by the user with cube attributes and
   * requirements. For more information see Process::SetInputCube
   *
   * @return Cube*
   *
   * @param file File name of cube
   *
   * @param att Cube attributes.
   *
   * @param requirements See Process::SetInputCube for more information.
   *                     Defaults to 0
   *
   * @throws Isis::iException::Message
   */
  Isis::Cube *ProcessByLine::SetInputCube(const QString &file,
                                          Isis::CubeAttributeInput &att,
                                          const int requirements) {
    int allRequirements = Isis::SpatialMatch | Isis::BandMatchOrOne;
    allRequirements |= requirements;
    return Process::SetInputCube(file, att, allRequirements);
  }


  /**
   * Set the InputCube vector to an opened Cube. This is used if there already
   * exists a valid opened cube
   *
   * @author 2011-05-07 Sharmila Prasad
   *
   * @param inCube - Pointer to input Cube
   */
  void ProcessByLine::SetInputCube(Isis::Cube *inCube) {
    Process::SetInputCube(inCube);
  }


  void ProcessByLine::SetBricks(IOCubes cn) {

      switch(cn){

        case InPlace:

          if (InputCubes.size() == 1) {
            SetBrickSize(InputCubes[0]->sampleCount(), 1, 1);
          }

          else {
            SetBrickSize(OutputCubes[0]->sampleCount(), 1, 1);
          }

          break;

        case InputOutput:

          SetInputBrickSize(InputCubes[0]->sampleCount(), 1, 1);
          SetOutputBrickSize(OutputCubes[0]->sampleCount(), 1, 1);

          break;

        case InputOutputList:

          for (unsigned int i = 0; i < InputCubes.size(); i++) {
            SetInputBrickSize(InputCubes[i]->sampleCount(), 1, 1, i + 1);
          }
          for (unsigned int i = 0; i < OutputCubes.size(); i++) {
            SetOutputBrickSize(OutputCubes[i]->sampleCount(), 1, 1, i + 1);
          }

          break;
      }
  }



  /**
   * This method invokes the process by line operation over a single input or
   * output cube. It will be an input cube if the method SetInputCube was
   * invoked exactly one time before calling StartProcess. It will be an output
   * cube if the SetOutputCube method was invoked exactly one time. Typically
   * this method can be used to obtain statistics, histograms, or other
   * information from an input cube.
   *
   * @deprecated Please use ProcessCubeInPlace()
   * @param funct (Isis::Buffer &b) Name of your processing function
   *
   * @throws Isis::IException::Message
   *
   * @internal
   *  @history 2005-02-28 Stuart Sides - Modified so cube that are opended
   *                                     ReadWrite will be written. Before
   *                                     only cube opened Write would be
   *                                     written.
   */
  void ProcessByLine::StartProcess(void funct(Isis::Buffer &inout)) {
    VerifyCubes(InPlace);
    SetBricks(InPlace);
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by line operation over a single input or
   * output cube. It will be an input cube if the method SetInputCube was
   * invoked exactly one time before calling StartProcess. It will be an output
   * cube if the SetOutputCube method was invoked exactly one time. Typically
   * this method can be used to obtain statistics, histograms, or other
   * information from an input cube.
   *
   * @deprecated Please use ProcessCubeInPlace()
   * @param funct (Isis::Buffer &b) Name of your processing function
   *
   * @throws Isis::IException::Message
   *
   * @internal
   *  @history 2005-02-28 Stuart Sides - Modified so cube that are opended
   *                                     ReadWrite will be written. Before
   *                                     only cube opened Write would be
   *                                     written.
   */
  void ProcessByLine::StartProcess(std::function<void(Isis::Buffer &in)> funct ) {
    VerifyCubes(InPlace);
    SetBricks(InPlace);
    ProcessByBrick::StartProcess(funct);
  }

  void ProcessByLine::StartProcess(std::function<void(Buffer &in, Buffer &out)> funct ) {
    VerifyCubes(InputOutput);
    SetBricks(InputOutput);
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by line operation over exactly one input
   * and one output cube. Typically, this method is used for simple operations
   * such as stretching a cube or applying various operators to a cube (add
   * constant, multiply by constant, etc).
   *
   * @deprecated Please use ProcessCube()
   * @param funct (Isis::Buffer &in, Isis::Buffer &out) Name of your processing
   *                                                    function
   *
   * @throws Isis::IException::Message
   */
  void ProcessByLine::StartProcess(void funct(Isis::Buffer &in, Isis::Buffer &out)) {
      VerifyCubes(InputOutput);
      SetBricks(InputOutput);
      ProcessByBrick::StartProcess(funct);
  }


  /**
   * This method invokes the process by line operation over multiple input and
   * output cubes. Typically, this method is used when two input cubes are
   * required for operations like ratios, differences, masking, etc.
   *
   * @deprecated Please use ProcessCubes()
   * @param funct (vector<Isis::Buffer *> &in, vector<Isis::Buffer *> &out) Name
   *                of your processing function
   *
   * @throws Isis::iException::Message
   */
  void ProcessByLine::StartProcess(void funct(std::vector<Isis::Buffer *> &in,
                  std::vector<Isis::Buffer *> &out)) {
      VerifyCubes(InputOutputList);
      SetBricks(InputOutputList);
      ProcessByBrick::StartProcess(funct);
  }
}
