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

int main(int argc, char *argv[]) {

  try {
    QApplication *app = new QApplication(argc, argv);
    QApplication::setApplicationName("qtie");
    app->setStyle("windows");

    Qisis::ViewportMainWindow *mw = new Qisis::ViewportMainWindow("Qtie");

    Qisis::Tool *rubberBandTool = Qisis::RubberBandTool::getInstance(mw);
    rubberBandTool->addTo(mw);

    Qisis::QtieFileTool *ftool = new Qisis::QtieFileTool(mw);
    ftool->Tool::addTo(mw);
    mw->permanentToolBar()->addSeparator();

    Qisis::Tool *btool = new Qisis::BandTool(mw);
    btool->addTo(mw);

    Qisis::Tool *ztool = new Qisis::ZoomTool(mw);
    ztool->addTo(mw);
    mw->getMenu("&View")->addSeparator();

    Qisis::Tool *ptool = new Qisis::PanTool(mw);
    ptool->addTo(mw);
    mw->getMenu("&View")->addSeparator();

    Qisis::Tool *stool = new Qisis::StretchTool(mw);
    stool->addTo(mw);

    Qisis::Tool *findTool = new Qisis::FindTool(mw);
    findTool->addTo(mw);

    Qisis::Tool *ttool = new Qisis::AdvancedTrackTool(mw);
    ttool->addTo(mw);

    Qisis::Tool *wtool = new Qisis::WindowTool(mw);
    wtool->addTo(mw);

    mw->permanentToolBar()->addSeparator();
    Qisis::Tool *htool = new Qisis::HelpTool(mw);
    htool->addTo(mw);

    Qisis::Tool *tieTool = new Qisis::QtieTool(mw);
    tieTool->addTo(mw);
    tieTool->activate(true);

    QObject::connect(ftool, SIGNAL(cubesOpened(Isis::Cube &, Isis::Cube &, Isis::ControlNet &)),
                     tieTool, SLOT(setFiles(Isis::Cube &, Isis::Cube &, Isis::ControlNet &)));

    QObject::connect(ftool, SIGNAL(newFiles()), tieTool, SLOT(clearFiles()));

    QObject::connect(mw, SIGNAL(closeWindow()), ftool, SLOT(exit()));

    QObject::connect(stool,
                     SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)),
                     tieTool,
                     SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)));

    mw->show();

    return app->exec();
  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

