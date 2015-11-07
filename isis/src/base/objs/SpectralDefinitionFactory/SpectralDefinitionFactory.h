#ifndef SpectralDefinitionFactory_h
#define SpectralDefinitionFactory_h

/**
 * @file                                                                  
 * $Revision: 6129 $ 
 * $Date: 2015-04-02 10:42:32 -0700 (Thu, 02 Apr 2015) $ $Id: 
 * SpectralDefinitionFactor.h 6129 2015-04-02 17:42:32Z 
 * jwbacker@GS.DOI.NET $ 
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


