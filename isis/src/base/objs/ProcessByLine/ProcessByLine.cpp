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
                                                                     

#include "ProcessByLine.h"

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
  Isis::Cube* ProcessByLine::SetInputCube (const std::string &parameter,
                                               const int requirements) {
    int allRequirements = Isis::SpatialMatch | Isis::BandMatchOrOne;
    allRequirements |= requirements;
    return Isis::Process::SetInputCube (parameter,allRequirements);
  }
                                

  Isis::Cube* ProcessByLine::SetInputCube (const std::string &file, 
                                               Isis::CubeAttributeInput &att,
                                               const int requirements) {
    int allRequirements = Isis::SpatialMatch | Isis::BandMatchOrOne;
    allRequirements |= requirements;
    return Isis::Process::SetInputCube (file,att,allRequirements);
  }
  
 /** 
  * This method invokes the process by line operation over a single input or 
  * output cube. It will be an input cube if the method SetInputCube was invoked
  * exactly one time before calling StartProcess. It will be an output cube if 
  * the SetOutputCube method was invoked exactly one time. Typically this method 
  * can be used to obtain statistics, histograms, or other information from
  * an input cube.
  * 
  * @param funct (Isis::Buffer &b) Name of your processing function
  * 
  * @throws Isis::iException::Message 
  * 
  * @internal 
  *  @history 2005-02-28 Stuart Sides - Modified so cube that are opended 
  *                                     ReadWrite will be written. Before
  *                                     only cube opened Write would be 
  *                                     written.
  */
  void ProcessByLine::StartProcess (void funct(Isis::Buffer &inout)) { 
    // Error checks
    if ((InputCubes.size() + OutputCubes.size()) > 1) { 
      string m = "You can only specify exactly one input or output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if ((InputCubes.size() + OutputCubes.size()) == 0) {
      string m = "You haven't specified an input or output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    
    // Determine if we have an input or output
    if (InputCubes.size() == 1) SetBrickSize(InputCubes[0]->Samples(), 1, 1);
    else SetBrickSize(OutputCubes[0]->Samples(), 1, 1);

    Isis::ProcessByBrick::StartProcess(funct);
  }
  
 /** 
  * This method invokes the process by line operation over exactly one input and
  * one output cube. Typically, this method is used for simple operations such 
  * as stretching a cube or applying various operators to a cube (add constant,
  * multiply by constant, etc). 
  * 
  * @param funct (Isis::Buffer &in, Isis::Buffer &out) Name of your processing 
  *                                                    function
  * 
  * @throws Isis::iException::Message
  */ 
  void ProcessByLine::StartProcess (void 
                  funct(Isis::Buffer &in, Isis::Buffer &out)) {
    // Error checks ... there must be one input and output
    if (InputCubes.size() != 1) { 
      string m = "You must specify exactly one input cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    else if (OutputCubes.size() != 1) {
      string m = "You must specify exactly one output cube";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // The lines in the input and output must match
    if (InputCubes[0]->Lines() != OutputCubes[0]->Lines()) {
      string m = "The number of lines in the input and output cubes ";
      m += "must match";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // The bands in the input and output must match
    if (InputCubes[0]->Bands() != OutputCubes[0]->Bands()) {
      string m = "The number of bands in the input and output cubes ";
      m += "must match";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    SetInputBrickSize(InputCubes[0]->Samples(), 1, 1);
    SetOutputBrickSize(OutputCubes[0]->Samples(), 1, 1);

    Isis::ProcessByBrick::StartProcess(funct);
  }
  
 /** 
  * This method invokes the process by line operation over multiple input and 
  * output cubes. Typically, this method is used when two input cubes are 
  * required for operations like ratios, differences, masking, etc. 
  * 
  * @param funct (vector<Isis::Buffer *> &in, vector<Isis::Buffer *> &out) Name  
  *                of your processing function
  * 
  * @throws Isis::iException::Message
  */
  void ProcessByLine::StartProcess (void 
   funct(std::vector<Isis::Buffer *> &in, std::vector<Isis::Buffer *> &out)) {
    // Make sure we had an image
    if (InputCubes.size()+OutputCubes.size() < 1) {
      string m = "You have not specified any input or output cubes";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
  
    // Make sure all the output images have the same number of bands as
    // the first input/output cube
    for (unsigned int i=0; i<OutputCubes.size(); i++) {
      if (OutputCubes[i]->Lines() != OutputCubes[0]->Lines()) {
        string m = "All output cubes must have the same number of lines ";
        m += "as the first input cube or output cube";
        throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
      }
      if (OutputCubes[i]->Bands() != OutputCubes[0]->Bands()) {
        string m = "All output cubes must have the same number of bands ";
        m += "as the first input cube or output cube";
        throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
      }
    }

    for (unsigned int i=0; i<InputCubes.size(); i++) {
      SetInputBrickSize(InputCubes[i]->Samples(), 1, 1, i+1);
    }
    for (unsigned int i=0; i<OutputCubes.size(); i++) {
      SetOutputBrickSize(OutputCubes[i]->Samples(), 1, 1, i+1);
    }

    Isis::ProcessByBrick::StartProcess(funct);
  }
}
