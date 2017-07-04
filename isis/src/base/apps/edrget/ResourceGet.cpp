#include "ResourceGet.h"

#include <iostream>

#include <QString>
#include <QtCore>
#include <QMessageBox>
#include <QtNetwork>
#include <QSysInfo>


#include "Application.h"
#include "IException.h"
#include "Progress.h"


using namespace std;

namespace Isis {

  ResourceGet::ResourceGet(QObject *parent) : QObject(parent) {
    m_error = false;
    m_lastDone = -1;
    m_timeOut = 60000; // default timeout (ms) 
    m_reply = NULL;
    m_isInteractive = Application::GetUserInterface().IsInteractive();

    //tjw:  A timer for detecting network timeouts and exiting the application gracefully
    connect(&m_timer, SIGNAL(timeout()), 
            this, SLOT(connectionTimeout()));
  }



  /**
   * Destructor
   */
  ResourceGet::~ResourceGet() {
    // m_reply already scheduled for deletion in connectionFinished
  }



  /**
   * Initiates the request for the resource at the given url
   * 
   * @param url (const QUrl &) The resource's URL
   * @param topath (QString) Local destination for the downloaded resource
   * @param timeout (int) Time (in milliseconds) to try before timeout occurs
   * 
   * @return (bool) Indicates if there were any problems creating the local file to write to
   */
  bool ResourceGet::getResource(const QUrl &url, QString topath, int timeout) {

    m_timeOut = timeout;

    // Need to setup output file 
    QString localFileName;
    if (topath.size() != 0) {
      localFileName += topath;
      localFileName += "/";
    }

    // The local file is named according to the external resource name
    // i.e. if there is no filename in the URL, we can't name our local file to write to
    localFileName +=  QFileInfo(url.path()).fileName();
    if (localFileName.isEmpty() ) {
      QString msg = QString("URL has no filename, can't create local output file");
      m_progress.SetText(msg);
      if (!m_isInteractive)
         cout << msg.toStdString() << endl;

      m_error = true;
      return m_error;
    }

    // Handle any problems with opening the local output file
    m_file.setFileName(localFileName);
    if (!m_file.open(QIODevice::WriteOnly) ) {
      QString msg = QString("Cannot open output file: ");
      msg += m_file.error();
      m_progress.SetText(msg);

      if (!m_isInteractive)
         cout << msg.toStdString() << endl;

      m_error =  true;
      return m_error;
    }

    // Establish the connection and start the GET request
    m_networkMgr.connectToHost(url.host(),url.port());


    // We obtain ownership of the QNetworkReply *, so need to delete later

    m_reply = m_networkMgr.get(QNetworkRequest(url));

    QString productType(QSysInfo::productType());
    productType = productType.toLower();

    //If this application is running on OS X, then this if statement gets around the SSL Handshaking
    //errors because Qt is not looking for the DOI root certificate in the correct
    //place. 

    if (productType.contains("osx") ) {
       
       m_reply->ignoreSslErrors();
    }


    connect(m_reply, SIGNAL(finished()),
            this, SLOT(connectionFinished()));
    connect(m_reply, SIGNAL(readyRead()),
            this, SLOT(downloadReadyRead()));
    connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(updateDownloadProgress(qint64, qint64)));

    m_lastDone = -1;
    m_error = false;
    return m_error;
  }



   //Timeout event handler monitors the ftp connection and gracefully
  //closes and exits the application if a timeout occurs.
  void ResourceGet::connectionTimeout() {
      QString timeoutSecs = QString("Timeout error:  GET request exceeded ") +
              QString::number(m_timeOut)+ QString(" ms.");

      m_progress.SetText(timeoutSecs);

      // Will let user know there was a timeout
      m_errorMessage = timeoutSecs;

      // Note that finished() SIGNAL will be emitted when aborting
      m_reply->abort();
  }



  //! Handles when the connection finishes
  void ResourceGet::connectionFinished() {
    // This will handle an abort() as well
    if (m_reply->error()) {
      // Error message is already set if we encountered a TIMEOUT
      if (m_errorMessage.isEmpty()) {
        m_errorMessage = m_reply->errorString();
        
      }
      
      if (m_errorMessage.contains("Timeout error")) {
        m_error = false;
        if (!m_isInteractive) {
          cout << m_errorMessage << endl;
        }
      }
      else {
        m_error = true;
      }

      removeLocalFile();
    }

    else {
      m_file.close();
      // this was added because final size may not match progress size so
      // you do not get 100% processed
      if (!m_isInteractive) {
        cout << "100% Processed" << endl;
      }
    }

    // QNetworkAccessManager::get gave us ownership of the QNetworkReply *
    m_reply->deleteLater();
    m_reply = NULL;

    emit done();
  }



  //! Slot that is invoked whenever there is data available to read over connection
  void ResourceGet::downloadReadyRead() {
    if (m_file.exists()) {
      m_file.write(m_reply->readAll());
    }
  }



  //! Removes the local file if there is an error with the download
  void ResourceGet::removeLocalFile() {
    bool fileExists = false;
    QString fileRemovedQStr;

    fileExists = m_file.exists();

    if (fileExists) {
      m_file.close();
      m_file.remove();
    }
  }



  // tjw:  ftpProgress uses the ISIS progress class to track progress
  void ResourceGet::updateDownloadProgress(qint64 read, qint64 total) {
    m_timer.start(m_timeOut);

    if (total == 0) return;
    if (total == -1) return;
    if (m_error) return;
    if (m_lastDone < 0) {        
      m_progress.SetText(QString("Downloading File ") + m_file.fileName());
      m_progress.SetMaximumSteps(total);
      m_progress.CheckStatus();
      m_lastDone = 1;
    }

    while (m_lastDone <= read) {
      m_progress.CheckStatus();
      m_lastDone++;
    }
   }

}
