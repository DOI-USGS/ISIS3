/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#define GUIHELPERS
#include "Isis.h"

#include "Application.h"
#include "Process.h"
#include "UserInterface.h"

#include "pointreg.h"

using namespace std;
using namespace Isis;

void printTemp();

map<QString, void *> GuiHelpers() {
  map<QString, void *> helper;
  helper["PrintTemp"] = (void *) printTemp;
  return helper;
}

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  pointreg(ui, &appLog); 
}

// Helper function to print out template to session log
void printTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template pvl
  Pvl userTemp;
  userTemp.read(ui.GetFileName("DEFFILE"));

  //Write template file out to the log
  Isis::Application::GuiLog(userTemp);
}
