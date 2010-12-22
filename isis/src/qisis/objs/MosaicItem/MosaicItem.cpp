#include "MosaicItem.h"

#include <iostream>
#include <cfloat>

#include <QStyleOptionGraphicsItem>
#include <QPen>
#include <QPainter>
#include <QEvent>
#include <QBrush>
#include <QTreeWidgetItem>
#include <QApplication>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ImagePolygon.h"
#include "SerialNumber.h"
#include "Table.h"
#include "iString.h"
#include "Histogram.h"
#include "Statistics.h"
#include "PolygonTools.h"
#include "FileDialog.h"


namespace Qisis {
  /**
   * MosaicItem constructor
   *
   *
   * @param cubeFilename
   * @param parent
   */
  MosaicItem::MosaicItem(const QString &cubeFilename, MosaicWidget *parent, Isis::PvlGroup *group)
    : QGraphicsPolygonItem() {

    if(parent->projection() == 0) {
      std::string msg = "Parent does not have projection in MosaicWidget";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    p_filename = cubeFilename.trimmed().toStdString();
    //p_levelOfDetail = 0.025;
    p_imageTransparency = 180;
    p_pixRes = 0;
    p_emissionAngle = 0;
    p_incidenceAngle = 0;
    p_mp = NULL;
    p_secondItem = NULL;
    p_groundMap = NULL;
    p_label = new QGraphicsSimpleTextItem(QString::fromStdString(p_filename.Name()));
    p_label->setFlag(QGraphicsItem::ItemIsMovable);
    p_labelFont = QFont("Helvetica", 10);
    p_label->setFont(p_labelFont);
    p_levelOfDetail = 0;
    p_updateFont = false;
    p_enablePaint = true;
    p_crossesBoundry = false;
    p_controlPointsVisible = false;
    p_lastExposedRect = QRectF(QPoint(0, 0), QPoint(0, 0));

    p_parent = parent;
    p_xmin = DBL_MAX;
    p_xmax = -DBL_MAX;
    p_ymin = DBL_MAX;
    p_ymax = -DBL_MAX;
    p_color = randomColor();

    p_treeItem = new QTreeWidgetItem();
    p_treeItem->setText(0, p_filename.Basename().c_str());
    p_treeItem->setBackground(0, QBrush(p_color));
    p_treeItem->setCheckState(1, Qt::Checked);
    p_treeItem->setCheckState(2, Qt::Checked);
    p_treeItem->setCheckState(3, Qt::Unchecked);
    p_treeItem->setCheckState(4, Qt::Unchecked);
    p_treeItem->setCheckState(5, Qt::Unchecked);

    createFootprint();

    //If this item was constructed with a PvlGroup
    //call setUpItem so set all the item's attributes
    //to the last state they were in.
    if(group != 0) {
      setUpItem(group);
    }

    setAcceptHoverEvents(true);
    
    setFootprintVisible(false);
    setOutlineVisible(true);
  }


  /**
   * 2nd MosaicItem constructor.
   * Private constructor only called from within this class if a
   * cube has >1 polygons. i.e. The image crosses the longitude
   * boundry.
   *
   *
   * @param parent
   */
  MosaicItem::MosaicItem(MosaicItem *parent) : QGraphicsPolygonItem() {
    setParentItem(parent);
    //This is what was cause the text label to not be moveable when
    // there were 2 polygon items!!!
    //parent->setHandlesChildEvents(true);

    p_proj = parent->getProj();
    if(p_groundMap != NULL) p_groundMap = parent->getGroundMap();

    p_filename = parent->p_filename;
    //p_levelOfDetail = 0.025;
    p_imageTransparency = 180;
    p_pixRes = parent->p_pixRes;
    p_incidenceAngle = parent->p_incidenceAngle;
    p_emissionAngle = parent->p_emissionAngle;
    p_mp = NULL;
    p_secondItem = NULL;
    p_levelOfDetail = 0;
    p_updateFont = false;
    p_enablePaint = true;
    p_label = parent->p_label;
    //p_label = new QGraphicsSimpleTextItem(QString::fromStdString(p_filename.Name()));
    //p_label->setFlag(QGraphicsItem::ItemIsMovable);
    //p_labelFont = QFont("Helvetica", 10);
    // p_label->setFont(p_labelFont);

    p_parent = parent->p_parent;
    p_xmin = parent->p_xmin;
    p_xmax = parent->p_xmax;
    p_ymin = parent->p_ymin;
    p_ymax = parent->p_ymax;
    p_color = parent->p_color;

    p_treeItem = NULL;
  }


  /**
   * Mosaic Item destructor
   *
   */
  MosaicItem::~MosaicItem() {
    if(p_mp == NULL) {
      delete p_mp;
    }
    if(p_treeItem == NULL) {
      delete p_treeItem;
    }
    if(p_secondItem == NULL) {
      delete p_secondItem;
    }
    if(p_camera == NULL) {
      delete p_camera;
    }
    if(p_proj == NULL) {
      delete p_proj;
    }
  }


  /**
   * This method is called from the constructor when there is a
   * group passed as one of the header args.
   * When this method is called it means a saved project file has
   * been read in and this sets up the item back to the way the
   * user had set it up before the save project command.
   *
   * @param grp
   */
  void MosaicItem::setUpItem(Isis::PvlGroup *grp) {

    //Set the item's color
    QString colorStr = grp->FindKeyword("Color")[0].ToQt();
    QColor color = QColor(colorStr);
    int alpha = grp->FindKeyword("Alpha")[0];
    color.setAlpha(alpha);
    this->setColor(color);

    //Set item state
    Isis::iString state = grp->FindKeyword("Item")[0];
    bool checked = state.Equal("Yes");
    this->setItemVisible(checked ? true : false);

    //Set footprint state
    state = grp->FindKeyword("Footprint")[0];
    checked = state.Equal("Yes");
    this->setFootprintVisible(checked ? true : false);

    //Set outline state
    state = grp->FindKeyword("Outline")[0];
    checked = state.Equal("Yes");
    this->setOutlineVisible(checked ? true : false);

    //Set image state
    state = grp->FindKeyword("Image")[0];
    checked = state.Equal("Yes");
    this->setImageVisible(checked ? true : false);

    //Set label state
    state = grp->FindKeyword("Label")[0];
    checked = state.Equal("Yes");
    this->setLabelVisible(checked ? true : false);

    //Look for the control points
    if(grp->HasKeyword("ControlPoints")) {
      p_controlPoints.clear();
      Isis::PvlKeyword points = grp->FindKeyword("ControlPoints");
      if(points[0].compare("Null") != 0) {
        for(int i = 0; i < points.Size(); i++) {
          Isis::iString point = points[i];
          double x = point.Token(":").ToDouble();
          double y = point.ToDouble();
          p_controlPoints.push_back(QPointF(x, y));
          //p_sceneToPointMap.insert(QString::fromStdString(point.Id()), mapToScene(x,y));
        }
      }

      //Set control points visible state
      if(grp->HasKeyword("ControlPointsVisible")) {
        state = grp->FindKeyword("ControlPointsVisible")[0];
        checked = state.Equal("Yes");
        this->setControlPointsVisible(checked);
      }
    }

  }

  /**
   * Re-paints the item
   *
   * @param painter
   * @param option
   * @param widget
   */
  void MosaicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    double itemLevelOfDetail =  option->levelOfDetail * p_pixRes;
    QColor tempColor =   p_color;

    // -------------------------------------------------------------------
    // If the level of detail is low, we do not want to allow the polygon
    // colors to have any transparency because it causes the application
    // to slow down significantly
    // -------------------------------------------------------------------
    if(itemLevelOfDetail < p_levelOfDetail) {
      tempColor.setAlpha(255);
    }

    // ------------------------------------------------------------------
    // If this item is a child item, then we need to check it's parent's
    // check state to see if we need to draw the OUTLINE.
    // ------------------------------------------------------------------
    if(p_treeItem == NULL) {
      if(((MosaicItem *)parentItem())->p_treeItem->checkState(3) == Qt::Checked) {
        paintOutline(painter);
      }
    }
    else if(p_treeItem->checkState(3) == Qt::Checked) {
      paintOutline(painter);
    }

    // ------------------------------------------------------------------
    // If this item is a child item, then we need th check it's parent's
    // check state for the FOOTPRINT.
    // ------------------------------------------------------------------
    if(p_treeItem == NULL) {
      if(((MosaicItem *)parentItem())->p_treeItem->checkState(2) == Qt::Checked) {
        paintFootprint(painter);
      }
    }
    else if(p_treeItem->checkState(2) == Qt::Checked) {
      paintFootprint(painter);
    }

    // ------------------------------------------------------------------
    // If this item is a child item, then we need th check it's parent's
    // check state for the IMAGE.
    // ------------------------------------------------------------------
    if(scene() != 0 && p_treeItem == NULL) {
      if(((MosaicItem *)parentItem())->p_treeItem->checkState(4) == Qt::Checked) {
        drawImage(painter, option);
      }
    }
    else if(scene() != 0 && p_treeItem->checkState(4) == Qt::Checked) {
      drawImage(painter, option);
    }

    // ----------------------------------------------------------------------
    // If this item is a PARENT item, then we need th check it's check state
    // for column 5 to know if we need to add the LABEL item.
    // ----------------------------------------------------------------------
    if(p_treeItem != NULL) {
      if(p_treeItem->checkState(5) == Qt::Checked) {
        // ----------------------------------------------
        // If this item has not child, then you know to
        // call paintLabel immediately.
        // ---------------------------------------------
        if(p_secondItem == NULL) {
          // ----------------------------------------------------------
          // The following if statement prevent you from getting into
          // an endless paint cycle.
          // -----------------------------------------------------------
          if(!scene()->items().contains(p_label) || !p_label->isVisible() ||
              (p_lastLevelOfDetail != option->levelOfDetail)) {
            paintLabel(option);
          }
        }
        // --------------------------------------------------
        // if this item does have a child and it's polygon
        // is smaller than this one, then call this paintLabel
        // ---------------------------------------------------
        if(p_secondItem != NULL && p_secondItem->boundingRect().width() < this->boundingRect().width()) {
          // ----------------------------------------------------------
          // The following if statement prevent you from getting into
          // an endless paint cycle.
          // -----------------------------------------------------------
          if(!scene()->items().contains(p_label) || !p_label->isVisible() ||
              (p_lastLevelOfDetail != option->levelOfDetail)) {
            paintLabel(option);
          }
        }

      }
      else {
        // User does not want the label item on the scene.
        if(scene()->items().contains(p_label)) {
          p_label->setVisible(false);
        }
      }
    }

    // ----------------------------------------------------------------------
    // If this item is a CHILD item, then we need th check it's parent's check
    // state for column 5 to know if we need to add the LABEL item.
    // ----------------------------------------------------------------------
    if(p_treeItem == NULL) {
      if(((MosaicItem *)parentItem())->p_treeItem->checkState(5) == Qt::Checked) {
        // --------------------------------------------------
        // If the parent item is smaller than this item then
        // we know we can the child item to paint the label.
        // ---------------------------------------------------
        if(parentItem()->boundingRect().width() < this->boundingRect().width()) {
          // ----------------------------------------------------------
          // The following if statement prevent you from getting into
          // an endless paint cycle.
          // -----------------------------------------------------------
          if(!scene()->items().contains(p_label) || !p_label->isVisible() ||
              (p_lastLevelOfDetail != option->levelOfDetail)) {
            paintLabel(option);
          }
        }
      }
      else {
        // User does not want the label item on the scene.
        if(scene()->items().contains(p_label)) {
          p_label->setVisible(false);
        }
      }
    }

    // ----------------------------------------------------
    // Display the CONTROL POINTS if they've been read in.
    // ----------------------------------------------------
    if(p_controlPoints.size() > 0 && p_controlPointsVisible) {
      paintControlPoints(painter, option);
    }

    // ----------------------------------------------------------------------
    // if the item is selected then we draw the dotted line around it
    // to signify that it's selected. see qt_graphicsItem_highlightSelected()
    // ----------------------------------------------------------------------
    if(option->state & QStyle::State_Selected)
      qt_graphicsItem_highlightSelected(this, painter, option);

    p_lastLevelOfDetail = option->levelOfDetail;
  }


