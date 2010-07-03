#include "Isis.h"

#include <QtCore>
#include <iostream>

#include "httpget.h"
#include "ftpget.h"
#include "UserInterface.h"
#include "iString.h"
#include "iException.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Get the file name from the GUI
  UserInterface &ui = Application::GetUserInterface();
  iString guiURL = ui.GetString("URL");
  iString guiPath;
  if (ui.WasEntered("TOPATH")) {
    guiPath = ui.GetString("TOPATH");
  }
  QUrl qurl(guiURL.c_str());
    //test if scheme is ftp and set port
    if (qurl.scheme().toLower() == "ftp") {
      qurl.setPort(21);

      if (ui.IsInteractive()) {
        string parameters = "URL=" + guiURL;
        if (ui.WasEntered("TOPATH")) {
          parameters += " TOPATH=" + guiPath;
        }
        iApp->Exec("edrget", parameters);
      }
      else {
        
        FtpGet getter;
        QObject::connect(&getter, SIGNAL(done()), QCoreApplication::instance(), SLOT(quit()));
        //a false getFile return means no error and we sould execute the get.
        if(!getter.getFile(qurl,guiPath))  QCoreApplication::instance()->exec();
        //if error occurred throw could not acquire 
        if (getter.error()) {
          QString localFileName;
          if (ui.WasEntered("TOPATH")){
            localFileName += guiPath.c_str();
            localFileName += "/";
          }
          localFileName +=  QFileInfo(qurl.path()).fileName();
          string localFileNameStr(localFileName.toStdString());
          remove(localFileNameStr.c_str());
          iString msg = "Could not acquire [" + guiURL + "]";
          throw iException::Message(iException::User,msg,_FILEINFO_);
        }
      } 
    }
    //test is scheme is http and set port
    else if (qurl.scheme().toLower() == "http") {
      qurl.setPort(80);

      if (ui.IsInteractive()) {
        string parameters = "URL=" + guiURL;
        if (ui.WasEntered("TOPATH")) {
          parameters += " TOPATH=" + guiPath;
        }
        iApp->Exec("edrget", parameters);
      }
      else {
        HttpGet getter;
        QObject::connect(&getter, SIGNAL(done()), QCoreApplication::instance(), SLOT(quit()));
        //a false getFile return means no error and we sould execute the get.
        if (!getter.getFile(qurl,guiPath)) QCoreApplication::instance()->exec();
        //if error occurred then throw could not acquire
        if (getter.error()) {
          QString localFileName;
          if (ui.WasEntered("TOPATH")){
            localFileName += guiPath.c_str();
            localFileName += "/";
          }
          string localFileNameStr(localFileName.toStdString());
          remove(localFileNameStr.c_str());
          iString msg = "Could not acquire [" + guiURL + "]";
          throw iException::Message(iException::User,msg,_FILEINFO_);
        }
      } 
    }
    //if scheme is not ftp or http throw error
    else {
      iString msg = "Scheme [" + qurl.scheme().toStdString() + "] not found, must be 'ftp' or 'http'";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
}

