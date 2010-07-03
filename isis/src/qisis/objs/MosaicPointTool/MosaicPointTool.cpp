#include "MosaicPointTool.h"

namespace Qisis {
  /**
   * MosaicPointTool constructor
   *
   *
   * @param parent
   */
  MosaicPointTool::MosaicPointTool(QWidget *parent) : Qisis::MosaicTool(parent) {
    p_parent = (MosaicWidget *)parent;
    p_pointInfoDialog = new QMessageBox(QMessageBox::Information,
                                        "Control Point Information",
                                        "Empty String", QMessageBox::Ok);
  }


  /**
   * Adds the action to the toolpad.
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *MosaicPointTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir() + "/stock_draw-connector-with-arrows.png"));
    action->setToolTip("Point (T)");
    action->setShortcut(Qt::Key_T);
    QString text  =
      "<b>Function:</b>  Set mode to Point selection mode. \
      <p><b>Shortcut:</b>  O</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Adds the pan action to the given menu.
   *
   *
   * @param menu
   */
  void MosaicPointTool::addToMenu(QMenu *menu) {

  }


  /**
   * Creates the widget to add to the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *MosaicPointTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);
    return hbox;
  }


  /**
   * Finds the given point in the control net, then dipslays an
   * information message box about the choosen control point.
   *
   *
   * @param p
   */
  void MosaicPointTool::findPoint(QPointF p, Isis::ControlNet *cn) {
    double minDistance = DBL_MAX;
    QPointF closestPoint;
    QString pointId = "No Point";

    // The allMosiacItem method returns all items including the item's children.
    QList<MosaicItem *> items = getWidget()->allMosaicItems();
    for(int i = 0; i < items.size(); i++) {
      QMap<QString, QPointF> pointsMap = items[i]->pointsMap();
      QList <QPointF> controlPoints = items[i]->controlPoints();
      double distance;
      for(int j = 0; j < controlPoints.size(); j++) {
        distance = sqrt(pow(p.x() - controlPoints[j].x(), 2) +
                        pow(p.y() - controlPoints[j].y(), 2));
        if(distance < minDistance) {
          minDistance = distance;
          closestPoint = controlPoints[j];
          pointId = pointsMap.key(closestPoint);
        }

      }

    }// End for i < items.size()

    //------------------------------------------------------------------
    // Make sure each item knows which point was selected so that it can
    // be painted red if the item contains that points.
    //------------------------------------------------------------------
    for(int k = 0; k < items.size(); k++) {
      items[k]->setSelectedPoint(closestPoint);
      //items[k]->update();
    }

    Isis::ControlPoint *controlPoint = cn->Find(pointId.toStdString());
    QString type = "no type";
    if(controlPoint->Type() == Isis::ControlPoint::Ground) {
      type = "Ground";
    }
    else if(controlPoint->Type() == Isis::ControlPoint::Tie) {
      type = "Tie";
    }

    QString serialNumbers;
    QList<MosaicItem *> measureItems;
    for(int m = 0; m < controlPoint->Size(); m++) {
      serialNumbers.append("\nControl Measure ");
      serialNumbers.append(QString::number(m + 1));
      serialNumbers.append(" Serial #:  ");
      serialNumbers.append(QString::fromStdString((*controlPoint)[m].CubeSerialNumber()));

      //------------------------------------------------------------------------
      // If the Mosaic Item is one of the control point's measures, then we want
      // to set that item's transparency to 255 and the others to 120.
      //------------------------------------------------------------------------
      for(int n = 0; n < items.size(); n++) {
        if((*controlPoint)[m].CubeSerialNumber().compare(items[n]->serialNumber()) == 0) {
          measureItems.push_back(items[n]);
        }
      }
    }

    QList<int> originalTrans;
    for(int p = 0; p < items.size(); p++) {
      // Save the original transparency values.
      originalTrans.push_back(items[p]->color().alpha());
      if(measureItems.contains(items[p])) {
        items[p]->setTransparency(255);
      }
      else {
        items[p]->setTransparency(20);
      }
    }

    p_pointInfoDialog->setText("Point ID: " + pointId
                               + "\nNumber of Measures: " + QString::number(controlPoint->Size())
                               + serialNumbers +
                               + "\nPoint Type: " + type);

    int ret = p_pointInfoDialog->exec();
    switch(ret) {
      case QMessageBox::Ok:
        //--------------------------------------------------------
        // This is where we need to reset the original transparency
        // values for all the items.
        //-------------------------------------------------------
        for(int q = 0; q < items.size(); q++) {
          items[q]->setTransparency(originalTrans[q]);
        }
        break;

      default:
        break;
    }

  }

}


