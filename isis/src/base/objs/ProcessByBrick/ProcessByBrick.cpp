/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <functional>

#include "ProcessByBrick.h"
#include "Brick.h"
#include "Cube.h"

using namespace std;

namespace Isis {
  ProcessByBrick::ProcessByBrick() {
    p_inputBrickSamples.clear();
    p_inputBrickLines.clear();
    p_inputBrickBands.clear();

    p_outputBrickSamples.clear();
    p_outputBrickLines.clear();
    p_outputBrickBands.clear();

    p_outputRequirements = 0;
    p_inputBrickSizeSet = false;
    p_outputBrickSizeSet = false;
    p_wrapOption = false;
    p_reverse = false;
  }




  ProcessByBrick::~ProcessByBrick() {
  }


  /**
   * Opens an input cube specified by the user and verifies requirements are
   * met. This method is overloaded and adds the requirements of
   * ISIS::SpatialMatch which requires all input cubes to have the same
   * dimensions or be exactly be exactly one.  For more information see
   * Process::SetInputCube
   *
   *  @return Cube*
   *
   * @param parameter User parameter to obtain file to open. Typically, the
   * value is "FROM". For example, the user can specify on the command line
   * FROM=myfile.cub and this method will attempt to open the cube "myfile.cub"
   * if the parameter was set to "FROM".
   *
   * @param requirements See Process::SetInputCube for more information.
   *                     Defaults to 0
   *
   * @throws iException::Message
   */
  Cube *ProcessByBrick::SetInputCube(const QString &parameter,
                                     int requirements) {
    int allRequirements = AllMatchOrOne;
    allRequirements |= requirements;
    return Process::SetInputCube(parameter, allRequirements);
  }

  /**
    * Opens an input cube specified by the user, sets the attributes, and
    * verifies requirements are met. This method is overloaded and adds the
    * requirements of ISIS::SpatialMatch which requires all input cubes to
    * have the same dimensions or be exactly be exactly one.  For more
    * information see Process::SetInputCube
    *
    *  @return Cube*
    *
    * @param file User parameter to obtain file to open. Typically, the
    * value is "FROM". For example, the user can specify on the command
    * line FROM=myfile.cub and this method will attempt to open
    * the cube "myfile.cub" if the parameter was set to "FROM".
    *
    * @param att See Process::SetInputCube for more information.
    *
    * @param requirements See Process::SetInputCube for more information.
    *                     Defaults to 0
    *
    * @throws iException::Message
    */
  Cube *ProcessByBrick::SetInputCube(const QString &file,
                                     const CubeAttributeInput &att,
                                     int requirements) {
    int allRequirements = AllMatchOrOne;
    allRequirements |= requirements;
    return Process::SetInputCube(file, att, allRequirements);
  }


void ProcessByBrick::SetOutputRequirements(int outputRequirements) {

  p_outputRequirements = outputRequirements;


}

  void ProcessByBrick::SetBricks(IOCubes cn){

      ;

  }

  /**
    *  Verifies the dimensions of the input/output cubes.
    *
    * @param cn  An IOCubes enumeration for one of three possible Input/Output
    * situations:
    *     InPlace:          The input cube is the output cube
    *     InputOutput:      One input cube and one output cube
    *     InputOutputList:  A vector of input and output cubes.  The input vector
    *                       is not necessarily the same length as the output vector
    *
    * @throws iException::Message
    */
    void ProcessByBrick::VerifyCubes(IOCubes cn){

      switch(cn){

           //Error check
            case InPlace:

              if (InputCubes.size() +OutputCubes.size() > 1) {
                string m = "You can only specify exactly one input or output cube";
                throw IException(IException::Programmer,m,_FILEINFO_);
              }
              else if ( (InputCubes.size() + OutputCubes.size() == 0) ){

                  string m = "You haven't specified an input or output cube";
                       throw IException(IException::Programmer, m, _FILEINFO_);
              }

              break;

            case InputOutput:

              //Error checks ... there must be one input and output
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

               if(InputCubes[0]->sampleCount() != OutputCubes[0]->sampleCount()) {
                   string m = "The number of samples in the input and output cubes ";
                   m += "must match";
                   throw IException(IException::Programmer, m, _FILEINFO_);
                 }


                 // The bands in the input and output must match

               //If we are only looking for a spatial match (just match lines/samples)
               //but not bands, then we skip over this check.

               if ( !(p_outputRequirements & Isis::SpatialMatch ) ) {
               if(InputCubes[0]->bandCount() != OutputCubes[0]->bandCount()) {
                   string m = "The number of bands in the input and output cubes ";
                   m += "must match";
                   throw IException(IException::Programmer, m, _FILEINFO_);
                 }

              }

              break;

            case InputOutputList:

              // Make sure we had an image
              if (InputCubes.size() + OutputCubes.size() < 1) {
                string m = "You have not specified any input or output cubes";
                throw IException(IException::Programmer, m, _FILEINFO_);
              }

              for (unsigned int i = 0; i < OutputCubes.size(); i++) {
                    if (OutputCubes[i]->lineCount() != OutputCubes[0]->lineCount() ) {
                      string m = "All output cubes must have the same number of lines ";
                      m += "as the first input cube or output cube";
                      throw IException(IException::Programmer, m, _FILEINFO_);
                    }
                   //If we are only looking for a spatial match (just match lines/samples)
                   //but not bands, then we skip over this check.
                   if ( !(p_outputRequirements & Isis::SpatialMatch ) ) {
                    if (OutputCubes[i]->bandCount() != OutputCubes[0]->bandCount() ) {
                      string m = "All output cubes must have the same number of bands ";
                      m += "as the first input cube or output cube";
                      throw IException(IException::Programmer, m, _FILEINFO_);
                    }
                    }
                  }

              break;


          }//end switch

  }



