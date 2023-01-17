#include "Isis.h"

#include "footprintinit.h"

#include "IException.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "Process.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "SerialNumber.h"
#include "Target.h"
#include "TProjection.h"


using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;
  footprintinit(ui, &appLog);
 }
