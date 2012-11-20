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

#include "Application.h"
#include "AdvancedTrackTool.h"
#include "BandTool.h"
#include "BlinkTool.h"
#include "EditTool.h"
#include "FeatureNomenclatureTool.h"
#include "FileName.h"
#include "FileTool.h"
#include "FindTool.h"
#include "Gui.h"
#include "HelpTool.h"
#include "HistogramTool.h"
#include "IString.h"
#include "MatchTool.h"
#include "MeasureTool.h"
#include "PanTool.h"
#include "Preference.h"
#include "PvlGroup.h"
#include "QIsisApplication.h"
#include "RubberBandTool.h"
#include "ScatterPlotTool.h"
#include "SpecialPixelTool.h"
#include "SpatialPlotTool.h"
#include "SpectralPlotTool.h"
#include "SocketThread.h"
#include "StatisticsTool.h"
#include "StereoTool.h"
#include "StretchTool.h"
#include "SunShadowTool.h"
#include "ViewportMainWindow.h"
#include "WindowTool.h"
#include "Workspace.h"
#include "ZoomTool.h"

void startMonitoringMemory();
void stopMonitoringMemory();

using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Gui::checkX11();

  // Check to see if the user wants to force a new window
  int newWindow = -1;
  for(int i = 1; i < argc; i++) {
    if(IString(argv[i]).UpCase() == "-NEW") {
      newWindow = i;
    }
  }

  std::string p_socketFile = "/tmp/isis_qview_" + Application::UserName();
  if(newWindow < 0) {
    struct sockaddr_un p_socketName;
    p_socketName.sun_family = AF_UNIX;
    strcpy(p_socketName.sun_path, p_socketFile.c_str());
    int p_socket;

    if(((FileName)p_socketFile).fileExists()) {
      // Create a socket
      if((p_socket = socket(PF_UNIX, SOCK_STREAM, 0)) >= 0) {
        // Try to connect to the socket
        if((connect(p_socket, (struct sockaddr *)&p_socketName,
                    sizeof(p_socketName))) >= 0) {
          std::string temp;
          for(int i = 1; i < argc; i++) {
            temp += QFileInfo(
                  FileName(argv[i]).expanded()).absoluteFilePath().toStdString()
                + " ";
          }
          temp += "raise ";
          // Try to send data to the socket
          if(send(p_socket, temp.c_str(), temp.size(), 0) >= 0) {
            // Success, the other qview will open this file.
            exit(0);
          }
          else {
            std::string msg = "Unable to write to socket";
            std::cout << msg << std::endl;
            remove(p_socketFile.c_str());
          }
        }

        // If the file already exists but we can't connect to it, assume the
        //   socket is no longer running & remove the tmp file...it falls out &
        //   create a new one. This happens if qview is not already running.
        else {
          remove(p_socketFile.c_str());
        }
      }
      else {
        std::string msg = "Unable to create socket";
        std::cout << msg << std::endl;
        remove(p_socketFile.c_str());
      }
    }
  }

  // Creates the qview application window
  QIsisApplication *app = new QIsisApplication(argc, argv);
  QApplication::setApplicationName("qview");

  // check for forcing of gui style
  PvlGroup &uiPref = Preference::Preferences().FindGroup(
                             "UserInterface");
  if(uiPref.HasKeyword("GuiStyle")) {
    std::string style = uiPref["GuiStyle"];
    QApplication::setStyle((IString) style);
  }

  // Add the Qt plugin directory to the library path
  FileName qtpluginpath("$ISISROOT/3rdParty/plugins");
  QCoreApplication::addLibraryPath(qtpluginpath.expanded().c_str());

  ViewportMainWindow *vw = new ViewportMainWindow("qview");

  Tool *rubberBandTool = RubberBandTool::getInstance(vw);
  rubberBandTool->addTo(vw);

  Tool *fileTool = new FileTool(vw);
  fileTool->addTo(vw);
  vw->permanentToolBar()->addSeparator();

  Tool *bandTool = new BandTool(vw);
  bandTool->addTo(vw);

  Tool *zoomTool = new ZoomTool(vw);
  zoomTool->addTo(vw);
  zoomTool->activate(true);
  vw->getMenu("&View")->addSeparator();

  Tool *panTool = new PanTool(vw);
  panTool->addTo(vw);
  vw->getMenu("&View")->addSeparator();

  Tool *stretchTool = new StretchTool(vw);
  stretchTool->addTo(vw);

  Tool *findTool = new FindTool(vw);
  findTool->addTo(vw);

  Tool *blinkTool = new BlinkTool(vw);
  blinkTool->addTo(vw);

  Tool *advancedTrackTool = new AdvancedTrackTool(vw);
  advancedTrackTool->addTo(vw);

  Tool *editTool = new EditTool(vw);
  editTool->addTo(vw);

  Tool *windowTool = new WindowTool(vw);
  windowTool->addTo(vw);

  Tool *measureTool = new MeasureTool(vw);
  measureTool->addTo(vw);

  Tool *sunShadowTool = new SunShadowTool(vw);
  sunShadowTool->addTo(vw);

  Tool *featureNomenclatureTool = new FeatureNomenclatureTool(vw);
  featureNomenclatureTool->addTo(vw);

  Tool *specialPixelTool = new SpecialPixelTool(vw);
  specialPixelTool->addTo(vw);

  Tool *spatialPlotTool = new SpatialPlotTool(vw);
  spatialPlotTool->addTo(vw);

  Tool *spectralPlotTool = new SpectralPlotTool(vw);
  spectralPlotTool->addTo(vw);

  Tool *scatterPlotTool = new ScatterPlotTool(vw);
  scatterPlotTool->addTo(vw);

  Tool *histTool = new HistogramTool(vw);
  histTool->addTo(vw);

  Tool *statsTool = new StatisticsTool(vw);
  statsTool->addTo(vw);

  Tool *stereoTool = new StereoTool(vw);
  stereoTool->addTo(vw);

  Tool *matchTool = new MatchTool(vw);
  matchTool->addTo(vw);

  Tool *helpTool = new HelpTool(vw);
  helpTool->addTo(vw);

  // Show the application window & open the cubes
  vw->show();

  bool openingAFileSucceeded = false;
  for(int i = 1; i < argc; i++) {
    if(i != newWindow) {
      try {
        vw->workspace()->addCubeViewport(QString(argv[i]));
        openingAFileSucceeded = true;
      }
      catch(IException &e) {
        e.print();

        // If we're trying to open more later or have opened a file, allow
        //   qview to continue running. Otherwise (this if), crash.
        if(i == argc - 1 && !openingAFileSucceeded) {
          return 1;
        }
      }
    }
  }

  SocketThread *temp = NULL;
  // We don't want to start a thread if the user is forcing a new window
  if(newWindow < 0) {
    temp = new SocketThread();
    temp->connect(temp, SIGNAL(newImage(const QString &)),
                  vw->workspace(), SLOT(addCubeViewport(const QString &)));
    temp->connect(temp, SIGNAL(focusApp()), vw, SLOT(raise()));
    temp->start();
  }

  //Connect the editTool to the file tool in order to save and discard changes
  QObject::connect(editTool, SIGNAL(cubeChanged(bool)),
                   fileTool, SLOT(enableSave(bool)));
  QObject::connect(fileTool, SIGNAL(saveChanges(CubeViewport *)),
                   editTool, SLOT(save(CubeViewport *)));
  QObject::connect(fileTool, SIGNAL(discardChanges(CubeViewport *)),
                   editTool, SLOT(undoAll(CubeViewport *)));
  QObject::connect(editTool, SIGNAL(save()), fileTool, SLOT(save()));
  QObject::connect(editTool, SIGNAL(saveAs()), fileTool, SLOT(saveAs()));
  // Connect the FindTool to the AdvancedTrackTool to record the point if the
  // "record" button is clicked
  QObject::connect(findTool, SIGNAL(recordPoint(QPoint)),
                   advancedTrackTool, SLOT(record(QPoint)));

  //Connect the viewport's close signal to the all windows/subwindows
  QObject::connect(vw, SIGNAL(closeWindow()), fileTool, SLOT(exit()));

  int status = app->exec();

  // If we created a thread for listening to qview connections, then stop the thread and free its memory
  if(temp) {
    temp->stop();
    temp->wait(); // wait for the stop to finish
    delete temp;

    remove(p_socketFile.c_str());
  }

  delete helpTool;
  delete matchTool;
  delete stereoTool;
  delete histTool;
  delete spatialPlotTool;
  delete spectralPlotTool;
  delete scatterPlotTool;
  delete specialPixelTool;
  delete featureNomenclatureTool;
  delete sunShadowTool;
  delete measureTool;
  delete windowTool;
  delete advancedTrackTool;
  delete blinkTool;
  delete findTool;
  delete stretchTool;
  delete bandTool;
  delete rubberBandTool;
  delete vw;
  delete app;
  //delete QCoreApplication::instance();
  return status;
}

