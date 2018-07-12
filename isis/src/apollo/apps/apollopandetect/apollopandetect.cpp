#include "Isis.h"

#include "Application.h"
#include "ApolloPanImage.h"
#include "ApolloPanTile.h"
#include "UserInterface.h"
#include "FileName.h"
#include "Pvl.h"

#include <QString>

using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Only detect a single tile
  if (ui.WasEntered("TILENUMBER")) {
    // Construct the tile
    ApolloPanTile tile(ui.GetString("IMAGENUMBER"), ui.GetInteger("TILENUMBER"));

    // Detect marks
    if (ui.WasEntered("TILETIF")) {
      tile.detectTile(FileName(ui.GetFileName("TILETIF")));
    }
    else {
      tile.detectTile();
    }

    // Write out the Pvl file
    QString filename = ui.GetFileName("PREFIX") + "AS15-P-" + ui.GetString("IMAGENUMBER")
                       + "_000" + toString(ui.GetInteger("TILENUMBER")) + ".pvl";
    Pvl tilePvl;
    tilePvl += tile.toPvl();
    tilePvl.write(filename.toLatin1().data());
  }

  // Detect an entire image
  else {
    // Construct the image object
    ApolloPanImage image(ui.GetString("IMAGENUMBER"));

    // Detect marks
    if (ui.WasEntered("TIFLIST")) {
      image.detectTiles(ui.GetFileName("TIFLIST"));
    }
    else {
      image.detectTiles();
    }

    // Decode timing marks
    image.decodeTimingMarks();

    // Write out the Pvl files
    image.writeToPvl(ui.GetFileName("PREFIX"));
  }
}