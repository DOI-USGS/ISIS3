#ifndef QIsisApplication_H
#define QIsisApplication_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QApplication>
#include <QUrl>

namespace Isis {
  /**
   * @brief Handles exceptions which the QT event handlers and
   *        QApplication do not handle.
   *
   * This class is derived of QApplication and handles the
   * exception on handled by QT event handlers and QApplication.
   * This class overrides QApplication's notify().
   *
   * @ingroup Qisis
   *
   * @author 2009-11-24 Sharmila Prasad
   *
   * @internal
   *   @history 2010-06-29 Steven Lambright - Added a setlocale to english for
   *                           numeric values
   *   @history 2012-03-22 Steven Lambright - Added the Url handler for http://
   *                           links. Currently using QWebView - we may want
   *                           options later.
   */
  class QIsisApplication : public QApplication {
      Q_OBJECT

    public:
      QIsisApplication(int &argc, char *argv[]);
      virtual bool notify(QObject *rec, QEvent *ev);

    public slots:
      void openUrl(QUrl url);
  };
};

#endif
