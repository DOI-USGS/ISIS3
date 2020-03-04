#include "Isis.h"

#include "cam2map.h"

#include "Camera.h"
#include "IException.h"
#include "IString.h"
#include "ProcessRubberSheet.h"
#include "ProjectionFactory.h"
#include "PushFrameCameraDetectorMap.h"
#include "Pvl.h"
#include "Target.h"
#include "TProjection.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  try {
    cam2map(ui, &appLog);
  }
  catch (...) {
    for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
      Application::Log(*grpIt);
    }
    throw;
  }

  for (auto grpIt = appLog.beginGroup(); grpIt!= appLog.endGroup(); grpIt++) {
    Application::Log(*grpIt);
  }
}
