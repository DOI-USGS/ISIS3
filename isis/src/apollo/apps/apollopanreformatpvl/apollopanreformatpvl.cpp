#include "Isis.h"

#include "Application.h"
#include "ApolloPanTile.h"
#include "FileList.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Construct tile object
  ApolloPanTile tile;

  QString inPvl = ui.GetFileName("FROM");
  QString outPvl = ui.GetFileName("TO");

  // validate input file exists
  if (!QFile::exists(inPvl)) {
    string msg = "Input pvl file does not exist";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // read input pvl
  tile.fromPvlNew(inPvl);

  Pvl tilePvl;
  tilePvl += tile.toPvlNew();
  tilePvl.write(outPvl.toLatin1().data());
}
