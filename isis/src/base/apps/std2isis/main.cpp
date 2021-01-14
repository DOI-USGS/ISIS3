#include "Isis.h"

#include <QString>

#include "ImageImporter.h"
#include "UserInterface.h"

#include "std2isis.h"

using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  std2isis(ui);
}