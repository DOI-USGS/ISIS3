#ifndef BandManager_h
#define BandManager_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/18 19:05:48 $                                                                 
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
 * @brief Buffer manager, for moving through a cube in bands  
 *                                                                        
 * This class is used as a manager for moving through a cube one
 * band buffer at a time. A band buffer is defined as a one 
 * dimensional sub-area of a cube. That is, the number of 
 * bands by 1 sample by 1 line (1,1,nb). The manager moves this 
 * (1,1,nb) shape through the cube sequentially accessing all 
 * the band buffers in the first sample before proceeding to the
 * second line. 
 *                                                                        
 * @ingroup LowLevelCubeIO                                                  
 *                                                                        
 * @author 2007-12-06 Christopher Austin 
 *  
 * @internal                                                              
 *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
 * 
 */                                                                       
  class BandManager : public Isis::BufferManager {
  
    public:
      // Constructors and Destructors
      BandManager(const Isis::Cube &cube, const bool reverse=false);

      //! Destroys the SampleManager object
      ~BandManager() {};

      bool SetBand(const int sample, const int line=1 );
  };
};

#endif

