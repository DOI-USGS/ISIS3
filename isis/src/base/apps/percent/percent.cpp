#include "Isis.h"

#include <vector>
#include <string>

#include "Process.h"
#include "Pvl.h"
#include "PvlFormat.h"
#include "Histogram.h"
#include "iString.h"


using namespace std; 
using namespace Isis;

void IsisMain() {
  // Use a regular Process
  Process p;

  // Open the input cube
  Cube *icube = p.SetInputCube("FROM",OneBand);

  // Get the desired percentage(s)
  UserInterface &ui = Application::GetUserInterface();
  string sPercentage;
  sPercentage = ui.GetString("PERCENTAGE");
  vector <string>tokens;
  iString::Split(',', sPercentage, tokens, true);
   PvlGroup results("Results");
   PvlKeyword kwPercent("Percentage");
   PvlKeyword kwValue("Value");

  for(unsigned int i = 0; i<tokens.size(); i++) {
    double percentage = iString::ToDouble(tokens[i]);
    // Obtain the Histogram and the value at the percentage
    Histogram *hist = icube->Histogram();
    double value = hist->Percent(percentage);
     kwPercent += percentage;
     kwValue += value;
  }
  results += kwPercent;
  results += kwValue;

  // Log the results
    Application::Log(results); 
  // Write an output file if requested
    if (ui.WasEntered("TO")) {
      Pvl temp;
      temp.AddGroup(results);
      temp.Write(ui.GetFilename("TO","txt"));
    }
}
