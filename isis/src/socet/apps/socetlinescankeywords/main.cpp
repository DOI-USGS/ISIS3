#include "Isis.h"

#include "socetlinescankeywords.h"

#include "Application.h"
#include "IException.h"
#include "UserInterface.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  socetlinescankeywords(ui);
}
