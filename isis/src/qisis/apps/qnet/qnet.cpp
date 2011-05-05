#include "IsisDebug.h"
#include <QApplication>
#include "AdvancedTrackTool.h"
#include "BandTool.h"
#include "Filename.h"
#include "FindTool.h"
#include "HelpTool.h"
#include "HistogramTool.h"
#include "iException.h"
#include "PanTool.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "QIsisApplication.h"
#include "QnetFileTool.h"
#include "QnetTool.h"
#include "QnetNavTool.h"
#include "RubberBandTool.h"
#include "StatisticsTool.h"
#include "StretchTool.h"
#include "ViewportMainWindow.h"
#include "WindowTool.h"
#include "ZoomTool.h"

#define IN_QNET
#include "qnet.h"

void startMonitoringMemory();
void stopMonitoringMemory();
using namespace std;

int main(int argc, char *argv[]) {
#ifdef CWDEBUG
  startMonitoringMemory();
#endif
  Qisis::Qnet::g_controlNetwork = NULL;
  Qisis::Qnet::g_serialNumberList = NULL;

  try {
    Qisis::QIsisApplication *app = new Qisis::QIsisApplication(argc,argv);
    QApplication::setApplicationName("qnet");

    // check for forcing of gui style
    Isis::PvlGroup &uiPref = Isis::Preference::Preferences().FindGroup(
        "UserInterface");
    if (uiPref.HasKeyword("GuiStyle")) {
      string style = uiPref["GuiStyle"];
      QApplication::setStyle((Isis::iString) style);
    }

    // Add the Qt plugin directory to the library path
    Isis::Filename qtpluginpath("$ISISROOT/3rdParty/plugins");
    QCoreApplication::addLibraryPath(qtpluginpath.Expanded().c_str());

    Qisis::Qnet::g_vpMainWindow = new Qisis::ViewportMainWindow("qnet");

    Qisis::Tool *rubberBandTool = Qisis::RubberBandTool::getInstance(Qisis::Qnet::g_vpMainWindow);
    rubberBandTool->addTo(Qisis::Qnet::g_vpMainWindow);

    /**** ADD FILE TOOL FIRST SO THAT IT APPEARS FIRST IN THE PERMANENT AND MENU TOOLBARS ****/
    // adds file tool buttons and separator on permanent toolbar
    // adds to "File" dropdown of Menu toolbar
    Qisis::QnetFileTool *ftool = new Qisis::QnetFileTool(Qisis::Qnet::g_vpMainWindow);
    ftool->Tool::addTo(Qisis::Qnet::g_vpMainWindow);
    Qisis::Qnet::g_vpMainWindow->permanentToolBar()->addSeparator();

    Qisis::QnetNavTool *ntool = new Qisis::QnetNavTool(Qisis::Qnet::g_vpMainWindow);
    ntool->Tool::addTo(Qisis::Qnet::g_vpMainWindow);

    /**** ADD TOOLS TO TOOL PAD ON LEFT/RIGHT ****/
    // adds band tool button to toolpad on left
    Qisis::Tool *btool = new Qisis::BandTool(Qisis::Qnet::g_vpMainWindow);
    btool->addTo(Qisis::Qnet::g_vpMainWindow);

    // adds zoom tool button to toolpad on left
    // adds to "View" dropdown of Menu toolbar with seperator
    Qisis::Tool *ztool = new Qisis::ZoomTool(Qisis::Qnet::g_vpMainWindow);
    ztool->addTo(Qisis::Qnet::g_vpMainWindow);
    Qisis::Qnet::g_vpMainWindow->getMenu("&View")->addSeparator();

    // adds pan tool button to toolpad on left
    // adds to "View" dropdown of Menu toolbar with seperator
    Qisis::Tool *ptool = new Qisis::PanTool(Qisis::Qnet::g_vpMainWindow);
    ptool->addTo(Qisis::Qnet::g_vpMainWindow);
    Qisis::Qnet::g_vpMainWindow->getMenu("&View")->addSeparator();

    // adds stretch tool button to toolpad on left
    Qisis::Tool *stool = new Qisis::StretchTool(Qisis::Qnet::g_vpMainWindow);
    stool->addTo(Qisis::Qnet::g_vpMainWindow);

    // adds find tool button to toolpad on left
    // adds to "Options" dropdown of Menu toolbar
    Qisis::Tool *findTool = new Qisis::FindTool(Qisis::Qnet::g_vpMainWindow);
    findTool->addTo(Qisis::Qnet::g_vpMainWindow);

    // adds hist tool button to toolpad on left
    // adds PlotTool to "Options" dropdown of Menu toolbar
    Qisis::Tool *histtool = new Qisis::HistogramTool(Qisis::Qnet::g_vpMainWindow);
    histtool->addTo(Qisis::Qnet::g_vpMainWindow);

    // adds stats tool button to toolpad on left
    Qisis::Tool *statstool = new Qisis::StatisticsTool(Qisis::Qnet::g_vpMainWindow);
    statstool->addTo(Qisis::Qnet::g_vpMainWindow);

    // adds tie tool button to toolpad on left
    // sets tie tool to active button
    Qisis::Tool *qnetTool = new Qisis::QnetTool(Qisis::Qnet::g_vpMainWindow);
    qnetTool->addTo(Qisis::Qnet::g_vpMainWindow);
    qnetTool->activate(true);

    /**** ADD REMAINING TOOLS TO PERMANENT TOOL PAD ON TOP/BOTTOM ****/
    // adds adv track tool button permanent toolbar
    // adds to "Options" dropdown of Menu toolbar
    Qisis::Tool *ttool = new Qisis::AdvancedTrackTool(Qisis::Qnet::g_vpMainWindow);
    ttool->addTo(Qisis::Qnet::g_vpMainWindow);

    // adds window tool (Link viewports) button and seperator to permanent toolbar
    // adds "Windows" dropdown of Menu toolbar
    Qisis::Tool *wtool = new Qisis::WindowTool(Qisis::Qnet::g_vpMainWindow);
    wtool->addTo(Qisis::Qnet::g_vpMainWindow);
    Qisis::Qnet::g_vpMainWindow->permanentToolBar()->addSeparator();

    // adds help tool button to permanent toolbar
    // adds "Help" dropdown of Menu toolbar
    Qisis::Tool *htool = new Qisis::HelpTool(Qisis::Qnet::g_vpMainWindow);
    htool->addTo(Qisis::Qnet::g_vpMainWindow);

    /**** MAKE CONNECTIONS ****/
    /**** LOADING IMAGES ****/
    // The fileTool needs to know when the navTool wants to load images
    QObject::connect(ntool, SIGNAL(loadImage(const QString &)),
        ftool, SLOT(loadImage(const QString &)));
    // The fileTool needs to know when to load the images associated with a point
    QObject::connect(ntool,SIGNAL(loadPointImages(Isis::ControlPoint *)),
                     ftool,SLOT(loadPointImages(Isis::ControlPoint *)));

    /**** UPDATING LIST ****/
    // The navTool needs to know when the file tool has changed the serialNumberList
    QObject::connect(ftool, SIGNAL(serialNumberListUpdated()),
                     ntool, SLOT(resetList()));
    // The navTool cube name filter needs to know when the file tool
    // has changed the serialNumberList in order to refresh the list
    // Jeannie Walldren 2009-01-26
    QObject::connect(ftool, SIGNAL(serialNumberListUpdated()),
                     ntool, SLOT(resetCubeList()));
    // The QnetTool needs to know when the file tool has changed the
    // serialNumberList and the controlnetwork.
    //  QObject::connect(ftool,SIGNAL(serialNumberListUpdated()),
    //                   qnetTool,SLOT(updateList()));

    /**** MAKE NAV TOOL DIALOG VISIBLE  ****/
    // NavigationTool needs to know when the QnetTool (Tie Tool) has been
    // acitvated on the tool pad in order to reopen the NavTool, if necessary.
    // 2010-07-01 Jeannie Walldren
    QObject::connect(qnetTool, SIGNAL(showNavTool()), ntool, SLOT(showNavTool()));

    /**** MODIFYING POINTS ****/
    // Qnet Tool needs to know when navTool modifies/ignores/deletes points
    QObject::connect(ntool, SIGNAL(modifyPoint(Isis::ControlPoint *)),
                     qnetTool, SLOT(modifyPoint(Isis::ControlPoint *)));
    QObject::connect(ntool, SIGNAL(ignoredPoints()), qnetTool, SLOT(refresh()));
    QObject::connect(ntool, SIGNAL(deletedPoints()), qnetTool, SLOT(refresh()));
    QObject::connect(ntool, SIGNAL(pointChanged(QString)),
                     qnetTool, SLOT(updatePointInfo(QString)));

    /**** RECORDING POINTS ****/
    // Connect the FindTool to the AdvancedTrackTool to record the point if the
    // "record" button is clicked in FindTool's dialog
    QObject::connect(findTool, SIGNAL(recordPoint(QPoint)),
                     ttool, SLOT(record(QPoint)));

    /**** UPDATING CONTROL NETWORK ****/
    // the next command was uncommented to allows the QnetTool to display the name
    // of the control network file and update when a new control network is selected
    // 2008-11-26 Jeannie Walldren
    QObject::connect(ftool, SIGNAL(controlNetworkUpdated(QString)),
                     qnetTool, SLOT(updateNet(QString)));
    QObject::connect(ftool, SIGNAL(newControlNetwork(Isis::ControlNet *)),
                     qnetTool, SIGNAL(newControlNetwork(Isis::ControlNet *)));
    QObject::connect(qnetTool, SIGNAL(editPointChanged(QString)),
                     ntool, SLOT(updateEditPoint(QString)));
    QObject::connect(qnetTool, SIGNAL(refreshNavList()),
                     ntool, SLOT(refreshList()));

    /**** SAVING CONTROL NET CHANGES ****/
    //  The FileTool needs to now if the control network has changed (delete/
    //  edit/create/ignore point) so that user can be prompted to save net
    QObject::connect(qnetTool, SIGNAL(netChanged()), ftool, SLOT(setSaveNet()));
    QObject::connect(ntool, SIGNAL(netChanged()), ftool, SLOT(setSaveNet()));
    QObject::connect(qnetTool, SIGNAL(qnetToolSave()), ftool, SLOT(saveAs()));

    // First hop for signal from StretchTool to ChipViewport
    QObject::connect(stool,
        SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)),
        qnetTool,
        SIGNAL(stretchChipViewport(Isis::Stretch *, Qisis::CubeViewport *)));


    /**** EXITING ****/
    // Connect the viewport's close signal to the file tool's exit method
    // Added 2008-12-04 by Jeannie Walldren
    QObject::connect(Qisis::Qnet::g_vpMainWindow , SIGNAL(closeWindow()),
                     ftool, SLOT(exit()));
    //-----------------------------------------------------------------

    Qisis::Qnet::g_vpMainWindow->show();
    return app->exec();
  }
  catch (Isis::iException &e) {
    e.Report();
  }
}

