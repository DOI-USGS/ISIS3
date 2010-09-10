#include "ControlNet.h"
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

int main() 
{
  Preference::Preferences(true);
  cout << "UnitTest for ControlNetStatistics ...." << endl << endl;
  
  ControlNet cnet("cnet.net");
  
  string sSerialFile = "serialNum.lis";
  ControlNetStatistics cnetStats(&cnet, sSerialFile);
  
  PvlGroup statsGrp;
  cnetStats.GenerateControlNetStats(statsGrp);
  
  cout << statsGrp;
 
}