  /**
  * Sets the input and output bricks sizes to the given number of samples,
  * lines, and bands.
  *
  * @param ns Number of samples
  *
  * @param nl Number of lines
  *
  * @param nb Number of bands
  */
 void ProcessByBrick::SetBrickSize(int ns, int nl, int nb) {
    SetInputBrickSize(ns, nl, nb);
    SetOutputBrickSize(ns, nl, nb);
    return;
  }


  /**
   * Sets the size of all input bricks
   *
   * @param ns Number of samples
   *
   * @param nl Number of lines
   *
   * @param nb Number of bands
   */
  void ProcessByBrick::SetInputBrickSize(int ns, int nl, int nb) {
    p_inputBrickSamples.clear();
    p_inputBrickSamples.resize(InputCubes.size() + 1, ns);
    p_inputBrickLines.clear();
    p_inputBrickLines.resize(InputCubes.size() + 1, nl);
    p_inputBrickBands.clear();
    p_inputBrickBands.resize(InputCubes.size() + 1, nb);

    p_inputBrickSizeSet = true;
  }


  /**
   * Sets the brick size of the specified cube
   *
   * @param ns Number of samples
   *
   * @param nl Number of lines
   *
   * @param nb Number of bands
   *
   * @param cube The index of the cube
   */
  void ProcessByBrick::SetInputBrickSize(int ns, int nl, int nb, int cube) {
    if (cube > (int)InputCubes.size()) {
      string m = "The specified cube is out of range";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // If a default size has already been set, use it to fill in
    if (p_inputBrickSamples.size() > 0) {
      p_inputBrickSamples.resize(InputCubes.size() + 1, p_inputBrickSamples[0]);
      p_inputBrickLines.resize(InputCubes.size() + 1, p_inputBrickLines[0]);
      p_inputBrickBands.resize(InputCubes.size() + 1, p_inputBrickBands[0]);
    }
    //  otherwise, make this the default size
    else {
      p_inputBrickSamples.resize(InputCubes.size() + 1, ns);
      p_inputBrickLines.resize(InputCubes.size() + 1, nl);
      p_inputBrickBands.resize(InputCubes.size() + 1, nb);
    }

    p_inputBrickSamples[cube] = ns;
    p_inputBrickLines[cube] = nl;
    p_inputBrickBands[cube] = nb;

    p_inputBrickSizeSet = true;
  }


  /**
   * Sets the size of all output bricks
   *
   * @param ns Number of samples
   *
   * @param nl Number of lines
   *
   * @param nb Number of bands
   */
  void ProcessByBrick::SetOutputBrickSize(int ns, int nl, int nb) {
    p_outputBrickSamples.clear();
    p_outputBrickSamples.resize(OutputCubes.size() + 1, ns);
    p_outputBrickLines.clear();
    p_outputBrickLines.resize(OutputCubes.size() + 1, nl);
    p_outputBrickBands.clear();
    p_outputBrickBands.resize(OutputCubes.size() + 1, nb);

    p_outputBrickSizeSet = true;
  }


  /**
   * Sets the brick size of the specified output cube
   *
   * @param ns Number of samples
   *
   * @param nl Number of lines
   *
   * @param nb Number of bands
   *
   * @param cube The index of the cube
   */
  void ProcessByBrick::SetOutputBrickSize(int ns, int nl, int nb, int cube) {
    if(cube > (int)OutputCubes.size()) {
      string m = "The specified cube is out of range";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // If a default size has already been set, use it to fill in
    if(p_outputBrickSamples.size() > 0) {
      p_outputBrickSamples.resize(OutputCubes.size() + 1,
                                  p_outputBrickSamples[0]);
      p_outputBrickLines.resize(OutputCubes.size() + 1, p_outputBrickLines[0]);
      p_outputBrickBands.resize(OutputCubes.size() + 1, p_outputBrickBands[0]);
    }
    //  otherwise, make this the default size
    else {
      p_outputBrickSamples.resize(OutputCubes.size() + 1, ns);
      p_outputBrickLines.resize(OutputCubes.size() + 1, nl);
      p_outputBrickBands.resize(OutputCubes.size() + 1, nb);
    }

    p_outputBrickSamples[cube] = ns;
    p_outputBrickLines[cube] = nl;
    p_outputBrickBands[cube] = nb;

    p_outputBrickSizeSet = true;
  }

  /**
   * Create the output file. This method assumes that the output cube size
   * matches the input cube size. Therefore, SetInputCube() must be called
   * before this method.
   *
   * @param parameter The output file name.
   *
   * @param att An output cube attribute to define the characteristics of the
   *            output cube.
   *
   * @return @b Isis::Cube Output cube.
   * @throws Isis::iException::Message "File is not in a supported
   *             organization."
   */
  Isis::Cube *ProcessByBrick::SetOutputCube(const QString &fname,
                                     const Isis::CubeAttributeOutput &att) {
    int nl = InputCubes[0]->lineCount();
    int ns = InputCubes[0]->sampleCount();
    int nb = InputCubes[0]->bandCount();
    return Process::SetOutputCube(fname, att, ns, nl, nb);
  }

  /**
   * Set the direction the data will be read, either all lines in a single band
   * proceeding to the next band (LinesFirst), or every band for every line
   * proceeding to the next line (BandsFirst).  The default is to process all
   * lines of a band first.
   *
   * @param direction The new enumerated processing direction
   */
  void ProcessByBrick::SetProcessingDirection(ProcessingDirection direction) {
    p_reverse = direction == BandsFirst ? true : false;
  }


  /**
   * Returns the direction the data will be read, either all lines in a single
   * band proceeding to the next band (LinesFirst), or every band for every line
   * proceeding to the next line (BandsFirst).  The default is to process all
   * lines of a band first.
   *
   * @return The selected enumerated processing direction
   */
  ProcessByBrick::ProcessingDirection ProcessByBrick::GetProcessingDirection() {
    return p_reverse ? BandsFirst : LinesFirst;
  }


  /**
   * This wrapping option only applys when there are two or more input cubes.
   * If wrapping is enabled and the second cube is smaller than the first
   * The brick will be wrapped back to the beginning of the second cube
   * once brick movement reaches the end of the cube.
   * For example, if the brick shape was a single line and the second cube
   * only had one line then function passed into StartProcess will
   * receive the same contents in the 2nd input buffer every time.
   *
   * @param wrap Specifies whether or not to wrap
   */
  void ProcessByBrick::SetWrap(bool wrap) {
    p_wrapOption = wrap;
  }


  /**
   * Returns true if the wrapping option is enabled.
   * @see SetWrap()
   * @return The value of the wrapping option
   */
  bool ProcessByBrick::Wraps() {
    return p_wrapOption;
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrarily-shaped
   * brick through the cube. This method requires that exactly one input
   * cube be loaded. No output cubes are produced.
   *
   * @param funct (Buffer &in) Receive an nxm brick in the input buffer.
   *                                If n=1 and m=lines this will process by
   *                                columns. Likewise if n=samples and m=1 this
   *                                will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByBrick::StartProcess(void funct(Buffer &in)) {
    Cube *cube = NULL;
    Brick *brick = NULL;

    bool haveInput = PrepProcessCubeInPlace(&cube, &brick);

    // Loop and let the app programmer work with the bricks
    p_progress->SetMaximumSteps(brick->Bricks());
    p_progress->CheckStatus();

    for (brick->begin(); !brick->end(); (*brick)++) {
      if (haveInput)
        cube->read(*brick);  // input only

      funct(*brick);

      // output only or input/output
      if ((!haveInput) || (cube->isReadWrite())) {
        cube->write(*brick);
      }

      p_progress->CheckStatus();
    }

    delete brick;
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrarily-shaped
   * brick through the cube. This method requires that exactly one input
   * cube be loaded. No output cubes are produced.
   *
   * @param funct (Buffer &in) Receive an nxm brick in the input buffer.
   *                                If n=1 and m=lines this will process by
   *                                columns. Likewise if n=samples and m=1 this
   *                                will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByBrick::StartProcess(std::function<void(Buffer &in)> funct ) {
    Cube *cube = NULL;
    Brick *brick = NULL;

    bool haveInput = PrepProcessCubeInPlace(&cube, &brick);

    // Loop and let the app programmer work with the bricks
    p_progress->SetMaximumSteps(brick->Bricks());
    p_progress->CheckStatus();

    for (brick->begin(); !brick->end(); (*brick)++) {
      if (haveInput)
        cube->read(*brick);  // input only

      funct(*brick);

      // output only or input/output
      if ((!haveInput) || (cube->isReadWrite())) {
        cube->write(*brick);
      }

      p_progress->CheckStatus();
    }

    delete brick;
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrarily-shaped
   * brick through the cube. This method requires that exactly one input
   * cube and one output cube be loaded using the SetInputCube and SetOutputCube
   * methods.
   *
   * @param funct (Buffer &in, Buffer &out) Receive an nxm brick in
   *              the input buffer and output the an nxm brick. If n=1 and
   *              m=lines this will process by columns. Likewise if n=samples
   *              and m=1 this will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByBrick::StartProcess(void funct(Buffer &in, Buffer &out)) {
    Brick *ibrick = NULL;
    Brick *obrick = NULL;

    int numBricks = PrepProcessCube(&ibrick, &obrick);

    // Loop and let the app programmer work with the bricks
    p_progress->SetMaximumSteps(numBricks);
    p_progress->CheckStatus();

    ibrick->begin();
    obrick->begin();

    for (int i = 0; i < numBricks; i++) {
      InputCubes[0]->read(*ibrick);
      funct(*ibrick, *obrick);
      OutputCubes[0]->write(*obrick);
      p_progress->CheckStatus();
      (*ibrick)++;
      (*obrick)++;
    }

    delete ibrick;
    delete obrick;
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrarily-shaped
   * brick through the cube. This method requires that exactly one input
   * cube and one output cube be loaded using the SetInputCube and SetOutputCube
   * methods.
   *
   * @param funct (Buffer &in, Buffer &out) Receive an nxm brick in
   *              the input buffer and output the an nxm brick. If n=1 and
   *              m=lines this will process by columns. Likewise if n=samples
   *              and m=1 this will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByBrick::StartProcess(std::function<void(Buffer &in, Buffer &out)> funct ) {
    Brick *ibrick = NULL;
    Brick *obrick = NULL;

    int numBricks = PrepProcessCube(&ibrick, &obrick);

    // Loop and let the app programmer work with the bricks
    p_progress->SetMaximumSteps(numBricks);
    p_progress->CheckStatus();

    ibrick->begin();
    obrick->begin();

    for (int i = 0; i < numBricks; i++) {
      InputCubes[0]->read(*ibrick);
      funct(*ibrick, *obrick);
      OutputCubes[0]->write(*obrick);
      p_progress->CheckStatus();
      (*ibrick)++;
      (*obrick)++;
    }

    delete ibrick;
    delete obrick;
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrarily-shaped
   * brick through the cube. This method allows multiple input and output
   * cubes.
   *
   * @param funct (vector<Buffer *> &in, vector<Buffer *> &out)
   *              Receive an nxm brick in the input buffer. If n=1 and m=lines
   *              this will process by columns.  Likewise if n=samples and m=1
   *              this will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByBrick::StartProcess(void funct(std::vector<Buffer *> &in,
                                               std::vector<Buffer *> &out)) {
    // Construct two vectors of brick buffer managers
    // The input buffer managers
    vector<Brick *> imgrs;
    vector<Buffer *> ibufs;

    // And the output buffer managers
    vector<Brick *> omgrs;
    vector<Buffer *> obufs;

    int numBricks = PrepProcessCubes(ibufs, obufs, imgrs, omgrs);

    // Loop and let the app programmer process the bricks
    p_progress->SetMaximumSteps(numBricks);
    p_progress->CheckStatus();

    for(int t = 0; t < numBricks; t++) {
      // Read the input buffers
      for(unsigned int i = 0; i < InputCubes.size(); i++) {
        InputCubes[i]->read(*ibufs[i]);
      }

      // Pass them to the application function
      funct(ibufs, obufs);

      // And copy them into the output cubes
      for(unsigned int i = 0; i < OutputCubes.size(); i++) {
        OutputCubes[i]->write(*obufs[i]);
        omgrs[i]->next();
      }

      for(unsigned int i = 0; i < InputCubes.size(); i++) {
        imgrs[i]->next();

        // if the manager has reached the end and the
        // wrap option is on, wrap around to the beginning
        if(Wraps() && imgrs[i]->end())
          imgrs[i]->begin();

        // Enforce same band
        if(imgrs[i]->Band() != imgrs[0]->Band() &&
           InputCubes[i]->bandCount() != 1) {
          imgrs[i]->SetBaseBand(imgrs[0]->Band());
        }
      }
      p_progress->CheckStatus();
    }

    for(unsigned int i = 0; i < ibufs.size(); i++) {
      delete ibufs[i];
    }
    ibufs.clear();
    imgrs.clear();

    for(unsigned int i = 0; i < obufs.size(); i++) {
      delete obufs[i];
    }
    obufs.clear();
    omgrs.clear();
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrarily-shaped
   * brick through the cube. This method allows multiple input and output
   * cubes.
   *
   * @param funct (vector<Buffer *> &in, vector<Buffer *> &out)
   *              Receive an nxm brick in the input buffer. If n=1 and m=lines
   *              this will process by columns.  Likewise if n=samples and m=1
   *              this will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByBrick::StartProcess(std::function<void(std::vector<Buffer *> &in,
                                                       std::vector<Buffer *> &out)> funct) {
    // Construct two vectors of brick buffer managers
    // The input buffer managers
    vector<Brick *> imgrs;
    vector<Buffer *> ibufs;

    // And the output buffer managers
    vector<Brick *> omgrs;
    vector<Buffer *> obufs;

    int numBricks = PrepProcessCubes(ibufs, obufs, imgrs, omgrs);

    // Loop and let the app programmer process the bricks
    p_progress->SetMaximumSteps(numBricks);
    p_progress->CheckStatus();

    for(int t = 0; t < numBricks; t++) {
      // Read the input buffers
      for(unsigned int i = 0; i < InputCubes.size(); i++) {
        InputCubes[i]->read(*ibufs[i]);
      }

      // Pass them to the application function
      funct(ibufs, obufs);

      // And copy them into the output cubes
      for(unsigned int i = 0; i < OutputCubes.size(); i++) {
        OutputCubes[i]->write(*obufs[i]);
        omgrs[i]->next();
      }

      for(unsigned int i = 0; i < InputCubes.size(); i++) {
        imgrs[i]->next();

        // if the manager has reached the end and the
        // wrap option is on, wrap around to the beginning
        if(Wraps() && imgrs[i]->end())
          imgrs[i]->begin();

        // Enforce same band
        if(imgrs[i]->Band() != imgrs[0]->Band() &&
           InputCubes[i]->bandCount() != 1) {
          imgrs[i]->SetBaseBand(imgrs[0]->Band());
        }
      }
      p_progress->CheckStatus();
    }

    for(unsigned int i = 0; i < ibufs.size(); i++) {
      delete ibufs[i];
    }
    ibufs.clear();
    imgrs.clear();

    for(unsigned int i = 0; i < obufs.size(); i++) {
      delete obufs[i];
    }
    obufs.clear();
    omgrs.clear();
  }

  /**
   * End the processing sequence and cleans up by closing cubes, freeing memory,
   *   etc.
   *
   * @deprecated Please use Finalize()
   */
  void ProcessByBrick::EndProcess() {
    p_inputBrickSizeSet = false;
    p_outputBrickSizeSet = false;
    Process::EndProcess();
  }


  /**
   * Cleans up by closing cubes and freeing memory.
   */
  void ProcessByBrick::Finalize() {
    EndProcess();
  }


  /**
   * This method blocks until the future reports that it is finished. This
   *   monitors the progress of the future and translates it's progress values
   *   into Isis progress class calls.
   *
   * @param future The future to monitor
   */
  void ProcessByBrick::BlockingReportProgress(QFuture<void> &future) {
    int isisReportedProgress = 0;
    int lastProgressValue = future.progressValue();
    // Using a mutex with a timeout isn't as bad of a hack as inheriting QThread
    //   but there ought to be a better way.
    // Does having a local mutex make sense?
    QMutex sleeper;
    sleeper.lock();
    while (!future.isFinished()) {
      sleeper.tryLock(100);

      if (future.progressValue() != lastProgressValue) {
        lastProgressValue = future.progressValue();
        // Progress min/max are reporting as 0's currently, so we're
        //   assuming the progress value is an Isis progress value.
        int isisProgressValue = lastProgressValue;
        while (isisReportedProgress < isisProgressValue) {
          p_progress->CheckStatus();
          isisReportedProgress++;
        }
      }
    }

    while (isisReportedProgress < future.progressValue()) {
      p_progress->CheckStatus();
      isisReportedProgress++;
    }

    // Need to unlock the mutex before it goes out of scope, otherwise Qt5 issues a warning
    sleeper.unlock();
  }


  /**
   * Calculates the maximum dimensions of all the cubes and returns them in a
   * vector where position 0 is the max sample, position 1 is the max line, and
   * position 2 is the max band. For example, if two cubes were passed in and
   * the first cube had 1 sample, 1 line, and 1 band, and the second cube had 2
   * samples, 2 lines, and 2 bands, the max dimensions would be 2 samples, 2
   * lines, and 2 bands.
   *
   * @param cubes The vector of cubes to calculate the maximum dimensions for.
   * @return A vector of size 3 containing the max sample dimension at position
   *   0, the max line dimension at position 1, and the max band dimension at
   *   position 2.
   */
  vector<int> ProcessByBrick::CalculateMaxDimensions(
      vector<Cube *> cubes) const {
    int maxSamples = 0;
    int maxLines = 0;
    int maxBands = 0;

    for (unsigned int i = 0; i < cubes.size(); i++) {
      int sampleCount = cubes[i]->sampleCount();
      int lineCount = cubes[i]->lineCount();
      int bandCount = cubes[i]->bandCount();

      if (sampleCount > maxSamples)
        maxSamples = sampleCount;
      if (lineCount > maxLines)
        maxLines = lineCount;
      if (bandCount > maxBands)
        maxBands = bandCount;
    }

    vector<int> maxDimensions;
    maxDimensions.push_back(maxSamples);
    maxDimensions.push_back(maxLines);
    maxDimensions.push_back(maxBands);
    return maxDimensions;
  }


  /**
   * Prepare and check to run "function" parameter for
   * StartProcess(void funct(Buffer &in)) and
   * StartProcessInPlace(Functor funct)
   *
   * @param cube - Pointer to input or output cube depending if the input
   *               cube is available
   * @param bricks - Pointer to first cube brick to be processed
   *
   * @return bool - If input cube is available(true/false)
   *
   * @history 2011-04-22 Sharmila Prasad - Ported from
   *                                StartProcess (void funct(Buffer
   *                                &in))
   */
  bool ProcessByBrick::PrepProcessCubeInPlace(Cube **cube, Brick **bricks) {
    // Error checks
    if ((InputCubes.size() + OutputCubes.size()) != 1) {
      string m = "You can only specify exactly one input or output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    bool haveInput;
    if (InputCubes.size() == 1) {

      SetBricks(InPlace);
      //  Make sure the brick size has been set
      if (!p_inputBrickSizeSet) {
        string m = "Use the SetBrickSize() or SetInputBrickSize() method to set"
                   " the input brick size";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
      // And the size is stored
      else if (p_inputBrickSamples.size() == 1) {
        SetInputBrickSize(p_inputBrickSamples[0], p_inputBrickLines[0],
                          p_inputBrickBands[0], 1);
      }

      haveInput = true;
      *cube = InputCubes[0];
      *bricks = new Brick(**cube, p_inputBrickSamples[1],
          p_inputBrickLines[1], p_inputBrickBands[1], p_reverse);
    }
    else {
      SetBricks(InPlace);
      //  Make sure the brick size has been set
      if (!p_outputBrickSizeSet) {
        string m = "Use the SetBrickSize() or SetOutputBrickSize() method to "
                   "set the output brick size";
        throw IException(IException::Programmer, m, _FILEINFO_);
      }
      // And the size is stored
      else if (p_outputBrickSamples.size() == 1) {
        SetOutputBrickSize(p_outputBrickSamples[0], p_outputBrickLines[0],
                           p_outputBrickBands[0], 1);
      }

      haveInput = false;
      *cube = OutputCubes[0];
      *bricks = new Brick(**cube, p_outputBrickSamples[1],
          p_outputBrickLines[1], p_outputBrickBands[1], p_reverse);
    }

    return haveInput;
  }


  /**
   * Prepare and check to run "function" parameter for
   * StartProcess(void funct(Buffer &in, Buffer &out)) and
   * StartProcessIO(Functor funct)
   *
   * @param ibrick - Pointer to first input cube brick
   * @param obrick - Pointer to first output cube brick
   *
   * @return int
   *
   * @internal
   *   @history 2011-04-22 Sharmila Prasad - Ported from
   *                           StartProcess (void funct(Buffer
   *                           &in, Buffer &out))
   */
  int ProcessByBrick::PrepProcessCube(Brick **ibrick, Brick **obrick) {
    // Error checks ... there must be one input and output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    SetBricks(InputOutput);
    //  Make sure the brick size has been set
    if (!p_inputBrickSizeSet || !p_outputBrickSizeSet) {
      string m = "Use the SetBrickSize(), SetInputBrickSize(), or "
                 "SetOutputBrickSize() method to set the brick sizes";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    // Make all input and/or output cubes have the same size
    if (p_outputBrickSamples.size() == 1) {
      SetOutputBrickSize(p_outputBrickSamples[0], p_outputBrickLines[0],
                         p_outputBrickBands[0], 1);
    }
    if (p_inputBrickSamples.size() == 1) {
      SetInputBrickSize(p_inputBrickSamples[0], p_inputBrickLines[0],
                        p_inputBrickBands[0], 1);
    }

    // Construct brick buffers
    if (Wraps()) {
      // Use the size of each cube as the area for the bricks to traverse since
      // we will be wrapping if we hit the end of one, but not the other.
      *ibrick = new Brick(*InputCubes[0], p_inputBrickSamples[1],
          p_inputBrickLines[1], p_inputBrickBands[1], p_reverse);
      *obrick = new Brick(*OutputCubes[0], p_outputBrickSamples[1],
          p_outputBrickLines[1], p_outputBrickBands[1], p_reverse);
    }
    else {
      // Since we are not wrapping, we need to find the maximum size of the
      // input cube and the output cube. We will use this size when
      // constructing each of the bricks' area to traverse so that we don't
      // read into nonexistent bands of the smaller of the two cubes.
      vector<Cube *> allCubes;
      allCubes.push_back(InputCubes[0]);
      allCubes.push_back(OutputCubes[0]);
      vector<int> maxDimensions = CalculateMaxDimensions(allCubes);
      int maxSamples = maxDimensions[0];
      int maxLines = maxDimensions[1];
      int maxBands = maxDimensions[2];

      *ibrick = new Brick(maxSamples, maxLines, maxBands,
                                p_inputBrickSamples[1],
                                p_inputBrickLines[1],
                                p_inputBrickBands[1],
                                InputCubes[0]->pixelType(),
                                p_reverse);
      *obrick = new Brick(maxSamples, maxLines, maxBands,
                                p_outputBrickSamples[1],
                                p_outputBrickLines[1],
                                p_outputBrickBands[1],
                                OutputCubes[0]->pixelType(),
                                p_reverse);
    }

    int numBricks;
    if((*ibrick)->Bricks() > (*obrick)->Bricks()) {
      numBricks = (*ibrick)->Bricks();
    }
    else {
      numBricks = (*obrick)->Bricks();
    }

    return numBricks;
  }


  /**
   * Prepare and check to run "function" parameter for
   * StartProcess(void funct(vector<Buffer *> &in,
   * vector<Buffer *> &out)), StartProcessIOList(Functor funct)
   *
   * @param ibufs - input buffer manager
   * @param obufs - output buffer manager
   * @param imgrs - input brick manager
   * @param omgrs - output brick manager
   *
   * @return int  - Number of bricks
   *
   * @history 2011-04-22 Sharmila Prasad - Ported from StartProcess
   *                         (void funct(vector<Buffer *> &in,
   *                         vector<Buffer *> &out))
   */
  int ProcessByBrick::PrepProcessCubes(vector<Buffer *> & ibufs,
                                       vector<Buffer *> & obufs,
                                       vector<Brick *> & imgrs,
                                       vector<Brick *> & omgrs) {
    // Make sure we had an image
    if(InputCubes.size() == 0 && OutputCubes.size() == 0) {
      string m = "You have not specified any input or output cubes";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }

    SetBricks(InputOutputList);

    //  Make sure the brick size has been set
    if(!p_inputBrickSizeSet && InputCubes.size() > 0) {
      string m = "Use the SetBrickSize() or SetInputBrick() method to set the "
                 "input brick size(s)";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if(p_inputBrickSizeSet && p_inputBrickSamples.size() == 1) {
      SetInputBrickSize(p_inputBrickSamples[0], p_inputBrickLines[0],
                        p_inputBrickBands[0], InputCubes.size());
    }
    if(!p_outputBrickSizeSet && OutputCubes.size() > 0) {
      string m = "Use the SetBrickSize() or SetOutputBrick() method to set the "
                 "output brick size(s)";
      throw IException(IException::Programmer, m, _FILEINFO_);
    }
    else if(p_outputBrickSizeSet && p_outputBrickSamples.size() == 1) {
      SetOutputBrickSize(p_outputBrickSamples[0], p_outputBrickLines[0],
                         p_outputBrickBands[0], OutputCubes.size());
    }

    // this parameter holds the number of bricks to be used in processing
    // which is the maximum number of bricks of all the cubes.
    int numBricks = 0;

    // If we are not wrapping, we need to find the maximum size of the input and
    // output cubes. We will use this size when constructing each of the bricks'
    // area to traverse so that we don't read into nonexistent bands of the
    // smaller cubes.
    int maxSamples = 0;
    int maxLines = 0;
    int maxBands = 0;
    if (!Wraps()) {
      vector<Cube *> allCubes(InputCubes);
      for (unsigned int i = 0; i < OutputCubes.size(); i++)
        allCubes.push_back(OutputCubes[i]);
      vector<int> maxDimensions = CalculateMaxDimensions(allCubes);
      maxSamples = maxDimensions[0];
      maxLines = maxDimensions[1];
      maxBands = maxDimensions[2];
    }

    for (unsigned int i = 1; i <= InputCubes.size(); i++) {
      Brick *ibrick = NULL;
      if (Wraps()) {
        // Use the size of each cube as the area for the bricks to traverse
        // since we will be wrapping if we hit the end of a cube before we are
        // done processing.
        ibrick = new Brick(*InputCubes[i-1],
                            p_inputBrickSamples[i],
                            p_inputBrickLines[i],
                            p_inputBrickBands[i],
                            p_reverse);
      }
      else {
        ibrick = new Brick(maxSamples, maxLines, maxBands,
                            p_inputBrickSamples[i],
                            p_inputBrickLines[i],
                            p_inputBrickBands[i],
                            InputCubes[i - 1]->pixelType(),
                            p_reverse);
      }
      ibrick->begin();
      ibufs.push_back(ibrick);
      imgrs.push_back(ibrick);
      if(numBricks < ibrick->Bricks()) {
        numBricks = ibrick->Bricks();
      }
    }

    for (unsigned int i = 1; i <= OutputCubes.size(); i++) {
      Brick *obrick = NULL;
      if (Wraps()) {
        obrick = new Brick(*OutputCubes[i-1],
                            p_outputBrickSamples[i],
                            p_outputBrickLines[i],
                            p_outputBrickBands[i],
                            p_reverse);
      }
      else {
        obrick = new Brick(maxSamples, maxLines, maxBands,
                            p_outputBrickSamples[i],
                            p_outputBrickLines[i],
                            p_outputBrickBands[i],
                            OutputCubes[i - 1]->pixelType(),
                            p_reverse);
      }
      obrick->begin();
      obufs.push_back(obrick);
      omgrs.push_back(obrick);
      if(numBricks < obrick->Bricks()) {
        numBricks = obrick->Bricks();
      }
    }

    return numBricks;
  }


  /**
   * Initialize a process iterator given a position.
   *
   * @param position The process iterator position to initialize with.
   */
  ProcessByBrick::ProcessIterator::ProcessIterator(int position) :
      m_currentPosition(position) {
  }


  /**
   * This class fully supports copy construction.
   *
   * @param other The process iterator to copy
   */
  ProcessByBrick::ProcessIterator::ProcessIterator(
      const ProcessIterator &other) :
        m_currentPosition(other.m_currentPosition) {
  }


  /**
   * Destructor
   */
  ProcessByBrick::ProcessIterator::~ProcessIterator() {
    m_currentPosition = -1;
  }


  /**
   * Increment the process iterator to the next position.
   * @return *this
   */
  ProcessByBrick::ProcessIterator &
      ProcessByBrick::ProcessIterator::operator++() {
    m_currentPosition++;
    return *this;
  }
} // end namespace isis
