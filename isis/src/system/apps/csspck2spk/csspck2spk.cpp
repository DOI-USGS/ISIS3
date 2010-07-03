//_VER $Id: csspck2spk.cpp,v 1.8 2009/12/04 21:58:45 caustin Exp $
#include "Isis.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Filename.h"
#include "TextFile.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Isis;
using namespace std;

void IsisMain(){
  /* Ingesting the pck2spk into mapped key/values; first grap latest file name,
     and then the individual lines for parsing into the map */
  string pckfile("$cassini/kernels/pck/pck2spk_????.map");
  Filename pckFilenm(pckfile);
  pckFilenm.HighestVersion();
  TextFile txt(pckFilenm.Expanded());
  iString line;
  map< string, string> lookuparray;
  while (txt.GetLine(line)) {
    iString pck = line.Token(",");
    pck.Trim("\n\r\t\v\f ");        // stripping the extraneous characters
    iString spk = line;
    spk.Trim("\n\r\t\v\f ");
    pair<string, string> pckspk(spk,pck);
    lookuparray.insert(pckspk);
  }

  /* Open the input file from the guior find
     the latest version of the db file */
  UserInterface &ui = Application::GetUserInterface();
  string inDBfile;
  if(ui.WasEntered("FROM")) {
    inDBfile = ui.GetFilename("FROM");
  } 
  else {
    string exDBfile("$cassini/kernels/spk/kernels.????.db");
    Filename exDBfilenm(exDBfile);
    exDBfilenm.HighestVersion();      // stores highest version and returns a void
    inDBfile = exDBfilenm.Expanded();
  }

  Filename basepckPath("$base/kernels/pck/kernels.????.db");
  basepckPath.HighestVersion();

  // Read SPK db file into a PVL
  Pvl spkdb (inDBfile);
  Pvl basepck (basepckPath.Expanded());
  PvlKeyword basefile = basepck.FindObject("TargetAttitudeShape").Group(0)["File"];

  //Search PVL for main object
  PvlObject &mainob = spkdb.FindObject("SpacecraftPosition");
  mainob.SetName("TargetAttitudeShape");

  /* Search for the Selection Groups, and the based on the File Keyword, add
     the appropriate pck file keyword.  First check to see if the input file has
     already been updated from an old version of the program. */
  for (int grpIndex =0; grpIndex < mainob.Groups(); grpIndex ++) {
    PvlGroup &grp = mainob.Group(grpIndex);
    if (grp.IsNamed("Selection")) {

      int count = 0;
      for (int keyIndex = 0; keyIndex < grp.Keywords(); keyIndex ++) {        
        if ( grp[keyIndex].IsNamed("File")){
          count ++;

          // Older versions of this program added the file to the SPK kernels DB file instead
          //   of creating a new one in the PCK directory. Check for this.
          if (count > 1) {
            string msg = "This file has already been updated [";
            msg += iString(inDBfile) + "] by an old version of this program.";
            msg += " This is not a valid input.";
            throw iException::Message(iException::User,msg,_FILEINFO_);
          }
        }
      }

      string value = (string)grp["File"];
      Filename fnm(value);
      string filename = fnm.Basename();
      if ( lookuparray[filename] == "" ) {
        std::string msg = "Spk [" + filename + "] does not exist in [" + pckFilenm.Name() + "]";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }
      else {
        PvlKeyword newfile("File", "$cassini/kernels/pck/"+lookuparray[filename]);
        grp.AddKeyword(basefile, Pvl::Replace);
        grp.AddKeyword(newfile);
        grp.DeleteKeyword("Type");
      }
    }
  }

  // Create new db file with the updated contents of the Pvl
  Filename outDBfile;
  if(ui.WasEntered("TO")) {
    outDBfile = ui.GetFilename("TO");
  }
  else{
    outDBfile = "$cassini/kernels/pck/kernels.????.db";
    outDBfile.NewVersion();
  }

  spkdb.Write(outDBfile.Expanded());
}
