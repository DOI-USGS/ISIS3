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

namespace Isis {
  class ControlNet;

  namespace CnetViz {
    class AbstractTableModel;
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

  /**
   * This widget provides full editing, filtering and viewing capabilities for
   * the raw data in a control network. The raw data is, for example, chooser
   * name or cube serial number. The display is all textual. Please use
   * the widget accessors to appropriately place the various ancillary sections
   * of the editor.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2015-10-07 Ian Humphrey - Icons updated and no longer embedded (in order
   *                           to not violate licensing terms). Fixes #1041.
   */
  class CnetEditorWidget : public QWidget {
      Q_OBJECT

    public:
      enum View {
        PointView,
        ImageView,
        ConnectionView
      };


    public:
      CnetEditorWidget(ControlNet *, QString);
      virtual ~CnetEditorWidget();
      void readSettings();
      void writeSettings();

      QWidget *pointTreeView();
      QWidget *serialTreeView();
      QWidget *connectionTreeView();
      QWidget *pointFilterWidget();
      QWidget *serialFilterWidget();
      QWidget *connectionFilterWidget();

      CnetViz::AbstractTableModel *measureTableModel();
      CnetViz::AbstractTableModel *pointTableModel();

      QMap< QAction *, QList< QString > > menuActions();
      QMap< QString, QList< QAction * > > toolBarActions();

      ControlNet *filteredNetwork() const;

      bool measureTableSortingEnabled() const;
      int measureTableSortLimit() const;
      bool pointTableSortingEnabled() const;
      int pointTableSortLimit() const;

      void setMeasureTableSortingEnabled(bool enabled);
      void setMeasureTableSortLimit(int limit);
      void setPointTableSortingEnabled(bool enabled);
      void setPointTableSortLimit(int limit);


    public slots:
      void configSorting();
      void setTablesFrozen(bool);


    signals:
      void cnetModified();


    private:
      void nullify();
      QBoxLayout *createMainLayout();
      void createActions();
      void createPointTreeView();
      void createSerialTreeView();
      void createConnectionTreeView();
      void createFilterArea();
      void createPointTableView();
      void createMeasureTableView();
      void upgradeVersion();
      void handleTableFilterCountsChanged(int visibleRows, int totalRows,
          QGroupBox *box, QString initialText);


    private slots:
      void rebuildModels();
      void rebuildModels(QList< CnetViz::AbstractTreeItem * > itemsToDelete);

      void pointColToggled();
      void measureColToggled();

      void handlePointTableFilterCountsChanged(int visibleRows, int totalRows);
      void handleMeasureTableFilterCountsChanged(int visibleRows,
          int totalRows);


    private: // data
      bool m_updatingSelection;
      ControlNet *m_controlNet;
      QString *m_workingVersion;
      static const QString VERSION;


    private: // widgets
      CnetViz::TreeView *m_pointTreeView;
      CnetViz::TreeView *m_imageTreeView;
      CnetViz::TreeView *m_connectionTreeView;

      CnetViz::TableView *m_pointTableView;
      CnetViz::TableView *m_measureTableView;

      QGroupBox *m_pointTableBox;
      QGroupBox *m_measureTableBox;

      QScrollArea *m_filterArea;

      QWidget *m_pointFilterWidget;
      QWidget *m_serialFilterWidget;
      QWidget *m_connectionFilterWidget;

      CnetViz::PointMeasureTreeModel *m_pointModel;
      CnetViz::ImagePointTreeModel *m_imageModel;
      CnetViz::ImageImageTreeModel *m_connectionModel;

      CnetViz::PointTableModel *m_pointTableModel;
      CnetViz::MeasureTableModel *m_measureTableModel;

      QSplitter *m_mainSplitter;

      QMap< QAction *, QList< QString > > * m_menuActions;
      QMap< QString, QList< QAction * > > * m_toolBarActions;

      QString *m_settingsPath;
  };
}

#endif

