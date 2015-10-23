#ifndef HTTPGET_H
#define HTTPGET_H


#include "Progress.h"

#include <QFile>
#include <QHttp>
#include <QTimer>


class QUrl;
namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class HttpGet : public QObject {
      Q_OBJECT

    public:
      HttpGet(QObject *parent = 0);

      bool getFile(const QUrl &url, QString topath,int timeout);

      bool error() const {
        return p_error;
      };


    signals:
      void done();

    private slots:
      void httpDone(bool error);
      void httpProgress(int done, int total);
      //tjw
      void httpTimeout();

    private:
      QHttp p_http;
      QFile p_file;
      bool p_error;
      int p_lastDone;
      int p_timeOut;
      QTimer p_timer;
      Progress p_progress;

  };
}
#endif
