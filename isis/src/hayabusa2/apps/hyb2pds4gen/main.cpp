#include "Isis.h"

#include "UserInterface.h"
#include "hyb2pds4gen.h"

using namespace Isis; 

void IsisMain() { 
  UserInterface &ui = Application::GetUserInterface();
  hyb2pds4gen(ui);
}
