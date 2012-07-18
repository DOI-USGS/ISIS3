#include "Isis.h"
#include "IsisDebug.h"
#include "Application.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlNetStatistics.h"
#include "IException.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SpecialPixel.h"
#include "TextFile.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace Isis;

void IsisMain() {
  Preference::Preferences(true);
  cout << "UnitTest for ControlNetStatistics ...." << endl << endl;

  Isis::UserInterface &ui = Isis::Application::GetUserInterface();

  cout << "CNET=" << ui.GetAsString("CNET") << endl;
  cout << "Serial File=" << ui.GetAsString("FROMLIST") << endl;

  Isis::ControlNet cnetOrig(ui.GetFileName("CNET"));
  Isis::ControlNet cnet = cnetOrig;

  std::string sSerialFile = ui.GetFileName("FROMLIST");
  ControlNetStatistics cnetStats(&cnet, sSerialFile);

  PvlGroup statsGrp;
  cnetStats.GenerateControlNetStats(statsGrp);

  cout << statsGrp << endl;

}
