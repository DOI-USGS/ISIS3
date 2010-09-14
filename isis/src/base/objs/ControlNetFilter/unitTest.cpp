
#include "Isis.h"
#include "IsisDebug.h"
#include "Application.h"
#include "ControlNet.h"
#include "ControlNetFilter.h"
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

void IsisMain()
{
  Isis::Preference::Preferences(true);
  cout << "UnitTest for ControlNetFilter ...." << endl << endl;

  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  
  cout << "CNET=" << ui.GetFilename("CNET") << endl;
  cout << "Serial File=" << ui.GetFilename("FROMLIST") << endl;
  Isis::ControlNet cnetOrig(ui.GetFilename("CNET"));
  Isis::ControlNet cnet = cnetOrig;

  //test filter
  Isis::PvlGroup filterGrp("Point_NumMeasures");
  Isis::PvlKeyword keyword("GreaterThan", 3);
  filterGrp += keyword;
  
  std::string sSerialFile = ui.GetFilename("FROMLIST");
  Isis::ControlNetFilter cnetFilter(&cnet, sSerialFile);
  cnetFilter.PointMeasuresFilter(filterGrp,  false);
  cnet.SetModifiedDate("current");
  cnet.SetCreatedDate("current");
  cnet.Write("cnetNew.net");  

  system ("cat cnetNew.net");
  system ("rm cnetNew.net"); 
}
