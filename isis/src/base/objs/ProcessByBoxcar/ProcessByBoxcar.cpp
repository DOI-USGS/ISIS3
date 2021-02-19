/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "BoxcarCachingAlgorithm.h"
#include "BoxcarManager.h"
#include "Buffer.h"
#include "LineManager.h"
#include "Process.h"
#include "ProcessByBoxcar.h"

using namespace std;
namespace Isis {

  /**
   * Sets the boxcar size
   *
   * @param ns Number of samples
   *
   * @param nl Number of lines
   */
  void ProcessByBoxcar::SetBoxcarSize(const int ns, const int nl) {

    p_boxSamples = ns;
    p_boxLines = nl;
    p_boxsizeSet = true;
  }

  /**
   * Starts the systematic processing of the input cube by moving a boxcar,
   * p_boxSamples by p_boxLines, through the cube one pixel at a time. The input
   * and output buffers contain a Boxcar of the size indicated in p_boxSamples
   * and p_boxLines. The input and output cube must be initialized prior to
   * calling this method.
   *
   * @param funct (Isis::Buffer &in, double &out) Name of your processing function
   *
   * @throws Isis::IException::Programmer
   */
  void ProcessByBoxcar::StartProcess(void funct(Isis::Buffer &in, double &out)) {
    // Error checks ... there must be one input and output
    if(InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if(OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // The lines in the input and output must match
    if(InputCubes[0]->lineCount() != OutputCubes[0]->lineCount()) {
      string m = "The number of lines in the input and output cubes ";
      m += "must match";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // The samples in the input and output must match
    if(InputCubes[0]->sampleCount() != OutputCubes[0]->sampleCount()) {
      string m = "The number of samples in the input and output cubes ";
      m += "must match";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // The bands in the input and output must match
    if(InputCubes[0]->bandCount() != OutputCubes[0]->bandCount()) {
      string m = "The number of bands in the input and output cubes ";
      m += "must match";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    //  Make sure the boxcar size has been set
    if(!p_boxsizeSet) {
      string m = "Use the SetBoxcarSize method to set the boxcar size";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // Construct boxcar buffer and line buffer managers
    Isis::BoxcarManager box(*InputCubes[0], p_boxSamples, p_boxLines);
    Isis::LineManager line(*OutputCubes[0]);
    double out;

    InputCubes[0]->addCachingAlgorithm(new BoxcarCachingAlgorithm());
    OutputCubes[0]->addCachingAlgorithm(new BoxcarCachingAlgorithm());

    // Loop and let the app programmer use the boxcar to change output pixel
    p_progress->SetMaximumSteps(InputCubes[0]->lineCount()*InputCubes[0]->bandCount());
    p_progress->CheckStatus();

    box.begin();
    for(line.begin(); !line.end(); line.next()) {
      for(int i = 0; i < line.size(); i++) {
        InputCubes[0]->read(box);
        funct(box, out);
        line[i] = out;
        box++;
      }
      OutputCubes[0]->write(line);
      p_progress->CheckStatus();
    }

  }

  /**
   * End the boxcar processing sequence and cleans up by closing cubes, freeing
   * memory, etc.
   *
   * @deprecated Please use Finalize()
   */
  void ProcessByBoxcar::EndProcess() {

    p_boxsizeSet = false;
    Isis::Process::EndProcess();

  }

  /**
   * End the boxcar processing sequence and cleans up by closing cubes, freeing
   * memory, etc.
   */
  void ProcessByBoxcar::Finalize() {

    p_boxsizeSet = false;
    Isis::Process::Finalize();

  }
} // end namespace isis
