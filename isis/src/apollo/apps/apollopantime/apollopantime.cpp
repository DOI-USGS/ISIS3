#include "Isis.h"

#include "Application.h"
#include "ApolloPanImage.h"
#include "FileList.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Construct the image object
  ApolloPanImage image;

  // Create the tiles from the input Pvl files
  if (ui.WasEntered("LASTTILE")) {
    int lastTile = ui.GetInteger("LASTTILE");

    // Create the tiles from the input Pvl files
    if (ui.WasEntered("PVLLIST")) {
      FileList pvlList(ui.GetFileName("PVLLIST"));
      image.readFromPvl(pvlList, lastTile);
    }
    else {
      image.readFromPvl(ui.GetFileName("FROM"), lastTile);
    }
  }
  else {
    if (ui.WasEntered("PVLLIST")) {
      FileList pvlList(ui.GetFileName("PVLLIST"));
      image.readFromPvl(pvlList);
    }
    else {
      image.readFromPvl(ui.GetFileName("FROM"));
    }
  }

  // Compute timing mark times
  image.readTimeCode();

  // Fill in timing information on overlaps
  image.fillExteriorTimingMarks();

  // Compute tile start and stop times
  image.computeStartStop();

  // Output new Pvl files
  image.writeToPvl(ui.GetFileName("PREFIX"));
}