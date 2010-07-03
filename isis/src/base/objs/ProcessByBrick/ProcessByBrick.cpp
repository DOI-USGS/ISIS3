/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2008/05/14 21:07:11 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                      

#include "ProcessByBrick.h"
#include "Brick.h"
#include "Cube.h"

using namespace std;
namespace Isis {

  ProcessByBrick::ProcessByBrick () {
    p_inputBrickSamples.clear();
    p_inputBrickLines.clear();
    p_inputBrickBands.clear();

    p_outputBrickSamples.clear();
    p_outputBrickLines.clear();
    p_outputBrickBands.clear();

    p_inputBrickSizeSet=false;
    p_outputBrickSizeSet=false;
    p_wrapOption = false;
  }

  /**
   * Opens an input cube specified by the user and verifies requirements are met. 
   * This method is overloaded and adds the requirements of ISIS::SpatialMatch 
   * which requires all input cubes to have the same dimensions or be exactly
   * be exactly one.  For more information see Process::SetInputCube
   * 
   *  @return Cube*
   * 
   * @param parameter User parameter to obtain file to open. Typically, the value
   * is "FROM". For example, the user can specify on the command
   * line FROM=myfile.cub and this method will attempt to open
   * the cube "myfile.cub" if the parameter was set to "FROM".
   * 
   * @param requirements See Process::SetInputCube for more information. 
   *                     Defaults to 0
   * 
   * @throws Isis::iException::Message
   */
  Isis::Cube* ProcessByBrick::SetInputCube (const std::string &parameter,
                                                const int requirements) {
    int allRequirements = Isis::AllMatchOrOne;
    allRequirements |= requirements;
    return Isis::Process::SetInputCube (parameter,allRequirements);
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
    * @throws Isis::iException::Message
    */
  Isis::Cube* ProcessByBrick::SetInputCube (const std::string &file, 
                                                const Isis::CubeAttributeInput &att,
                                                const int requirements) {
    int allRequirements = Isis::AllMatchOrOne;
    allRequirements |= requirements;
    return Isis::Process::SetInputCube (file,att,allRequirements);
  }


   /**
   * Sets the brick size
   * 
   * @param ns Number of samples
   * 
   * @param nl Number of lines
   * 
   * @param nb Number of bands
   */
  void ProcessByBrick::SetBrickSize (const int ns, const int nl, const int nb) {
    p_inputBrickSamples.clear();
    p_inputBrickSamples.resize(InputCubes.size()+1, ns);
    p_inputBrickLines.clear();
    p_inputBrickLines.resize(InputCubes.size()+1, nl);
    p_inputBrickBands.clear();
    p_inputBrickBands.resize(InputCubes.size()+1, nb);

    p_outputBrickSamples.clear();
    p_outputBrickSamples.resize(OutputCubes.size()+1, ns);
    p_outputBrickLines.clear();
    p_outputBrickLines.resize(OutputCubes.size()+1, nl);
    p_outputBrickBands.clear();
    p_outputBrickBands.resize(OutputCubes.size()+1, nb);

    p_inputBrickSizeSet=true;
    p_outputBrickSizeSet=true;
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
  void ProcessByBrick::SetInputBrickSize (const int ns, const int nl, const int nb){
    p_inputBrickSamples.clear();
    p_inputBrickSamples.resize(InputCubes.size()+1, ns);
    p_inputBrickLines.clear();
    p_inputBrickLines.resize(InputCubes.size()+1, nl);
    p_inputBrickBands.clear();
    p_inputBrickBands.resize(InputCubes.size()+1, nb);

    p_inputBrickSizeSet=true;
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
  void ProcessByBrick::SetInputBrickSize (const int ns, const int nl, const int nb, const int cube){
    if (cube > (int)InputCubes.size()) {
      string m = "The specified cube is out of range";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    // If a default size has already been set, use it to fill in
    if (p_inputBrickSamples.size() > 0) {
      p_inputBrickSamples.resize(InputCubes.size()+1, p_inputBrickSamples[0]);
      p_inputBrickLines.resize(InputCubes.size()+1, p_inputBrickLines[0]);
      p_inputBrickBands.resize(InputCubes.size()+1, p_inputBrickBands[0]);
    }
    //  otherwise, make this the default size
    else{
      p_inputBrickSamples.resize(InputCubes.size()+1, ns);
      p_inputBrickLines.resize(InputCubes.size()+1, nl);
      p_inputBrickBands.resize(InputCubes.size()+1, nb);
    }

    p_inputBrickSamples[cube] = ns;
    p_inputBrickLines[cube] = nl;
    p_inputBrickBands[cube] = nb;

    p_inputBrickSizeSet=true;
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
  void ProcessByBrick::SetOutputBrickSize (const int ns, const int nl, const int nb){
    p_outputBrickSamples.clear();
    p_outputBrickSamples.resize(OutputCubes.size()+1, ns);
    p_outputBrickLines.clear();
    p_outputBrickLines.resize(OutputCubes.size()+1, nl);
    p_outputBrickBands.clear();
    p_outputBrickBands.resize(OutputCubes.size()+1, nb);

    p_outputBrickSizeSet=true;
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
  void ProcessByBrick::SetOutputBrickSize (const int ns, const int nl, const int nb, const int cube){
    if (cube > (int)OutputCubes.size()) {
      string m = "The specified cube is out of range";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    // If a default size has already been set, use it to fill in
    if (p_outputBrickSamples.size() > 0) {
      p_outputBrickSamples.resize(OutputCubes.size()+1, p_outputBrickSamples[0]);
      p_outputBrickLines.resize(OutputCubes.size()+1, p_outputBrickLines[0]);
      p_outputBrickBands.resize(OutputCubes.size()+1, p_outputBrickBands[0]);
    }
    //  otherwise, make this the default size
    else{
      p_outputBrickSamples.resize(OutputCubes.size()+1, ns);
      p_outputBrickLines.resize(OutputCubes.size()+1, nl);
      p_outputBrickBands.resize(OutputCubes.size()+1, nb);
    }

    p_outputBrickSamples[cube] = ns;
    p_outputBrickLines[cube] = nl;
    p_outputBrickBands[cube] = nb;

    p_outputBrickSizeSet=true;
  }

  /** 
   * Starts the systematic processing of the input cube by moving an arbitrary 
   * shaped brick through the cube. This method requires that exactly one input
   * cube and one output cube be loaded using the SetInputCube and SetOutputCube
   * methods.
   * 
   * @param funct (Isis::Buffer &in, Isis::Buffer &out) Receive an nxm brick in   
   *              the input buffer and output the an nxm brick. If n=1 and m=lines 
   *              this will process by columns. Likewise if n=samples and m=1 
   *              this will process by lines.
   * 
   * @throws Isis::iException::Programmer
   */
  void ProcessByBrick::StartProcess (void 
                                     funct(Isis::Buffer &in, Isis::Buffer &out)) {
    // Error checks ... there must be one input and output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    } else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    //  Make sure the brick size has been set
    if (!p_inputBrickSizeSet || !p_outputBrickSizeSet) {
      string m = "Use the SetBrickSize, SetInputBrickSize, or SetOutputBrickSize method to set the brick sizes";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    // Make all input and/or output cubes have the same size
    if (p_outputBrickSamples.size() == 1) {
      SetOutputBrickSize(p_outputBrickSamples[0], p_outputBrickLines[0], p_outputBrickBands[0], 1);
    }
    if (p_inputBrickSamples.size() == 1) {
      SetInputBrickSize(p_inputBrickSamples[0], p_inputBrickLines[0], p_inputBrickBands[0], 1);
    }

    // Construct brick buffers
    Isis::Brick ibrick(*InputCubes[0],p_inputBrickSamples[1],p_inputBrickLines[1], p_inputBrickBands[1]);
    Isis::Brick obrick(*OutputCubes[0],p_outputBrickSamples[1],p_outputBrickLines[1], p_outputBrickBands[1]);

    int numBricks;
    if (ibrick.Bricks() > obrick.Bricks()) numBricks = ibrick.Bricks();
    else numBricks = obrick.Bricks();

    // Loop and let the app programmer work with the bricks
    p_progress->SetMaximumSteps(numBricks);
    p_progress->CheckStatus();

    ibrick.begin();
    obrick.begin();
    for (int i=0; i<numBricks; i++) {
      InputCubes[0]->Read(ibrick);
      funct (ibrick,obrick);
      OutputCubes[0]->Write(obrick);
      p_progress->CheckStatus();
      ibrick++;
      obrick++;
    }
  }

  /** 
   * Starts the systematic processing of the input cube by moving an arbitrary 
   * shaped brick through the cube. This method requires that exactly one input 
   * cube be loaded. No output cubes are produced.   
   * 
   * @param funct (Isis::Buffer &in) Receive an nxm brick in the input buffer. If 
   *                                n=1 and m=lines this will process by columns.
   *                                Likewise if n=samples and m=1 this will 
   *                                process by lines.
   * 
   * @throws Isis::iException::Programmer
   */ 
  void ProcessByBrick::StartProcess (void funct(Isis::Buffer &in)) {
    // Error checks
    if ((InputCubes.size() + OutputCubes.size()) != 1) {
      string m = "You can only specify exactly one input or output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    // Determine if we have an input or output
    Isis::Cube *cube;
    // Construct brick buffers
    Isis::Brick *bricks;
    bool haveInput;
    if (InputCubes.size() == 1) {
      //  Make sure the brick size has been set
    if (!p_inputBrickSizeSet) {
      string m = "Use the SetBrickSize or SetInputBrickSize method to set the input brick size";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    // And the size is stored
    else if (p_inputBrickSamples.size() == 1) {
      SetInputBrickSize(p_inputBrickSamples[0], p_inputBrickLines[0], p_inputBrickBands[0], 1);
    }

      haveInput = true;
      cube = InputCubes[0];
      bricks = new Isis::Brick(*cube,p_inputBrickSamples[1],p_inputBrickLines[1], p_inputBrickBands[1]);
    }
    else {
      //  Make sure the brick size has been set
    if (!p_outputBrickSizeSet) {
      string m = "Use the SetBrickSize or SetOutputBrickSize method to set the output brick size";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    // And the size is stored
    else if (p_outputBrickSamples.size() == 1) {
      SetOutputBrickSize(p_outputBrickSamples[0], p_outputBrickLines[0], p_outputBrickBands[0], 1);
    }

      haveInput = false;
      cube = OutputCubes[0];
      bricks = new Isis::Brick(*cube,p_outputBrickSamples[1],p_outputBrickLines[1], p_outputBrickBands[1]);
    }

    // Loop and let the app programmer work with the bricks
    p_progress->SetMaximumSteps(bricks->Bricks());
    p_progress->CheckStatus();

    for (bricks->begin(); !bricks->end(); (*bricks)++) {
      if (haveInput) cube->Read(*bricks); // input only
      funct (*bricks);
      if ((!haveInput) || (cube->IsReadWrite())) cube->Write(*bricks); // output only or input/output
      p_progress->CheckStatus();
    }

    delete bricks;
  }

  /** 
   * Starts the systematic processing of the input cube by moving an arbitrary 
   * shaped brick through the cube. This method allows multiple input and output
   * cubes.   
   * 
   * @param funct (vector<Isis::Buffer *> &in, vector<Isis::Buffer *> &out) Receive
   *                                an nxm brick in the input buffer. If 
   *                                n=1 and m=lines this will process by columns.
   *                                Likewise if n=samples and m=1 this will 
   *                                process by lines.
   * 
   * @throws Isis::iException::Programmer
   */ 
  void ProcessByBrick::StartProcess (void funct(std::vector<Isis::Buffer *> &in, 
                                                std::vector<Isis::Buffer *> &out)) {

    // Make sure we had an image
    if ( InputCubes.size()==0 && OutputCubes.size()==0) {
      string m = "You have not specified any input or output cubes";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    //  Make sure the brick size has been set
    if (!p_inputBrickSizeSet && InputCubes.size() >0) {
      string m = "Use the SetBrickSize or SetInputBrick method to set the input brick size(s)";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if (p_inputBrickSizeSet && p_inputBrickSamples.size() == 1) {
      SetInputBrickSize(p_inputBrickSamples[0], p_inputBrickLines[0], p_inputBrickBands[0], InputCubes.size());
    }
     if (!p_outputBrickSizeSet && OutputCubes.size() >0) {
      string m = "Use the SetBrickSize or SetOutputBrick method to set the output brick size(s)";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
     else if (p_outputBrickSizeSet && p_outputBrickSamples.size() == 1) {
      SetOutputBrickSize(p_outputBrickSamples[0], p_outputBrickLines[0], p_outputBrickBands[0], OutputCubes.size());
    }

    // this parameter holds the number of bricks to be used in processing
    //      which is the maximum number of bricks of all the cubes.
    int numBricks = 0;

    // Construct two vectors of brick buffer managers
    // The input buffer managers
    std::vector<Isis::Brick *> imgrs;
    vector<Isis::Buffer *> ibufs;
    for (unsigned int i=1; i<=InputCubes.size(); i++) {
      Isis::Brick *ibrick = new Isis::Brick(*InputCubes[i-1],p_inputBrickSamples[i],p_inputBrickLines[i], p_inputBrickBands[i]);
      ibrick->begin();
      ibufs.push_back(ibrick);
      imgrs.push_back(ibrick);
      if ( numBricks < ibrick->Bricks() ) numBricks = ibrick->Bricks();
    }

    // And the output buffer managers
    std::vector<Isis::Brick *> omgrs;
    vector<Isis::Buffer *> obufs;
    for (unsigned int i=1; i<=OutputCubes.size(); i++) {
      Isis::Brick *obrick = new Isis::Brick(*OutputCubes[i-1],p_outputBrickSamples[i],p_outputBrickLines[i], p_outputBrickBands[i]);
      obrick->begin();
      obufs.push_back(obrick);
      omgrs.push_back(obrick);
      if ( numBricks < obrick->Bricks() ) numBricks = obrick->Bricks();
    }

    // Loop and let the app programmer process the bricks
    p_progress->SetMaximumSteps(numBricks);
    p_progress->CheckStatus();

    for (int t=0; t<numBricks; t++) {
      // Read the input buffers
      for (unsigned int i=0; i<InputCubes.size(); i++) {
        InputCubes[i]->Read(*ibufs[i]);
      }

      // Pass them to the application function
      funct (ibufs,obufs);

      // And copy them into the output cubes
      for (unsigned int i=0; i<OutputCubes.size(); i++) {
        OutputCubes[i]->Write(*obufs[i]);
        omgrs[i]->next();
      }

      for (unsigned int i=0; i<InputCubes.size(); i++) {
        imgrs[i]->next();
        // if the manager has reached the end and the 
        // wrap option is on, wrap around to the beginning
        if (Wraps() && imgrs[i]->end()) imgrs[i]->begin();

		// Enforce same band
        if(imgrs[i]->Band() != imgrs[0]->Band() && InputCubes[i]->Bands() != 1) {
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
   * etc.
   */
  void ProcessByBrick::EndProcess () {

    p_inputBrickSizeSet = false;
    p_outputBrickSizeSet = false;
    Isis::Process::EndProcess ();

  }
} // end namespace isis

