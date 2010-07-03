#ifndef MosaicMainWindow_h
#define MosaicMainWindow_h

#include <QToolBar>
#include <QMenu>
#include <QGraphicsView>

#include "Filename.h"
#include "MainWindow.h"
#include "MosaicWidget.h"
#include "MosaicItem.h"


namespace Qisis {
  /**
  * @brief 
  *
  * @ingroup Visualization Tools
  *
  * @author Stacy Alley
  *
  * @internal
  *  
  *  @history 2010-05-10 Christopher Austin - added cnet connectivity
  *  	      functionality
  */
  class ToolPad;

  class MosaicMainWindow : public Qisis::MainWindow {
    Q_OBJECT
    public:
      MosaicMainWindow (QString title, QWidget *parent=0);
      QToolBar *permanentToolBar () { return p_permToolbar; };
      QToolBar *activeToolBar () { return p_activeToolbar; };
      Qisis::ToolPad *toolPad() { return p_toolpad; };
      QProgressBar *progressBar() { return p_progressBar; };
       /**
       * Returns the View menu.
       * 
       * 
       * @return QMenu* 
       */
      QMenu *viewMenu() const { return p_viewMenu; };

    protected:
      bool eventFilter(QObject *o,QEvent *e);
      void readSettings();
      void writeSettings();

    public slots:
      void open();
      void openList();
      void saveList();
      void saveProject();
      void saveProjectAs();
      void loadProject();
      void exportView();
      void hideShowColumns(QAction *action);
 
    private:
      void setupMenus();
      void setupPvlToolBar(); 

      QToolBar *p_permToolbar; //!< Tool bar attached to mainwindow
      Qisis::ToolPad *p_toolpad; //!< Tool pad on this mainwindow
      QToolBar *p_activeToolbar; //!< The active toolbar
      std::map<QString,QMenu *> p_menus; //!< Maps Menus to string names
      std::string p_appName; //!< The main application
      QString p_filename;

      QProgressBar *p_progressBar; //!< The mainwindow's progress bar.

      QAction *p_nameColumn;
      QAction *p_itemColumn;
      QAction *p_footprintColumn;
      QAction *p_outlineColumn;
      QAction *p_imageColumn;
      QAction *p_labelColumn;
      QAction *p_resolutionColumn;
      QAction *p_emissionAngleColumn;
      QAction *p_incidenceAngleColumn;
QAction *p_islandColumn;
      QAction *p_notesColumn;
      QAction *p_referenceFootprint;

      QMenu *p_viewMenu;
      
  };
};

#endif
