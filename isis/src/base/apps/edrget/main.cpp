#include "Isis.h"

#include <iostream>

#include <QtCore>
#include <QDir>

#include "ResourceGet.h"
#include "UserInterface.h"
#include "ProgramLauncher.h"
#include "IException.h"

using namespace Isis;
using namespace std;

void IsisMain() {
  std::cout << "\n*********************************** WARNING ***********************************\n"
    " This program is deprecated and will be made unavailable in a future release of\n"
    " ISIS.  A brief discussion that lead to this decision can be found at          \n"
    " https://github.com/USGS-Astrogeology/ISIS3/issues/3313.  Users who require    \n"
    " similar functionality are encouraged to explore wget as a replacement.        \n"
    "*******************************************************************************\n" << '\n';

  // Get the file name from the GUI
  int timeOut = 60000;
  QString timeOutStr;
  UserInterface &ui = Application::GetUserInterface();
  QString guiURL = ui.GetString("URL");
  QString guiPath;

  if (ui.WasEntered("TOPATH")) {
    guiPath = ui.GetString("TOPATH");
  }

  if (ui.WasEntered("TIMEOUT")) {
        timeOut = ui.GetInteger("TIMEOUT");
  }

  QUrl qurl(guiURL);

  //test if scheme is ftp or http
  if (qurl.scheme().toLower() == "ftp" || qurl.scheme().toLower() == "http" ||
      qurl.scheme().toLower() == "https") {

    if (ui.IsInteractive()) {
      QString parameters = "URL=" + guiURL;

      if (ui.WasEntered("TOPATH") ) {
        parameters += " TOPATH=" + guiPath;
      }

      if (ui.WasEntered("TIMEOUT") ) {
        parameters += " TIMEOUT=" + QString::number(timeOut);
      }

      //////////////////////////////////////////////////////////////////////////////////
      // The line below starts a child process that launches
      // $ISISROOT/bin/edrget .  This was done because QMainWindow::instance()->exec()
      // which starts the event processing loop has already been called, and
      // cannot be called again to catch events from the FtpGet/HttpGet objects.
      //////////////////////////////////////////////////////////////////////////////////

      ProgramLauncher::RunIsisProgram("edrget", parameters);
    }

    else {
      ResourceGet getter;
      QObject::connect(&getter, SIGNAL(done()), QCoreApplication::instance(), SLOT(quit()));

      //a false getResource return means no error and we sould execute the get.

      //Starts the main event-processing loop for the application.  Since IsisMain already
      //started an event-processing loop, a child process was launched above.
      if (!getter.getResource(qurl, guiPath,timeOut))  QCoreApplication::instance()->exec();

      //if error occurred throw could not acquire
      if (getter.error() ) {
        //tested
        QString msg = "Could not acquire [" + guiURL + "].";
        msg += " " + getter.errorMessage();
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }

  else {
    QString msg = "Scheme [" + qurl.scheme() + "] not found, must be 'ftp' or 'http'";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}
