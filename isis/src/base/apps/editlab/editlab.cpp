#include "Isis.h"

#include "Cube.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

using namespace Isis;
using namespace std;

PvlKeyword &modifyKeyword(UserInterface &ui, PvlKeyword &keyword);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Extract label from file
  Pvl *label = new Pvl(ui.GetFileName("FROM"));
  PvlObject *pvl = label;
  string option = ui.GetString("OPTION");

  // Get user entered option & create IsisCube Object if needed
  Cube *cube = NULL;
  if(label->HasObject("IsisCube")) {
    cube = new Cube();
    cube->open(ui.GetFileName("FROM"), "rw");
    pvl = &(cube->getLabel()->FindObject("IsisCube"));
  }

  // Add Template File
  if(option == "ADDTEMP") {
    string tempfile = ui.GetFileName("TEMPFILE");
    Pvl tempobj(tempfile);
    for(int i = 0; i < tempobj.Groups(); ++i) {
      pvl->AddGroup(tempobj.Group(i));
    }
  }

  else {
    string grpname = ui.GetString("GRPNAME");

    // Add Group
    if(option == "ADDG") {
      PvlGroup g(grpname);
      if(ui.WasEntered("COMMENT"))
        g.AddComment(ui.GetString("COMMENT"));
      pvl->AddGroup(g);
    }

    // Delete Group
    else if(option == "DELG") {
      pvl->DeleteGroup(grpname);
    }

    else {
      string key = ui.GetString("KEYWORD");
      PvlGroup &grp = pvl->FindGroup(grpname, PvlObject::Traverse);

      // Add Keyword
      if(option == "ADDKEY") {
        PvlKeyword keywrd(key);
        grp.AddKeyword(modifyKeyword(ui, keywrd));
      }

      // Delete Keyword
      else if(option == "DELKEY") {
        grp.DeleteKeyword(key);
      }

      // Modify Keyword
      else if(option == "MODKEY") {
        modifyKeyword(ui, grp.FindKeyword(key));
      }

      // Set Keyword
      else if(option == "SETKEY") {
        if(grp.HasKeyword(key)) {
          PvlKeyword *first = NULL;
          // Clean duplicate keywords of ONLY the provided keyword
          for(int i = 0; i < grp.Keywords(); i++) {
            if(grp[i].IsNamed(key)) {
              if(not first)
                first = &grp[i];
              else
                grp.DeleteKeyword(i);
            }
          }
          modifyKeyword(ui, *first);
        }
        else {
          PvlKeyword keywrd(key);
          grp.AddKeyword(modifyKeyword(ui, keywrd));
        }
      }

    }
  }

  // Write and clean the data
  if(cube) {
    cube->close();
    delete cube;
    cube = NULL;
  }
  else {
    label->Write(ui.GetFileName("FROM"));
  }

  delete label;
  label = NULL;
}

PvlKeyword &modifyKeyword(UserInterface &ui, PvlKeyword &keyword) {
  if(ui.WasEntered("UNITS"))
    keyword.SetValue(ui.GetString("VALUE"), ui.GetString("UNITS"));
  else
    keyword.SetValue(ui.GetString("VALUE"));
  if(ui.WasEntered("COMMENT"))
    keyword.AddComment(ui.GetString("COMMENT"));
  return keyword;
}
