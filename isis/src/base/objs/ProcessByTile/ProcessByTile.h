#ifndef ProcessByTile_h
#define ProcessByTile_h
/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:09 $
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
 * @brief Process cubes by tile               
 *                                                                        
 * This is the processing class used to move a tile through cube data. This 
 * class allows only one input cube and one output cube or one input cube. If 
 * the tile size does not evenly divide into the image the tile will be padded
 * with Null pixels as it falls off the right and/or bottom edge of the image. 
 * The tile shape is only spatial-oriented with one band of data.             
 *                                                                        
 * @ingroup HighLevelCubeIO                                                  
 *                                                                        
 * @author 2004-05-14 Jeff Anderson                                     
 *                                                                                                                                                                                      
 * @internal                                                              
 *  @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen 
 *                                          documentation          
 *  @history 2005-11-28 Jacob Danton - Modified file to allow processing
 *                                     with multiple input and output cubes.
 *  @history 2006-04-03 Jacob Danton - Rewrote the code to extend ProcessByBrick class
 * 
 *  @todo 2005-02-08 Jeff Anderson - add coded example, and implementation 
 *                                   example to class documentation
 */                                                                       
  class ProcessByTile : public Isis::ProcessByBrick {
  
   private:
    bool p_tileSizeSet;  //!<Indicates whether the tile size has been set
    int p_tileSamples;   //!<Number of samples in the tile
    int p_tileLines;     //!<Number of lines in the tile
  
    public:

      //! Constructs a ProcessByTile object
      ProcessByTile () {p_tileSizeSet=false;};

      //! Destroys the ProcessByTile object
      ~ProcessByTile () {};
  
      void SetTileSize (const int ns, const int nl);
  
      void StartProcess (void funct(Isis::Buffer &in));
      void StartProcess (void funct(Isis::Buffer &in, Isis::Buffer &out));

      void StartProcess (void funct(std::vector<Isis::Buffer *> &in,
                                    std::vector<Isis::Buffer *> &out));
      void EndProcess ();
  };
};

#endif
