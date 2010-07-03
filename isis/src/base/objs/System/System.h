#ifndef System_h
#define System_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/07/09 17:59:17 $
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

#include <string>
#include "PvlGroup.h"

namespace Isis {
 /**
  * @brief Execute a system command
  *
  * Allows system commands to be executed. These include but are not limited to  
  * running Isis programs.
  *
  * @ingroup Utility
  *
  * @author 2004-01-12 Stuart Sides 
  *
  * @internal 
  *  @history - 2005-02-16 Elizabeth Ribelin - Modified file to support Doxygen 
  *                                            documentation
  * 
  *  @history - 2007-10-04 Steven Koechle - Added GetUnameInfo,
  *                                   GetEnviromentInfo, SystemDiskSpace, and
  *                                   GetLibraryDependencies.
  *  @history - 2008-07-09 SK - fixed problem with one of the printenv calls
  * 
  *  @todo 2005-02-16 Stuart Sides - add coded and implementation examples
  */

    void System (std::string &command);
    Isis::PvlGroup GetUnameInfo ();
    Isis::PvlGroup GetEnviromentInfo ();
    Isis::iString SystemDiskSpace ();
    Isis::iString GetLibraryDependencies (const std::string &file);
}

#endif