  /**
   * Called from the paint() function to paint the outline.
   *
   *
   * @param painter
   */
  void MosaicItem::paintOutline(QPainter *painter) {
    QColor tempColor = p_color;

    tempColor.setAlpha(255);
    painter->setPen(tempColor);
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(polygon(), fillRule());
  }


  /**
   * Called from the paint() function to paint the footprint.
   *
   *
   * @param painter
   */
  void MosaicItem::paintFootprint(QPainter *painter) {
    QColor tempColor = p_color;

    tempColor.setAlpha(p_imageTransparency);
    painter->setPen(tempColor);
    painter->setBrush(tempColor);
    painter->drawPolygon(polygon(), fillRule());
  }

  /**
   * Called from the paint() function to paint the control points.
   *
   *
   * @param painter
   */
  void MosaicItem::paintControlPoints(QPainter *painter,
                                      const QStyleOptionGraphicsItem *option) {
    QColor tempColor = p_color;
    tempColor.setAlpha(255);

    for(int i = 0; i < p_controlPoints.size(); i++) {
      if(p_controlPoints[i] == p_selectedPoint) {
        //We only need this threshold if we are going to display all control point measures.
        //if(p_controlPoints[i].x() <= p_selectedPoint.x()+.1 && p_controlPoints[i].x() >= p_selectedPoint.x()-.1
        // && p_controlPoints[i].y() <= p_selectedPoint.y()+.1 && p_controlPoints[i].y() >= p_selectedPoint.y()-.1) {
        painter->setPen(Qt::red);
      }
      else {
        painter->setPen(Qt::green);
      }

      //Now draw the points
      double lod =  option->levelOfDetailFromTransform(painter->worldTransform());
      painter->drawLine(QPointF(p_controlPoints[i].x() - 5 / lod, p_controlPoints[i].y()),
                        QPointF(p_controlPoints[i].x() + 5 / lod, p_controlPoints[i].y()));
      painter->drawLine(QPointF(p_controlPoints[i].x(), p_controlPoints[i].y() - 5 / lod),
                        QPointF(p_controlPoints[i].x(), p_controlPoints[i].y() + 5 / lod));
    }

    setSelectedPoint(QPointF(0, 0));
  }

