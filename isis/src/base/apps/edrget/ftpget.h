#ifndef FTPGET_H
#define FTPGET_H

#include <QFile>
#include <QFtp>
#include "Progress.h"

class QUrl;
namespace Isis{

  class FtpGet : public QObject
  {
      Q_OBJECT
  
  public:
      FtpGet(QObject *parent = 0);
  
      bool getFile(const QUrl &url, std::string topath);

      bool error() const{return p_error;};

      
  
  signals:
      void done();
  
  private slots:
      void ftpDone(bool error);
      void ftpProgress(qint64 done, qint64 total);
  
  private:
      QFtp p_ftp;
      QFile p_file;
      bool p_error;
      int p_lastDone;
      Progress p_progress;

  };
}
#endif
