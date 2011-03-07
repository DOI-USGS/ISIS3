#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H

#include <QWidget>

class QBoxLayout;
class QItemSelection;
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
      CnetEditorWidget(Isis::ControlNet *);
      virtual ~CnetEditorWidget();


    signals:
      void cnetModified();


    private:
      QBoxLayout * createMainLayout();
      void nullify();
      void focusView(QTreeView * view, QString label);


    private slots:
      void selectionChanged(const QItemSelection & selected,
          const QItemSelection & deselected);


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

      Isis::ControlNet * controlNet;
  };

}

#endif

