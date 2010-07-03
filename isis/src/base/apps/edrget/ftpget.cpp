#include <QtCore>
#include <QtNetwork>
#include <iostream>

#include "ftpget.h"
#include "iString.h"
#include "iException.h"
#include "Progress.h"
#include  "Application.h"

using namespace std;

namespace Isis {

  FtpGet::FtpGet(QObject *parent) : QObject(parent) {
      //conncet the Qftp done signal to the ftpDone function
      connect(&p_ftp, SIGNAL(done(bool)), this, SLOT(ftpDone(bool)));
      //connect the Qftp progress signal to the ftpProgress function(ISIS progress)
      connect(&p_ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
              this, SLOT(ftpProgress(qint64,qint64)));
  }
  //**************************************
  // getFile function will check URL, if URL is good function will connect,
  // login, and get the file.  This function returns P_error.
  bool FtpGet::getFile(const QUrl &url, string topath) {
      //next four if check the URL and return true is there is error.
      if (!url.isValid()) {
          string msg ="invalid URL";
          iException::Message(iException::User,msg,_FILEINFO_);
          p_error = true;
          return p_error;
      }
      if (url.scheme().toLower() != "ftp") {
         string msg ="URL must start with 'ftp:'";
         iException::Message(iException::User,msg,_FILEINFO_);
         p_error =  true;
         return p_error;
      }
      if (url.path().isEmpty()) {
         string msg ="URL has no path";
         iException::Message(iException::User,msg,_FILEINFO_);
         p_error =  true;
         return p_error;
      }

      QString localFileName;
      if (topath.size() != 0){
        localFileName += topath.c_str();
        localFileName += "/";
      }
      localFileName +=  QFileInfo(url.path()).fileName();
      if (localFileName.isEmpty()) {
        string msg ="URL has no filename";
        iException::Message(iException::User,msg,_FILEINFO_);
        p_error = true;
        return p_error;
      }
      // check local file.
      p_file.setFileName(localFileName);
      if (!p_file.open(QIODevice::WriteOnly)) {
         string msg ="Cannot open output file";
         iException::Message(iException::User,msg,_FILEINFO_);
         p_error =  true;
         return p_error;
      }

      p_ftp.connectToHost(url.host(), url.port());
      p_ftp.login();
      p_ftp.get(url.path(), &p_file);
      p_lastDone = -1;
      p_error = false;
      return p_error;
  }

  void FtpGet::ftpDone(bool error) {
      if (error) {
        p_error = true;
        iString msg =p_ftp.errorString().toStdString();
        msg.Remove("\n");
        iException::Message(iException::User,msg,_FILEINFO_);
      }
      else{
        p_error = false;
      }
      if (!p_error) {
        p_file.close();
        // this was added because final size may not match progress size so
        // you do not get 100% processed
        if(!Application::GetUserInterface().IsInteractive()){
          cout<<"100% Processed" << endl;
        }
      }
      emit done();
      return;
  }
  // ftpProgress uses the ISIS progress class to track progress
  void FtpGet::ftpProgress(qint64 done, qint64 total) {

    if (total == 0) return;
    if (total == -1) return;
    if (p_error) return;
    if (p_lastDone < 0) {
      p_progress.SetText(string("Downloading File ") + p_file.fileName().toStdString() );
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
