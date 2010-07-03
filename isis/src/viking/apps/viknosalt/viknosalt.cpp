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

  // Trim the edges of the cube
  string inFile = ui.GetFilename("FROM");
  string outFile = name + ".step1.cub";
  string parameters = "FROM=" + inFile + " TO=" + outFile 
    + " top=1 left=1 right=1";
  Isis::iApp ->Exec("trim",parameters);

  // Run a trimfilter on the cube
  inFile = outFile;
  outFile = name + ".step2.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + " samp=3 line=3 minimum=3";
  Isis::iApp ->Exec("trimfilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a standard deviation filter on the cube
  inFile = outFile;
  outFile = name + ".step3.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile 
    + " toldef=stddev samp=3 line=3 minimum=3 tolmin=100 tolmax=3.0";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a noise filter on the cube
  inFile = outFile;
  outFile = name + ".step4.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " samp=3 line=3 minimum=2 tolmin=300 tolmax=100";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a second noise filter on the cube
  inFile = outFile;
  outFile = name + ".step5.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " samp=3 line=3 minimum=2 tolmin=300 tolmax=60";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a second standard deviation filter on the cube
  inFile = outFile;
  outFile = name + ".step6.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile 
    + " toldef=stddev samp=3 line=3 minimum=7 tolmin=100 tolmax=2.0";
  Isis::iApp ->Exec("noisefilter",parameters);   
  if (rmv) remove(inFile.c_str());

  // Run a third noise filter on the cube
  inFile = outFile;
  outFile = name + ".step7.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " samp=3 line=3 minimum=7 tolmin=300 tolmax=46";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a low pass filter on the invalid data in the cube
  inFile = outFile;
  outFile = ui.GetFilename("TO");                                                 
  parameters = "FROM=" + inFile + " TO=" + outFile +
    " samp=3 line=3 minimum=2 filter=outside null=true lis=true his=true lrs=true";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());
}

