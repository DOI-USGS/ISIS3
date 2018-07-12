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

  // Process for images with less than 8 tiles
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

    // Decode the tiles
    image.decodeTimingMarks();

    // Match the tiles
    image.matchTiles();

    // Number the fiducial and timing marks
    image.numberFiducialMarks(ui.GetInteger("FIRSTFIDINDEX"));
    image.numberTimingMarks();
  }
  
  // Standard 8 tile image process
  else {
    // Create the tiles from the input Pvl files
    if (ui.WasEntered("PVLLIST")) {
      FileList pvlList(ui.GetFileName("PVLLIST"));
      image.readFromPvl(pvlList);
    }
    else {
      image.readFromPvl(ui.GetFileName("FROM"));
    }

    // Decode the tiles
    image.decodeTimingMarks();

    // Match the tiles
    image.matchTiles();

    // Number the fiducial and timing marks
    image.numberFiducialMarks();
    image.numberTimingMarks();
  }

  // Compute affines
  image.computeAffines(ui.GetFileName("CALIBRATED"));

  // Flag fiducial marks that have residuals that are > 10 pixels
  image.flagOutliers(ui.GetDouble("TOLERANCE"));

  // Check the time code
  image.checkTimeCode();

  // Output new Pvl files
  image.writeToPvl(ui.GetFileName("PREFIX"));
}