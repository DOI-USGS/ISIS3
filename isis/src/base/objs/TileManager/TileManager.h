#if !defined(TileManager_h)
#define TileManager_h
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

#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
/**                                                                       
 * @brief Buffer manager, for moving through a cube in tiles               
 *                                                                        
 * This class is used as a manager for moving through a cube one tile at a time. 
 * A tile is defined as a two dimensional (n samples by m lines) sub area of a 
 * cube. The band direction is always one deep. The sequence of tiles starts 
 * with the tile containing sample one, line one and band one. It then moves 
 * across the cube in the sample direction then to the next tile in the line 
 * direction and finally to the next tile in the band direction. 
 * 
 * If you would like to see TileManager being used in implementation, 
 * see the ProcessByTile class
 *                                                                        
 * @ingroup LowLevelCubeIO                                                  
 *                                                                        
 * @author 2002-10-10 Stuart Sides                                                                           
 *                                                                        
 * @internal
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...                                                              
 *  @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen
 *                                          documentation
 * 
 *  @todo 2005-05-23 Jeff Anderson - There could be problems with 2GB files if
 *  the tile size is very small, 1x1, 2x1, 1x2.  Should we worry about this?
 */                                                                       
  class TileManager : public Isis::BufferManager {
  
    private:
      int p_numSampTiles; //!<Stores the number of tiles in the sample direction
      int p_numLineTiles; //!<Stores the number of tiles in the line direction
  
    public:
      //  Constructors and Destructors
      TileManager(const Isis::Cube &cube, 
               const int &bufNumSamples=128, const int &bufNumLines=128);

      //! Destroys the TileManager object
      ~TileManager() {};
  
      bool SetTile(const int Tile, const int band=1);

     /** 
      * Returns the number of Tiles in the cube.
      * 
      * @return int
      */
      inline int Tiles() { return MaxMaps(); };
  };
};

#endif


