/**
 * @file
 * $Date: 2010/06/30 03:42:28 $
 * $Revision: 1.2 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include "MdiCubeViewport.h"

#include <QIcon>
#include <QPainter>

#include <iostream>
#include <string>

#include "Filename.h"
#include "Tool.h"
#include "RubberBandTool.h"
#include "StretchTool.h"


using namespace Isis;
using namespace std;


namespace Qisis {
  MdiCubeViewport::MdiCubeViewport(Cube *cube, QWidget *parent) :
    CubeViewport(cube, parent) {
    p_linked = false;

    string unlinkedIcon = Filename("$base/icons/unlinked.png").Expanded();
    static QIcon unlinked(unlinkedIcon.c_str());
    parentWidget()->setWindowIcon(unlinked);

  }

  MdiCubeViewport::~MdiCubeViewport() {

  }


  /**
   * Registers the tool given tool.
   *
   * @param tool
   */
  void MdiCubeViewport::registerTool(Tool *tool) {
    p_toolList.push_back(tool);
  }


  /**
   * Change the linked state of the viewport
   *
   *
   * @param b
   */
  void MdiCubeViewport::setLinked(bool b) {
    if(!parentWidget() || !parentWidget()->parentWidget())
      return;

    string unlinkedIcon = Filename("$base/icons/unlinked.png").Expanded();
    static QIcon unlinked(unlinkedIcon.c_str());
    string linkedIcon = Filename("$base/icons/linked.png").Expanded();
    static QIcon linked(linkedIcon.c_str());

    bool notify = false;
    if(b != p_linked)
      notify = true;

    p_linked = b;
    if(p_linked) {
      parentWidget()->parentWidget()->setWindowIcon(linked);
    }
    else {
      parentWidget()->parentWidget()->setWindowIcon(unlinked);
    }

    if(notify)
      emit linkChanging(b);
  }


  /**
   * Repaint the viewport
   *
   * @param e [in]  (QPaintEvent *)  event
   *
   * @internal
   *
   * @history  2007-04-30  Tracie Sucharski - Add the QPainter to the call to
   *                           Tool::paintViewport.
   */
  void MdiCubeViewport::paintEvent(QPaintEvent *e) {
    CubeViewport::paintEvent(e);

    QPainter painter(viewport());
    painter.drawPixmap(0, 0, p_pixmap);
    emit viewportUpdated();

    // Draw anything the tools might need
    for(int i = 0; i < p_toolList.size(); i++) {
      p_toolList[i]->paintViewport(this, &painter);
    }

    painter.end();
  }


  void MdiCubeViewport::viewGray(int band) {
    CubeViewport::viewGray(band);

    for(int i = 0; i < p_toolList.size(); i++)
      p_toolList[i]->updateTool();
  }


  void MdiCubeViewport::viewRGB(int rband, int gband, int bband) {
    CubeViewport::viewRGB(rband, gband, bband);

    for(int i = 0; i < p_toolList.size(); i++)
      p_toolList[i]->updateTool();
  }


  void MdiCubeViewport::restretch(ViewportBuffer *buffer) {
    if(buffer == grayBuffer()) {
      emit requestRestretch(this, (int)StretchTool::Gray);
    }
    else if(buffer == redBuffer()) {
      emit requestRestretch(this, (int)StretchTool::Red);
    }
    else if(buffer == greenBuffer()) {
      emit requestRestretch(this, (int)StretchTool::Green);
    }
    else if(buffer == blueBuffer()) {
      emit requestRestretch(this, (int)StretchTool::Blue);
    }
  }
}
