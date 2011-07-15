#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H


#include <QWidget>


class QBoxLayout;
class QItemSelection;
class QModelIndex;
class QScrollArea;
class QSplitter;
class QString;
class QTableView;

namespace Isis
{
  class CnetView;
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
      CnetEditorWidget(Isis::ControlNet *, QString);
      virtual ~CnetEditorWidget();
      int getDriverView() const;
      void readSettings();
      void writeSettings();


    public slots:
      void setDriverView(int);
      void activatePointView();
      void activateSerialView();
      void activateConnectionView();


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
      void focusView(CnetView * view, QStringList label);
      void syncFilterWidgets();
      void upgradeVersion();


    private slots:
      void pointViewSelectionChanged();
      void serialViewSelectionChanged();
      void connectionViewSelectionChanged();

      void itemExpanded(const QModelIndex & index);
      void itemCollapsed(const QModelIndex & index);
      void rebuildModels();
      void scrollFilterAreaToBottom();
      void doScroll();
      void pointColToggled();
      void measureColToggled();


    private: // data
      bool updatingSelection;
      ControlNet * controlNet;
      QByteArray * topSplitterDefault;
      QString * workingVersion;
      static const QString VERSION;


    private: // widgets
      CnetView * pointView;
      CnetView * serialView;
      CnetView * connectionView;

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

      QString * settingsPath;
  };
}

#endif

