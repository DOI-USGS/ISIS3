#include "IpceTool.h"

#include <sstream>
#include <vector>
#include <iomanip>

#include <QBrush>
#include <QDebug>
#include <QList>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QString>

#include "Application.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "CubeDnView.h"
#include "Directory.h"
#include "IException.h"
#include "MdiCubeViewport.h"
#include "Project.h"
#include "SerialNumber.h"
#include "ToolPad.h"
#include "UniversalGroundMap.h"
#include "ViewportMainWindow.h"
#include "Workspace.h"

using namespace std;

namespace Isis {
  /**
   * Ipce (Qnet) tool - Handles mouse button actions and drawing control points on viewports
   *
   * @param parent Pointer to the parent widget for the Ipce tool
   *
   * @author 2016-09-01 Tracie Sucharski
   *
   * @internal
   *
   */
  IpceTool::IpceTool (Directory *directory, QWidget *parent) : Tool(parent) {

    m_directory = directory;
    m_view = qobject_cast<CubeDnView *>(parent);
  }


  IpceTool::~IpceTool () {
  }


  /**
    * Adds the Ipce tool action to the tool pad.
    *
    * @param pad Tool pad
    * @return @b QAction* Pointer to Tie tool action
    *
    * @internal
    */
   QAction *IpceTool::toolPadAction(ToolPad *pad) {
     QAction *action = new QAction(pad);
//   action->setIcon(QPixmap(toolIconDir()+"/stock_draw-connector-with-arrows.png"));
     action->setIcon(QPixmap(toolIconDir()+"/HILLBLU_molecola.png"));
     action->setToolTip("Control Point Editor (T)");
     action->setShortcut(Qt::Key_T);
//     QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(showNavWindow(bool)));
     return action;
   }


   void IpceTool::setControlNet(ControlNet *cnet) {
     m_controlNet = cnet;
     refresh();
   }


  /**
   * Handle mouse events on CubeViewport
   *
   * @param p[in]   (QPoint)            Point under cursor in cubeviewport
   * @param s[in]   (Qt::MouseButton)   Which mouse button was pressed
   *
   * @author 2016-09-01 Tracie Sucharski
   *
   * @internal
   */
  void IpceTool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    MdiCubeViewport *cvp = cubeViewport();
    if (cvp  == NULL) return;

    // Determine if the cvp is a Shape
    //  Get all ShapeLists from Project
    bool isGroundSource = m_view->viewportContainsShape(cvp);

    double samp,line;
    cvp->viewportToCube(p.x(),p.y(),samp,line);
    QString sn = SerialNumber::Compose(cvp->cube()->fileName());

    if (s == Qt::LeftButton) {

//      if (sn == m_groundSN) {
//        QString message = "Cannot select point for editing on ground source.  Select ";
//        message += "point using un-projected images or the Navigator Window.";
//        QMessageBox::critical(m_ipceTool, "Error", message);
//        return;
//      }

      //  Find closest control point in network
      // since we are in a connected slot, we need to handle exceptions thrown by FindClosest
      try {
        ControlPoint *point = m_controlNet->FindClosest(sn, samp, line);
        emit modifyControlPoint(point);
      }
      catch (IException &ie) {
        QString message = "No points exist for editing. Create points using the right mouse";
        message += " button.";
        QMessageBox::warning(m_ipceTool, "Warning", message);
        return;
      }
    }
    
