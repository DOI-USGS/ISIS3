/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LatLonGridTool.h"

#include <QAction>
#include <QApplication>
#include <QPixmap>
#include <QStackedWidget>
#include <QPainter>

#include "MdiCubeViewport.h"
#include "ToolPad.h"
#include "Workspace.h"


namespace Isis {
  /**
   * Constructs an LatLonGridTool object.
   *
   * @param parent Parent widget
   *
   *
   */
  LatLonGridTool::LatLonGridTool(QWidget *parent) : Tool(parent) {
  }

  /**
   * Adds the LatLonGridTool to the tool pad.
   *
   * @param pad input - The tool pad that LatLonGridTool is to be added to
   *
   * @return QAction*
   */
  QAction *LatLonGridTool::toolPadAction(ToolPad *pad) {
    QAction *action = new QAction(pad);
    action->setIcon(QPixmap(toolIconDir() + "/grid.png"));
    action->setToolTip("Lat Lon Grid Tool (G)");
    action->setShortcut(Qt::Key_G);

    QString text  =
      "<b>Function:</b>  View lat lon grid \
      <p><b>Shortcut:</b> G</p> ";
    action->setWhatsThis(text);

    return action;
  }

  /**
   * Draws grid onto cube viewport
   * This is overiding the parents paintViewport member.
   *
   * @param vp Pointer to Viewport to be painted
   * @param painter
   */
  void LatLonGridTool::paintViewport(MdiCubeViewport *mvp, QPainter *painter) {
      drawLatLonGrid (mvp, painter);
  }

  /**
   * Draws grid
   * @param vp Viewport whose measurements will be drawn
   * @param painter
   */
  void LatLonGridTool::drawLatLonGrid(MdiCubeViewport *mvp, QPainter *painter) {
    double samp = 100;
    double line = 100;
    int x, y;
    mvp->cubeToViewport(samp, line, x, y);
    QBrush brush(Qt::gray);
    QPen pen(brush, 2);

    painter->setPen(pen);
    // TODO: draw grid lines based on lat lons
    painter->drawLine(20, 120, 250, 120);
  }

  /**
   * Creates the toolbar containing the lat-lon grid tool widgets
   *
   * @param active  input  The widget that will contain the lat-lon grid tool
   *                       specific widgets
   *
   * @return QWidget*
   */
  QWidget *LatLonGridTool::createToolBarWidget(QStackedWidget *active) {
    QWidget *container = new QWidget(active);
    container->setObjectName("LatLonGridToolActiveToolBarWidget");

    m_container = container;
    return container;
  }
}
