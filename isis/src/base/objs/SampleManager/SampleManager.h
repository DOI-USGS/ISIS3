#ifndef SampleManager_h
#define SampleManager_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/18 18:32:45 $                                                                 
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
 * @brief Buffer manager, for moving through a cube in samples
 *                                                                        
 * This class is used as a manager for moving through a cube one
 * sample buffer at a time. A sample buffer is defined as a one 
 * dimensional sub-area of a cube. That is, the number of 
 * lines by 1 sample by 1 band (1,nl,1). The manager moves this 
 * (1,nl,1) shape through the cube sequentially accessing all 
 * the sample buffer in the first band before proceeding to the 
 * second band. 
 *                                                                        
 * @ingroup LowLevelCubeIO                                                  
 *                                                                        
 * @author 2007-12-06 Christopher Austin 
 *  
 * @internal                                                              
 *  @history 2007-12-06 Christopher Austin Original Version
 *  @history 2008-06-18 Steven Lambright Fixed documentation
 * 
 */                                                                       
  class SampleManager : public Isis::BufferManager {
  
    public:
      // Constructors and Destructors
      SampleManager(const Isis::Cube &cube, const bool reverse=false);

      //! Destroys the SampleManager object
      ~SampleManager() {};

      bool SetSample(const int sample, const int band=1 );
  };
};

#endif

