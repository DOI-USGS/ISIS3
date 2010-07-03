#ifndef ProcessBySpectra_h
#define ProcessBySpectra_h
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
  * @brief Process cubes by spectra                
  *                                                                        
  * This class allows a programmer to develop a program which process input and 
  * output cubes sequentially by spectra. That is, receive the input data for spectra 
  * one, manipulate the data, and pass back the data for output spectra one. Then 
  * proceed to the spectra two and so one. This class is derived from the ProcessByBrick 
  * class which give many functions for setting up input and output cubes.
  *                                                  
  *                                                                        
  * @ingroup HighLevelCubeIO                                                  
  *                                                                        
  * @author  2006-03-27 Jacob Danton
  *                                                                                                                                                                                      
  * @internal                                                                                                                           
  *  @history 2006-03-27 Jacob Danton - Original Version
  *  @history 2006-08-07 Tracie Sucharski, Fixed bug in StartProcess with
  *                        a single input buffer.  Error checks and set-up
  *                        of brick was not being done correctly.
  * 
  */                                                                       
  class ProcessBySpectra : public Isis::ProcessByBrick {
    private:
      int p_spectraType;
  
    public:
      ProcessBySpectra(const int type=PerPixel):ProcessByBrick(){
        SetType(type);
      };

      Isis::Cube* SetInputCube (const std::string &parameter,
                                    const int requirements=0);
      Isis::Cube* SetInputCube (const std::string &file, 
                                    Isis::CubeAttributeInput &att,
                                    const int requirements=0);

      void SetType (const int type);
      int Type() {return p_spectraType;};
  
      void StartProcess (void funct(Isis::Buffer &in));
  
      void StartProcess (void funct(Isis::Buffer &in, Isis::Buffer &out));
      
      void StartProcess (void funct(std::vector<Isis::Buffer *> &in,
                                    std::vector<Isis::Buffer *> &out));

      static const int PerPixel = 0;
      static const int ByLine = 1;
      static const int BySample = 2;
  };
};

#endif
