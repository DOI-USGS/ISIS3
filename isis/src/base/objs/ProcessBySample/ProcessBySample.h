#ifndef ProcessBySample_h
#define ProcessBySample_h
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
  * @brief Process cubes by sample                
  *                                                                        
  * This class allows a programmer to develop a program which process input and 
  * output cubes sequentially by sample. That is, receive the input data for sample 
  * one, manipulate the data, and pass back the data for output sample one. Then 
  * proceed to the sample two and so one. This class is derived from the Process 
  * class which give many functions for setting up input and output cubes.
  *                                                 
  *                                                                        
  * @ingroup HighLevelCubeIO                                                  
  *                                                                        
  * @author  2006-03-27 Jacob Danton
  *                                                                                                                                                                                      
  * @internal                                                                                                                           
  *  @history 2006-03-27 Jacob Danton - Original Version
  */                                                                       
  class ProcessBySample : public Isis::ProcessByBrick {
  
    public:
      ProcessBySample():ProcessByBrick(){
        SetWrap(true);
      };

      Isis::Cube* SetInputCube (const std::string &parameter,
                                    const int requirements=0);
      Isis::Cube* SetInputCube (const std::string &file, 
                                    Isis::CubeAttributeInput &att,
                                    const int requirements=0);
  
      void StartProcess (void funct(Isis::Buffer &inout));
  
      void StartProcess (void funct(Isis::Buffer &in, Isis::Buffer &out));
      
      void StartProcess (void funct(std::vector<Isis::Buffer *> &in,
                                    std::vector<Isis::Buffer *> &out));
  };
};

#endif
