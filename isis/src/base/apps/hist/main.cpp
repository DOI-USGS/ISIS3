#define GUIHELPERS
#include "Isis.h"

#include "Application.h"
#include "ImageHistogram.h"
#include "LineManager.h"
#include "hist.h"

using namespace Isis;
using namespace std;

void helperButtonCalcMinMax();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["helperButtonCalcMinMax"] = (void *) helperButtonCalcMinMax;
  return helper;
}

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  hist(ui);
}

// Helper function to fill in the auto calculated min/max.
void helperButtonCalcMinMax() {

  UserInterface &ui = Application::GetUserInterface();

  // Setup a cube for gathering stats from the user requested band
  QString file = ui.GetCubeName("FROM");

  Cube inCube;
  CubeAttributeInput attrib = ui.GetInputAttribute("FROM");
  if (attrib.bands().size() != 0) {
     vector<QString> bands = attrib.bands();
     inCube.setVirtualBands(bands);
  }

  inCube.open(file, "r");

  LineManager line(inCube);
  Statistics cubeStats;

  for (int i = 1; i <= inCube.lineCount(); i++) {
    line.SetLine(i);
    inCube.read(line);
    cubeStats.AddData(line.DoubleBuffer(), line.size());
  }

  inCube.close();

  // Write ranges to the GUI
  ui.Clear("MINIMUM");
  ui.PutDouble("MINIMUM", cubeStats.Minimum());
  ui.Clear("MAXIMUM");
  ui.PutDouble("MAXIMUM", cubeStats.Maximum());

}
