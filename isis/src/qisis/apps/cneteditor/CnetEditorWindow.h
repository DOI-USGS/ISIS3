#ifndef CnetEditorWindow_H
#define CnetEditorWindow_H

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
  class ControlNet;
  class CnetEditorWidget;

  class CnetEditorWindow : public QMainWindow
  {
      Q_OBJECT

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
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void readSettings();
      void writeSettings();
      bool okToContinue();
      void load(QString filename);
      void setHasFileState(QString filename);
      void setNoFileState();
      void setDirty(bool);


    private slots:
      void setDirty();
      void setSaveAsPvl(int);
      void open();
      void save();
      void saveAs();
      void closeNetwork();


    private: // data
      ControlNet * cnet;
      CnetEditorWidget * editorWidget;
      QString * curFile;
      QFont * labelFont;
      bool dirty;
      bool saveAsPvl;


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

      QButtonGroup * driveViewGrp;
  };
}

#endif
