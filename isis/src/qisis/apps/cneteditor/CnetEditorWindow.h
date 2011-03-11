#ifndef CnetEditorWindow_H
#define CnetEditorWindow_H

#include <QMainWindow>

class QAction;
class QCloseEvent;
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
      void handleSynchronizeViews();


    private: // data
      ControlNet * cnet;
      CnetEditorWidget * editorWidget;
      QString * curFile;
      bool dirty;
      bool saveAsPvl;
      bool synchronized;


    private: // widgets
      QAction * openAct;
      QAction * saveAct;
      QAction * saveAsAct;
      QAction * aboutAct;
      QAction * closeAct;
      QAction * synchronizeAct;
      QAction * quitAct;

      QMenu * fileMenu;
      QMenu * helpMenu;

      QToolBar * mainToolBar;
  };
}

#endif
