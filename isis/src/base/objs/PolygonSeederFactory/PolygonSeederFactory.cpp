/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/12/17 21:43:53 $                                                                 
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

#include "PolygonSeederFactory.h"
#include "Plugin.h"
#include "iException.h"
#include "Filename.h"

namespace Isis {
  /** 
   * Create a PolygonSeeder object using a PVL specification.
   * An example of the PVL required for this is:
   * 
   * @code
   * Object = AutoSeed
   *   Group = PolygonSeederAlgorithm
   *     Name      = Grid
   *     Tolerance = 0.7
   *   EndGroup
   * 
   *   Group = InterestOperatorAlgorithm
   *     Name      = StandardDeviation
   *     Tolerance = 10
   *   EndGroup
   * 
   * EndObject
   * @endcode
   * 
   * @param pvl The pvl object containing the PolygonSeeder specification
   * 
   **/
  PolygonSeeder *PolygonSeederFactory::Create(Pvl &pvl) {
    // Get the algorithm name to create
    PvlGroup &algo = pvl.FindGroup("PolygonSeederAlgorithm",Pvl::Traverse);
    std::string algorithm = algo["Name"];

    // Open the factory plugin file
    Plugin p;
    Filename f("PolygonSeeder.plugin");
    if (f.Exists()) {
      p.Read("PolygonSeeder.plugin");
    }
    else {
      p.Read("$ISISROOT/lib/PolygonSeeder.plugin");
    }

    // Get the algorithm specific plugin and return it
    PolygonSeeder* (*plugin) (Pvl &pvl);
    plugin = (PolygonSeeder * (*)(Pvl &pvl)) p.GetPlugin(algorithm);
    return (*plugin)(pvl);
  }
} // end namespace isis
