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

  // Run a standard deviation filter on the cube
  string inFile = ui.GetFilename("FROM");
  string outFile = name + ".step1.cub";
  string parameters = "FROM=" + inFile + " TO=" + outFile 
    + " toldef=stddev flattol=10 line=9 samp=9 minimum=9 tolmin=4.0 tolmax=4.0"
    + " replace=null";
  Isis::iApp ->Exec("noisefilter",parameters);

  // Run a standard deviation filter on the cube 
  inFile = outFile;
  outFile = name + ".step2.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " toldef=stddev flattol=10 line=3 samp=3 minimum=3 tolmin=3.5 tolmax=3.5";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a standard deviation filter on the cube
  inFile = outFile;
  outFile = name + ".step3.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile 
    + " toldef=stddev flattol=10 samp=9 line=9 minimum=9 tolmin=4.0 tolmax=4.0"
    + " replace=null";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a standard deviation filter on the cube
  inFile = outFile;
  outFile = name + ".step4.cub";
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " toldef=stddev samp=3 line=3 minimum=3 tolmin=3.5 tolmax=3.5";
  Isis::iApp ->Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run a lowpass filter on the cube
  inFile = outFile;
  outFile = ui.GetFilename("TO");
  parameters = "FROM=" + inFile + " TO=" + outFile + 
    " filter=outside samp=3 line=3 minimum=5 replacement=null";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());
}
