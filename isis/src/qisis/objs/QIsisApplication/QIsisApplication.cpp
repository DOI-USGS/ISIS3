#include "QIsisApplication.h"

#include <locale.h>

#include <QDesktopServices>
#include <QObject>
#include <QMessageBox>
#include <QUrl>
#include <QWebView>

#include "FileName.h"
#include "IException.h"
#include "IString.h"

namespace Isis {
  /**
   * Constructor
   *
   * @param argc Pass this in from main(argc, argv)
   * @param argv Pass this in from main(argc, argv)
   */
  QIsisApplication::QIsisApplication(int &argc, char *argv[]) :
    QApplication(argc, argv) {
    // try to use US locale for numbers so we don't end up printing "," instead
    //   of "." where it might count.
    setlocale(LC_NUMERIC, "en_US");

    QDesktopServices::setUrlHandler("http", this, "openUrl");
  }


  /**
  * notify - this function overrides the QApplication notify as
  * QT event handlers do not handle exceptions. QIsisApplication
  * handles the exception by catching it and displaying the
  * MessageBox Warning.
  *
  * @author Sharmila Prasad (11/24/2009)
  *
  * @param rec - QObject where the exception occured
  * @param ev  - Event where the exception occured
  *
  * @return bool
  *
  * @internal
  * @history 2011-03-11 Tracie Sucharski - Create dialog with thrown errors
  *                        instead of ignoring and clearing.
  */
  bool QIsisApplication::notify(QObject *rec, QEvent *ev) {
    try {
      return QApplication::notify(rec, ev);
    }
    catch(IException &e) {
      QMessageBox::critical(NULL, "Error", e.what());
    }
    return false;
  }


  /**
   * Open a URL in the browser specified by Isis.
   */
  void QIsisApplication::openUrl(QUrl url) {
     QWebView *view = new QWebView(NULL);
     view->setAttribute(Qt::WA_DeleteOnClose);
     view->load(url);
     view->show();
  }
}