  /**
   * Called from the paint() function to add the label.
   *
   *
   * @param levelOfDetail
   */
  void MosaicItem::paintLabel(const QStyleOptionGraphicsItem *option) {
    // ------------------------------------------------------
    // Position the label in the center of it's parent item.
    // If there are two polygons, we need to union them to get
    // the center of the total size.
    // ------------------------------------------------------
    QPolygonF poly2 = mapToScene(this->polygon());
    if(p_secondItem != NULL) {
      poly2 = mapToScene(p_secondItem->polygon().united(this->polygon()));
    }
    QPolygonF poly3 = mapToScene(option->exposedRect.toRect());
    QPolygon poly4 = poly2.toPolygon().intersected(poly3.toPolygon());
    //std::cout << "Painting label and set position to the center of poly 4" << std::endl;
    p_label->setPos(QPointF(poly4.boundingRect().center()));

    // ----------------------------------------------------
    // This part ensures that the label font size is always
    // reasonable now matter what the current zoom is.
    // ----------------------------------------------------
    QTransform m = scene()->views().last()->transform();
    p_label->resetTransform();
    p_label->scale(1.0 / m.m11(), 1.0 / m.m22());

    // ----------------------------------------------------------------
    // if this item has children, then we need to use the sum of the
    // height vs. the sum of double the width.
    // ----------------------------------------------------------------
    QRectF totalRect;
    if(p_crossesBoundry && p_secondItem != NULL) {
      totalRect = p_secondItem->boundingRect().united(polygon().boundingRect());
    }
    else {
      totalRect = polygon().boundingRect();
    }

    // --------------------------------------------------------------------
    // now we have the totalRectangle bounding the item even if it crosses
    // the lat/lon boundry.
    // --------------------------------------------------------------------
    if(totalRect.height() > 1.5 * (totalRect.width())) {
      p_label->rotate(90);
    }

    p_label->setZValue(zValue() + 1);
    p_label->setVisible(true);
    //TODO: check into why This causes problems
    //p_label->setParentItem(this);


    // -------------------------------------------------------
    // if this is the first time the label has been requested,
    // we need to add it to the scene
    // -------------------------------------------------------
    if(!scene()->items().contains(p_label)) {
      scene()->addItem(p_label);
      p_label->installSceneEventFilter(this);
    }
  }

  /**
   *
   *
   */
  /*void MosaicItem::screenResolution(){
   // Using these two points we can calculated the scene's width.
   QPointF point1 = getGraphicsView()->mapToScene(0,0);
   QPointF point2 = getGraphicsView()->mapToScene((int) getGraphicsView()->width(),0);
   double newWidth = point2.x() - point1.x();
   // The scene width divided by the viewport's width gives us the screen res.
   p_screenResolution = newWidth/getGraphicsView()->viewport()->width();
  }*/


  /**
   * This lets the item know which point has been selected.  We
   * need to know this for the paint function to know which point
   * to color red instead of green.
   *
   *
   * @param p
   */
  void MosaicItem::setSelectedPoint(QPointF p) {
    p_selectedPoint = p;
  }


  /**
   * Allows the programmer to set the level of detail at which the
   * paint function will not allow transparent footprint colors.
   *
   *
   * @param detail
   */
  void MosaicItem::setLevelOfDetail(double detail) {
    p_levelOfDetail = detail;
    if(children().size() > 0)((MosaicItem *)children()[0])->p_levelOfDetail = detail;
    if(parentItem() != 0)((MosaicItem *)parentItem())->p_levelOfDetail = detail;
  }


