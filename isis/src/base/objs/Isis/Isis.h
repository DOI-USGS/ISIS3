/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2009/07/29 21:14:35 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#ifdef __GTHREADS
#error *****Isis.h MUST be included before any other files!*****
#endif

#include "IsisDebug.h"

#include <signal.h>

#include <QCoreApplication>

#include "Application.h"
#include "UserInterface.h" // this is an unnecessary include

#ifndef APPLICATION
#define APPLICATION IsisMain
#endif

/**
 * @brief Base include file for all Isis applications
 * 
 * This is not technically a class.  Instead it is the main include file
 * the must be placed at the beginning of every executable program. For
 * example:
 * @code
 * using namespace std;
 * #include "Isis.h"
 * @endcode 
 * The include file
 * handles starting the Isis GUI, error messages, and a host of other
 * Isis related duties that should be performed in all Isis programs.
 * It actually contains the standard C++
 * declaration of main (i.e., int main (int argc, char *argv[])) and
 * invokes the function IsisMain which must
 * be supplied by the programmer. Therefore, your program should
 * always start as follows:
 * @code 
 * using namespace std;
 * #include "Isis.h"
 * void IsisMain() {
 *   ...
 * }
 * @endcode
 * The Isis.h file includes code that catches all thrown exceptions.  It
 * first trys to catch and report Isis specific errors.  That is,
 * those thrown
 * within IsisMain or by Isis objects created in IsisMain.  It will
 * then catch C++ system type errors and respond accordingly.  This
 * eliminates the need for the application programmer to trap errors.
 * They may however, throw errors as necessary to indicate error
 * conditions within their application.
 * 
 * @ingroup Utility
 * 
 * @author 2002-01-01 Jeff Anderson
 * 
 * @internal 
 *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
 *                                      isis.astrogeology...
 *   @history 2006-03-18 Elizabeth Miller - Added gui helper stuff
 *   @history 2006-05-17 Elizabeth Miller - Removed .xml and documented .h file 
 */
std::map<std::string,void*> GuiHelpers();
#ifndef GUIHELPERS
std::map<std::string,void*> GuiHelpers() {
  std::map<std::string,void*> empty;
  return empty;
}
#endif

void APPLICATION ();

void startMonitoringMemory();
void stopMonitoringMemory();
void SegmentationFault(int);
void Abort(int);
void InterruptSignal(int);

/**
 * The programmer supplied main function
 * 
 * @param argc - The application
 * @param argv - The parameter string for the application
 * 
 * @return int
 */
int main (int argc, char *argv[]) {
#ifdef CWDEBUG
  startMonitoringMemory();
  signal(SIGSEGV, SegmentationFault);
  signal(SIGABRT, Abort);
  signal(SIGINT, InterruptSignal);
#endif

  Isis::Application *app = new Isis::Application(argc,argv);
  app->RegisterGuiHelpers(GuiHelpers());
  int status = app->Exec(APPLICATION);
  delete app;
  delete QCoreApplication::instance();
  return status;
}

#ifdef CWDEBUG
void startMonitoringMemory() {
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
}


void stopMonitoringMemory() {
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
}


void SegmentationFault(int) {
  std::vector<std::string> currentStack;
  StackTrace::GetStackTrace(&currentStack);

  std::cerr << "Segmentation Fault" << std::endl;
  for(unsigned int i = 1; i < currentStack.size(); i++) {
    std::cerr << currentStack[i] << std::endl;
  }

  exit(1);
}

void Abort(int) {
  std::vector<std::string> currentStack;
  StackTrace::GetStackTrace(&currentStack);

  std::cerr << "Abort" << std::endl;
  for(unsigned int i = 1; i < currentStack.size(); i++) {
    std::cerr << currentStack[i] << std::endl;
  }

  exit(1);
}


void InterruptSignal(int) {
  exit(1);
}

#endif
