/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2007/02/20 16:55:12 $                                                                 
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

#include "NormModelFactory.h"
#include "NormModel.h"
#include "Plugin.h"
#include "iException.h"
#include "Filename.h"

namespace Isis {
  /** 
   * Create a NormModel object using a PVL specification.
   * An example of the PVL required for this is:
   * 
   * @code
   * Object = NormalizationModel
   *   Group = Algorithm
   *     Name = ShadeAtm
   *     PhotoModel = Minnaert
   *     AtmosModel = Isotropic1
   *   EndGroup
   * EndObject
   * @endcode
   * 
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   * 
   * @param pvl The pvl object containing the specification
   * 
   * @see normalizationModels.doc
   **/
  NormModel *NormModelFactory::Create(Pvl &pvl, PhotoModel &pmodel) {
    // Get the algorithm name to create
    PvlGroup &algo = pvl.FindObject("NormalizationModel")
                     .FindGroup("Algorithm",Pvl::Traverse);
    std::string algorithm = algo["Name"];

    // Open the factory plugin file
    Plugin *p = new Plugin;
    Filename f("NormModel.plugin");
    if (f.Exists()) {
      p->Read("NormModel.plugin");
    }
    else {
      p->Read("$ISISROOT/lib/NormModel.plugin");
    }

    // Get the algorithm specific plugin and return it
    NormModel * (*plugin) (Pvl &pvl, PhotoModel &pmodel);
    plugin = (NormModel * (*)(Pvl &pvl, PhotoModel &pmodel)) 
        p->GetPlugin(algorithm);
    return (*plugin)(pvl, pmodel);
  }

  /** 
   * Create a NormModel object using a PVL specification.
   * An example of the PVL required for this is:
   * 
   * @code
   * Object = NormalizationModel
   *   Group = Algorithm
   *     Name = ShadeAtm
   *     PhotoModel = Minnaert
   *     AtmosModel = Isotropic1
   *   EndGroup
   * EndObject
   * @endcode
   * 
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   * 
   * @param pvl The pvl object containing the specification
   * 
   * @see normalizationModels.doc
   **/
  NormModel *NormModelFactory::Create(Pvl &pvl, PhotoModel &pmodel,
      AtmosModel &amodel) {
    // Get the algorithm name to create
    PvlGroup &algo = pvl.FindObject("NormalizationModel")
                     .FindGroup("Algorithm",Pvl::Traverse);
    std::string algorithm = algo["Name"];

    // Open the factory plugin file
    Plugin *p = new Plugin;
    Filename f("NormModel.plugin");
    if (f.Exists()) {
      p->Read("NormModel.plugin");
    }
    else {
      p->Read("$ISISROOT/lib/NormModel.plugin");
    }

    // Get the algorithm specific plugin and return it
    NormModel * (*plugin) (Pvl &pvl, PhotoModel &pmodel, 
        AtmosModel &amodel);
    plugin = (NormModel * (*)(Pvl &pvl, PhotoModel &pmodel,
        AtmosModel &amodel)) 
        p->GetPlugin(algorithm);
    return (*plugin)(pvl, pmodel, amodel);
  }
} // end namespace isis
