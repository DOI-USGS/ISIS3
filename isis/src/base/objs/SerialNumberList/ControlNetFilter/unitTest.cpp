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

int main() 
{
  Isis::Preference::Preferences(true);
  cout << "UnitTest for ControlNetFilter ...." << endl << endl;
  
  Isis::ControlNet cnetOrig("cnet.net");
  Isis::ControlNet cnet = cnetOrig;

  //test filter
  Isis::PvlGroup filterGrp("Point_NumMeasures");
  Isis::PvlKeyword keyword("GreaterThan", 2);
  filterGrp += keyword;
  
  std::string sSerialFile = "serialNum.lis";
  Isis::ControlNetFilter cnetFilter(&cnet, sSerialFile);
  cnetFilter.PointMeasuresFilter(filterGrp,  false);
  cnet.Write("cnetNew.net");  

  system ("cat cnetNew.net");
  system ("rm cnetNew.net"); 
}
