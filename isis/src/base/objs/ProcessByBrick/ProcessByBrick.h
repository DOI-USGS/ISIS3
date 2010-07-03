#ifndef ProcessByBrick_h
#define ProcessByBrick_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/06/18 20:59:40 $
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

#include "Process.h"
#include "Buffer.h"

namespace Isis {
/**                                                                       
 * @brief Process cubes by brick               
 *                                                                        
 * This is the processing class used to move a brick through cube data. This 
 * class allows only one input cube and one output cube or one input cube. If 
 * the brick size does not evenly divide into the image the brick will be padded
 * with Null pixels as it falls off the right and/or bottom edge of the image. 
 * The brick shape is only spatial-oriented with one band of data.             
 *                                                                        
 * @ingroup HighLevelCubeIO                                                  
 *                                                                        
 * @author 2006-03-24 Jacob Danton                                     
 *                                                                                                                                                                                      
 * @internal                                                              
 *  @history 2005-02-08 Jacob Danton - Original Version
 *  @history 2006-08-08 Debbie A. Cook - Added overloaded method
 *           SetInputCube
 *  @history 2007-12-14 Steven Lambright - Same band is now forced 
 *           on input cubes when there are multiple input cubes
 *  @history 2008-01-09 Steven Lambright - Fixed a memory leak
 *  @history 2008-06-18 Steven Koechle - Fixed Documentation
 */                                                                       
  class ProcessByBrick : public Isis::Process {
  
   private:
    bool p_wrapOption;    //!<Indicates whether the brick manager will wrap
    bool p_inputBrickSizeSet;  //!<Indicates whether the brick size has been set
    bool p_outputBrickSizeSet;  //!<Indicates whether the brick size has been set
    std::vector<int> p_inputBrickSamples;   //!<Number of samples in the input bricks
    std::vector<int> p_inputBrickLines;     //!<Number of lines in the input bricks
    std::vector<int> p_inputBrickBands;     //!<Number of bands in the input bricks
    std::vector<int> p_outputBrickSamples;   //!<Number of samples in the output bricks
    std::vector<int> p_outputBrickLines;     //!<Number of lines in the output bricks
    std::vector<int> p_outputBrickBands;     //!<Number of bands in the output bricks
  
    public:

      //! Constructs a ProcessByBrick object
      ProcessByBrick ();

      //! Destroys the ProcessByBrick object
      ~ProcessByBrick () {};
  
      Isis::Cube* SetInputCube (const std::string &parameter,
                                    const int requirements=0);

      Isis::Cube* SetInputCube (const std::string &fname,
                                    const Isis::CubeAttributeInput &att,
                                    const int requirements=0);

      void SetBrickSize (const int ns, const int nl, const int nb);

      void SetInputBrickSize (const int ns, const int nl, const int nb);
      void SetInputBrickSize (const int ns, const int nl, const int nb, const int tile);

      void SetOutputBrickSize (const int ns, const int nl, const int nb);
      void SetOutputBrickSize (const int ns, const int nl, const int nb, const int tile);

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
      void SetWrap(bool wrap) {p_wrapOption = wrap;};

      /**
       * Return the wrap option
       *
       * @return bool
       */
      bool Wraps() {return p_wrapOption;};
  
      void StartProcess (void funct(Isis::Buffer &in));
      void StartProcess (void funct(Isis::Buffer &in, Isis::Buffer &out));
      void StartProcess (void funct(std::vector<Isis::Buffer *> &in,
                                    std::vector<Isis::Buffer *> &out));
      void EndProcess ();
  };
};

#endif
