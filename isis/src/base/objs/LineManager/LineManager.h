#if !defined(LineManager_h)
#define LineManager_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/12/06 23:51:31 $                                                                 
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
 * @brief Buffer manager, for moving through a cube in lines                
 *                                                                        
 * This class is used as a manager for moving through a cube one line at a time.
 * A line is defined as a one dimensional sub-area of a cube. That is, the 
 * number of samples by 1 line by 1 band (ns,1,1). The manager moves this 
 * (ns,1,1) shape through the cube sequentially accessing all the lines in the 
 * first band before proceeding to the second band.
 *                                                                                                                                      
 * If you would like to see LineManager being used in implementation, 
 * see the ProcessByLine class.                                     
 *                                                                        
 * @ingroup LowLevelCubeIO                                                  
 *                                                                        
 * @author 2003-02-13 Jeff Anderson                                                                           
 *                                                                        
 * @internal                                                              
 *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                     isis.astrogeology...
 *  @history 2005-02-28 Elizabeth Ribelin - Modified file to support Doxygen
 *                                          documentation
 *  @history 2007-12-06 Chris Austin - Added option
 *              to constructor to change the order of the
 *              progression through the cube
 */                                                                       
  class LineManager : public Isis::BufferManager {
  
    public:
      // Constructors and Destructors
      LineManager(const Isis::Cube &cube, const bool reverse=false);

      //! Destroys the LineManager object
      ~LineManager() {};
  
      bool SetLine(const int line, const int band=1 );
  };
};

#endif

