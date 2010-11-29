#if !defined(Application_h)
#define Application_h

/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/06/29 23:42:18 $
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

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <iostream>
#include <fstream>
#include <QString>
#include <string>
#include <ctime>

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  class Gui;
  class Progress;

  /**
   *
   *
   * @internal
   *   @history 2005-12-16 Elizabeth Miller - added documentation and -BATCHLIST
   *                                          capabilities
   *   @history 2006-02-13 Elizabeth Miller - Added GuiHelper Capabilities
   *   @history 2006-02-16 Jeff Anderson - Fixed race condition on sockets
   *                       between communicating ISIS programs
   *   @history 2006-02-17 Fixed bug with application name being unknown if
   *                       the user interface threw an error
   *   @history 2006-02-27 Elizabeth Miller - Added GuiLog and GuiReportError
   *                       methods
   *   @history 2006-07-28 Jeff Anderson - Fixed another race condition with
   *                       sockets between communicating ISIS programs.  Also
   *                       updated progress to output name of the program
   *   @history 2006-08-30 Jeff Anderson - Create a
   *                       QCoreApplication if in command line
   *                       mode.
   *   @history 2007-10-04 Steven Koechle - Added output capability for
   *                       debugging log for -info flag
   *   @history 2008-01-04 Jeannie Walldren - Changed
   *                       description of Log method
   *   @history 2008-01-09 Steven Lambright - Fixed Memory Leak
   *   @history 2008-04-16 Steven Lambright - Added parameter check that was
   *                       removed from UserInterface
   *   @history 2008-06-18 Christopher Austin - Fixed documentation error
   *   @history 2008-06-19 Steven Lambright - Added CubeManager::CleanUp call to
   *                       clean up cubes in memory after calling IsisMain.
   *   @history 2008-06-24 Steven Koechle - Added Preferences to Debugging Log.
   *   @history 2008-07-08 Steven Lambright - Singletons now destroy themselves
   *            instead of Application deleting them
   *   @history 2008-07-08 Steven Lambright - p_ui is no longer static, which
   *            fixes issues with the mac unit tests.
   *   @history 2009-11-19 Kris Becker - Made argc pass by reference since Qt's
   *            QApplication/QCoreApplication requires it
   *   @history 2010-03-17 Stuart Sides - Added the location of the Qt plugins
   *                                      into the library path
   *   @history 2010-06-29 Steven Lambright - Added a setlocale to english for
   *            numeric values
   *   @history 2010-11-29 Steven Lambright - Added the Version() method
   */
  class Application {
    public:
      Application(int &argc, char *argv[]);
      ~Application();

      int Exec(void (*funct)());
      PvlGroup Accounting();
      PvlObject History();

      static UserInterface &GetUserInterface();
      static void Log(PvlGroup &results);
      static void GuiLog(Pvl &results);
      static void GuiLog(PvlGroup &results);
      static void GuiLog(std::string &results);
      static std::string Name();

      void RegisterGuiHelpers(std::map<std::string, void *> helpers) {
        p_guiHelpers = helpers;
      };
      void *GetGuiHelper(std::string helper) {
        return p_guiHelpers[helper];
      };

      void GuiReportError(Isis::iException &e);

      void Exec(const std::string &program, const std::string &parameters);

      static iString UserName();
      static iString HostName();
      static iString DateTime(time_t *curtime = 0);
      static iString Version();

    private:
      int p_BatchlistPass;
      int DirectIO();
      int PageFaults();
      int ProcessSwaps();

      time_t p_startTime;
      clock_t p_startClock;
      std::string p_datetime;
      int p_startDirectIO;
      int p_startPageFaults;
      int p_startProcessSwaps;
      pid_t p_childPid;
      bool p_childCaught;

      UserInterface *p_ui;  //!<Pointer to a User Interface object

      static bool HasParent();
      static void SendParentData(const std::string code, const std::string &message);
      void SendParentErrors(Isis::PvlObject &errors);

      friend class Gui;
      void FunctionCleanup();
      int FunctionError(Isis::iException &e);

      friend class Progress;
      void UpdateProgress(const std::string &text, bool print);
      void UpdateProgress(int percent, bool print);
      void ProcessGuiEvents();

      void ParentFork(const std::string &command, const std::string &program);
      void ChildFork(const std::string &command);
      void EstablishConnections();
      void WaitForCommand(int childSocket);

      int p_socket;
      static int p_childSocket;
      pid_t p_pid;
      std::string p_buffer;
      std::string p_socketFile;
      struct sockaddr_un p_socketName;
      std::vector<std::string> p_queue;
      std::map<std::string, void *> p_guiHelpers;

      static QString p_appName;
  };

  extern Application *iApp;
};

#endif
