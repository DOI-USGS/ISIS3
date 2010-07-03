/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:10 $                                                                 
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

#include "TileManager.h"

using namespace std;
namespace Isis {

 /** 
  * Constructs a TileManager object
  * 
  * @param cube  The cube this buffer will be assiciated with.
  * 
  * @param bufNumSamples  The number of samples in each tile buffer.
  *                       Defaults to 128
  * 
  * @param bufNumLines The number of lines in each tile buffer. Defaults to 128
  * 
  */
  TileManager::TileManager(const Isis::Cube &cube,
                     const int &bufNumSamples, const int &bufNumLines) :
                     Isis::BufferManager(cube.Samples(),cube.Lines(),cube.Bands(),
                                       bufNumSamples,bufNumLines,1,
                                       cube.PixelType()) {
  
    p_numSampTiles = (cube.Samples() - 1) / bufNumSamples + 1;
    p_numLineTiles = (cube.Lines() - 1) / bufNumLines + 1;
  }
  
 /** 
  * Sets the current tile as requested
  * 
  * @param tile  The tile number within a band. This number starts with the 
  *              upper left corner of the cube and proceedes across the samples 
  *              then down the lines. The upper left tile of each band is always
  *              tile one (1) in band (n).
  * 
  * @param band The band number within the cube. The first band in a cube is 
  *             always one (1).
  * 
  * @return bool
  * 
  * @throws Isis::iException::Programmer - invalid argument value
  */
  bool TileManager::SetTile (const int tile, const int band) {
    if (tile < 1) {
      string message = "Invalid value for argument [tile]";
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_); 
    }
  
    if (band < 1) {
      string message = "Invalid value for argument [band]";
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_); 
    }
  
    int map = (band - 1) * (p_numSampTiles * p_numLineTiles) + tile - 1;
    
    return setpos(map);
  }
} // end namespace isis

