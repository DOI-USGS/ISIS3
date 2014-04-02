#ifndef Qisis_SocketThread_h
#define Qisis_SocketThread_h

#include <QThread>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2013-12-30 Kimberly Oyama and Stuart Sides - In run, changed the token used to
   *                           parse the input buffer from a space to the escape character
   *                           (ascii #27). Fixes #1551.
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


