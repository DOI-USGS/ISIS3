#include  "Application.h"
#include "ftpget.h"
#include "IString.h"
#include "IException.h"
#include "Progress.h"


#include <iostream>
#include <QtCore>
#include <QtNetwork>


using namespace std;

namespace Isis {

  FtpGet::FtpGet(QObject *parent) : QObject(parent) {



    //connect the Qftp done signal to the ftpDone function
    connect(&p_ftp, SIGNAL(done(bool)), this, SLOT(ftpDone(bool) ) );

    //tjw:  connect the QFtp progress signal to the ftpProgress function(ISIS progress)
    connect(&p_ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
            this, SLOT(ftpProgress(qint64, qint64)));

    //tjw:  A timer for detecting network timeouts and exiting the application gracefully
    connect(&p_timer, SIGNAL(timeout()),this,SLOT(ftpTimeout() ) );


  }

  //*************************************************************************
  // getFile function will check URL, if URL is good, getFile will connect,
  // login, and get the file.  This function returns p_error.
  //*************************************************************************

  bool FtpGet::getFile(const QUrl &url, QString topath, int timeout) {


      //tjw:
      p_timeOut = timeout;




    //next four if check the URL and return true is there is error.
    if (!url.isValid() ) {

     //tested
     QString msg = QString("Invalid URL");
      p_progress.SetText(msg);
      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

      p_error = true;
      return p_error;
    }

    //Dead code:  This condition is already checked before the function is hit
    //if (url.scheme().toLower() != "ftp") {
    //  QString msg = QString("URL must start with 'ftp:'");
    //  p_progress.SetText(msg);

    //  if (!Application::GetUserInterface().IsInteractive() )
    //     cout << msg.toStdString() << endl;

    //  p_error =  true;
    //  return p_error;
    //}
    //tested
    if (url.path().isEmpty() ) {
      QString msg = QString("URL has no path");
      p_progress.SetText(msg);

      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

      p_error =  true;
      return p_error;
    }

    QString localFileName;
    if (topath.size() != 0) {
      localFileName += topath;
      localFileName += "/";
    }
    //tested
    localFileName +=  QFileInfo(url.path()).fileName();
    if (localFileName.isEmpty() ) {
      QString msg = QString("URL has no filename");
      p_progress.SetText(msg);
      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

      p_error = true;
      return p_error;
    }
    // check local file.
    p_file.setFileName(localFileName);
    if (!p_file.open(QIODevice::WriteOnly) ) {
      QString msg = QString("Cannot open output file");
      p_progress.SetText(msg);

      if (!Application::GetUserInterface().IsInteractive() )
         cout << msg.toStdString() << endl;

      p_error =  true;
      return p_error;
    }

    p_ftp.connectToHost(url.host(), url.port() );
    p_ftp.login();
    p_ftp.get(url.path(), &p_file);

    p_lastDone = -1;
    p_error = false;
    return p_error;
  }


  void FtpGet::ftpDone(bool error) {

    if (error) {
      p_error = true;
      QString msg = p_ftp.errorString();
      msg.remove("\n");

    }
    else {
      p_error = false;
    }
    if (!p_error) {
      p_file.close();

      // this was added because final size may not match progress size so
      // you do not get 100% processed
      if (!Application::GetUserInterface().IsInteractive() ) {
        cout << "100% Processed" << endl;
      }
    }
    emit done();
    return;

  }



  //tjw:  Timeout event handler monitors the ftp connection and gracefully
  //closes and exits the application if a timeout occurs.
  void FtpGet::ftpTimeout() {

      bool fileExists = false;
      bool fileRemoved = false;
      QString fileRemovedQStr;

      QString timeoutSecs = QString("Timeout error:  An ftp get request exceeded ") +
              QString::number(p_timeOut)+ QString(" ms.");

      p_progress.SetText(timeoutSecs);

      if (!Application::GetUserInterface().IsInteractive() )
        cout << timeoutSecs.toStdString() << endl;


      p_ftp.abort();
      p_ftp.close();

      fileExists = p_file.exists();

      if (fileExists) {
          fileRemoved = p_file.remove();
      }


      if (!fileExists || fileRemoved)
          fileRemovedQStr = p_file.fileName() + QString(" successfully deleted.");



      if (!Application::GetUserInterface().IsInteractive() )
        cout << fileRemovedQStr.toStdString() << endl;



      emit done();
      return;
  }



  // tjw:  ftpProgress uses the ISIS progress class to track progress
  void FtpGet::ftpProgress(qint64 done, qint64 total) {

    p_timer.start(p_timeOut);

    //double percentDone = 0;
    //percentDone = 100*((double)done/total);
    //cout << percentDone << endl;

    if (total == 0) return;
    if (total == -1) return;
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
