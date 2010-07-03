#include "Isis.h"
#include "Application.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  Filename fname = ui.GetFilename("FROM");
  string name = fname.Basename();
  bool rmv = ui.GetBoolean("REMOVE");

  // Figure out which masking cube to use
  Pvl p(ui.GetFilename("FROM"));
  PvlGroup &inst = p.FindGroup("Instrument",Pvl::Traverse);
  int spn;
  string scn = (string)inst["SpacecraftName"];
  if (scn == "VIKING_ORBITER_1") spn = 1;
  else if (scn == "VIKING_ORBITER_2") spn = 2;
  else {
    string msg = "Invalid spacecraftname [" + scn + "]";
    throw iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
  }
  // determine if # of cols is even or odd
  bool even = true;
  PvlGroup &arch = p.FindGroup("Archive",Pvl::Traverse);
  string id = (string)arch["ProductId"];
  int num = iString(id.substr(5,1)).ToInteger(); 
  if (num == 1 || num == 3 || num == 5 || num == 7 || num == 9) even = false;

  // Run a standard deviation filter on the cube
  string inFile = ui.GetFilename("FROM");
  string outFile = name + ".step1.cub";
  string parameters = "FROM=" + inFile + " TO=" + outFile + 
    " toldef=stddev flattol=10  samp=3 line=3 minimum=5 tolmin=2.5 tolmax=2.5"
    + " replace=null";
  Isis::iApp ->Exec("noisefilter",parameters);

  // Run a lowpass filter on the cube
  inFile = outFile;
  outFile = name + ".step2.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " filter=outside line=3 samp=3 minimum=5";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());

  // Run mask on the cube with the correct masking cube
  inFile = outFile;
  outFile = name + ".step3.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " MASK=$ISIS3DATA/viking" + iString(spn) + "/calibration/vik" + iString(spn);
  if (even) parameters += "evenMask.cub"; 
  else parameters += "oddMask.cub";
  Isis::iApp ->Exec("mask",parameters);
  if (rmv) remove(inFile.c_str());
 
  // Run a low pass filter on the invalid data in the cube
  inFile = outFile;
  outFile = name + ".step4.cub";                                                 
  parameters = "FROM=" + inFile + " TO=" + outFile +
    " samp=3 line=3 filter=outside replace=null";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());
 
  // Run a lowpass filter on the cube
  inFile = outFile;
  outFile = name + ".step5.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " filter=outside samp=7 line=7 replace=null";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a lowpass filter on the cube
  inFile = outFile;
  outFile = name + ".step6.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " filter=outside line=11 samp=11 replace=null";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());

  inFile = outFile;
  outFile = ui.GetFilename("TO");
  parameters = "FROM=" + inFile + " TO=" + outFile 
    + " bottom=20 top=25 left=30 right=30";
  Isis::iApp ->Exec("trim",parameters);
  if (rmv) remove(inFile.c_str());
}

