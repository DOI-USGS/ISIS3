#ifndef Qisis_SocketThread_h
#define Qisis_SocketThread_h

#include <QThread>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class SocketThread : public QThread {
      Q_OBJECT

    public:
      SocketThread(QObject *parent = 0);
      ~SocketThread();

      void run();
      void stop() {
        p_done = true;
      };

    signals:
      /**
       * New image signal.
       *
       * @param image
       */
      void newImage(const QString &image);
      //! Application has focus signal.
      void focusApp();

    private:
      bool p_done;
  };
};

#endif


