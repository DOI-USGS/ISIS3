#ifndef CnetEditorWindow_H
#define CnetEditorWindow_H

#include <QFileDialog>
#include <QMainWindow>
#include <qaction.h>

class QAction;
class QButtonGroup;
class QCloseEvent;
class QFont;
class QMenu;
class QString;
class QToolBar;


namespace Isis {
  class ConcurrentControlNetReader;
  class Control;
  class ControlNet;
  class CnetDisplayProperties;
  class CnetEditorWidget;
  class ProgressBar;


  /**
   * This is the cneteditor program.
   *
   * @author 2011-??-?? Eric Hyer
   *
   * @internal
   *   @history 2017-08-11 Christopher Combs - Changed networkLoaded() to take in a QString of
   *                           the current ControlNet file to create a Control object for
   *                           CnetEditorWidget's new constructor.
   */
  class CnetEditorWindow : public QMainWindow {
      Q_OBJECT

    public:
      enum FileState {
        HasFile,
        NoFile,
        FileLoading
      };


    public:
      CnetEditorWindow();
      virtual ~CnetEditorWindow();


    protected:
      void closeEvent(QCloseEvent *event);


    private:
      CnetEditorWindow(const CnetEditorWindow &);
      const CnetEditorWindow &operator=(CnetEditorWindow);


    private:
      void nullify();
      void createActions();
      void createDockWidgets();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void readSettings();
      void writeSettings();
      bool okToContinue();
      void load(QString filename);
      void loadCubeList(QString filename);
      void populateMenus();
      int indexOfActionList(QList< QAction * > actionList, QString actionText);
      void populateToolBars();
      int indexOfToolBar(QString objName);
      void removeEmptyMenus();
      void setFileState(FileState state, QString filename);
      void setDirty(bool);
      void setDockWidgetsVisible(bool visibilityState);


    private slots:
      void about();
      void setDirty();
      void setSaveAsPvl(int);
      void setSaveFilteredNetwork(bool);
      void openCubeList();
      void openNet();
      void save();
      void saveAs();
      void closeNetwork(bool promptToSave = true);
      void networkLoaded(ControlNet *, QString currentFile);
      void networkLoaded(QList<Control *>);
      void cubeListLoaded();


    private: // widgets
      QAction *openCubeListAct;
      QAction *openNetAct;
      QAction *saveAct;
      QAction *saveAsAct;
      QAction *aboutAct;
      QAction *closeAct;
      QAction *quitAct;

      QMenu *fileMenu;
      QMenu *helpMenu;

      QToolBar *mainToolBar;
      QList< QToolBar * > * toolBars;

      ProgressBar *cubeListProgressBar;

      QDockWidget *pointTreeDockWidget;
      QDockWidget *serialTreeDockWidget;
      QDockWidget *connectionTreeDockWidget;

      QDockWidget *pointFilterDockWidget;
      QDockWidget *serialFilterDockWidget;
      QDockWidget *connectionFilterDockWidget;


    private: // data
      ControlNet *cnet;
      CnetDisplayProperties *displayProperties;
      ConcurrentControlNetReader *cnetReader;
      CnetEditorWidget *editorWidget;
      QString *curFile;
      QString *cubeListFile;
      QFont *labelFont;
      bool dirty;
      bool saveAsPvl;
      bool saveFilteredNetwork;


    private: // constants
      static const int defaultWindowWidth = 1100;
      static const int defaultWindowHeight = 700;
  };


  /**
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   */
  class CnetEditorFileDialog : public QFileDialog {
    public:
      CnetEditorFileDialog(QLayout *l, QWidget *parent = NULL);
  };

}

#endif