void startMonitoringMemory() {
#ifdef CWDEBUG
#ifndef NOMEMCHECK
  MyMutex *mutex = new MyMutex();
  std::fstream *alloc_output = new std::fstream("/dev/null");
  Debug(make_all_allocations_invisible_except(NULL));
  ForAllDebugChannels(if(debugChannel.is_on()) debugChannel.off());
  Debug(dc::malloc.on());
  Debug(libcw_do.on());
  Debug(libcw_do.set_ostream(alloc_output));
  Debug(libcw_do.set_ostream(alloc_output, mutex));
  atexit(stopMonitoringMemory);
#endif
#endif
}


void stopMonitoringMemory() {
#ifdef CWDEBUG
#ifndef NOMEMCHECK
  Debug(
    alloc_filter_ct alloc_filter;
    std::vector<std::string> objmasks;
    objmasks.push_back("libc.so*");
    objmasks.push_back("libstdc++*");
    std::vector<std::string> srcmasks;
    srcmasks.push_back("*new_allocator.h*");
    srcmasks.push_back("*set_ostream.inl*");
    alloc_filter.hide_objectfiles_matching(objmasks);
    alloc_filter.hide_sourcefiles_matching(srcmasks);
    alloc_filter.hide_unknown_locations();
    delete libcw_do.get_ostream();
    libcw_do.set_ostream(&std::cout);
    list_allocations_on(libcw_do, alloc_filter);
    dc::malloc.off();
    libcw_do.off()
  );
#endif
#endif
}

