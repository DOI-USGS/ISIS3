/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "SpectralDefinitionFactory.h"
#include "FileName.h" 
#include "SpectralDefinition1D.h"
#include "SpectralDefinition2D.h"

namespace Isis {
  SpectralDefinition* SpectralDefinitionFactory::NewSpectralDefinition(FileName smileDefFilename){
    //csvs will be output definitions, others will (hopefully) be input defs
    //TODO: make "smarter" (actually open file & read extension)
    if (smileDefFilename.extension() == "csv"){
      return new SpectralDefinition1D(smileDefFilename); 
    } 
    else {
      return new SpectralDefinition2D(smileDefFilename);
    }
  }
}
