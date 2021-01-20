#include "Isis.h"

#include "cam2cam.h"

#include "CameraFactory.h"
#include "Camera.h"
#include "Distance.h"
#include "ProcessRubberSheet.h"
#include "IException.h"


using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  cam2cam(ui);
}

