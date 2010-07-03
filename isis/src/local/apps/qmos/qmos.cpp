#include "IsisDebug.h"

#include <QApplication>

#include "iException.h"
#include "ProjectionFactory.h"
#include "Filename.h"
#include "MosaicWidget.h"
#include "MosaicMainWindow.h"
#include "Preference.h"
#include "PvlGroup.h"

void startMonitoringMemory();
void stopMonitoringMemory();

int main (int argc, char *argv[]) {
  #ifdef CWDEBUG
    startMonitoringMemory();
  #endif
 
  try {
    QApplication *app = new QApplication(argc,argv);
    QApplication::setApplicationName("qmos");

    // check for forcing of gui style
    Isis::PvlGroup & uiPref = Isis::Preference::Preferences().FindGroup(
        "UserInterface");
    if (uiPref.HasKeyword("GuiStyle"))
    {
      std::string style = uiPref["GuiStyle"];
      QApplication::setStyle((Isis::iString) style);
    }

    // Add the Qt plugin directory to the library path
    Isis::Filename qtpluginpath ("$ISISROOT/3rdParty/plugins");
    QCoreApplication::addLibraryPath(qtpluginpath.Expanded().c_str());


    Qisis::MosaicMainWindow *mainWindow = new Qisis::MosaicMainWindow("qmos");

    Isis::PvlGroup mapping("Mapping");
    {
      mapping += Isis::PvlKeyword("ProjectionName", "PolarStereographic");
      mapping += Isis::PvlKeyword("CenterLatitude", "45.0");
      mapping += Isis::PvlKeyword("CenterLongitude", "0.0");
      mapping += Isis::PvlKeyword("TargetName", "Mars");
      mapping += Isis::PvlKeyword("LatitudeType","Planetocentric");
      mapping += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
      mapping += Isis::PvlKeyword("LongitudeDomain", "360");
    }

    Isis::Pvl pvl;
    pvl.AddGroup(mapping);  
    Isis::Projection *proj = Isis::ProjectionFactory::Create(pvl);

    Qisis::MosaicWidget *mos = new Qisis::MosaicWidget(mainWindow);
    mos->setProjection(proj);
    mos->setLabelText("Polar Stereographic");
    mainWindow->setCentralWidget(mos);
    mainWindow->show();

    int status = app->exec();
    delete mos;
    delete mainWindow;
    delete app;

    return status;
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
  Debug( make_all_allocations_invisible_except(NULL) );
  ForAllDebugChannels( if (debugChannel.is_on()) debugChannel.off() );
  Debug( dc::malloc.on() );
  Debug( libcw_do.on() );
  Debug( libcw_do.set_ostream(alloc_output) );
  Debug( libcw_do.set_ostream(alloc_output, mutex) );
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

