#ifndef QIsisApplication_H
#define QIsisApplication_H

/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/06/29 23:46:35 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */


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
