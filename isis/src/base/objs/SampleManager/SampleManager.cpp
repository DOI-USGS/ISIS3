
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

#include "SampleManager.h"

using namespace std;
namespace Isis {

 /** 
  * Constructs a SampleManager object
  * 
  * @param cube The cube this buffer manager will be associated with. 
  *  
  * @param reverse Modifies the order of progression 
  *             SampleManager takes through the cube.  By
  *             default, progresses samples first then bands. If
  *             reverse = true, then the buffer progresses bands
  *             first, then samples.
  */

  SampleManager::SampleManager(const Isis::Cube &cube, const bool reverse) : 
                     Isis::BufferManager(cube.Samples(),cube.Lines(),
                                       cube.Bands(),1,cube.Lines(),1,
                                       cube.PixelType(), reverse ) {
  }

 /** 
  * Positions the buffer at the requested line and returns a status indicator 
  * if the set was succesful or not
  *
  * @param sample The sample number within a band (1-based).
  * @param band The band number within the cube (1-based). Defaults to 1
  * 
  * @return bool Status indicator of the set being successful or not
  */

  bool SampleManager::SetSample (const int sample, const int band) {
    if (sample < 1) {
      string message = "Invalid value for argument [sample]";
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_); 
    }
  
    if (band < 1) {
      string message = "Invalid value for argument [band]";
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_); 
    }
  
    int map = (band - 1) * MaxSamples() + sample - 1;
    return setpos(map);
  }

} // end namespace isis
