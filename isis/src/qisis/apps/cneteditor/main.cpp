#include "IsisDebug.h"

#include <QtGui/QApplication>
#include <QEvent>
#include <QObject>

#include "CnetEditorWindow.h"


#include <iostream>
using namespace std;


using namespace Qisis;

void startMonitoringMemory();
void stopMonitoringMemory();


class CnetEditorApp : public QApplication
{
  public:
    CnetEditorApp(int c, char ** v) : QApplication(c, v) {}
    virtual bool notify(QObject * o, QEvent e)
    {
      cerr << "\n\n\n\nCnetEditorApp::notify called!\n\n\n";
      return false;
    }
};


int main(int argc, char * argv[])
{
#ifdef CWDEBUG
  startMonitoringMemory();
#endif

  CnetEditorApp app(argc, argv);
  CnetEditorWindow window;
  window.show();
  return app.exec();
}


void startMonitoringMemory()
{
#ifdef CWDEBUG
#ifndef NOMEMCHECK
  MyMutex * mutex = new MyMutex();
  std::fstream * alloc_output = new std::fstream("/dev/null");
  Debug( make_all_allocations_invisible_except(NULL));
  ForAllDebugChannels(if (debugChannel.is_on()) debugChannel.off());
  Debug(dc::malloc.on());
  Debug(libcw_do.on());
  Debug(libcw_do.set_ostream(alloc_output));
  Debug(libcw_do.set_ostream(alloc_output, mutex));
  atexit(stopMonitoringMemory);
#endif
#endif
}


void stopMonitoringMemory()
{
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
