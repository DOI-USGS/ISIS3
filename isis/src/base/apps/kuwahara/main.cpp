#include "Isis.h"

#include "kuwahara.h"

#include "Application.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  kuwahara(ui);
}
