#include <QtCore>
#include <QtNetwork>
#include <iostream>

#include "httpget.h"
#include "iString.h"
#include "iException.h"
#include "Progress.h"


using namespace std;

namespace Isis {

  HttpGet::HttpGet(QObject *parent) : QObject(parent) {
      //connect the QHttp done signal to the httpDone function
      connect(&p_http, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
      //connect the QHttp progress signal to the httpProgress function(Isis progress)
      connect(&p_http, SIGNAL(dataReadProgress(int,int)),
              this, SLOT(httpProgress(int,int)));
  }
  //**********************************************
  // getFile function will check URL, if URL is good the function will connect,
  // login, and get the file.  This function returns p_error 
  bool HttpGet::getFile(const QUrl &url, string topath) {
      // The next four ifs will check the URL and return error is bad
      if (!url.isValid()) {
          string msg ="invalid URL";
          iException::Message(iException::User,msg,_FILEINFO_);
          p_error = true;
          return p_error;
      }
      if (url.scheme().toLower() != "http") {
         string msg ="URL must start with 'http:'";
         iException::Message(iException::User,msg,_FILEINFO_);
         p_error = true;
         return p_error;
      }
      if (url.path().isEmpty()) {
         string msg ="URL has no path";
         iException::Message(iException::User,msg,_FILEINFO_);
         p_error = true;
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
      // check the local file.
      p_file.setFileName(localFileName);
      if (!p_file.open(QIODevice::WriteOnly)) {
         string msg ="Cannot open output file";
         iException::Message(iException::User,msg,_FILEINFO_);
         p_error = true;
         return p_error;
      }
      p_http.setHost(url.host(), url.port());
      p_http.get(url.path(), &p_file);

      p_lastDone = -1;
      p_error = false;
      return p_error;
  }

  void HttpGet::httpDone(bool error)
  {
      map <int,string> errLUT;
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
        string msg =p_http.errorString().toStdString();
        iException::Message(iException::User,msg,_FILEINFO_);
      }
      else if (p_http.lastResponse().statusCode()  != 200 && p_http.lastResponse().statusCode()  != 0) {
        p_error = true;
        string msg ="error code: [" + errLUT[p_http.lastResponse().statusCode()] + "]";
        iException::Message(iException::User,msg,_FILEINFO_);
      }
      else{
        p_error = false;
      }
      if (!p_error) p_file.close();
      emit done();
      return;
  }
  // This function setsup and useses Isis progress classs to track progress.
  void HttpGet::httpProgress(int done, int total) {
    if (total == 0) return;
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
