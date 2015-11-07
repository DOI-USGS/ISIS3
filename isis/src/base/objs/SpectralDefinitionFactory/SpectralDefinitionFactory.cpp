/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/01/07 18:33:38 $
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
