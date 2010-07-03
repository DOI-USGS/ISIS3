//  $Id: mdisedrinfo.cpp,v 1.8 2008/07/11 22:38:41 nhilt Exp $
#include "Isis.h"

#include <cstdio>
#include <string>
#include <vector> 
#include <algorithm>
#include <sstream>
#include <iostream>

#include "Filename.h"
#include "UserInterface.h"
#include "Cube.h"
#include "OriginalLabel.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iString.h"

#include "MdisGeometry.h"
#include "MdisEdrKeys.h"

using namespace Isis;
using namespace std;


void IsisMain(){

//  Input parameters
  UserInterface &ui = Application::GetUserInterface();
  string sourceFile = ui.GetAsString("FROM");    //  Save off source filename
  Filename from(sourceFile);

  string to;
  if (ui.WasEntered("TO")) to = ui.GetAsString("TO");
  bool delete_from(false);

//  Is there a separate file containing keywords to report?
  string keylist;
  if (ui.WasEntered("KEYLIST")) keylist = ui.GetAsString("KEYLIST");
  else keylist = to;

//  Get PVL parameter
  string pvl;
  if (ui.WasEntered("PVL")) pvl = ui.GetAsString("PVL");

// Run mdis2isis if necessary.  If this step must be done, we must also
//  run spiceinit to initialize it with spice.
  if (iString::UpCase(from.Extension()) != "CUB") {
    Filename temp;
    temp.Temporary(from.Basename(), "cub");
    string params = "from=" + from.Expanded()  + " to=" + temp.Expanded();
  
    try {
      iApp->Exec("mdis2isis", params);

// Ensure a proper target before initialization
      Cube cube;
      cube.Open(temp.Expanded(), "rw");
      Pvl *label = cube.Label();
      MdisGeometry::validateTarget(*label, true);
      cube.Close();

      //  Run spiceinit on it
      iApp->Exec("spiceinit", "from="+temp.Expanded());
    } catch (iException &ie ) {
      string tempName(temp.Expanded());
      remove (tempName.c_str());
      ie.Message(iException::User,"Failed to execute mdis2isis/spiceinit",
                  _FILEINFO_);
      throw;
    }

    //  FROM file is now the ISIS cube.  Stage cube file for deletion as well.
    from = temp;
    delete_from = true;
  }

//  Now process the label and instantiate the camera model(s)
  try {

//  Get the orginal PDS EDR labels and initialize the keyword map
    Pvl edrlab = OriginalLabel(from.Expanded()).ReturnLabels();
    MdisEdrKeys edrkeys(edrlab);

//  Compute the Geometry
    MdisGeometry geom(from.Expanded());
    Pvl geomkeys = geom.getGeometry(sourceFile);
    edrkeys.updateKeys(geomkeys);

    PvlGroup mdiskeys("MdisPdsKeys");

//  Only process the PDS EDR keyword mapping if KEYMAP (or TO) is entered.
    if (!keylist.empty()) {
      Filename kmap(keylist);
      if (!kmap.Exists()) {
        string mess = "EDR keyword map source file, " + kmap.Expanded() 
                       + ", does not exist!";
        throw iException::Message(iException::User, mess.c_str(), _FILEINFO_);
      }
  
  // Get the keylist source line
      string kmapName(kmap.Expanded());
      ifstream ifile(kmapName.c_str(), ios::in);
      if (!ifile) {
        string mess = "Unable to open key map source file " + kmap.Expanded();
        throw iException::Message(iException::User, mess, _FILEINFO_);
      }
  
      string keystring;
      if (!getline(ifile, keystring)) {
        string mess = "I/O error reading key map line from  " + kmap.Expanded();
        throw iException::Message(iException::User, mess, _FILEINFO_);
      }
  
      ifile.close();
  
      //  Split the line using semi-colons as the delimiter
      vector<string> keys;
      iString::Split(';', keystring, keys);
      std::string keyvalues = edrkeys.extract(keys, geom.getNull(), &mdiskeys);

      if (!to.empty()) {
        //  Now open the output file and write the result
        Filename tomap(to);
        string tomapName(tomap.Expanded());
        bool toExists = tomap.Exists();
        ofstream ofile;
        if (toExists) {
          ofile.open(tomapName.c_str(), std::ios::out|std::ios::app);
        }
        else {
          ofile.open(tomapName.c_str(), std::ios::out);
        }
    
        if (!ofile) {
          string mess = "Could not open or create output TO file " + 
                        tomapName;
          throw iException::Message(iException::User, mess, _FILEINFO_);
        }
    
        //  Write the header if requested by the user
        if (!toExists) ofile << keystring << endl;
        ofile << keyvalues << endl;
        ofile.close();
      }
    }
    else {
      PvlContainer::PvlKeywordIterator keyIter = geomkeys.Begin();
      for ( ; keyIter != geomkeys.End() ;  ++keyIter) {
        mdiskeys.AddKeyword(*keyIter);
      }
    }

//  See if the user wants to write out the PVL keywords
    if (!pvl.empty()) {
      Pvl pout;
      pout.AddGroup(mdiskeys);
      pout.Write(pvl);
    }

//  Log the results to the log/terminal/gui
    Application::Log(mdiskeys);
  } 
  catch (iException &ie) {

    string fromName(from.Expanded());
    if (delete_from) remove (fromName.c_str());
    throw;
  }
  
// Delete the from file if it was temporarily created from an EDR
  string fromName(from.Expanded());
  if (delete_from) remove (fromName.c_str());
}

