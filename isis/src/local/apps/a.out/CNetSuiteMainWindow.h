#ifndef CNetSuiteMainWindow_H
#define CNetSuiteMainWindow_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include <QPointer>
#include <QProgressBar>

namespace Isis {
  class Directory;

  /**
   * @author 2012-??-?? Steven Lambright and Stuart Sides
   * 
   * @internal
   *   @history 2012-07-27 Kimberly Oyama and Steven Lambright - Removed progress and warnings
   *                           tab widgets. They are now dock widgets.
   *   @history 2012-08-28 Tracie Sucharski - The Directory no longer takes a container it its
   *                           constructor.
   *   @history 2012-09-17 Steven Lambright - Dock widgets now delete themselves on close. This
   *                          gives the user the correct options when proceeding in the interface,
   *                          but undo/redo are not implemented (it needs to eventually be
   *                          encapsulated in a work order). The undo/redo didn't work correctly
   *                          anyways before setting this flag, so it's an improvement. Example
   *                          change: If I close Footprint View 1, I'm no longer asked if I want
   *                          to view images in footprint view 1.
   */
  class CNetSuiteMainWindow : public QMainWindow {
      Q_OBJECT
    public:
      explicit CNetSuiteMainWindow(QWidget *parent = 0);
      ~CNetSuiteMainWindow();

    public slots:
      void addDock(QWidget *newWidgetForDock, Qt::DockWidgetArea area,
                   Qt::Orientation orientation);

    protected:
      void closeEvent(QCloseEvent *event);

    private slots:
      void configureThreadLimit();
      void enterWhatsThisMode();
      void removeCentralWidgetTab(int);

    private:
      Q_DISABLE_COPY(CNetSuiteMainWindow);

      void applyMaxThreadCount();
      void createMenus();
      void writeSettings();
      void readSettings();

    private:
      /**
       * The directory stores all of the work orders that this program is capable of doing. This
       *   drives most of the functionality.
       */
      QPointer<Directory> m_directory;

      /**
       * This is the "goal" or "estimated" maximum number of active threads running in this program
       *   at once. For now, the GUI consumes 1 thread and QtConcurrent
       *   (QThreadPool::globalInstance) consumes the remaining threads. Anything <= 1 means that we
       *   should perform a best-guess for best perfomance.
       */
      int m_maxThreadCount;
  };
}

#endif // CNetSuiteMainWindow_H
