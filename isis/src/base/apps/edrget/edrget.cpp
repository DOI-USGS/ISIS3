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
      // tjw(ref#2259):  The line below starts a child process that launches
      // $ISISROOT/bin/edrget .  This was done because QMainWindow::instance()->exec()
      // which starts the event processing loop has already been called, and
      // cannot be called again to catch events from the FtpGet/HttpGet objects.
      // Launching a child process appears to have been a quick and dirty way
      // to get around this.  This code should be refactored under a future ticket.
      // There is a lot of code duplication.
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