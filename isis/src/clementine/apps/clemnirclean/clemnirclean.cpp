#include "Isis.h"
#include "Application.h"
#include "Filename.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  // Open the input cube and get filename of output cube
  UserInterface &ui = Application::GetUserInterface();
  Filename infname = ui.GetFilename("FROM");
  string name = infname.Basename();
  Filename outfname = ui.GetFilename("TO");
  string outpath = outfname.Path() + "/";
  string outname = infname.Basename();
  bool rmv = ui.GetBoolean("REMOVE");

  // Run noisefilter on the cube and replace with nulls, 3x3 boxcar
  string inFile = ui.GetFilename("FROM");
  Filename temp1;
  temp1.Temporary(outname+".box1_","cub");
  string outFile = outpath+temp1.Name();
  string parameters = "FROM=" + inFile + " TO=" + outFile +
     " toldef=stddev tolmax=1.25 tolmin=1.25 samples=3 lines=3 replace=null";
  Isis::iApp ->Exec("noisefilter",parameters);

  // Run lowpass on the cube using outside filter, 3x3 boxcar
  inFile = outFile;
  Filename temp2;
  temp2.Temporary(outname+".box2_","cub");
  outFile = outpath+temp2.Name();
  parameters = "FROM=" + inFile + " TO=" + outFile +
    " samples=3 lines=3 filter=outside";
  Isis::iApp ->Exec("lowpass", parameters);
  if (rmv) remove(inFile.c_str());

  // Run lowpass on the cube using outside filter, 3x3 boxcar
  inFile = outFile;
  Filename temp3;
  temp3.Temporary(outname+".box3_","cub");
  outFile = outpath+temp3.Name();

  parameters = "FROM=" + inFile + " TO=" + outFile +
    " samples=3 lines=3 filter=outside";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());

  // Run noisefilter on the cube and replace with avg, 3x3 boxcar
  inFile = outFile;
  Filename temp4;
  temp4.Temporary(outname+".box4_","cub");
  outFile = outpath+temp4.Name();
  parameters = "FROM=" + inFile + " TO=" + outFile +
    " toldef=stddev tolmax=1.5 tolmin=1.5 samples=3 lines=3 nullisnoise=yes";
  Isis::iApp -> Exec("noisefilter",parameters);
  if (rmv) remove(inFile.c_str());

  // Run lowpass on the cube using outside filter, 5x5 boxcar
  inFile = outFile;
  parameters = "FROM=" + inFile + " TO=" + outfname.Expanded() +
    " samples=5 lines=5 filter=outside";
  Isis::iApp ->Exec("lowpass",parameters);
  if (rmv) remove(inFile.c_str());       
}
