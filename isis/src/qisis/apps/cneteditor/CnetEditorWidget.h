#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H

#include <QWidget>

class QBoxLayout;
class QItemSelection;
class QModelIndex;
class QScrollArea;
class QSplitter;
class QStringList;
class QTableView;
class QTreeView;


namespace Isis
{
  class ConnectionModel;
  class ControlNet;
  class FilterWidget;
  class MeasureTableDelegate;
  class MeasureTableModel;
  class PointModel;
  class PointTableDelegate;
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
      CnetEditorWidget(Isis::ControlNet *);
      virtual ~CnetEditorWidget();
      int getDriverView() const;
      void readSettings();
      void writeSettings();


    public slots:
      void setDriverView(int);


    signals:
      void cnetModified();


    private:
      void nullify();
      QBoxLayout * createMainLayout();
      void createPointView();
      void createSerialView();
      void createConnectionView();
      void createFilterArea();
      void createEditPointView();
      void createEditMeasureView();
      void focusView(QTreeView * view, QStringList label);
      void syncFilterWidgets();
      void updateTreeItemsWithNewSelection(const QItemSelection & newSelected,
          const QItemSelection & newDeselected);


    private slots:
      void pointViewSelectionChanged(const QItemSelection &,
          const QItemSelection &);
      void serialViewSelectionChanged(const QItemSelection &,
          const QItemSelection &);
      void connectionViewSelectionChanged(const QItemSelection &,
          const QItemSelection &);

      void itemExpanded(const QModelIndex & index);
      void itemCollapsed(const QModelIndex & index);
      void rebuildModels();


    private: // data
      bool updatingSelection;
      Isis::ControlNet * controlNet;


    private: // widgets
      QTreeView * pointView;
      QTreeView * serialView;
      QTreeView * connectionView;

      QScrollArea * filterArea;

      FilterWidget * pointFilterWidget;
      FilterWidget * serialFilterWidget;
      FilterWidget * connectionFilterWidget;

      PointModel * pointModel;
      SerialModel * serialModel;
      ConnectionModel * connectionModel;

      PointTableModel * editPointModel;
      MeasureTableModel * editMeasureModel;

      PointTableDelegate * editPointDelegate;
      MeasureTableDelegate * editMeasureDelegate;

      QTableView * editPointView;
      QTableView * editMeasureView;

      QSplitter * topSplitter;
      QSplitter * mainSplitter;
  };
}

#endif

