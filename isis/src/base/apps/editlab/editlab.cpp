#include "Isis.h"

#include "Cube.h"
#include "History.h"
#include "Process.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

using namespace Isis;
using namespace std;

PvlKeyword &modifyKeyword(UserInterface &ui, PvlKeyword &keyword);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Extract label from file
  Pvl *label = new Pvl(ui.GetFileName("FROM"));
  PvlObject *pvl = label;
  QString option = ui.GetString("OPTION");

  // Get user entered option & create IsisCube Object if needed
  Cube *cube = NULL;
  if(label->HasObject("IsisCube")) {
    cube = new Cube();
    cube->open(ui.GetFileName("FROM"), "rw");
    pvl = &(cube->label()->FindObject("IsisCube"));
  }

  // Add Template File
  if(option == "ADDTEMP") {
    QString tempfile = ui.GetFileName("TEMPFILE");
    Pvl tempobj(tempfile);
    for(int i = 0; i < tempobj.Groups(); ++i) {
      pvl->AddGroup(tempobj.Group(i));
    }
  }

  else {
    QString grpname = ui.GetString("GRPNAME");

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
      QString key = ui.GetString("KEYWORD");
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

  // Add history, write, and clean the data
  if(cube) {
    History hist = History("IsisCube");
    try {
      // read history from cube, if it exists.
      cube->read(hist);
    }
    catch(IException &e) {
      // if the history does not exist in the cube, continue. In this case, 
      // editlab will be the first History entry.
    }
    hist.AddEntry();
    cube->write(hist);

    // clean up
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

/**
 * Modifies the given keyword with the user entered value, units, and/or
 * comment.
 * 
 * @param ui UserInterface object for this application.
 * @param keyword PvlKeyword to be modified.
 * 
 * @return PvlKeyword Modified keyword.
 */
PvlKeyword &modifyKeyword(UserInterface &ui, PvlKeyword &keyword) {
  if(ui.WasEntered("UNITS"))
    keyword.SetValue(ui.GetString("VALUE"), ui.GetString("UNITS"));
  else
    keyword.SetValue(ui.GetString("VALUE"));
  if(ui.WasEntered("COMMENT"))
    keyword.AddComment(ui.GetString("COMMENT"));
  return keyword;
}
