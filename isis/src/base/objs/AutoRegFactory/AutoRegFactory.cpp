/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/19 23:35:38 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                     

#include "AutoRegFactory.h"
#include "AutoReg.h"
#include "Plugin.h"
#include "iException.h"
#include "Filename.h"

namespace Isis {
  /** 
   * Create an AutoReg object using a PVL specification.
   * An example of the PVL required for this is:
   * 
   * @code
   * Object = AutoRegistration
   *   Group = Algorithm
   *     Name      = MaximumCorrelation
   *     Tolerance = 0.7
   *   EndGroup
   * 
   *   Group = PatternChip
   *     Samples = 21
   *     Lines   = 21
   *   EndGroup
   * 
   *   Group = SearchChip
   *     Samples = 51
   *     Lines = 51
   *   EndGroup
   * EndObject
   * @endcode
   * 
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   * 
   * @param pvl The pvl object containing the specification
   * 
   * @see automaticRegistration.doc
   **/
  AutoReg *AutoRegFactory::Create(Pvl &pvl) {
    // Get the algorithm name to create
    PvlGroup &algo = pvl.FindGroup("Algorithm",Pvl::Traverse);
    std::string algorithm = algo["Name"];

    // Open the factory plugin file
    Plugin p;
    Filename f("AutoReg.plugin");
    if (f.Exists()) {
      p.Read("AutoReg.plugin");
    }
    else {
      p.Read("$ISISROOT/lib/AutoReg.plugin");
    }

    // Get the algorithm specific plugin and return it
    AutoReg * (*plugin) (Pvl &pvl);
    plugin = (AutoReg * (*)(Pvl &pvl)) p.GetPlugin(algorithm);
    return (*plugin)(pvl);
  }
} // end namespace isis
