#ifndef ProcessByLine_h
#define ProcessByLine_h
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
                                                                
#include "ProcessByBrick.h"
#include "Buffer.h"

namespace Isis {
 /**                                                                       
  * @brief Process cubes by line                
  *                                                                        
  * This class allows a programmer to develop a program which process input and 
  * output cubes sequentially by line. That is, receive the input data for line 
  * one, manipulate the data, and pass back the data for output line one. Then 
  * proceed to the line two and so one. This class is derived from the Process 
  * class which give many functions for setting up input and output cubes.
  * 
  * Here is an example of how to use ProcessByLine                         
  * @code                                                                  
  *  using namespace std; 
  *  #include "Isis.h"
  *  #include "ProcessByLine.h" 
  *  #include "SpecialPixel.h" 
  *  void mirror (Buffer &in, Buffer &out); 
  *  void IsisMain() { 
  *    // We will be processing by line ProcessByLine p; 
  *    // Setup the input and output cubes 
  *    p.SetInputCube("FROM"); 
  *    p.SetOutputCube ("TO"); 
  *    // Start the processing
  *    p.StartProcess(mirror); 
  *    p.EndProcess(); 
  *  } .
  *  // Line processing routine 
  *  void mirror (Buffer &in, Buffer &out) { 
  *    // Loop and flip pixels in the line. 
  *    int index = in.size() - 1; 
  *    for (int i=0; i<in.size(); i++) { 
  *      out[i] = in[index - i]; 
  *    } 
  *  }                                 
  * @endcode                                                               
  * If you would like to see ProcessByLine being used in implementation with 
  * multiple input cubes, see ratio.cpp                                                 
  *                                                                        
  * @ingroup HighLevelCubeIO                                                  
  *                                                                        
  * @author  2003-02-13 Jeff Anderson                                  
  *                                                                                                                                                                                      
  * @internal                                                                                                                           
  *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
  *                                     isis.astrogeology...
  *  @history 2003-08-29 Jeff Anderson - Fixed bug in StartProcess method for 
  *                                      multiple inputs/outputs which require 
  *                                      cubes to have the same number of lines 
  *                                      and samples.
  *  @history 2004-07-15 Jeff Anderson - Modified to allow for a different 
  *                                      number of samples in the output cube(s) 
  *                                      than are in the input cube(s). This   
  *                                      facilitates the ability to scale or 
  *                                      concatenate in the sample direction.
  *  @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen 
  *                                          documentation
  * 
  *  @history 2006-03-29 Jacob Danton Rewrote code to extend ProcessByBrick class.
  */                                                                       
  class ProcessByLine : public Isis::ProcessByBrick {
  
    public:
      ProcessByLine():ProcessByBrick(){
        SetWrap(true);
      };

      Isis::Cube* SetInputCube (const std::string &parameter,
                                    const int requirements=0);
      Isis::Cube* SetInputCube (const std::string &file, 
                                    Isis::CubeAttributeInput &att,
                                    const int requirements=0);
  
      void StartProcess (void funct(Isis::Buffer &inout));
  
      void StartProcess (void funct(Isis::Buffer &in, Isis::Buffer &out));
      
      void StartProcess (void 
       funct(std::vector<Isis::Buffer *> &in, std::vector<Isis::Buffer *> &out));
  };
};

#endif
