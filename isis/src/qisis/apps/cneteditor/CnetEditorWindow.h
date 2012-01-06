#ifndef CnetEditorWindow_H
#define CnetEditorWindow_H

#include <QFileDialog>
#include <QMainWindow>

class QAction;
class QButtonGroup;
class QCloseEvent;
class QFont;
class QMenu;
class QString;
class QToolBar;


namespace Isis
{
  class ConcurrentControlNetReader;
  class ControlNet;
  class CnetEditorWidget;
  class ProgressBar;

  /**
   * This is the cneteditor program.
   *
   * @author 2011-??-?? Eric Hyer
   *
   * @internal
   */
  class CnetEditorWindow : public QMainWindow
  {
      Q_OBJECT

    public:
      enum FileState
      {
        HasFile,
        NoFile,
        FileLoading
      };


    public:
      CnetEditorWindow();
      virtual ~CnetEditorWindow();


    protected:
      void closeEvent(QCloseEvent * event);


    private:
      CnetEditorWindow(const CnetEditorWindow &);
      const CnetEditorWindow & operator=(CnetEditorWindow);
      
      
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
      void open();
      void save();
      void saveAs();
      void closeNetwork();
      void networkLoaded(ControlNet *);


    private: // widgets
      QAction * openAct;
      QAction * saveAct;
      QAction * saveAsAct;
      QAction * aboutAct;
      QAction * closeAct;
      QAction * quitAct;

      QMenu * fileMenu;
      QMenu * helpMenu;

      QToolBar * mainToolBar;
      QList< QToolBar * > * toolBars;

      ProgressBar * loadingProgressBar;

      QDockWidget * pointTreeDockWidget;
      QDockWidget * serialTreeDockWidget;
      QDockWidget * connectionTreeDockWidget;

      QDockWidget * pointFilterDockWidget;
      QDockWidget * serialFilterDockWidget;
      QDockWidget * connectionFilterDockWidget;


    private: // data
      ControlNet * cnet;
      ConcurrentControlNetReader * cnetReader;
      CnetEditorWidget * editorWidget;
      QString * curFile;
      QFont * labelFont;
      bool dirty;
      bool saveAsPvl;


    private: // constants
      static const int defaultWindowWidth = 1100;
      static const int defaultWindowHeight = 700;
  };


  /**
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   */
  class CnetEditorFileDialog : public QFileDialog
  {
    public:
      CnetEditorFileDialog(QLayout * l, QWidget * parent = NULL);
  };

}

#endif

