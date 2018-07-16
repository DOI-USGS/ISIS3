#include "Isis.h"

#include <QDir>
#include <QStringList>

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

  QStringList nameFilter("*_000*.pvl");
  QDir pvlDirectory(FileName(ui.GetFileName("FROM")).expanded());
  
  int tileCount = QStringList(pvlDirectory.entryList(nameFilter)).count();
  cout << tileCount << endl;
  image.readFromPvl(ui.GetFileName("FROM"), tileCount);
  
  image.decodeTimingMarks();

  // Match the tiles
  image.matchTiles();

  // Number the fiducial and timing marks
  image.numberFiducialMarks();
  image.numberTimingMarks();

  // Compute affines
  image.computeAffines(ui.GetFileName("CALIBRATED"));

  // Flag fiducial marks that have residuals that are > 10 pixels
  image.flagOutliers(ui.GetDouble("TOLERANCE"));

  // Check the time code
  image.checkTimeCode();

  // Output new Pvl files
  image.writeToPvl(ui.GetString("OUTPUTDIRECTORY"));
}
