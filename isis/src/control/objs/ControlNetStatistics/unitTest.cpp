#include "Isis.h"
#include "IsisDebug.h"
#include "Application.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "ControlNetStatistics.h"
#include "iException.h"
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

void IsisMain() 
{
  Preference::Preferences(true);
  cout << "UnitTest for ControlNetStatistics ...." << endl << endl;

  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  
  cout << "CNET=" << ui.GetFilename("CNET") << endl;
  cout << "Serial File=" << ui.GetFilename("FROMLIST") << endl;

  Isis::ControlNet cnetOrig(ui.GetFilename("CNET"));
  Isis::ControlNet cnet = cnetOrig;
  
  std::string sSerialFile = ui.GetFilename("FROMLIST");
  ControlNetStatistics cnetStats(&cnet, sSerialFile);
  
  PvlGroup statsGrp;
  cnetStats.GenerateControlNetStats(statsGrp);
  
  cout << statsGrp;
 
}
