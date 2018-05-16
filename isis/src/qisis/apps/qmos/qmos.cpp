#include "IsisDebug.h"

#include <QCoreApplication>

#include "Gui.h"
#include "IException.h"
#include "MosaicMainWindow.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "QIsisApplication.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  if (getenv("ISISROOT") == NULL || QString(getenv("ISISROOT")) == "") {
    std::cerr << "Please set ISISROOT before running any Isis applications" << std::endl;
    exit(1);
  }
  Isis::Gui::checkX11();

  // Add the Qt plugin directory to the library path
  FileName qtpluginpath("$ISISROOT/3rdParty/plugins");
  QCoreApplication::addLibraryPath(qtpluginpath.expanded());

  QApplication *app = new QIsisApplication(argc, argv);
  QApplication::setApplicationName("qmos");

  MosaicMainWindow *mainWindow = new MosaicMainWindow("qmos");
  mainWindow->show();

  int status = app->exec();

  delete mainWindow;
  delete app;

  return status;
}
