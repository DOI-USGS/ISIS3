/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

//  $Id: hical.cpp 6715 2016-04-28 17:58:43Z tsucharski@GS.DOI.NET $
//  @history 2021-02-28 Moses Milazzo Added ZeroDarkRate as an optional
//  module to enable a new dark currrent rate calculation.

#include "Isis.h"

#include "Application.h"
#include "Pvl.h"
#include "hical.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  hical(ui, &appLog); 
}