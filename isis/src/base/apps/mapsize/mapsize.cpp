#include "Isis.h"
#include "Process.h"
#include "ProjectionFactory.h"
#include "Projection.h"

using namespace std; 
using namespace Isis;

void IsisMain() {
  Process p;

  // Get the output map projection file and create an output projection object
  UserInterface &ui = Application::GetUserInterface();
  Pvl lab;
  lab.Read(ui.GetFilename("MAP"));

  int samples,lines;
  Projection *outmap = ProjectionFactory::CreateForCube(lab,samples,lines);

  // Obtain x/y min/max distances in meters
  double xmin,xmax,ymin,ymax;
  outmap->XYRange(xmin,xmax,ymin,ymax);
  double xdist = xmax - xmin;
  double ydist = ymax - ymin;
  double scale, width, height;
  double inchesPerMeter = 39.37;

  // See if the user specified inches or scale
  if (ui.GetString("OPTION") == "SCALE") {
    scale = ui.GetDouble("SCALE");
    width = xdist / scale * inchesPerMeter;
    height = ydist / scale * inchesPerMeter;
  }
  else {
    width = ui.GetDouble("WIDTH");
    height = ui.GetDouble("HEIGHT");
    double xscale = xdist / width * inchesPerMeter;
    double yscale = ydist / height * inchesPerMeter;
    scale = max(xscale,yscale);
    // Recompute as one is likely to be reduced
    width = xdist / scale * inchesPerMeter;
    height = ydist / scale * inchesPerMeter;
  }

  // Create a label and log it
  PvlGroup results("Results");
  results += PvlKeyword("Map",ui.GetFilename("MAP"));
  results += PvlKeyword("Scale",scale);
  results += PvlKeyword("Width",width,"inches");
  results += PvlKeyword("Height",height,"inches");
  results += PvlKeyword("Samples",samples);
  results += PvlKeyword("Lines",lines);
  results += PvlKeyword("RealSize", ((Isis::BigInt)lines * samples * 4) / 1024.0, "KB");
  results += PvlKeyword("SignedWordSize", ((Isis::BigInt)lines * samples * 2) / 1024.0, "KB");
  results += PvlKeyword("UnsignedByteSize", ((Isis::BigInt)lines * samples) / 1024.0, "KB");
  Application::Log(results);

  // Write the output file if requested
  if (ui.WasEntered("TO")) {
    Pvl temp;
    temp.AddGroup(results);
    temp.AddGroup(lab.FindGroup("Mapping",Pvl::Traverse));
    temp.Write(ui.GetFilename("TO","txt"));
  }

  Application::Log(lab.FindGroup("Mapping",Pvl::Traverse));

  p.EndProcess();
}
