#ifndef SpectralDefinitionFactory_h
#define SpectralDefinitionFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "SpectralDefinition.h"

namespace Isis{

  /**
  * @brief Constructs a 2D or 1D spectral definition based on the file name extension 
  *  
  * @author 2015-05-11 Kristin Berry  
  *  
  * @internal
  *  @history 2015-06-16 Stuart Sides - Added the doc header
  */
  class SpectralDefinitionFactory {
    public:
      static SpectralDefinition* NewSpectralDefinition(FileName smileDefFilename); 
  };
} 

#endif


