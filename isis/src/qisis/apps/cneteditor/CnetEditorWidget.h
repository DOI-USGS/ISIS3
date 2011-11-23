#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H


#include <QWidget>

class QAction;
class QBoxLayout;
class QGroupBox;
template< typename T > class QList;
class QMenu;
class QScrollArea;
class QSplitter;
class QString;
class QToolBar;

namespace Isis
{
  class ControlNet;
  
  namespace CnetViz
  {
    class AbstractTreeItem;
    class FilterWidget;
    class ImageImageTreeModel;
    class ImagePointTreeModel;
    class MeasureTableModel;
    class PointMeasureTreeModel;
    class PointTableModel;
    class TableView;
    class TreeView;
  }

  class CnetEditorWidget : public QWidget
  {
      Q_OBJECT

    public:
      enum View
      {
        PointView,
        ImageView,
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
      void setTablesFrozen(bool);


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
      void handleTableFilterCountsChanged(int visibleRows, int totalRows,
          QGroupBox * box, QString initialText);


    private slots:
      void rebuildModels();
      void rebuildModels(QList< CnetViz::AbstractTreeItem * > itemsToDelete);
      
      void pointColToggled();
      void measureColToggled();
      
      void handlePointTableFilterCountsChanged(int visibleRows, int totalRows);
      void handleMeasureTableFilterCountsChanged(int visibleRows,
                                                 int totalRows);


    private: // data
      bool updatingSelection;
      ControlNet * controlNet;
      QByteArray * topSplitterDefault;
      QString * workingVersion;
      static const QString VERSION;


    private: // widgets
      CnetViz::TreeView * pointTreeView;
      CnetViz::TreeView * imageTreeView;
      CnetViz::TreeView * connectionTreeView;

      CnetViz::TableView * pointTableView;
      CnetViz::TableView * measureTableView;
      
      QGroupBox * pointTableBox;
      QGroupBox * measureTableBox;
      
      QScrollArea * filterArea;

      QWidget * pointFilterWidget;
      QWidget * serialFilterWidget;
      QWidget * connectionFilterWidget;

      CnetViz::PointMeasureTreeModel * pointModel;
      CnetViz::ImagePointTreeModel * imageModel;
      CnetViz::ImageImageTreeModel * connectionModel;

      CnetViz::PointTableModel * pointTableModel;
      CnetViz::MeasureTableModel * measureTableModel;

      QSplitter * topSplitter;
      QSplitter * mainSplitter;

      QMap< QAction *, QList< QString > > * menuActions;
      QMap< QString, QList< QAction * > > * toolBarActions;

      QString * settingsPath;
  };
}

#endif

