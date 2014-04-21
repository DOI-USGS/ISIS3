/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "IsisDebug.h"

#include <QLocale>
#include <QTranslator>

#include "Gui.h"
#include "IException.h"
#include "CNetSuiteMainWindow.h"
#include "QIsisApplication.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {

  Gui::checkX11();

  try {
    QApplication *app = new QIsisApplication(argc, argv);
    QApplication::setApplicationName("cnetsuite");

    CNetSuiteMainWindow *mainWindow = new CNetSuiteMainWindow();

    mainWindow->show();

    int status = app->exec();

    delete mainWindow;
    delete app;

    return status;
  }
  catch(IException &e) {
    e.print();
  }

}


