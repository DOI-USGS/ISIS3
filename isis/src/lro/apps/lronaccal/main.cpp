#include "Isis.h"
#include "Application.h"
#include "lronaccal.h"

using namespace Isis;

/**
  *
  * @brief Lronaccal
  *
  * Performs radiometric corrections to images acquired by the Narrow Angle
  *    Camera aboard the Lunar Reconnaissance Orbiter spacecraft.
  *
  * @author 2016-09-16 Victor Silva
  *
  * @internal
  *   @history 2016-09-19 Victor Silva - Adapted from lrowacpho written by Kris Becker
  *   @history 2021-03-12 Victor Silva - Updates include ability to run with default values
  *                                       Added new values for 2019 version of LROC Empirical function.
  *   @history 2022-04-18 Victor Silva - Refactored to make callable for GTest framework
  */
void IsisMain (){
  UserInterface &ui = Application::GetUserInterface();
  lronaccal(ui);
}
