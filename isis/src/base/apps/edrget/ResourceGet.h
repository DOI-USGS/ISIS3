#ifndef RESOURCEGET_H
#define RESOURCEGET_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QString>
#include <QTimer>

#include "Progress.h"

class QNetworkReply;
class QUrl;

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2016-02-08 Ian Humphrey - Replaced ftpget and httpget classes with ResourceGet
   *                           class to handle generic resource requests (Qt5). This was done
   *                           as the previous classes relied on QFtp and QHttp, which are 
   *                           deprecated in Qt5.
   */
  class ResourceGet : public QObject {
      Q_OBJECT

    public:
      ResourceGet(QObject *parent = 0);
      ~ResourceGet();

      bool getResource(const QUrl &url, QString topath, int timeout);

      inline bool error() const {
        return m_error;
      };
      inline QString errorMessage() const {
        return m_errorMessage;
      }

    signals:
      void done();

    private slots:
      //tjw
      void connectionTimeout();
      void connectionFinished();
      void downloadReadyRead();
      void updateDownloadProgress(qint64 read, qint64 total);
      

    private:
      void removeLocalFile();
      
      bool m_error;                       //!< Indicates if an error has occurred
      bool m_isInteractive;               //!< Indicates if application is interactive
      int m_lastDone;                     //!< Last read byte during download
      int m_timeOut;                      //!< Value (in milliseconds) before timeout occurs
      Progress m_progress;                //!< Keeps track of download progress
      QFile m_file;                       //!< Local file to write download data to
      QNetworkAccessManager m_networkMgr; //!< Manages the connection for the download
      QNetworkReply *m_reply;             //!< The reply that will contain data to read
      QString m_errorMessage;             //!< A string representation of an error that occurs
      QTimer m_timer;                     //!< Timer used to determine timeout
  };
}
#endif
