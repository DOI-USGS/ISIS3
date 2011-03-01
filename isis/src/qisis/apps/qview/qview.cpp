#include "IsisDebug.h"

#include <iostream>
#include <QApplication>
#include <QWorkspace>
#include <QFontDialog>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include "QIsisApplication.h"
#include "Filename.h"
#include "ViewportMainWindow.h"
#include "Workspace.h"
#include "FileTool.h"
#include "ZoomTool.h"
#include "StretchTool.h"
#include "WindowTool.h"
#include "HelpTool.h"
#include "PanTool.h"
#include "RubberBandTool.h"
#include "BandTool.h"
#include "EditTool.h"
#include "FindTool.h"
#include "BlinkTool.h"
#include "AdvancedTrackTool.h"
#include "Filename.h"
#include "Application.h"
#include "SocketThread.h"
#include "iString.h"
#include "MeasureTool.h"
#include "SpecialPixelTool.h"
#include "PlotTool.h"
#include "HistogramTool.h"
#include "StatisticsTool.h"
#include "Preference.h"
#include "PvlGroup.h"

void startMonitoringMemory();
void stopMonitoringMemory();

int main(int argc, char *argv[]) {
#ifdef CWDEBUG
  startMonitoringMemory();
#endif

  // Check to see if the user wants to force a new window
  int newWindow = -1;
  for(int i = 1; i < argc; i++) {
    if(Isis::iString(argv[i]).UpCase() == "-NEW") {
      newWindow = i;
    }
  }

  if(newWindow < 0) {
    std::string p_socketFile = "/tmp/isis_qview_" + Isis::Application::UserName();
    struct sockaddr_un p_socketName;
    p_socketName.sun_family = AF_UNIX;
    strcpy(p_socketName.sun_path, p_socketFile.c_str());
    int p_socket;

    if(((Isis::Filename)p_socketFile).Exists()) {
      // Create a socket
      if((p_socket = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        std::string msg = "Unable to create socket";
        std::cout << msg << std::endl;
        exit(0);
      }

      // Try to connect to the socket
      if((connect(p_socket, (struct sockaddr *)&p_socketName,
                  sizeof(p_socketName))) >= 0) {
        std::string temp;
        for(int i = 1; i < argc; i++) {
          temp +=  Isis::Filename(argv[i]).Expanded() + " ";
        }
        temp += "raise ";

        // Try to send data to the socket
        if(send(p_socket, temp.c_str(), temp.size(), 0) < 0) {
          std::string msg = "Unable to write to socket";
          std::cout << msg << std::endl;
          exit(0);
        }

        exit(0);
      }

      //If the file already exists but we can't connect to it, assume the socket
      //is no longer running & remove the tmp file...it falls out & create a new one
      else {
        remove(p_socketFile.c_str());
      }
    }
  }

  // Creates the qview application window
  Qisis::QIsisApplication *app = new Qisis::QIsisApplication(argc, argv);
  QApplication::setApplicationName("qview");

  // check for forcing of gui style
  Isis::PvlGroup &uiPref = Isis::Preference::Preferences().FindGroup(
                             "UserInterface");
  if(uiPref.HasKeyword("GuiStyle")) {
    std::string style = uiPref["GuiStyle"];
    QApplication::setStyle((Isis::iString) style);
  }

  // Add the Qt plugin directory to the library path
  Isis::Filename qtpluginpath("$ISISROOT/3rdParty/plugins");
  QCoreApplication::addLibraryPath(qtpluginpath.Expanded().c_str());

  Qisis::ViewportMainWindow *vw = new Qisis::ViewportMainWindow("qview");

  Qisis::Tool *rubberBandTool = Qisis::RubberBandTool::getInstance(vw);
  rubberBandTool->addTo(vw);

  Qisis::Tool *ftool = new Qisis::FileTool(vw);
  ftool->addTo(vw);
  vw->permanentToolBar()->addSeparator();

  Qisis::Tool *btool = new Qisis::BandTool(vw);
  btool->addTo(vw);

  Qisis::Tool *ztool = new Qisis::ZoomTool(vw);
  ztool->addTo(vw);
  ztool->activate(true);
  vw->getMenu("&View")->addSeparator();

  Qisis::Tool *ptool = new Qisis::PanTool(vw);
  ptool->addTo(vw);
  vw->getMenu("&View")->addSeparator();

  Qisis::Tool *stool = new Qisis::StretchTool(vw);
  stool->addTo(vw);

  Qisis::Tool *findtool = new Qisis::FindTool(vw);
  findtool->addTo(vw);

  Qisis::Tool *blinktool = new Qisis::BlinkTool(vw);
  blinktool->addTo(vw);

  Qisis::Tool *attool = new Qisis::AdvancedTrackTool(vw);
  attool->addTo(vw);

  Qisis::Tool *edittool = new Qisis::EditTool(vw);
  edittool->addTo(vw);

  Qisis::Tool *wtool = new Qisis::WindowTool(vw);
  wtool->addTo(vw);

  Qisis::Tool *mtool = new Qisis::MeasureTool(vw);
  mtool->addTo(vw);

  Qisis::Tool *sptool = new Qisis::SpecialPixelTool(vw);
  sptool->addTo(vw);

  Qisis::Tool *pltool = new Qisis::PlotTool(vw);
  pltool->addTo(vw);

  Qisis::Tool *histtool = new Qisis::HistogramTool(vw);
  histtool->addTo(vw);

  Qisis::Tool *statstool = new Qisis::StatisticsTool(vw);
  statstool->addTo(vw);

  Qisis::Tool *htool = new Qisis::HelpTool(vw);
  htool->addTo(vw);

  // Show the application window & open the cubes
  vw->show();

  bool openingAFileSucceeded = false;
  for(int i = 1; i < argc; i++) {
    if(i != newWindow) {
      try {
        vw->workspace()->addCubeViewport(QString(argv[i]));
        openingAFileSucceeded = true;
      }
      catch(Isis::iException &e) {
        e.Report();
        e.Clear();

        // If we're trying to open more later or have opened a file, allow
        //   qview to continue running. Otherwise (this if), crash.
        if(i == argc - 1 && !openingAFileSucceeded) {
          return 1;
        }
      }
    }
  }

  Qisis::SocketThread *temp = NULL;
//  Doesn't yet work on Mac OS X 10.4 (Tiger) systems for some reason.
#if !defined(__APPLE__)
  // We don't want to start a thread if the user is forcing a new window
  if(newWindow < 0) {
    temp = new Qisis::SocketThread();
    temp->connect(temp, SIGNAL(newImage(const QString &)),
                  vw->workspace(), SLOT(addCubeViewport(const QString &)));
    temp->connect(temp, SIGNAL(focusApp()), vw, SLOT(raise()));
    temp->start();
  }
#endif

  //Connect the edittool to the file tool in order to save and discard changes
  QObject::connect(edittool, SIGNAL(cubeChanged(bool)), ftool, SLOT(enableSave(bool)));
  QObject::connect(ftool, SIGNAL(saveChanges(MdiCubeViewport *)), edittool, SLOT(save(MdiCubeViewport *)));
  QObject::connect(ftool, SIGNAL(discardChanges(MdiCubeViewport *)), edittool, SLOT(undoAll(MdiCubeViewport *)));
  QObject::connect(edittool, SIGNAL(save()), ftool, SLOT(confirmSave()));
  QObject::connect(edittool, SIGNAL(saveAs()), ftool, SLOT(saveAs()));
  //Connect the FindTool to the AdvancedTrackTool to record the point if the "record" button is clicked
  QObject::connect(findtool, SIGNAL(recordPoint(QPoint)), attool, SLOT(record(QPoint)));

  //Connect the viewport's close signal to the file tool's exit method
  QObject::connect(vw , SIGNAL(closeWindow()), ftool, SLOT(exit()));

  int status = app->exec();

  // If we created a thread for listening to qview connections, then stop the thread and free its memory
  if(temp) {
    temp->stop();
    temp->wait(); // wait for the stop to finish
    delete temp;
  }

  delete htool;
  delete histtool;
  delete pltool;
  delete sptool;
  delete mtool;
  delete wtool;
  delete attool;
  delete blinktool;
  delete findtool;
  delete stool;
  delete ptool;
  delete btool;
  delete rubberBandTool;
  delete vw;
  delete app;
  //delete QCoreApplication::instance();
  return status;
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
