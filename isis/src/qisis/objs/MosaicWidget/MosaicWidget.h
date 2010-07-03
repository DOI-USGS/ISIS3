#ifndef MosaicWidget_H
#define MosaicWidget_H

#include <QSplitter>
#include <QMenu>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsScene>

#include <QLabel>
class QToolButton;

#include "ControlNet.h"
#include "Filename.h"
#include "MosaicTreeWidget.h"
#include "Projection.h"

namespace Qisis {
class MosaicItem;      
class MosaicMainWindow;
class MosaicZoomTool;  
class MosaicPanTool;   
class MosaicSelectTool;
class MosaicTrackTool; 
class MosaicPointTool; 
class MosaicControlNetTool; 
class MosaicFindTool;

  class MosaicWidget : public QSplitter {
    /**
    * @brief 
    *
    * @ingroup Visualization Tools
    *
    * @author Stacy Alley
    *
    * @internal
    *  
    *  @history 2010-05-10 Christopher Austin - added cnet connectivity
    *  	      functionality and fixed a few design issues
    */
    Q_OBJECT

    public:
      MosaicWidget(QWidget *parent = 0);

      /**
       * Returns the selected projection type.
       * 
       * 
       * @return Isis::Projection* 
       */
      Isis::Projection *projection() const { return p_projection; };

      /**
       * 
       * 
       * 
       * @return Isis::ControlNet* 
       */
      Isis::ControlNet *controlNet() const { return p_cn; };
      void setControlNet( Isis::Filename cnet );

      void hideControlPoints();
      void displayConnectivity( bool connected );

      /**
       * Returns the graphics scene.
       * 
       * 
       * @return QGraphicsScene* 
       */
      QGraphicsScene *scene() const { return p_graphicsScene; };

      /**
       * 
       * 
       * 
       * @return double 
       */
      double screenResolution() { return p_screenResolution; };

      /**
       * Returns a list of all the mosaic items.
       * 
       * 
       * @return QList* 
       */
      const QList<MosaicItem *> &mosaicItems() const { return p_mosaicItems; };
      

      QList<MosaicItem *> allMosaicItems();
      void updateScreenResolution(double resolution);

      // This is the column number for each column
      enum ColumnIndex {
	NameColumn = 0,
	ItemColumn = 1,
	FootprintColumn = 2,
	OutlineColumn = 3,
	ImageColumn = 4,
	LabelColumn = 5,
	ResolutionColumn = 6,
	EmissionColumn = 7,
	IncidenceColumn = 8,
	IslandColumn = 9,
	NotesColumn = 10
      };
                                              
    public slots:
      void addGroup(const QString &groupName);
      void deleteGroup(const QString &groupName);

      void addItem(MosaicItem *item, QString groupName = "Group1");
      void addItem(QString itemName);

      void open();
      void openList();

      void setProjection(Isis::Projection *proj); 
      void setLonDomain(QString domain);

      void updateGraphicsView(QTreeWidgetItem *item, int column);
      void updateGraphicsView();
      void updateTreeWidget();

      void refit();

      void viewMenuAction(QAction *action);

      /**
       * Returns the number of items currently in the scene.
       * 
       * 
       * @return int 
       */
      int numItems() const { return p_graphicsScene->items().size(); };
      void setLabelText(QString text){p_mapDisplay->setText(text);}

      void reprojectItems();
      void saveList(QString filename);
      void saveProject(QString filename);
      void readProject(QString filename);
      void setMapFile(QString mapfile);
      void setMapFile();

    protected:

    private slots:
      //void itemChanged(QTreeWidgetItem * item, int column);
      void groupChanged(QTreeWidgetItem * item, int column);
      void dropAction(QPoint point);
      void bringToFront();
      void sendToBack();
      void moveUpOne();
      void moveDownOne();
      void cut();
      void paste();
      void paste(QPoint point);
      void changeColor();
      void changeTransparency();
      void renameGroup();
      void hideItem();
      void showItem();
      void showItem(MosaicItem *item);
      void hideImage();
      void hideImage(MosaicItem *item);
      void setReferenceItemVisible(bool show);
      void showImage();
      void showImage(MosaicItem *item);
      void hideLabel();
      void showLabel();
      void hideOutline();
      void showOutline();
      void hideFootprint();
      void showFootprint();
      void setLabelFont();
      void contextMenuEvent(QContextMenuEvent * event);
      void changeLevelOfDetail(int detail);
      void deleteCube();
      void deleteAllCubes();
      void insertCube();
      void addGroup();
      void deleteGroup();
      void mergeGroups();
      void readFile(QString listFile);
      void displayControlPoints();
      void sortByResolution();
      void sortByEmission();
      void sortByIncidence();
      void sortByIsland();
      void zoomToItem();

