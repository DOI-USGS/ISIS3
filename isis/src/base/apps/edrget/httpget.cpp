#include "Application.h"
#include "httpget.h"
#include "IException.h"
#include "IString.h"
#include "Progress.h"


#include <iostream>
#include <QtCore>
#include <QtNetwork>




using namespace std;

namespace Isis {

  HttpGet::HttpGet(QObject *parent) : QObject(parent) {


    //connect the QHttp done signal to the httpDone function
    connect(&p_http, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
    //connect the QHttp progress signal to the httpProgress function(Isis progress)
    connect(&p_http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(httpProgress(int, int)));

    //tjw:  A timer for detecting network timeouts and exiting
    //      the application gracefully
    connect(&p_timer, SIGNAL(timeout()),this,SLOT(httpTimeout() ) );

  }
  //****************************************************************************
  // getFile function will check URL, if URL is good the function will connect,
  // login, and get the file.  This function returns p_error
  //****************************************************************************

  bool HttpGet::getFile(const QUrl &url, QString topath,int timeout) {

      p_timeOut = timeout;

    //tested
    // The next four ifs will check the URL and return error is bad
    if (!url.isValid() ) {
      QString msg = "Invalid URL";
      p_progress.SetText(msg);
      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

//       iException::Message(iException::User, msg, _FILEINFO_);
      p_error = true;
      return p_error;
    }

    //Dead code:  this condition is already checked before this function is entered
    //if (url.scheme().toLower() != "http") {
    //  QString msg = "URL must start with 'http:'";
    //  p_progress.SetText(msg);
    //  if (!Application::GetUserInterface().IsInteractive() )
    //     cout << msg.toStdString() << endl;

//       iException::Message(iException::User, msg, _FILEINFO_);
    //p_error = true;
    //  return p_error;
    //}

    //tested
    if (url.path().isEmpty() ) {
      QString msg = "URL has no path";
      p_progress.SetText(msg);
      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

//       iException::Message(iException::User, msg, _FILEINFO_);
      p_error = true;
      return p_error;
    }

    QString localFileName;
    if (topath.size() != 0) {
      localFileName += topath;
      localFileName += "/";
    }
    localFileName +=  QFileInfo(url.path()).fileName();
    //tested
    if (localFileName.isEmpty() ) {
      QString msg = "URL has no filename";
      p_progress.SetText(msg);
      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

//       iException::Message(iException::User, msg, _FILEINFO_);
      p_error = true;
      return p_error;
    }
    // check the local file.
    p_file.setFileName(localFileName);
    if (!p_file.open(QIODevice::WriteOnly) ) {
      QString msg = "Cannot open output file";
      p_progress.SetText(msg);
      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

//       iException::Message(iException::User, msg, _FILEINFO_);
      p_error = true;
      return p_error;
    }
    p_http.setHost(url.host(), url.port() );
    p_http.get(url.path(), &p_file);

    p_lastDone = -1;
    p_error = false;
    return p_error;
  }


  //tjw:  Timeout event handler monitors the http connection and gracefully
  //closes and exits the application if a timeout occurs.
  void HttpGet::httpTimeout() {

      bool fileExists = false;
      bool fileRemoved = false;
      QString fileRemovedQStr;


      QString timeoutSecs = QString("Timeout error:  An http get request exceeded ") +
              QString::number(p_timeOut)+ QString(" ms.");

      p_progress.SetText(timeoutSecs);

      if (!Application::GetUserInterface().IsInteractive() )
        cout << timeoutSecs.toStdString() << endl;

      p_http.close();

      fileExists = p_file.exists();

      if (fileExists) {
          fileRemoved = p_file.remove();
      }


      if (!fileExists || fileRemoved)
          fileRemovedQStr = p_file.fileName() + QString(" successfully deleted.");


      p_progress.SetText(fileRemovedQStr);

      if (!Application::GetUserInterface().IsInteractive() )
        cout << fileRemovedQStr.toStdString() << endl;


      emit done();
      return;
  }


  void HttpGet::httpDone(bool error) {
    map <int, QString> errLUT;
    errLUT [204] = "No content";
    errLUT [301] = "Moved Permanently";
    errLUT [302] = "Moved Temporarily";
    errLUT [400] = "Bad Request";
    errLUT [401] = "Unauthorized";
    errLUT [403] = "Forbidden";
    errLUT [404] = "File Not Found";
    errLUT [500] = "Internal server Error";
    errLUT [502] = "Bad GateWay";
    errLUT [503] = "service Unavailable";

    if (error) {
      p_error =  true;
      QString msg = p_http.errorString();
//       iException::Message(iException::User, msg, _FILEINFO_);
    }
    else if (p_http.lastResponse().statusCode()  != 200 &&
             p_http.lastResponse().statusCode()  != 0) {
      p_error = true;
      QString msg = "error code: [" + errLUT[p_http.lastResponse().statusCode()] + "]";
//       iException::Message(iException::User, msg, _FILEINFO_);
    }
    else {
      p_error = false;
    }
    if (!p_error) {

      if (!Application::GetUserInterface().IsInteractive() ) {
          cout << "100% Processed" << endl;
      }
      p_file.close();

     }

    emit done();
    return;
  }
  // This function setsup and useses Isis progress classs to track progress.
  void HttpGet::httpProgress(int done, int total) {

      p_timer.start(p_timeOut);
      //double percentDone = 0;
      //percentDone = 100*((double)done/total);



    if (total == 0) return;
    if (p_error) return;
    if (p_lastDone < 0) {
      p_progress.SetText(QString("Downloading File ") + p_file.fileName());
      p_progress.SetMaximumSteps(total);
      p_progress.CheckStatus();
      p_lastDone = 1;
    }
    while (p_lastDone <= done) {
      p_progress.CheckStatus();
      p_lastDone++;
    }
  }
}
