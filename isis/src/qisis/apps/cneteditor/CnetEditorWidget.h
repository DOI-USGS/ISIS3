#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H


#include <QWidget>

class QBoxLayout;
class QScrollArea;
class QSplitter;
class QString;

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
      int getDriverView() const;
      void readSettings();
      void writeSettings();

      QWidget * getPointTreeView();
      QWidget * getSerialTreeView();
      QWidget * getConnectionTreeView();
      QWidget * getPointFilterWidget();
      QWidget * getSerialFilterWidget();
      QWidget * getConnectionFilterWidget();


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
      void createPointTreeView();
      void createSerialTreeView();
      void createConnectionTreeView();
      void createFilterArea();
      void createPointTableView();
      void createMeasureTableView();
      void focusView(CnetTreeView * treeView, QStringList label);
      void upgradeVersion();


    private slots:
//      void pointTreeViewSelectionChanged();
      void serialTreeViewSelectionChanged();
      void connectionTreeViewSelectionChanged();

      void rebuildModels();
      void rebuildModels(QList<AbstractTreeItem *> itemsToDelete);
//       void scrollFilterAreaToBottom();
//       void doScroll();
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

      QString * settingsPath;
  };
}

#endif

