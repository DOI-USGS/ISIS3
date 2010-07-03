#include "Isis.h"
#include "Pvl.h"
#include "Cube.h"
#include "Blob.h"
#include "History.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Isis;
using namespace std;

void IsisMain(){
            
  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  Cube cube;
  cube.Open(ui.GetFilename("FROM"), "rw");

  // Extract label from cube file
  Pvl *label = cube.Label();

  // Get user entered option & create IsisCube Object
  string option = ui.GetString("OPTION");   
  PvlObject &o = label->FindObject("IsisCube");

  // Add Template File
  if(option=="ADDTEMP") {
    string tempfile = ui.GetFilename("TEMPFILE");
    Pvl tempobj(tempfile);
    for(int i=0; i<tempobj.Groups(); ++i) {
      o.AddGroup(tempobj.Group(i));
    }
  }

  else {
    string grpname = ui.GetString("GRPNAME");

    // Delete Group
    if(option=="DELG") {
      o.DeleteGroup(grpname);
    }

    // Add Group
    else if(option=="ADDG") {
      PvlGroup g(grpname);
      if( ui.WasEntered("COMMENT") ) {
        string cmmnt = ui.GetString("COMMENT");
        g.AddComment( cmmnt );
      }
      o.AddGroup(g);
    }

    // Delete Keyword
    else if(option=="DELKEY") {
      string key = ui.GetString("KEYWORD");
      o.FindGroup(grpname).DeleteKeyword(key);
    }

    // Add Keyword
    else if(option=="ADDKEY") {
      string key = ui.GetString("KEYWORD");
      iString val = ui.GetString("VALUE");
      PvlKeyword keywrd(key);
      if(ui.WasEntered("UNITS")){
        string unit = ui.GetString("UNITS");
        keywrd.SetValue(val, unit); 
      }
      else {
        keywrd.SetValue(val);
      }
      if( ui.WasEntered("COMMENT") ) {
        string cmmnt = ui.GetString("COMMENT");
        keywrd.AddComment( cmmnt );
      }
      o.FindGroup(grpname).AddKeyword(keywrd);
    }

    // Modify Keyword
    else if(option=="MODKEY") {
      string key = ui.GetString("KEYWORD");
      PvlKeyword &keywrd = o.FindGroup(grpname).FindKeyword(key);
      string val = ui.GetString("VALUE");
      if(ui.WasEntered("UNITS")) {
        string unit = ui.GetString("UNITS");
        keywrd.SetValue(val, unit);
      }
      else {
        keywrd.SetValue(val);
      }
      if( ui.WasEntered("COMMENT") ) {
        string cmmnt = ui.GetString("COMMENT");
        keywrd.AddComment( cmmnt );
      }
    }
  }
   /* History hist = History("IsisCube");
    try {
        cube.Read(hist);
    }
    catch(iException &e) {
        e.Clear();
    }
    hist.AddEntry();
    cube.Write(hist); */
    cube.Close();  
}