    else if (s == Qt::MidButton) {

      if (!m_controlNet || m_controlNet->GetNumPoints() == 0) {
        QString message = "No points exist for deleting.  Create points ";
        message += "using the right mouse button.";
        QMessageBox::warning(m_ipceTool, "Warning", message);
        return;
      }

//      if (m_groundOpen && file == m_groundCube->fileName()) {
//        QString message = "Cannot select point for deleting on ground source.  Select ";
//        message += "point using un-projected images or the Navigator Window.";
//        QMessageBox::critical(m_ipceTool, "Error", message);
//        return;
//      }
 
      //  Find closest control point in network
      ControlPoint *point = NULL;
      try {
        point = m_controlNet->FindClosest(sn, samp, line);

        if (point == NULL) {
          QString message = "No points exist for deleting.  Create points ";
          message += "using the right mouse button.";
          QMessageBox::warning(m_ipceTool, "Warning", message);
          return;
        }
      }
      catch (IException &e) {
        QString message = "Cannot find point on this image for deleting.";
        QMessageBox::critical(m_ipceTool, "Error", message);
        return;
      }

      emit deleteControlPoint(point);
    }
    else if (s == Qt::RightButton) {

      UniversalGroundMap *gmap = cvp->universalGroundMap();
      if (!gmap->SetImage(samp,line)) {
        QString message = "Invalid latitude or longitude at this point. ";
        QMessageBox::critical(NULL, "Error", message);
        return;
      }
      double lat = gmap->UniversalLatitude();
      double lon = gmap->UniversalLongitude();
//    qDebug()<<"IpceTool::mouseButton  lat = "<<lat<<"  lon = "<<lon;
      emit createControlPoint(lat, lon, cvp->cube(), isGroundSource);
//      if (m_groundOpen && file == m_groundCube->fileName()) {
//        createFixedPoint (lat,lon);
//      }
//      else {
//        createPoint(lat,lon);
//      }
    }
  }


  void IpceTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {
//  qDebug()<<"IpceTool::paintViewport";
    if (m_controlNet) {
      drawAllMeasurements(vp, painter);
    }
  }


  /**
   * This method will repaint the given Point ID in each viewport
   * Note: The pointId parameter is here even though it's not used because
   * the signal (IpceTool::editPointChanged connected to this slot is also
   * connected to another slot (QnetNavTool::updateEditPoint which needs the
   * point Id.  TODO:  Clean this up, use 2 different signals?
   *
   * @param pointId
   *
   * @internal
   * @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   */
  void IpceTool::paintAllViewports(QString pointId) {
//  qDebug()<<"IpceTool::paintAllViewports";

    // Take care of drawing things on all viewPorts.
    // Calling update will cause the Tool class to call all registered tools
    // if point has been deleted, this will remove it from the main window
    MdiCubeViewport *vp;
    for (int i=0; i<(int)cubeViewportList()->size(); i++) {
      vp = (*(cubeViewportList()))[i];
       vp->viewport()->update();
    }
  }

  /**
   * Draw all measurments which are on this viewPort
   * @param vp Viewport whose measurements will be drawn
   * @param painter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *   @history 2010-06-08 Jeannie Walldren - Fixed bug that was causing ignored
   *                          measures not be drawn as yellow unless IpceTool was
   *                          open
   *   @history 2010-07-01 Jeannie Walldren - Modified to draw points selected in
   *                          IpceTool last so they lay on top of all other points
   *                          in the image.
   *   @history 2011-04-15 Tracie Sucharski - Fixed bug which was causing all
   *                          measures to be drawn on all cubes.  Also removed
   *                          loop through measures, instead just get measure
   *                          for given serial number.
   *   @history 2011-10-20 Tracie Sucharski - Add check for a control network
   *                          that does not yet have any control points.
   *   @history 2011-11-09 Tracie Sucharski - If there are no measures for
   *                          this cube, return.
   */
  void IpceTool::drawAllMeasurements(MdiCubeViewport *vp, QPainter *painter) {
//  qDebug()<<"IpceTool::drawAllMeasurements  m_controlNet = "<<m_controlNet<<" # points = "<<m_controlNet->GetNumPoints();
    // Without a controlnetwork there are no points, or if new net, no points
    if (m_controlNet == 0 || m_controlNet->GetNumPoints() == 0) return;

    // Don't show the measurments on cubes not in the serial number list
    // TODO: Should we show them anyway
    // TODO: Should we add the SN to the viewPort
    QString serialNumber = SerialNumber::Compose(*vp->cube(), true);

//    if (serialNumber == m_groundSN) {
//      drawGroundMeasures(vp, painter);
//      return;
//    }
    if (!m_controlNet->GetCubeSerials().contains(
                      serialNumber)) return;
//    if (!m_serialNumberList->hasSerialNumber(serialNumber)) return;
    QList<ControlMeasure *> measures =
        m_controlNet->GetMeasuresInCube(serialNumber);
    // loop through all measures contained in this cube
    for (int i = 0; i < measures.count(); i++) {
      ControlMeasure *m = measures[i];
      // Find the measurments on the viewport
      double samp = m->GetSample();
      double line = m->GetLine();
      int x, y;
      vp->cubeToViewport(samp, line, x, y);
      // if the point is ignored,
      if (m->Parent()->IsIgnored()) {
        painter->setPen(QColor(255, 255, 0)); // set point marker yellow
      }
      // point is not ignored, but measure matching this image is ignored,
      else if (m->IsIgnored()) {
        painter->setPen(QColor(255, 255, 0)); // set point marker yellow
      }
      // Neither point nor measure is not ignored and the measure is fixed,
      else if (m->Parent()->GetType() != ControlPoint::Free) {
        painter->setPen(Qt::magenta);// set point marker magenta
      }
      else {
        painter->setPen(Qt::green); // set all other point markers green
      }
      // draw points
      painter->drawLine(x - 5, y, x + 5, y);
      painter->drawLine(x, y - 5, x, y + 5);
    }
//    // if IpceTool is open,
//    if (m_editPoint != NULL) {
//      // and the selected point is in the image,
//      if (m_editPoint->HasSerialNumber(serialNumber)) {
//        // find the measurement
//        double samp = (*m_editPoint)[serialNumber]->GetSample();
//        double line = (*m_editPoint)[serialNumber]->GetLine();
//        int x, y;
//        vp->cubeToViewport(samp, line, x, y);
//        // set point marker red
//        QBrush brush(Qt::red);
//        // set point marker bold - line width 2
//        QPen pen(brush, 2);
//        // draw the selected point in each image last so it's on top of the rest of the points
//        painter->setPen(pen);
//        painter->drawLine(x - 5, y, x + 5, y);
//        painter->drawLine(x, y - 5, x, y + 5);
//      }
//    }
  }


  /**
   * Refresh all necessary widgets in IpceTool including the PointEditor and
   * CubeViewports.
   *
   * @author 2008-12-09 Tracie Sucharski
   *
   * @internal
   * @history 2010-12-15 Tracie Sucharski - Before setting m_editPoint to NULL,
   *                        release memory.  TODO: Why is the first if statement
   *                        being done???
   * @history 2011-10-20 Tracie Sucharski - If no control points exist in the
   *                        network, emit proper signal and make sure editor
   *                        and measure table are hidden.
   *
   */
  void IpceTool::refresh() {
//  qDebug()<<"IpceTool::refresh   control net = "<<m_controlNet;
//    if (m_editPoint == NULL) {
//      paintAllViewports("");
//    }
//    else {
//      paintAllViewports(m_editPoint->GetId());
//    }
  }
}

