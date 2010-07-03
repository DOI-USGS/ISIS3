#include "Pvl.h"
#include "ProcessMosaic.h"
#include "ProcessByLine.h"
#include "Brick.h"
#include "FileList.h"
#include "iException.h"

using namespace std;
using namespace Isis;


int main(int argc, char *argv[]) {


  // X offset (pixels) of each CCD relative to CCD 10
  const char * const ccdNames[] = { "RED0", "RED1", "RED2", "RED3", "RED4", 
                                    "RED5", "RED6", "RED7", "RED8", "RED9",
                                    "IR10", "IR11", "BG12", "BG13" };

  const int xFPloc[] = {-8000,-6000,-4004,-2003,0,2000, 4000,6000, 8000,10000,
                                        0,2000,0,2000};
  const int yFPloc[] = { -1219, -1793, -1171, -1791, -1205, -1789, -1189,
                         -1786, -1210, -1817, 0, -606, -2396, -3002 };

  const int xoffset[] = {-8000-10,-6000+11,-4004-17+3,-2003+10+3,0-13+3,
                          2000+16+3, 4000-16+3,6000+18+3, 8000-18+3,10000+20+3,
                          0,2000,0,2000};
  const int yoffset[] = { 0+0+5, 0+6+5, 0+14+5, 0+15+5, 0+17+5, 0+17+5, 0+16+5,
                          0+11+5, 0+5+5, 0-5+5, 0, 0, 0, 0};

  PvlObject hiCCD("Hiccdstitch");
  hiCCD.AddComment("This file describes the line and sample offsets for each HiRISE");
  hiCCD.AddComment("CCD in the focal plane.  Negative values shift CCDs left and up.");
  hiCCD.AddComment("Positive values shift CCD right and down.");

  for (int i = 0 ; i < 14 ; i++) {
    PvlGroup ccdGroup(ccdNames[i]);
    ccdGroup += PvlKeyword("FocalPlaneSample",xFPloc[i]);
    ccdGroup += PvlKeyword("FocalPlaneLine",yFPloc[i]);
    ccdGroup += PvlKeyword("ImageSample",xFPloc[i]-xFPloc[0]+1);
    ccdGroup += PvlKeyword("ImageLine",1);
#if defined(RED_00001_0000)
    ccdGroup += PvlKeyword("SampleOffset",xoffset[i]-xFPloc[i]);
    ccdGroup += PvlKeyword("LineOffset",0+yoffset[i]);
#else
    ccdGroup += PvlKeyword("SampleOffset",0);
    ccdGroup += PvlKeyword("LineOffset",0);
#endif
    hiCCD.AddGroup(ccdGroup);
  }

  Pvl pvl;
  pvl.AddObject(hiCCD);
#if defined(RED_00001_0000)
  pvl.Write("hiccdstitch.000001_0000_RED.def");
#else
  pvl.Write("hiccdstitch.offsets.def");
#endif
  return (0);
}	// End of IsisMain
