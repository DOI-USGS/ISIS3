#include <QApplication>
#include <QToolBar>
#include "iException.h"
#include "ViewportMainWindow.h"
#include "QtieFileTool.h"
#include "BandTool.h"
#include "ZoomTool.h"
#include "PanTool.h"
#include "StretchTool.h"
#include "FindTool.h"
#include "WindowTool.h"
#include "AdvancedTrackTool.h"
#include "HelpTool.h"
#include "QtieTool.h"
#include "RubberBandTool.h"

#define IN_QTIE

using namespace Isis;

int main(int argc, char *argv[]) {

  try {
    QApplication *app = new QApplication(argc, argv);
    QApplication::setApplicationName("qtie");
    app->setStyle("windows");

    ViewportMainWindow *mw = new ViewportMainWindow("Qtie");

    Tool *rubberBandTool = RubberBandTool::getInstance(mw);
    rubberBandTool->addTo(mw);

    QtieFileTool *ftool = new QtieFileTool(mw);
    ftool->Tool::addTo(mw);
    mw->permanentToolBar()->addSeparator();

    Tool *btool = new BandTool(mw);
    btool->addTo(mw);

    Tool *ztool = new ZoomTool(mw);
    ztool->addTo(mw);
    mw->getMenu("&View")->addSeparator();

    Tool *ptool = new PanTool(mw);
    ptool->addTo(mw);
    mw->getMenu("&View")->addSeparator();

    Tool *stool = new StretchTool(mw);
    stool->addTo(mw);

    Tool *findTool = new FindTool(mw);
    findTool->addTo(mw);

    Tool *ttool = new AdvancedTrackTool(mw);
    ttool->addTo(mw);

    Tool *wtool = new WindowTool(mw);
    wtool->addTo(mw);

    mw->permanentToolBar()->addSeparator();
    Tool *htool = new HelpTool(mw);
    htool->addTo(mw);

    Tool *tieTool = new QtieTool(mw);
    tieTool->addTo(mw);
    tieTool->activate(true);

    QObject::connect(ftool, SIGNAL(cubesOpened(Cube *, Cube *, ControlNet *)),
                     tieTool, SLOT(setFiles(Cube *, Cube *, ControlNet *)));

    QObject::connect(ftool, SIGNAL(newFiles()), tieTool, SLOT(clearFiles()));

    QObject::connect(mw, SIGNAL(closeWindow()), ftool, SLOT(exit()));

    QObject::connect(stool,
                     SIGNAL(stretchChipViewport(Stretch *, CubeViewport *)),
                     tieTool,
                     SIGNAL(stretchChipViewport(Stretch *, CubeViewport *)));

    mw->show();

    return app->exec();
  }
  catch (iException &e) {
    e.Report();
  }
}