  private:
      void initWidget();
      void insertCube(MosaicItem *item);
      bool eventFilter(QObject *o,QEvent *e);

      void bringToFront(MosaicItem *item);
      void bringToFront(QTreeWidgetItem *group);

      void sendToBack(MosaicItem *item);
      void sendToBack(QTreeWidgetItem *group);

      void moveUpOne(MosaicItem *item);
      void moveUpOne(QTreeWidgetItem *group);

      void moveDownOne(MosaicItem *item);
      void moveDownOne(QTreeWidgetItem *group);

      void fixZValue(QTreeWidgetItem *groupItem);
      void setInitialZValue(QTreeWidgetItem *groupItem);

      void reorderAllZValues();
      void reorderGroupZValues(QTreeWidgetItem *groupItem);
      void reorderMosaicItemsList();

      void createReferenceFootprint();
      void findPoint(QPointF p);

      void sortBy( ColumnIndex index );

      QList<QTreeWidgetItem *> selectedGroups(); //!< List of the selected groups in the tree widget.
      QList<MosaicItem *> selectedMosaicItems(); //!< List of the selected items in the tree widget.

      MosaicMainWindow *p_parent; //!< This object's parent

      QMenu* contextMenu();//!< The menu that pops up with a right mouse click.
      Isis::Projection *p_projection; //!< The current projection type.
      QGraphicsScene *p_graphicsScene; //!< The graphics scene that makes up this widget.
      QGraphicsView *p_graphicsView; //!< The graphics view
      MosaicTreeWidget *p_treeWidget; //!< The tree widget that makes up this widget.
      QList<MosaicItem *> p_pasteItems; //!< A list of items that can be pasted into a new group.
      QTreeWidgetItem *p_pasteGroup; //!< The group at which the paste items would like to be pasted in.
      QGraphicsTextItem *p_textItem; // !< A graphics items used to display messages in the graphics scene.

      QMap<QString, QTreeWidgetItem *> p_groupToTreeMap;//!< 
      QList<MosaicItem *> p_mosaicItems; //!< A list of all the currently display mosaic items.
      QMap<QTreeWidgetItem *, MosaicItem *> p_treeToMosaicMap;
      
      double p_xmin; //!< The graphics view's min x value.
      double p_xmax; //!< The graphics view's max x value.
      double p_ymin; //!< The graphics view's min y value.
      double p_ymax; //!< The graphics view's max y value.

      QStringList p_filterList;//!< Filter list for the open cube dialog.
      QStringList p_filterList2;//!< Filter list for the open list dialog.
      QDir p_dir;//!< Directory for the open and open list dialog boxes.

      int p_insertItemAt;//!< Insert item value.

      QLabel *p_mapDisplay;
      QToolButton *p_mapFileButton;
      QToolButton *p_controlPointButton;
      QToolButton *p_connectivityButton;
      QToolButton *p_stopProcessButton;
      QGraphicsPolygonItem *p_footprintItem;

      QAction *p_pan;
      double p_screenResolution;
      QRubberBand *p_rubberBand;
      QPoint p_origin;

      MosaicZoomTool *p_ztool;
      MosaicPanTool *p_ptool;
      MosaicSelectTool *p_stool;
      MosaicTrackTool *p_ttool;
      MosaicPointTool *p_pntool;
      MosaicControlNetTool *p_cntool;
      MosaicFindTool *p_ftool;

      QString p_mapfile;
      QString p_controlnetfile;
      QString p_lonDomain;
      QTreeWidgetItem *p_dropItem;
      Isis::ControlNet *p_cn;

  };

  bool sortResolution(QTreeWidgetItem *a,QTreeWidgetItem *b);
  bool sortEmission(QTreeWidgetItem *a,QTreeWidgetItem *b);
  bool sortIncidence(QTreeWidgetItem *a,QTreeWidgetItem *b);
  bool sortIsland(QTreeWidgetItem *a,QTreeWidgetItem *b);
};

#endif