  /**
   * This method gets the footprint polygon of the cube
   *
   */
  void MosaicItem::createFootprint() {
    Isis::Cube cube;
    QString qstring;

    try {
      cube.Open(p_filename.Expanded());
      //cube.Open(p_filename.Name());
    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }

    // The Archive group should never be searched for anything ever!
    //Get the product ID and set the label text to that.
    //Isis::PvlKeyword keyword = cube.Label()->FindObject("IsisCube").FindGroup("Archive").FindKeyword("ProductId");
    //p_label->setPlainText(keyword[0].ToQt());
    //p_label->setText(keyword[0].ToQt());

    //Get the camera stats table from the cube.
    //This is how we know the cube's resolution.
    //This is the reason we need to run camstats
    //with the attach option set to yes.
    try {
      Isis::Table table("CameraStatistics", p_filename.Expanded());
      //Isis::Table table("CameraStatistics", p_filename.Name());
      for(int i = 0; i < table.Records(); i++) {
        for(int j = 0; j < table[i].Fields(); j++) {
          if(table[i][j].IsText()) {
            qstring = QString::fromStdString((std::string)table[i][j]);
            qstring.truncate(10);
          }

          // Get the average resolution for this mosaic item.
          if(table[i][j].IsText() && qstring.compare("Resolution") == 0) {
            if(j + 3 < table[i].Fields()) {
              if(table[i][j+3].IsInteger()) {
              }
              else if(table[i][j+3].IsDouble()) {
                p_pixRes = (double)table[i][j+3];
              }
              else if(table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average emission angle for this mosaic item.
          if(table[i][j].IsText() && qstring.compare("EmissionAn") == 0) {
            if(j + 3 < table[i].Fields()) {
              if(table[i][j+3].IsInteger()) {
              }
              else if(table[i][j+3].IsDouble()) {
                p_emissionAngle = (double)table[i][j+3];
              }
              else if(table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average incidence angle for this mosaic item.
          if(table[i][j].IsText() && qstring.compare("IncidenceA") == 0) {
            if(j + 3 < table[i].Fields()) {
              if(table[i][j+3].IsInteger()) {
              }
              else if(table[i][j+3].IsDouble()) {
                p_incidenceAngle = (double)table[i][j+3];
              }
              else if(table[i][j+3].IsText()) {
              }
            }
          }

        } // end for table[i].Fields
      } // end for table.Records

    }
    catch(Isis::iException &e) {
      std::string msg = "Could not find the CameraStatistics Table.  Please run camerastats with the 'attach' option";
      QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }

    //Construct a polygon object and set the name to the cube's serial number.

    Isis::ImagePolygon *poly;
    try {
      poly = new Isis::ImagePolygon();
      cube.Read(*poly);

    }
    catch(Isis::iException &e) {
      cube.Close();
      std::string msg = "footprintinit must be run before reading the polygon.";
      QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }

    cube.Close();

    //Get the footprint polygon(s) of this cube.  All polys will
    //be in 0-360 longitude domain because footprintinit make 'em that way.
    //So if the user wants a -180-180 longitude domain, then we have to have
    //the polygon ready to for that domain too.
    p_mp = (geos::geom::MultiPolygon *)poly->Polys()->clone();
    delete poly;

    reproject();
  }


  /**
   * Called anytime the user reprojects the cube. (Selects a new
   * map file.) And everytime a mosaic item is created.
   */
  void MosaicItem::reproject() {
    double xmin = DBL_MAX;
    double xmax = -DBL_MAX;
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;

    geos::geom::MultiPolygon *mp;
    p_proj = p_parent->projection();

    if(p_proj->Has180Domain()) {
      p_180mp = Isis::PolygonTools::To180(p_mp);
      mp = p_180mp;
    }
    else {
      mp = p_mp;
    }

    //----------------------------------------------------------
    // We need to loop thru the num. geom. because some of the
    // cubes will have more than one geom. if it crosses lat/lon
    // boundries.
    //----------------------------------------------------------
    for(unsigned int i = 0; i < mp->getNumGeometries(); i++) {
      const geos::geom::Geometry *geom = mp->getGeometryN(i);
      geos::geom::CoordinateSequence *pts;

      pts = geom->getCoordinates();
      double lat, lon;
      QVector<QPointF> polyPoints;

      //--------------------------------------------------------------
      // We need to convert the footprint polygons from lat/lon to x/y
      // in order to display them in the QGraphicsScene
      //--------------------------------------------------------------
      for(unsigned int j = 0; j < pts->getSize(); j++) {
        lat = pts->getY(j);
        lon = pts->getX(j);
        if(p_proj->SetUniversalGround(lat, lon)) {
          double x = p_proj->XCoord();
          double y = -1 * (p_proj->YCoord());

          //-----------------------------------------------
          // determine x/y min/max for the polygons(s)
          // so we know how big to make the QGraphicsScene
          //-----------------------------------------------
          if(x < xmin) xmin = x;
          if(y < ymin) ymin = y;
          if(x > xmax) xmax = x;
          if(y > ymax) ymax = y;

          polyPoints.push_back(QPointF(x, y));
        }
      }

      p_xmin = xmin;
      p_xmax = xmax;
      p_ymin = ymin;
      p_ymax = ymax;

      if(i == 0) {
        p_footprintPoly = QPolygonF(polyPoints);
        setPolygon(p_footprintPoly);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setBrush(p_color);
        setPen(p_color);
      }
      else {
        //----------------------------------------------------
        // This means we have a second polygon, so we need to
        // create a new MosaicItem for it. (i.e. this cube
        // crosses a lat/lon boundry.)
        //---------------------------------------------------
        if(p_secondItem == NULL) {
          p_secondItem = new MosaicItem(this);
        }
        p_footprintPoly = QPolygonF(polyPoints);
        p_secondItem->setPolygon(p_footprintPoly);
        p_secondItem->setFlag(QGraphicsItem::ItemIsSelectable);
        p_secondItem->setBrush(p_color);
        p_secondItem->setPen(p_color);
        p_crossesBoundry = true;
      }
      delete pts;
    }
    if(p_controlPoints.size() > 0 && p_controlPointsVisible) {
      displayControlPoints(p_controlNet);
    }
  }


  /**
   * Translates the screen coordinates(x,y) to camera coordinates.
   *
   *
   * @param x
   * @param y
   *
   * @return QPointF
   */
  QPointF MosaicItem::screenToCam(int x, int y) {
    QPointF camPoints;
    QPointF scenePoint = scene()->views().last()->mapToScene(QPoint(x, y));

    if(!p_proj->SetWorld(scenePoint.x(), -scenePoint.y())) {
      camPoints.setX(-1);
      camPoints.setY(-1);
      return camPoints;
    }

    double lat = p_proj->UniversalLatitude();
    double lon = p_proj->UniversalLongitude();
    if(!p_groundMap->SetUniversalGround(lat, lon)) {
      camPoints.setX(-1);
      camPoints.setY(-1);
      return camPoints;
    }

    camPoints.setX(p_groundMap->Sample() + 0.5);
    camPoints.setY(p_groundMap->Line() + 0.5);

    return camPoints;
  }


  /**
   * Translates the screen coordinates(x,y) to line/sample.
   *
   *
   * @param x
   * @param y
   *
   * @return QPointF
   */
  QPointF MosaicItem::screenToCam(QPointF p) {
    QPointF camPoints;

    if(!p_proj->SetWorld(p.x(), -p.y())) {
      camPoints.setX(-1);
      camPoints.setY(-1);
      return camPoints;
    }

    double lat = p_proj->UniversalLatitude();
    double lon = p_proj->UniversalLongitude();
    if(!p_groundMap->SetUniversalGround(lat, lon)) {
      camPoints.setX(-1);
      camPoints.setY(-1);
      return camPoints;
    }

    camPoints.setX(p_groundMap->Sample());
    camPoints.setY(p_groundMap->Line());

    return camPoints;
  }


  /**
   * Translates screen points to lat/lon for reporting in the
   * lower right corner of the qmos window.
   *
   *
   * @param point
   *
   * @return QPointF
   */
  QPointF MosaicItem::screenToGround(QPointF point) {
    QPointF groundPoints;
    if(!p_proj->SetWorld(point.x(), -point.y())) {
      groundPoints.setX(-1);
      groundPoints.setY(-1);
      return groundPoints;
    }

    double lat = p_proj->Latitude();
    double lon = p_proj->Longitude();
    groundPoints.setX(lat);
    groundPoints.setY(lon);

    return groundPoints;
  }


  /**
   * Returns true if the distance between two points is less than
   * .5, otherwise returns false.
   *
   *
   * @param trueMidX
   * @param trueMidY
   * @param testMidX
   * @param testMidY
   *
   * @return bool
   */
  bool MosaicItem::midTest(double trueMidX, double trueMidY,
                           double testMidX, double testMidY) {
    //since sqrt is expensive, we will just compare dist to .5^2.
    //double dist = sqrt(pow(testMidX - trueMidX,2) + pow(testMidY - trueMidY,2));
    double dist = (testMidX - trueMidX) * (testMidX - trueMidX) +
                  (testMidY - trueMidY) * (testMidY - trueMidY);
    if(dist < .5 * .5) {
      return true;
    }
    else {
      return false;
    }
  }


  /**
   * Returns the pixel value at the given sample/line.
   *
   *
   * @param sample
   * @param line
   *
   * @return int
   */
  double MosaicItem::getPixelValue(int sample, int line) {
    Isis::Brick gryBrick(1, 1, 1, p_cube.PixelType());
    gryBrick.SetBasePosition((int)(sample + 0.5), (int)(line + 0.5), 1);
    p_cube.Read(gryBrick);

    double pixelValue = gryBrick[0];
    if(pixelValue == Isis::Null) {
      return Isis::Null;
    }
    if(pixelValue < 0) pixelValue = 0;
    if(pixelValue > 255) pixelValue = 255;
    return pixelValue;
  }


  /**
   * This method reads in and draws the image associated with this
   * item.
   * @param painter
   */
  void MosaicItem::drawImage(QPainter *painter, const QStyleOptionGraphicsItem *option) {
    //std::cout << "\nTop drawImage for " << filename().Name() << std::endl;
    //if paint is not enabled, then just draw the last image and return
    //the paint is only disable when in zoom mode and the mouse button is down.
    if(!p_enablePaint) {
      painter->drawImage(this->polygon().boundingRect(), p_lastImage);
      return;
    }

    //If the exposed rect is the same as the last exposed rect, then don't draw again.
    //just draw the last image and return
    /*if(option->exposedRect == p_lastExposedRect) {
      std::cout << "This is the same exposed rect, just drawing the last image and returning." << std::endl;
      painter->drawImage(this->polygon().boundingRect(), p_lastImage);
      return;
    }
    p_lastExposedRect = option->exposedRect;*/

    try {
      p_cube.Open(p_filename.Expanded());
      //p_cube.Open(p_filename.Name());
    }
    catch(Isis::iException &e) {
      std::string msg = "Can not open this cube!";
      QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }

    if(p_groundMap == NULL) {
      try {
        Isis::Pvl pvl(p_filename.Expanded());
        //Isis::Pvl pvl(p_filename.Name());
        p_groundMap = new Isis::UniversalGroundMap(pvl);
      }
      catch(Isis::iException &e) {
        std::string msg = "Could not get a ground map for this cube.";
        QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
        return;
      }
    }

    getStretch();

    try {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      QPolygon poly2 = scene()->views().last()->mapFromScene(this->polygon());
      //QPolygon poly3 = scene()->views().last()->mapFromScene(option->exposedRect.toRect());
      //QPolygon poly4 = poly2.intersected(poly3);
      //QRect boundingBox = poly4.boundingRect();
      QRect boundingBox = poly2.boundingRect();
      //QRect boundingBox = poly3.boundingRect();
      //std::cout << "x Top right of bounding Rect = " << poly2.boundingRect().topRight().x() << " , " << poly2.boundingRect().topRight().y() << std::endl;
      //std::cout << "x Top right of exposed Rect = " << boundingBox.topRight().x() << " , " << boundingBox.topRight().y()<< std::endl;

      int bbWidth = (int)boundingBox.width();
      int bbHeight = (int)boundingBox.height();

      int bbx = boundingBox.x();
      int bby = boundingBox.y();


      //create a QImage the size of the polygon's bounding box.
      QImage image(bbWidth, bbHeight, QImage::Format_ARGB32);

      // Looping through the height of the bounding box.
      for(int h = bby; h <= (const int)(bby + bbHeight - 1); h++) {
        //check to make sure we are not outside the viewport's height.
        if(h < 0)continue;
        if(h > scene()->views().last()->viewport()->height())continue;

        QRgb *rgb = (QRgb *)image.scanLine(h - bby);

        //Fill the bounding Box with a white, transparent color.
        for(int w = bbx; w <= (const int)(bbx + bbWidth - 1); w++) {
          rgb[w - bbx] = qRgba(255, 255, 255, 0);
        }

        //--------------------------------------------------------
        // Get a list of x values where the polygon intersects with
        // the bounding box at the current line (h).
        //---------------------------------------------------------
        QList<int> inter = scanLineIntersections(poly2, h, bbWidth);

        //Check to see if the number of x values returned for that line is even.
        //if (inter.size() % 2 != 0) continue;

        //----------------------------------------------
        // now loop thru the x values and fill in the
        // pixels between the two points.
        //---------------------------------------------

        while(inter.size() > 1) {

          //------------------------------------------
          // set starting x to the second to last value
          //------------------------------------------
          int sX = inter[inter.size() - 2];

          //---------------------------------------------
          // set ending x to the last value and remove it
          // so that inter.size shrinks by 1
          //---------------------------------------------
          int eX = inter.takeLast();

          // the way it was before 2/6/09 SLA
          //int eX = inter.takeLast();
          //int sX = inter.takeLast();


          //-------------------------------------------------
          // when I comment out this if statement, it gets
          // rid or the problem with the verticle lines
          // ------------------------------------------------
          //if (eX - sX < 10) {
          for(int i = sX; i < eX; i++) {
            QPointF cameraCoords = screenToCam(i, h);
            double samp = cameraCoords.x();
            double line = cameraCoords.y();
            if(samp < 0.5) continue;
            if(line < 0.5) continue;
            if(line > p_cube.Lines() + 0.5) continue;
            if(samp > p_cube.Samples() + 0.5) continue;

            double pixelValue = getPixelValue((int)(samp + 0.5), (int)(line + 0.5));
            int strValue = (int)p_stretch.Map(pixelValue);

            rgb[i - bbx] = qRgba(strValue, strValue, strValue, 255);
          }
          //---------------- Just a test
        }
      }


      //---------------- End test
      /*  continue;
      //}

      // Starting line/sample
      QPointF cameraCoords = screenToCam(sX,h);
      double sSamp = cameraCoords.x();
      double sLine = cameraCoords.y();

      // Ending line/sample
      cameraCoords = screenToCam(eX,h);
      double eSamp = cameraCoords.x();
      double eLine = cameraCoords.y();

      // Midpoint
      int trueMidX = (sX + eX) / 2;
      cameraCoords = screenToCam(trueMidX,h);
      double trueSamp = cameraCoords.x();
      double trueLine = cameraCoords.y();

      if (sSamp == -1 || eSamp == -1 || trueSamp == -1) {
        inter.push_back(sX);
        inter.push_back(trueMidX);
        inter.push_back(trueMidX+1);
        inter.push_back(eX);
        continue;
      }

      // Calculate the slopes
      double lineSlope = (eLine - sLine) / (eX - sX);
      double sampSlope = (eSamp - sSamp) / (eX - sX);

      // Test to see if trueMidX is within 1/2 a pixel of samp midX
      double testSamp = sSamp + sampSlope * (trueMidX - sX);
      double testLine = sLine + lineSlope * (trueMidX - sX);
      if (!midTest(trueSamp, trueLine, testSamp, testLine)) {
        inter.push_back(sX);
        inter.push_back(trueMidX);
        inter.push_back(trueMidX+1);
        inter.push_back(eX);
        continue;
      }

      // start the loop here.
      double line = sLine;
      double samp = sSamp;
      for (int x = sX; x <= eX; x++, samp+=sampSlope, line+=lineSlope) {

        if (samp < 0.5) continue;
        if (line < 0.5) continue;
        if (line > p_cube.Lines() + 0.5) continue;
        if (samp > p_cube.Samples() + 0.5) continue;

        double pixelValue = getPixelValue((int)(samp+0.5), (int)(line+0.5));
        int strValue = (int)p_stretch.Map(pixelValue);


        std::cout << "eX -sX > 10 drawing at Height = " << h << std::endl;
        rgb[x - bbx] = qRgba(strValue, strValue, strValue, 255);

      } //end inter loop
      } // end while
      } // end for height loop*/

      // Paint the image on the scene.
      //std::cout << "Painting image" << std::endl;
      painter->drawImage(this->polygon().boundingRect(), image);
      p_lastImage = image;
      QApplication::restoreOverrideCursor();

    }
    catch(Isis::iException &e) {
      std::cout << e.Errors() << std::endl;
    }

    p_cube.Close();
    //if (p_groundMap != NULL) delete p_groundMap;
    //std::cout << "Bottom drawImage" << std::endl;
  }


  /**
   * This method returns a list of x values where the polygon
   * intersects with the bounding box.
   *
   * @param poly
   * @param line
   * @param boxWidth
   *
   * @return QList<int>
   */
  QList<int> MosaicItem::scanLineIntersections(QPolygon poly, int y, int boxWidth) {
    QList <int> inter;
    for(int i = 0; i < poly.size() - 1; i++) {
      int n = i + 1;

      int yMax = qMax(poly.point(i).y(), poly.point(n).y());
      int yMin = qMin(poly.point(i).y(), poly.point(n).y());

      if(y < yMin) continue;
      if(y > yMax) continue;
      if(yMin == yMax) continue;

      if((poly.point(n).x() - poly.point(i).x()) == 0) {
        inter.push_back(poly.point(n).x());
      }
      else {
        double slope = (double)(poly.point(n).y() - poly.point(i).y()) / (poly.point(n).x() - poly.point(i).x());
        double x = (y - poly.point(i).y()) / slope + poly.point(i).x();
        inter.push_back((int)(x + 0.5));
      }
    }

    qSort(inter.begin(), inter.end());

    QList<int> uniqueList;
    for(int i = 0; i < inter.size(); i++) {
      if(uniqueList.size() == 0) {
        uniqueList.push_back(inter[i]);
      }
      else if(inter[i] != uniqueList.last()) {
        uniqueList.push_back(inter[i]);
      }
    }
    if(uniqueList.size() == 3) {
      uniqueList.removeAt(1);
    }

    return uniqueList;
  }


  /**
   * Creates and returns  a random color for the intial color of
   * the footprint polygon.
   */
  QColor MosaicItem::randomColor() {
    static bool firstTime = true;
    // This seeds the random number.
    if(firstTime) srand(5390);

    // Gives a random number between 0 and 255
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;

    firstTime = false;
    return QColor(red, green, blue, 180);
  }


  /**
   * Sets the color of the footprint
   * @param color
   */
  void MosaicItem::setColor(QColor color) {
    this->setBrush(color);
    this->setPen(color);
    if(children().size() > 0) {
      //if(children()[0]->isSelected()) {
      ((MosaicItem *)children()[0])->setPen(color);
      ((MosaicItem *)children()[0])->setBrush(color);
      ((MosaicItem *)children()[0])->p_color = color;
      ((MosaicItem *)children()[0])->setTransparency(color.alpha());
      //}
    }
    if(parentItem() != 0) {
      ((MosaicItem *)parentItem())->setPen(color);
      ((MosaicItem *)parentItem())->setBrush(color);
    }
    p_treeItem->setBackground(0, color);

    //ensure good contrast against bgcolor.
    const QColor bgcolor = color;
    if(bgcolor.red() > 127 || bgcolor.green() > 127 || bgcolor.blue() > 127
        || bgcolor.alpha() < 127) {
      // black text
      const QColor fgcolor(0, 0, 0);
      p_treeItem->setForeground(0, fgcolor);
    }
    else {
      // white text
      const QColor fgcolor(255, 255, 255);
      p_treeItem->setForeground(0, fgcolor);
    }

    p_color = color;

    setTransparency(color.alpha());

  }


  /**
   * Sets the alpha channel of the footprint.
   *
   *
   * @param alpha
   */
  void MosaicItem::setTransparency(int alpha) {
    p_imageTransparency = alpha;
    p_color.setAlpha(alpha);

    if(p_treeItem != NULL) {
      p_treeItem->setBackground(0, p_color);
    }

    update();

    // if(p_treeItem == NULL) {
    // return;
    //}

    //if(children().size() > 0) {
    //((MosaicItem *) children()[0])->setTransparency(alpha);
    //}
    /* update();*/
  }


  /**
   * Hides/shows footprint
   * @param visible
   */
  void MosaicItem::setItemVisible(bool visible) {
    setVisible(visible);
    if(p_treeItem != NULL) p_treeItem->setCheckState(1, visible ? Qt::Checked : Qt::Unchecked);
    if(p_label != NULL) p_label->setVisible(visible);
    if(p_secondItem != NULL) p_secondItem->setItemVisible(visible);
  }


  /**
   * Hide/show the image associated with this item.
   * @param visible
   */
  void MosaicItem::setImageVisible(bool visible) {
    p_treeItem->setCheckState(4, visible ? Qt::Checked : Qt::Unchecked);
    update();
  }


  /**
   * Selects/Unselects the tree item associated with this item.
   * @param selected
   */
  void MosaicItem::setTreeItemSelected(bool selected) {
    if(p_treeItem->isSelected() != selected) {
      p_treeItem->setSelected(selected);
    }
  }


  /**
   * This method draws a dashed line around the item representing
   * the item's bounding box.
   * Called from the paint method.
   *
   * @param item
   * @param painter
   * @param option
   */
  void MosaicItem::qt_graphicsItem_highlightSelected(
    QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option) {

    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if(qFuzzyCompare(qMax(murect.width(), murect.height()), qreal(0.0)))
      return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if(qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
      return;

    qreal itemPenWidth;
    switch(item->type()) {

      case QGraphicsPolygonItem::Type:
        itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
        break;
      default:
        itemPenWidth = 1.0;
    }
    const qreal pad = itemPenWidth / 2;

    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
      fgcolor.red()   > 127 ? 0 : 255,
      fgcolor.green() > 127 ? 0 : 255,
      fgcolor.blue()  > 127 ? 0 : 255);

    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    // Ensure that the children of this item are selected whenever the parent is.
    QList<QGraphicsItem *>children = item->children();
    for(int j = 0; j < children.size(); j++) {
      children[j]->setSelected(true);
    }
    // If the child was the item selected, make sure that the parent is also selected
    if(item->parentItem() != 0) item->parentItem()->setSelected(true);
  }


  /**
   * This method sets the z values for this item and gives it's
   * children, if there are any, the same z value.
   *
   *
   * @param z
   */
  void MosaicItem::setZValue(qreal z) {
    QGraphicsItem::setZValue(z);
    if(children().size() > 0) children()[0]->QGraphicsItem::setZValue(z);
    if(parentItem() != 0) parentItem()->QGraphicsItem::setZValue(z);

  }


  /**
   * This method makes sure that when this item is selected, so
   * are its children if there are any.
   *
   * @param selected
   */
  void MosaicItem::setSelected(bool selected) {
    QGraphicsItem::setSelected(selected);
    if(children().size() > 0) children()[0]->QGraphicsItem::setSelected(selected);
    if(parentItem() != 0) parentItem()->QGraphicsItem::setSelected(selected);

  }


  /**
   * This method calculates the stretch for the image associated
   * with this mosaic item.
   */
  void MosaicItem::getStretch() {
    if(p_stretch.Pairs() != 0) return;
    //Isis::Histogram hist(p_cube,1);
    Isis::Histogram hist = *p_cube.Histogram(1);
    double bestMin = hist.BestMinimum();
    double bestMax = hist.BestMaximum();

    p_stretch.AddPair(bestMin, 0.0);
    p_stretch.AddPair(bestMax, 255.0);

    p_stretch.SetNull(0.0);
    p_stretch.SetLis(0.0);
    p_stretch.SetLrs(0.0);
    p_stretch.SetHis(255.0);
    p_stretch.SetHrs(255.0);
    p_stretch.SetMinimum(0.0);
    p_stretch.SetMaximum(255.0);

  }


  /**
   * Display or hide the product ID on the footprint.
   *
   * @param visible
   */
  void MosaicItem::setLabelVisible(bool visible) {
    if(visible) {
      p_treeItem->setCheckState(5, Qt::Checked);
    }
    else {
      p_treeItem->setCheckState(5, Qt::Unchecked);
    }
  }


  /**
   * Display or hide the outline of the image footprint.
   *
   * @param visible
   */
  void MosaicItem::setOutlineVisible(bool visible) {
    if(visible) {
      p_treeItem->setCheckState(3, Qt::Checked);
    }
    else {
      p_treeItem->setCheckState(3, Qt::Unchecked);
    }
  }


  /**
   * Display or hide the outline of the image footprint.
   *
   * @param visible
   */
  void MosaicItem::setFootprintVisible(bool visible) {
    if(visible) {
      p_treeItem->setCheckState(2, Qt::Checked);
    }
    else {
      p_treeItem->setCheckState(2, Qt::Unchecked);
    }

  }


  /**
   * Supplies the user with a font dialog box where they can
   * select the font size of their choice.
   */
  void MosaicItem::setFontSize() {
    bool ok;
    const QString caption = "Qmos rules! Select your font size";
    QFont font = QFontDialog::getFont(&ok, QFont("Helvetica", 10), p_parent, caption);
    if(ok) {
      // font is set to the font the user selected
      p_label->setFont(font);
      if(p_secondItem != NULL) setFontSize(font);
      p_updateFont = true;
      update();
    }

  }


  /**
   * Overloaded methoded for convenience.
   * Set the font size of the item's label.
   *
   *
   * @param font
   */
  void MosaicItem::setFontSize(QFont font) {
    p_label->setFont(font);
    p_updateFont = true;
    update();
  }


  /**
   * This is a way to allow the programmer to disable
   * the paint routine if need be.
   *
   * @param paint
   */
  void MosaicItem::setEnableRepaint(bool paint) {
    p_enablePaint = paint;
  }


  /**
   * This event filter catches all the events that happen to the
   * p_label (QGraphicTextItem).
   *
   *
   * @param watched
   * @param event
   *
   * @return bool
   */
  bool MosaicItem::sceneEventFilter(QGraphicsItem *watched, QEvent *event) {

    // if the item is not in the movable mode, then just need to return.
    if(watched->flags() != QGraphicsItem::ItemIsMovable)return false;

    switch(event->type()) {
        //-----------------------------------------------------------------------
        // We need to make sure the user does not drag the label beyond the area
        // of the bounding rectangle of the parent item's polygon.
        //-----------------------------------------------------------------------
      case QEvent::UngrabMouse: {
          QRectF totalRect;
          if(((QMouseEvent *) event)->button() == Qt::RightButton) return false;

          if(watched->parentItem() != 0) {
            // This give us the coordinates at which the label was dropped.
            QPointF dropPoint = p_label->mapToParent(((QGraphicsSceneMouseEvent *)event)->scenePos());

            //---------------------------------------------------------
            // If there are two polygon items (i.e. the item crosses
            // the lat/lon boundry then we need to combine the two
            // polygons boundingboxes together before we check if
            // the user droped the label within the correct area
            // for this item.
            //-------------------------------------------------------
            if(p_crossesBoundry) {
              totalRect = p_secondItem->boundingRect().united(watched->parentItem()->boundingRect());
            }
            else {
              totalRect = watched->parentItem()->boundingRect();
            }

            //-----------------------------------------------------------------
            // Check to make sure the point at which the label was dropped is
            // within the parent's bounding rectangle.  If the drop point is
            // outside of the bounding rect, then we put the label right
            // back in the center
            //-----------------------------------------------------------------
            if(!totalRect.contains(dropPoint)) {
              p_label->setPos(watched->parentItem()->boundingRect().center());
            }

          }
          break;
        }

      default: {
        }
    }
    return false;
  }


  /**
   * This method takes all the points in the passed in control
   * network and decides if the point is within this item.
   * If so, the point is then converted to projects space and then
   * converted to GraphicsScene space and pushed back in to a
   * QList of QPointF which is then used in the paint() method to
   * paint the plus symbols to represent the points.
   *
   *
   * @param netFile
   */
  void MosaicItem::displayControlPoints(Isis::ControlNet *cn) {
    p_controlPointsVisible = true;
    p_controlNet = cn;
    p_sceneToPointMap.clear();
    p_proj = p_parent->projection();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    Isis::Cube cube;
    try {
      cube.Open(p_filename.Expanded());
      //cube.Open(p_filename.Name());
    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }

    if(p_groundMap == NULL) {
      try {
        Isis::Pvl pvl(p_filename.Expanded());
        //Isis::Pvl pvl(p_filename.Name());
        p_groundMap = new Isis::UniversalGroundMap(pvl);
      }
      catch(Isis::iException &e) {
        std::string msg = e.Errors();
        QMessageBox::information(p_parent, "Error", QString::fromStdString(msg), QMessageBox::Ok);
        return;
      }
    }

    if(p_serialNumber.empty()) p_serialNumber = Isis::SerialNumber::Compose(cube);

    // Remove the old control points
    p_controlPoints.clear();

    for(int i = 0; i < cn->Size(); i++) {
      Isis::ControlPoint p = (*cn)[i];
      //------------------------------------------------------------
      // we are now looping thru all the measures, so therefore all
      // measures are being drawn to the scene.
      // TODO: Allow user to see all control point measures, as an
      // option.
      //------------------------------------------------------------
      //for (int j = 0; j < p.Size(); j++) {
      // This is how we decide if this point is in this item.
      if(p[0].GetCubeSerialNumber().compare(p_serialNumber))  continue;
      //if (p[j].CubeSerialNumber().compare(p_serialNumber))  continue;

      //----------------------------------------------
      // only use the first measure for all items.
      // because that is usually the reference point.
      //----------------------------------------------
      p_groundMap->SetImage(p[0].GetSample(), p[0].GetLine());
      //-------------------------------------------------------
      // the following commented out line causes us to display
      // all the measures in each control point.
      //---------------------------------------------------
      //p_groundMap->SetImage(p[j].Sample(),p[j].Line());
      double lat = p_groundMap->UniversalLatitude();
      double lon = p_groundMap->UniversalLongitude();
      // convert long. if necessary
      if(p_proj->Has180Domain()) {
        lon = p_proj->To180Domain(lon);
        if(p_proj->IsPositiveWest()) lon = p_proj->ToPositiveWest(lon, 180);
      }
      else if(p_proj->IsPositiveWest()) {
        lon = p_proj->ToPositiveWest(lon, 360);
      }
      // convert lat. if necessary
      if(p_proj->IsPlanetographic()) {
        lat = p_proj->ToPlanetographic(lat, p_proj->EquatorialRadius(), p_proj->PolarRadius());
      }

      if(p_proj->SetGround(lat, lon)) {
        double x = p_proj->XCoord();
        double y = -1 * (p_proj->YCoord());

        //---------------------------------------------------------------
        // check here so see if the point is within the bounding
        // box of this polygon.  This way we know if the point
        // is in the parent or the child polygon if there are two polys.
        //---------------------------------------------------------------
        if(this->polygon().boundingRect().contains(QPointF(x, y))) {
          p_controlPoints.push_back(mapToScene(x, y));
          p_sceneToPointMap.insert(QString::fromStdString(p.Id()), mapToScene(x, y));
        }

      }
      //}
    }

    cube.Close();
    update();
    QApplication::restoreOverrideCursor();

    // Now take care of the children
    QList<QGraphicsItem *>children = this->children();
    for(int j = 0; j < children.size(); j++) {
      ((MosaicItem *)children[j])->displayControlPoints(cn);
    }
  }


  /**
   * When the control points are visislbe, then the paint() method
   * knows to paint the points on the item.
   *
   *
   * @param visible
   */
  void MosaicItem::setControlPointsVisible(bool visible) {
    p_controlPointsVisible = visible;

    // If we do not have the points for this item yet, we need to get them!
    if(p_controlPoints.size() == 0 && p_controlPointsVisible) {
      displayControlPoints(p_parent->controlNet());
    }

    update();

    // Now take care of the children
    QList<QGraphicsItem *>children = this->children();
    for(int j = 0; j < children.size(); j++) {
      ((MosaicItem *)children[j])->setControlPointsVisible(visible);
    }
  }



  /**
   * This method saves all the item's current attributes to a pvl
   * group which is returned.
   *
   *
   * @return Isis::PvlGroup
   */
  Isis::PvlGroup MosaicItem::saveState() {
    Isis::PvlGroup grp("test");
    grp += Isis::PvlKeyword("Filename", this->filename().Expanded());
    grp += Isis::PvlKeyword("Color", this->color().name());
    grp += Isis::PvlKeyword("Alpha", this->color().alpha());
    grp += Isis::PvlKeyword("Group_Name", this->treeItem()->parent()->text(0).toStdString());

    // Check item state
    if(this->treeItem()->checkState(1) == Qt::Checked) {
      grp += Isis::PvlKeyword("Item", "Yes");
    }
    else {
      grp += Isis::PvlKeyword("Item", "No");
    }

    // Check footprint state
    if(this->treeItem()->checkState(2) == Qt::Checked) {
      grp += Isis::PvlKeyword("Footprint", "Yes");
    }
    else {
      grp += Isis::PvlKeyword("Footprint", "No");
    }

    // Check outline state
    if(this->treeItem()->checkState(3) == Qt::Checked) {
      grp += Isis::PvlKeyword("Outline", "Yes");
    }
    else {
      grp += Isis::PvlKeyword("Outline", "No");
    }

    // Check image state
    if(this->treeItem()->checkState(4) == Qt::Checked) {
      grp += Isis::PvlKeyword("Image", "Yes");
    }
    else {
      grp += Isis::PvlKeyword("Image", "No");
    }

    // Check label state
    if(this->treeItem()->checkState(5) == Qt::Checked) {
      grp += Isis::PvlKeyword("Label", "Yes");
    }
    else {
      grp += Isis::PvlKeyword("Label", "No");
    }

    // Save control points
    Isis::PvlKeyword keyword("ControlPoints");
    if(p_controlPointsVisible) {
      for(int i = 0; i < p_controlPoints.size(); i++) {
        Isis::iString controlPoint =  p_controlPoints[i].x();
        controlPoint.append(":");
        Isis::iString temp =  p_controlPoints[i].y();
        controlPoint.append(temp);
        keyword.AddValue(controlPoint);
      }
      grp.AddKeyword(keyword);
    }

    // Check control points visible state
    if(p_controlPointsVisible) {
      grp += Isis::PvlKeyword("ControlPointsVisible", "Yes");
    }
    else {
      grp += Isis::PvlKeyword("ControlPointsVisible", "No");
    }

    return grp;
  }

  /*void MosaicItem::resizeEvent(QGraphicsSceneResizeEvent*){
    // Centre the contained text item
    QTransform m = sceneTransform();
    p_label->resetTransform();
    p_label->scale( 1.0 / m.m11(), 1.0 / m.m22() );
    QRectF m_textRect = p_label->boundingRect();
    QPolygonF itemTextPoly = p_label->mapToItem( this, m_textRect );
    QRectF itemTextRect = itemTextPoly.boundingRect();
    double dx = 0.5 * m.m11() * ( rect().width() - itemTextRect.width() );
    double dy = 0.5 * m.m22() * ( rect().height() - itemTextRect.height() );
    p_label->translate( dx, dy );
  }*/


}

