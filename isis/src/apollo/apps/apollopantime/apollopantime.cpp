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
  image.readFromPvl(ui.GetFileName("FROM"), tileCount);

  // Compute timing mark times
  image.readTimeCode();

  // Fill in timing information on overlaps
  image.fillExteriorTimingMarks();

  // Compute tile start and stop times
  image.computeStartStop();

  // Output new Pvl files
  image.writeToPvl(ui.GetFileName("OUTPUTDIRECTORY"));
}
