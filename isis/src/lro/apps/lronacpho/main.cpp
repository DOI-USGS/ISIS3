/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"
#include "Application.h"
#include "Pvl.h"
#include "lronacpho.h"

using namespace Isis;

/**
  *
  * @brief Photometric application for the LRO NAC cameras
  *
  * This application provides features that allow multiband cubes for LRO NAC cameras
  *   to be photometrically corrected
  *
  * @author 2016-09-16 Victor Silva
  *
  * @internal
  *   @history 2016-09-19 Victor Silva - Adapted from lrowacpho written by Kris Becker
  *	  @history 2021-03-12 Victor Silva - Updates include ability to run with default values
  * 																			Added new values for 2019 version of LROC Empirical function.
  *  @history 2022-04-18 Victor Silva - Refactored to make callable for GTest framework
  */
void IsisMain (){
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  
  lronacpho(ui, &appLog); 
}

