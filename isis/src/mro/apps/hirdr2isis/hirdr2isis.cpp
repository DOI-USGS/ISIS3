#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Filename.h"
#include "Spice.h"

using namespace std; 
using namespace Isis;

void IsisMain ()
{
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  string labelFile = ui.GetFilename("FROM");

  p.SetPdsFile (labelFile, "", label);
  Cube *ocube = p.SetOutputCube("TO");

  p.StartProcess ();

  // Get the mapping labels
  Pvl otherLabels;
  p.TranslatePdsProjection(otherLabels);

  // Get the directory where the MRO HiRISE RDR translation tables are.
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Mro"] + "/translations/";

  // Translate the BandBin group
  Filename transFile (transDir + "hiriseRdrBandBin.trn");
  PvlTranslationManager bandBinXlater (label, transFile.Expanded());
  bandBinXlater.Auto(otherLabels);

  // Translate the Mosaic group
  transFile = transDir + "hiriseRdrMosaic.trn";
  PvlTranslationManager archiveXlater (label, transFile.Expanded());
  archiveXlater.Auto(otherLabels);

  // Write the BandBin, Archive, and Mapping groups to the output cube label
  ocube->PutGroup(otherLabels.FindGroup("BandBin"));

  // Reorder CPMM keywords back to original arrangement.  This copies the values
  //  back to keywords in place.
  PvlGroup mosgrp = otherLabels.FindGroup("Mosaic");
  PvlKeyword &ccdFlag = mosgrp.FindKeyword("SpecialProcessingFlag");
  PvlKeyword &ccdBin = mosgrp.FindKeyword("cpmmSummingFlag");
  PvlKeyword &ccdTdi = mosgrp.FindKeyword("cpmmTdiFlag");

  //  Make temp copies of keywords
  PvlKeyword tempccdFlag = ccdFlag;
  PvlKeyword tempccdBin = ccdBin;
  PvlKeyword tempccdTdi = ccdTdi;
  const unsigned int cpmmByCcd[] = {0,1,2,3,5,8,10,11,12,13,6,7,4,9};
  for (int ccd=0; ccd<14; ++ccd) {
    ccdFlag[cpmmByCcd[ccd]] = tempccdFlag[ccd];
    ccdBin[cpmmByCcd[ccd]]  = tempccdBin[ccd];
    ccdTdi[cpmmByCcd[ccd]]  = tempccdTdi[ccd];
  }
  ocube->PutGroup(mosgrp);

//  Modify the output Mosaic group if the Projection is of type
//  Equirectangular.
  PvlGroup mapgrp = otherLabels.FindGroup("Mapping");
  if ( iString::UpCase(mapgrp["ProjectionName"]) == "EQUIRECTANGULAR" ) {
    static bool pckLoaded(false);
    if ( !pckLoaded ) {
      Filename pck("$base/kernels/pck/pck?????.tpc");
      pck.HighestVersion();
      furnsh_c(pck.Expanded().c_str());
      pckLoaded = true;
    }

    //  Get the target and check for validity
    PvlKeyword &target = label.FindKeyword("TargetName", PvlObject::Traverse);
    SpiceInt tcode;
    SpiceBoolean found;
    (void) bodn2c_c(target[0].c_str(), &tcode, &found);
    if (found) {
      // Get radii and fix labels
      SpiceInt n;
      SpiceDouble radii[3];
      bodvar_c(tcode,"RADII",&n,radii);

      // Set existing radius to CenterLatitudeRadius
      PvlKeyword &eqRadius =  mapgrp.FindKeyword("EquatorialRadius"); 
      PvlKeyword &polRadius = mapgrp.FindKeyword("PolarRadius"); 

      // Derive (copy, actually) the center radius from the equator radii and 
      // update the name
      PvlKeyword clatrad =  eqRadius;
      clatrad.SetName("CenterLatitudeRadius");

      //  Assign the proper radii to the group keywords
      eqRadius.SetValue(iString(radii[0]*1000.0), "meters");
      polRadius.SetValue(iString(radii[2]*1000.0), "meters");
      mapgrp += clatrad;  // Don't do this before updating the above 
                          // keyword references!  Bad things happen!
    }
  }

  //  Write the group to the label
  ocube->PutGroup(mapgrp);

  p.EndProcess ();

  return;
}
