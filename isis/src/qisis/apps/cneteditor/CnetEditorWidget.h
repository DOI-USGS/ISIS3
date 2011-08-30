#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H


#include <QWidget>

class QAction;
class QBoxLayout;
template< typename T > class QList;
class QMenu;
class QScrollArea;
class QSplitter;
class QString;
class QToolBar;

namespace Isis
{
  class AbstractTreeItem;
  class CnetMeasureTableModel;
  class CnetPointTableModel;
  class CnetTableView;
  class CnetTreeView;
  class ConnectionModel;
  class ControlNet;
  class FilterWidget;
  class PointModel;
  class PointTableModel;
  class SerialModel;

  class CnetEditorWidget : public QWidget
  {
      Q_OBJECT

    public:
      enum View
      {
        PointView,
        SerialView,
        ConnectionView
      };


    public:
      CnetEditorWidget(ControlNet *, QString);
      virtual ~CnetEditorWidget();
      void readSettings();
      void writeSettings();

      QWidget * getPointTreeView();
      QWidget * getSerialTreeView();
      QWidget * getConnectionTreeView();
      QWidget * getPointFilterWidget();
      QWidget * getSerialFilterWidget();
      QWidget * getConnectionFilterWidget();
      
      QMap< QAction *, QList< QString > > getMenuActions();
      QMap< QString, QList< QAction * > > getToolBarActions();
      
      
    public slots:
      void setSortingEnabled(bool);


    signals:
      void cnetModified();
      

    private:
      void nullify();
      QBoxLayout * createMainLayout();
      void createActions();
      void createPointTreeView();
      void createSerialTreeView();
      void createConnectionTreeView();
      void createFilterArea();
      void createPointTableView();
      void createMeasureTableView();
      void upgradeVersion();


    private slots:
      void rebuildModels();
      void rebuildModels(QList< AbstractTreeItem * > itemsToDelete);
      void pointColToggled();
      void measureColToggled();


    private: // data
      bool updatingSelection;
      ControlNet * controlNet;
      QByteArray * topSplitterDefault;
      QString * workingVersion;
      static const QString VERSION;


    private: // widgets
      CnetTreeView * pointTreeView;
      CnetTreeView * serialTreeView;
      CnetTreeView * connectionTreeView;

      CnetTableView * pointTableView;
      CnetTableView * measureTableView;

      QScrollArea * filterArea;

      QWidget * pointFilterWidget;
      QWidget * serialFilterWidget;
      QWidget * connectionFilterWidget;

      PointModel * pointModel;
      SerialModel * serialModel;
      ConnectionModel * connectionModel;

      CnetPointTableModel * pointTableModel;
      CnetMeasureTableModel * measureTableModel;

      QSplitter * topSplitter;
      QSplitter * mainSplitter;

      QAction * enableSortAct;

      QMap< QAction *, QList< QString > > * menuActions;
      QMap< QString, QList< QAction * > > * toolBarActions;

      QString * settingsPath;
  };
}

#endif

