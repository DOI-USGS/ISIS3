/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/05/14 21:07:12 $                                                                 
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


#include "ProcessBySpectra.h"

using namespace std;
namespace Isis {
  /** 
   * Opens an input cube specified by the user and verifies requirements are met.  
   * This method is overloaded and adds the requirements of ic_base::SpatialMatch 
   * which requires all input cubes to have the same number of samples and lines. 
   * It also added the requirement ic_base::BandMatchOrOne which forces 2nd, 3rd, 
   * 4th, etc input cubes to match the number of bands in the 1st input cube 
   * or to have exactly one band. For more information see Process::SetInputCube
   * 
   * @return Cube*
   *
   * @param parameter User parameter to obtain file to open. Typically, the value 
   *                  is "FROM". For example, the user can specify on the command 
   *                  line FROM=myfile.cub and this method will attempt to open 
   *                  the cube "myfile.cub" if the parameter was set to "FROM".
   * 
   * @param requirements See Process::SetInputCube for more information.  
   *                     Defaults to 0
   * 
   * @throws Isis::iException::Message
   */ 
  Isis::Cube* ProcessBySpectra::SetInputCube (const std::string &parameter,
                                                  const int requirements) {
    return Isis::ProcessByBrick::SetInputCube (parameter,Isis::SpatialMatch | requirements);
  }


  Isis::Cube* ProcessBySpectra::SetInputCube (const std::string &file, 
                                                  Isis::CubeAttributeInput &att,
                                                  const int requirements) {
    return Isis::ProcessByBrick::SetInputCube (file,att,Isis::SpatialMatch | requirements);
  }

  /** 
   * This method invokes the process by spectra operation over a single input or 
   * output cube. It will be an input cube if the method SetInputCube was invoked
   * exactly one time before calling StartProcess. It will be an output cube if 
   * the SetOutputCube method was invoked exactly one time. Typically this method 
   * can be used to obtain statistics, histograms, or other information from
   * an input cube.
   * 
   * @param funct (Isis::Buffer &b) Name of your processing function
   * 
   * @throws Isis::iException::Message 
   */
  void ProcessBySpectra::StartProcess (void funct(Isis::Buffer &in)) { 

    // Error checks
    if ((InputCubes.size() + OutputCubes.size()) > 1) { 
      string m = "You can only specify exactly one input or output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if ((InputCubes.size() + OutputCubes.size()) == 0) {
      string m = "You haven't specified an input or output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    int ns, nl, nb;
    if (InputCubes.size() == 1) {
      ns = InputCubes[0]->Samples();
      nl = InputCubes[0]->Lines();
      nb = InputCubes[0]->Bands();
    }
    else {
      ns = OutputCubes[0]->Samples();
      nl = OutputCubes[0]->Lines();
      nb = OutputCubes[0]->Bands();
    }
    if (Type() == PerPixel) SetBrickSize(1, 1, nb);
    else if (Type() == ByLine) SetBrickSize(ns, 1, nb);
    else SetBrickSize(1, nl, nb);
    Isis::ProcessByBrick::StartProcess(funct);
  }

  /** 
   * This method invokes the process by spectra operation over exactly one input and
   * one output cube. Typically, this method is used for simple operations such 
   * as stretching a cube or applying various operators to a cube (add constant,
   * multiply by constant, etc). 
   * 
   * @param funct (Isis::Buffer &in, Isis::Buffer &out) Name of your processing 
   *                                                    function
   * 
   * @throws Isis::iException::Message
   */ 
  void ProcessBySpectra::StartProcess (void 
                                       funct(Isis::Buffer &in, Isis::Buffer &out)) {
    // Error checks ... there must be one input and output
    if (InputCubes.size() != 1) {
      string m = "You must specify exactly one input cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    } else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    int numLines=1, numSamples=1, numBands=1;

    if (InputCubes[0]->Bands() > OutputCubes[0]->Bands())
      numBands=InputCubes[0]->Bands();
    else numBands = OutputCubes[0]->Bands();
    if (Type() == ByLine) {
      numSamples = std::max(InputCubes[0]->Samples(), OutputCubes[0]->Samples());
    } else if (Type() == BySample) {
      numLines = std::max(InputCubes[0]->Lines(), OutputCubes[0]->Lines());
    }

    if (Type() == PerPixel) {
      SetInputBrickSize(1, 1, InputCubes[0]->Bands());
      SetOutputBrickSize(1, 1, OutputCubes[0]->Bands());
    }
    else if (Type() == ByLine) {
      SetInputBrickSize(InputCubes[0]->Samples(), 1, InputCubes[0]->Bands());
      SetOutputBrickSize(OutputCubes[0]->Samples(), 1, OutputCubes[0]->Bands());
    }
    else {
      SetInputBrickSize(1, InputCubes[0]->Lines(), InputCubes[0]->Bands());
      SetOutputBrickSize(1, OutputCubes[0]->Lines(), OutputCubes[0]->Bands());
    }

    Isis::ProcessByBrick::StartProcess(funct);
  }

  /** 
   * This method invokes the process by spectra operation over multiple input and 
   * output cubes. Typically, this method is used when two input cubes are 
   * required for operations like ratios, differences, masking, etc. 
   * 
   * @param funct (vector<Isis::Buffer *> &in, vector<Isis::Buffer *> &out) Name  
   *                of your processing function
   * 
   * @throws Isis::iException::Message
   */
  void ProcessBySpectra::StartProcess (void funct(std::vector<Isis::Buffer *> &in, 
                                                  std::vector<Isis::Buffer *> &out)) {
    if (Type() == PerPixel) {
      for (unsigned int i=0; i<InputCubes.size(); i++) {
        SetInputBrickSize(1, 1, InputCubes[i]->Bands(), i+1);
      }
      for (unsigned int i=0; i<OutputCubes.size(); i++) {
        SetOutputBrickSize(1, 1, OutputCubes[i]->Bands(), i+1);
      }
    } else if (Type() == ByLine) {
      for (unsigned int i=0; i<InputCubes.size(); i++) {
        SetInputBrickSize(InputCubes[i]->Samples(), 1, InputCubes[i]->Bands(), i+1);
      }
      for (unsigned int i=0; i<OutputCubes.size(); i++) {
        SetOutputBrickSize(OutputCubes[i]->Samples(), 1, OutputCubes[i]->Bands(), i+1);
      }
    } else {
      for (unsigned int i=0; i<InputCubes.size(); i++) {
        SetInputBrickSize(1, InputCubes[i]->Lines(), InputCubes[i]->Bands(), i+1);
      }
      for (unsigned int i=0; i<OutputCubes.size(); i++) {
        SetOutputBrickSize(1, OutputCubes[i]->Lines(), OutputCubes[i]->Bands(), i+1);
      }
    }

    Isis::ProcessByBrick::StartProcess(funct);
  }

  void ProcessBySpectra::SetType(const int type){
    if (type !=PerPixel && type!=ByLine && type!=BySample) {
      string m = "The specified type is invalid";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    } else p_spectraType = type;
  }
}
