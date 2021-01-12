#include "Isis.h"

#include "Application.h"
#include "UserInterface.h"

#include "stretch_app.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl results;
  try{
      stretch(ui, &results);
  }
  catch(...){
      for (int resultIndex = 0; resultIndex < results.groups(); resultIndex++) {
          Application::Log(results.group(resultIndex));
      }
      throw;
  }
  for (int resultIndex = 0; resultIndex < results.groups(); resultIndex++) {
      Application::Log(results.group(resultIndex));
  }
}
