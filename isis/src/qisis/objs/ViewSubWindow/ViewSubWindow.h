#ifndef ViewSubWindow_h
#define ViewSubWindow_h
/**
 * @file
 * $Date$
 * $Revision$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <QMainWindow>

namespace Isis {

  /**
   * @brief This class exists to contain detached views from ipce.
   *          the purpose is for it to emit the closeWindow() signal
   *          so that we can track when detached windows are closed. 
   *
   * @ingroup Visualization Tools
   *
   * @author 2017-10-25 Adam Goins
   *
   * @internal
   *
   *  @history 2017-10-25 Adam Goins - This class created. 
   */
  class ViewSubWindow : public QMainWindow {
      Q_OBJECT

    signals:
      void closeWindow(); //!< Signal called when the window receives a close event

    public:
      ViewSubWindow(QWidget *parent, Qt::WindowFlags flags = 0);
      virtual ~ViewSubWindow();

    protected:
      virtual void closeEvent(QCloseEvent *event);
  };
};

#endif
