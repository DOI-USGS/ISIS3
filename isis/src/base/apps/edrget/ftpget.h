#ifndef FTPGET_H
#define FTPGET_H

#include <QFile>
#include <QFtp>
#include <QTimer>
#include "Progress.h"

class QUrl;
namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class FtpGet : public QObject {
      Q_OBJECT

    public:
      FtpGet(QObject *parent = 0);

      bool getFile(const QUrl &url, QString topath,int timeout);

      bool error() const {
        return p_error;
      };



    signals:
      void done();

      //tjw
      void dataTransferProgress(qint64, qint64);


    private slots:
      void ftpDone(bool error);
      void ftpProgress(qint64 done, qint64 total);

      //tjw
      void ftpTimeout();


    private:

      QFtp p_ftp;
      QFile p_file;
      bool p_error;
      int p_lastDone;
      Progress p_progress;


      int p_timeOut;
      QTimer p_timer;

  };
}
#endif
