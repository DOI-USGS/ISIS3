#ifndef CnetEditorWidget_H
#define CnetEditorWidget_H


#include <QWidget>

#include "XmlStackedHandler.h"

class QAction;
class QBoxLayout;
class QGroupBox;
template< typename T > class QList;
class QMenu;
class QScrollArea;
class QSplitter;
class QString;
class QToolBar;
class QXmlStreamWriter;
class QXmlAttributes;

namespace Isis {
  class AbstractTableModel;
  class AbstractTreeItem;
  class Control;
  class ControlNet;
  class ControlPoint;
  class Directory;
  class FileName;
  class Image;
  class MosaicGraphicsView;
  class MosaicSceneItem;
  class MosaicTool;
  class ProgressBar;
  class Projection;
  class Project;
  class PvlGroup;
  class PvlObject;
  class ToolPad;
  class FilterWidget;
  class ImageImageTreeModel;
  class ImagePointTreeModel;
  class MeasureTableModel;
  class PointMeasureTreeModel;
  class PointTableModel;
  class TableView;
  class TreeView;

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
   *   @history 2017-05-18 Tracie Sucharski - Added a signal to indicate the control point chosen
   *                           from either the point table or the measure table.  If the point was
   *                           chosen from the measure table, the serial number of the measure is
   *                           also passed.  This was added for IPCE, for the interaction with other
   *                           views.
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2017-07-24 Makayla Shepherd - Fixed a seg fault in ipce that occurs when attempting
   *                           to edit a control point when there is not an active control network.
   *                           Fixes #5048.
   *   @history 2017-08-10 Christopher Combs - Added Apriori lat, lon, and radius labels. Fixes#5066
   *   @history 2017-08-11 Christopher Combs - Changed constructor to take in a Control instead of
   *                           a ControlNet. Added load and save methods as well as an XmlHandler
   *                           to allow for serialization of the widget into the project.
   *                           Fixes #4989.
   *   @history 2018-04-11 Tracie Sucharski - Moved the Xml serialization to the newly created
   *                           CnetEditorView class for ipce.
   */
  class CnetEditorWidget : public QWidget {
      Q_OBJECT

    public:
      enum View {
        PointView,
        ImageView,
        ConnectionView
      };

      CnetEditorWidget(Control *control, QString pathForSettings);
      virtual ~CnetEditorWidget();
      void readSettings();
      void writeSettings();

      QWidget *pointTreeView();
      QWidget *serialTreeView();
      QWidget *connectionTreeView();
      QWidget *pointFilterWidget();
      QWidget *serialFilterWidget();
      QWidget *connectionFilterWidget();
      TableView *pointTableView();
      TableView *measureTableView();
      ControlNet *control();


      AbstractTableModel *measureTableModel();
      AbstractTableModel *pointTableModel();

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
      void rebuildModels();


    signals:
      void cnetModified();
      void editControlPoint(ControlPoint *controlPoint, QString serialNumber);

    private slots:
      void rebuildModels(QList< AbstractTreeItem * > itemsToDelete);

      void pointColToggled();
      void measureColToggled();

      void handlePointTableFilterCountsChanged(int visibleRows, int totalRows);
      void handleMeasureTableFilterCountsChanged(int visibleRows,
          int totalRows);

    private:
      //methods
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

      // data
      bool m_updatingSelection;                                 //!< Updates selection
      Control *m_control;                                       //!< Control for this widget
      QString *m_workingVersion;                                //!< Working version
      static const QString VERSION;                             //!< Version

      //widgets
      TreeView *m_pointTreeView;                       //!< Point tree view
      TreeView *m_imageTreeView;                       //!< Image tree view
      TreeView *m_connectionTreeView;                  //!< Connection tree view

      TableView *m_pointTableView;                     //!< Point table view
      TableView *m_measureTableView;                   //!< Measure table view

      QGroupBox *m_pointTableBox;                               //!< Point table box
      QGroupBox *m_measureTableBox;                             //!< Measure table box

      QScrollArea *m_filterArea;                                //!< Scroll area for filters

      QWidget *m_pointFilterWidget;                             //!< Point filter widget
      QWidget *m_serialFilterWidget;                            //!< Serial filter widget
      QWidget *m_connectionFilterWidget;                        //!< Connection filter widget

      PointMeasureTreeModel *m_pointModel;             //!< Point tree model
      ImagePointTreeModel *m_imageModel;               //!< Image tree model
      ImageImageTreeModel *m_connectionModel;          //!< Connection tree model

      PointTableModel *m_pointTableModel;              //!< Point table model
      MeasureTableModel *m_measureTableModel;          //!< Measure table model

      QSplitter *m_mainSplitter;                                //!< Splitter

      QMap< QAction *, QList< QString > > * m_menuActions;      //!< QMap of menu actions
      QMap< QString, QList< QAction * > > * m_toolBarActions;   //!< QMap of tool bar actions

      QString *m_settingsPath; //!< Path to read/write settings
  };
}

#endif
