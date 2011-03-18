#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H

#include <QWidget>

class QBoxLayout;
class QItemSelection;
class QSplitter;
class QStringList;
class QTableView;
class QTreeView;


namespace Isis
{
  class ConnectionModel;
  class ControlNet;
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
      QBoxLayout * createMainLayout();
      void nullify();
      void focusView(QTreeView * view, QStringList label);


    private slots:
      void pointViewSelectionChanged();
      void serialViewSelectionChanged();
      void connectionViewSelectionChanged();


    private: // data
      bool updatingSelection;


    private: // widgets
      QTreeView * pointView;
      QTreeView * serialView;
      QTreeView * connectionView;

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


    private:
      Isis::ControlNet * controlNet;
  };

}

#endif

