#ifndef HTTPGET_H
#define HTTPGET_H

#include <QFile>
#include <QHttp>
#include "Progress.h"

class QUrl;
namespace Isis{

  class HttpGet : public QObject
  {
      Q_OBJECT
  
  public:
      HttpGet(QObject *parent = 0);
  
      bool getFile(const QUrl &url, std::string topath);

      bool error() const{return p_error;};
      
  
  signals:
      void done();
  
  private slots:
      void httpDone(bool error);
      void httpProgress(int done, int total);
  
  private:
      QHttp p_http;
      QFile p_file;
      bool p_error;
      int p_lastDone;
      Progress p_progress;

  };
}
#endif
