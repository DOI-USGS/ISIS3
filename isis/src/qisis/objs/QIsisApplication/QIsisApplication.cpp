#include "QIsisApplication.h"

#include <locale.h>

#include <QObject>

#include "Filename.h"

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
    catch(iException &e) {
      QString message = e.what();
      QMessageBox::critical(NULL,"Error",message);
      e.Clear();
    }
    return false;
  }
}
