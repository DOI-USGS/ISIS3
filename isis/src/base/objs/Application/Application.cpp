#include "IsisDebug.h"

#include <cstdio>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

extern int errno;

#include <iostream>
#include <sstream>
#include <locale.h>

#include <QCoreApplication>
#include <QString>

#include "Application.h"
#include "Constants.h"
#include "CubeManager.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Gui.h"
#include "Message.h"
#include "Preference.h"
#include "SessionLog.h"
#include "System.h"
#include "TextFile.h"
#include "UserInterface.h"

/**
 * @file
 * $Revision: 1.22 $
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

namespace Isis {
  Application *iApp = NULL;

  /**
   * Constuctor for the Application object   
   *
   * @param argc Number of arguments in argv[].  This must be passed by
   *             reference!!
   *
   * @param *argv[] An array containing the command line arguments
   *
   * @throws Isis::iException::Io - FileOpen error
   */
  Application::Application(int &argc, char *argv[]) {
    // Init socket communications
    p_socket = -1;
    p_childSocket = -1;
    p_socketFile = "";
    p_ui = NULL;

    // Child pid and caught status
    p_childPid = -1;
    p_childCaught = false;

    // Save the application name
    p_appName = argv[0];

    // Get the starting wall clock time
    p_datetime = DateTime(&p_startTime);

    // Init
    p_startClock = 0;
    p_startDirectIO = 0;
    p_startPageFaults = 0;
    p_startProcessSwaps = 0;
    p_BatchlistPass = 0;

    // try to use US locale for numbers so we don't end up printing "," instead
    //   of "." where it might count.
    setlocale(LC_ALL, "en_US");
    putenv("LANG=en_US");

    // Verify ISISROOT was set
    if (getenv("ISISROOT") == NULL || iString(getenv("ISISROOT")) == "") {
      std::string message = "Please set ISISROOT before running any Isis applications";
      std::cerr << message << std::endl;
      abort();
    }

    // Get the starting cpu time, direct I/Os, page faults, and swaps
    p_startClock = clock();
    p_startDirectIO = DirectIO();
    p_startPageFaults = PageFaults();
    p_startProcessSwaps = ProcessSwaps();

    // Create user interface and log
    try {
      Filename f = std::string(argv[0]) + ".xml";

      // Create preferences
      Preference::Preferences(f.Name() == "unitTest.xml");

      if(!f.Exists()) {
        f = "$ISISROOT/bin/xml/" + f.Name();
        if(!f.Exists()) {
          std::string message = Message::FileOpen(f.Expanded());
          throw iException::Message(iException::Io, message, _FILEINFO_);
        }
      }
      std::string xmlfile = f.Expanded();

      p_ui = new UserInterface(xmlfile, argc, argv);
      if(!p_ui->IsInteractive()) {
        new QCoreApplication(argc, argv);

        // Add the Qt plugin directory to the library path
        Filename qtpluginpath("$ISISROOT/3rdParty/plugins");
        QCoreApplication::addLibraryPath(qtpluginpath.Expanded().c_str());
      }

    }
    catch(iException &e) {
      exit(e.Report());
    }

    iApp = this;
  }

  //! Destroys the Application object
  Application::~Application() {
    if(HasParent()) {
      SendParentData("DONE", "");
      close(p_childSocket);
    }

    if(p_socket >= 0) {
      close(p_socket);
      remove(p_socketFile.c_str());
    }

    if(p_ui) {
      delete p_ui;
    }
  }

  /**
   * Executes a function
   *
   * @param *funct
   *
   * @return int Status of the function execution
   */
  int Application::Exec(void (*funct)()) {
    int status = 0;
    try {
      if(p_ui->IsInteractive()) {
        p_ui->TheGui()->Exec(funct);
      }
      else {
        if(p_ui->BatchListSize() > 0) {
          for(int i = 0; i < p_ui->BatchListSize(); i++) {
            try {
              p_ui->SetBatchList(i);

              if(i != 0) {
                p_datetime = DateTime(&p_startTime);
                p_startClock = clock();
                p_startDirectIO = DirectIO();
                p_startPageFaults = PageFaults();
                p_startProcessSwaps = ProcessSwaps();
                SessionLog::TheLog(true);
              }

              funct();
              Application::FunctionCleanup();
              p_BatchlistPass++;
            }
            catch(iException &e) {
              p_ui->SetErrorList(i);
              status = Application::FunctionError(e);
              if(p_ui->AbortOnError()) {
                for(int j = (i + 1); j < p_ui->BatchListSize(); j++) {
                  p_ui->SetErrorList(j);
                  p_BatchlistPass++;
                }
                break;
              }
            }
          }
        }
        else {
          p_ui->SaveHistory();
          // The gui checks everything but not the command line mode so
          // verify if necessary. Batchlist verifies on SetBatchList
          p_ui->VerifyAll();
          funct();
          Application::FunctionCleanup();
        }
      }
    }
    catch(iException &e) {
      status = Application::FunctionError(e);
    }

#if 0
    catch(std::exception &e) {
      std::string message = e.what();
      Isis::iExceptionSystem i(message, _FILEINFO_);
      status = i.Report();
    }
    catch(...) {
      std::string message = "Unknown error expection";
      Isis::iExceptionSystem i(message, _FILEINFO_);
      status = i.Report();
    }
#endif

    return status;
  }

  /**
   * Creates an application history PvlObject
   *
   * @return PvlObject
   */
  PvlObject Application::History() {
    PvlObject history(p_ui->ProgramName());
    history += PvlKeyword("IsisVersion", Version());
    history += PvlKeyword("ProgramVersion", p_ui->Version());
    QString path = QCoreApplication::applicationDirPath();
    history += PvlKeyword("ProgramPath", path);
    history += PvlKeyword("ExecutionDateTime", p_datetime);
    history += PvlKeyword("HostName", HostName());
    history += PvlKeyword("UserName", UserName());
    history += PvlKeyword("Description", p_ui->Brief());

    // Add the user parameters
    Pvl pvl;
    p_ui->CommandLine(pvl);
    history.AddGroup(pvl.FindGroup("UserParameters"));

    return history;
  }

  /**
   * Creates accounting PvlGroup
   *
   * @return PvlGroup Accounting Group
   */
  PvlGroup Application::Accounting() {
    // Grab the ending time to compute connect time
    time_t endTime = time(NULL);
    double seconds = difftime(endTime, p_startTime);
    int minutes = (int)(seconds / 60.0);
    seconds = seconds - minutes * 60.0;
    int hours = minutes / 60;
    minutes = minutes - hours * 60;
    char temp[80];
    sprintf(temp, "%02d:%02d:%04.1f", hours, minutes, seconds);
    std::string conTime = temp;

    // Grab the ending cpu time to compute total cpu time
    clock_t endClock = clock();
    seconds = (double(endClock) - (double)p_startClock) / CLOCKS_PER_SEC;
    minutes = (int)(seconds / 60.0);
    seconds = seconds - minutes * 60.0;
    hours = minutes / 60;
    minutes = minutes - hours * 60;
    sprintf(temp, "%02d:%02d:%04.1f", hours, minutes, seconds);
    std::string cpuTime = temp;

    // Add this information to the log
    PvlGroup acct("Accounting");
    acct += PvlKeyword("ConnectTime", conTime);
    acct += PvlKeyword("CpuTime", cpuTime);

    // Not sure if these are really valuable.  If deemed so then
    // uncomment and complete private methods (DirectIO, Pagefaults, and
    // ProcessSwaps).
    //int directIO = DirectIO();
    //int pageFaults = PageFaults();
    //int processSwaps = ProcessSwaps();
    //acct += Isis::PvlKeyword("DirectIo",directIO);
    //acct += Isis::PvlKeyword("PageFaults",pageFaults);
    //acct += Isis::PvlKeyword("ProcessSwaps",processSwaps);

    return acct;
  }

  /**
   * Returns the current number of I/O's
   *
   * @return int The current number of I/O's
   */
  int Application::DirectIO() {
    return 0 - p_startDirectIO;
  }

  /**
   * Returns the current number of faults
   *
   * @return The current number of page faults
   */
  int Application::PageFaults() {
    return 0 - p_startPageFaults;
  }

  /**
   * Returns the current number of swaps
   *
   * @return The current number of process swaps
   */
  int Application::ProcessSwaps() {
    return 0 - p_startProcessSwaps;
  }

  /**
   * Writes Pvl results to sessionlog and printfile.
   *
   * @param results PvlGroup of results to add to the session log
   */
  void Application::Log(PvlGroup &results) {
    // Add it to the log file
    static bool blankLine = false;
    SessionLog::TheLog().AddResults(results);

    // See if the log file will be written to the terminal/gui
    if(SessionLog::TheLog().TerminalOutput()) return;

    // See if we should write the info to our parents gui
    if(HasParent()) {
      std::ostringstream ostr;
      if(blankLine) ostr << std::endl;
      ostr << results << std::endl;
      std::string data = ostr.str();
      SendParentData(std::string("LOG"), data);
    }

    // Otherwise see if we need to write to our gui
    else if(iApp->GetUserInterface().IsInteractive()) {
      std::ostringstream ostr;
      if(blankLine) ostr << std::endl;
      ostr << results << std::endl;
      iApp->GetUserInterface().TheGui()->Log(ostr.str());
      iApp->GetUserInterface().TheGui()->ShowLog();
    }

    // Otherwise its command line mode
    else {
      if(blankLine) std::cout << std::endl;
      std::cout << results << std::endl;
    }
    blankLine = true;
  }

  /**
   * Writes the Pvl results to the sessionlog, but not to the printfile
   *
   * @param results Pvl containing the results to add to the session log
   */
  void Application::GuiLog(Pvl &results) {
    // See if we should write the info to our parents gui
    if(HasParent()) {
      std::ostringstream ostr;
      ostr << results << std::endl;
      std::string data = ostr.str();
      SendParentData(std::string("LOG"), data);
    }

    // Otherwise see if we need to write to our gui
    else if(iApp->GetUserInterface().IsInteractive()) {
      std::ostringstream ostr;
      ostr << results << std::endl;
      iApp->GetUserInterface().TheGui()->Log(ostr.str());
      iApp->GetUserInterface().TheGui()->ShowLog();
    }
  }

  /**
   * Writes the PvlGroup results to the sessionlog, but not to the printfile
   *
   * @param results PvlGroup containing the results to add to the session log
   */
  void Application::GuiLog(PvlGroup &results) {
    // See if we should write the info to our parents gui
    if(HasParent()) {
      std::ostringstream ostr;
      ostr << results << std::endl;
      std::string data = ostr.str();
      SendParentData(std::string("LOG"), data);
    }

    // Otherwise see if we need to write to our gui
    else if(iApp->GetUserInterface().IsInteractive()) {
      std::ostringstream ostr;
      ostr << results << std::endl;
      iApp->GetUserInterface().TheGui()->Log(ostr.str());
      iApp->GetUserInterface().TheGui()->ShowLog();
    }
  }

  /**
   * Writes the results to the sessionlog, but not to the printfile
   *
   * @param results string containing the results to add to the session log
   */
  void Application::GuiLog(std::string &results) {
    // See if we should write the info to our parents gui
    if(HasParent()) {
      SendParentData(std::string("LOG"), results);
    }

    // Otherwise see if we need to write to our gui
    else if(iApp->GetUserInterface().IsInteractive()) {
      iApp->GetUserInterface().TheGui()->Log(results);
      iApp->GetUserInterface().TheGui()->ShowLog();
    }
  }

  /**
   * Returns the UserInterface object
   *
   * @return A pointer to the UserInterface object
   */
  Isis::UserInterface &Application::GetUserInterface() {
    return *iApp->p_ui;
  }

  /**
   * Returns whether the application has a parent or not
   *
   * @return bool Returns true if it has a parent, and false if it does not
   */
  bool Application::HasParent() {
    if(iApp == NULL) return false;
    if(iApp->p_ui == NULL) return false;
    if(iApp->p_ui->ParentId() == 0) return false;
    return true;
  }

  /**
   * Sends errors to the parent
   *
   * @param &errors A PvlObject of the errors
   */
  void Application::SendParentErrors(Isis::PvlObject &errors) {
    for(int i = 0; i < errors.Groups(); i++) {
      std::ostringstream ostr;
      ostr << errors.Group(i) << std::endl;
      std::string data = ostr.str();
      SendParentData(std::string("ERROR"), data);
    }
  }

  /**
   * Sends data from the child to the parent
   *
   * @param code
   *
   * @param &message The message the child is sending to the parent
   *
   * @throws Isis::iException::System - Unable to connect to the parent
   * @throws Isis::iException::System - Unable to send to the parent
   */
  int Application::p_childSocket = -1;
  void Application::SendParentData(const std::string code, const std::string &message) {
    // See if we need to connect to the parent
    if(p_childSocket < 0) {
      std::string socketFile = "/tmp/isis_" + iString(iApp->GetUserInterface().ParentId());
      sockaddr_un socketName;
      socketName.sun_family = AF_UNIX;
      strcpy(socketName.sun_path, socketFile.c_str());
      p_childSocket = socket(PF_UNIX, SOCK_STREAM, 0);
      if(p_childSocket == -1) {
        std::string msg = "Unable to create child-to-parent socket [" +
                          socketFile + "]";
        std::cout << msg << std::endl;
        throw iException::Message(Isis::iException::System,
                                        msg, _FILEINFO_);
      }

      errno = 0;
      int len = strlen(socketName.sun_path) + sizeof(socketName.sun_family);
      int status = connect(p_childSocket, (struct sockaddr *)&socketName, len);
      if(status == -1) {
        std::string msg = "Unable to connect to parent [" +
                          iString(iApp->GetUserInterface().ParentId()) + "] errno = " +
                          iString(errno);
        std::cout << msg << std::endl;
        throw iException::Message(Isis::iException::System,
                                        msg, _FILEINFO_);
      }
    }

    // Have connection so build data string and send it
    std::string data = code;
    data += char(27);
    if(message != "") {
      data += message;
      data += char(27);
    }

    if(send(p_childSocket, data.c_str(), data.size(), 0) < 0) {
      std::string msg = "Unable to send to parent [" +
                        iString(iApp->GetUserInterface().ParentId()) + "]";
      std::cout << msg << std::endl;
      throw iException::Message(iException::System, msg, _FILEINFO_);
    }
  }

  /**
   * Cleans up after the function by writing the log, saving the history, and
   * either sending the log to the parent if it has one, printing the log data
   * to the terminal or showing the log in the gui.
   */
  void Application::FunctionCleanup() {
    CubeManager::CleanUp();

    SessionLog::TheLog().Write();

    if(SessionLog::TheLog().TerminalOutput()) {
      if(HasParent()) {
        std::ostringstream ostr;
        ostr << SessionLog::TheLog() << std::endl;
        std::string data = ostr.str();
        SendParentData(std::string("LOG"), data);
      }
      else if(p_ui->IsInteractive()) {
        std::ostringstream ostr;
        ostr << SessionLog::TheLog() << std::endl;
        p_ui->TheGui()->Log(ostr.str());
        p_ui->TheGui()->ShowLog();
      }
      else {
        std::cout << SessionLog::TheLog() << std::endl;
      }
    }

    // If debugging flag on write debugging log
    if(p_ui->GetInfoFlag()) {
      std::string filename = p_ui->GetInfoFileName();
      Pvl log;
      iString app = (iString)QCoreApplication::applicationDirPath() + "/" + (iString)p_appName;
      if(p_BatchlistPass == 0) {
        std::stringstream ss ;
        ss << SessionLog::TheLog();
        ss.clear();
        ss >> log;
        PvlGroup uname = GetUnameInfo();
        PvlGroup env = GetEnviromentInfo();
        log.AddGroup(uname);
        log.AddGroup(env);
      }

      // Write to file
      if(filename.compare("") != 0) {

        if(p_BatchlistPass == 0) {
          std::ofstream debugingLog(filename.c_str());
          if(!debugingLog.good()) {
            std::string msg = "Error opening debugging log file [" + filename + "]";
            throw iException::Message(Isis::iException::System, msg, _FILEINFO_);
          }
          debugingLog << log << std::endl;
          debugingLog << "\n############### User Preferences ################\n" << std::endl;
          debugingLog << Preference::Preferences();
          debugingLog << "\n############## System Disk Space ################\n" << std::endl;
          debugingLog << SystemDiskSpace() << std::endl;
          debugingLog << "\n############ Executable Information #############\n" << std::endl;
          debugingLog << GetLibraryDependencies(app) << std::endl;
          debugingLog.close();
        }
        else {
          std::ofstream debugingLog(filename.c_str(), std::ios_base::app);
          debugingLog << SessionLog::TheLog() << std::endl;
          debugingLog.close();
        }
      }
      else {   // Write to std out
        if(p_BatchlistPass == 0) {
          std::cout << log << std::endl;
          std::cout << "\n############### User Preferences ################\n" << std::endl;
          std::cout << Preference::Preferences();
          std::cout << "\n############## System Disk Space ################\n" << std::endl;
          std::cout << SystemDiskSpace() << std::endl;
          std::cout << "\n############ Executable Information #############\n" << std::endl;
          std::cout << GetLibraryDependencies(app) << std::endl;
        }
        else {
          std::cout << SessionLog::TheLog() << std::endl;
        }
      }
    }


  }

  /**
   * Adds the error to the session log, sends the error to the parent if it has
   * one, loads the error message into the gui or prints it to the command line,
   * gets the error type and returns it.
   *
   * @param e The Isis::iException
   *
   * @return int The Error Type
   */
  int Application::FunctionError(Isis::iException &e) {
    Pvl errors = e.PvlErrors();
    SessionLog::TheLog().AddError(errors);
    SessionLog::TheLog().Write();

    if(HasParent()) {
      SendParentErrors(errors);
    }
    else if(p_ui->IsInteractive()) {
      if(e.IsPvlFormat()) {
        std::ostringstream ostr;
        ostr << errors << std::endl;
        p_ui->TheGui()->LoadMessage(ostr.str());
      }
      else {
        p_ui->TheGui()->LoadMessage(e.Errors());
      }
    }
    else if(SessionLog::TheLog().TerminalOutput()) {
      std::cerr << SessionLog::TheLog() << std::endl;
    }
    else if(e.IsPvlFormat()) {
      std::cerr << errors << std::endl;
    }
    else {
      std::cerr << e.Errors() << std::endl;
    }

    // If debugging flag on write debugging log
    if(p_ui->GetInfoFlag()) {
      std::string filename = p_ui->GetInfoFileName();
      Pvl log;
      iString app = (iString)QCoreApplication::applicationDirPath() + "/" + (iString)p_appName;
      if(p_BatchlistPass == 0) {
        std::stringstream ss ;
        ss << SessionLog::TheLog();
        ss.clear();
        ss >> log;
        PvlGroup uname = GetUnameInfo();
        PvlGroup env = GetEnviromentInfo();
        log.AddGroup(uname);
        log.AddGroup(env);
      }

      // Write to file
      if(filename.compare("") != 0) {
        if(p_BatchlistPass == 0) {
          std::ofstream debugingLog(filename.c_str());
          if(!debugingLog.good()) {
            std::string msg = "Error opening debugging log file [" + filename + "]";
            throw iException::Message(iException::System, msg, _FILEINFO_);
          }
          debugingLog << log << std::endl;
          debugingLog << "\n############### User Preferences ################\n" << std::endl;
          debugingLog << Preference::Preferences();
          debugingLog << "\n############ System Disk Space #############\n" << std::endl;
          debugingLog << SystemDiskSpace() << std::endl;
          debugingLog << "\n############ Executable Information #############\n" << std::endl;
          debugingLog << GetLibraryDependencies(app) << std::endl;
          debugingLog.close();
        }
        else {
          std::ofstream debugingLog(filename.c_str(), std::ios_base::app);
          debugingLog << SessionLog::TheLog() << std::endl;
          debugingLog.close();
        }
      }
      else {   // Write to std out
        if(p_BatchlistPass == 0) {
          std::cout << log << std::endl;
          std::cout << "\n############### User Preferences ################\n" << std::endl;
          std::cout << Preference::Preferences();
          std::cout << "\n############ System Disk Space #############\n" << std::endl;
          std::cout << SystemDiskSpace() << std::endl;
          std::cout << "\n############ Executable Information #############\n" << std::endl;
          std::cout << GetLibraryDependencies(app) << std::endl;
        }
        else {
          std::cout << SessionLog::TheLog() << std::endl;
        }
      }
    }

    int errorType = (int)e.Type();
    e.Clear();
    return errorType;
  }

  /**
   * Loads the error message into the gui, but does not write it to the session
   *  log.
   *
   * @param e The Isis::iException
   */
  void Application::GuiReportError(Isis::iException &e) {
    Pvl errors = e.PvlErrors();
    if(e.Type() == iException::Cancel) {
      e.Clear();
      p_ui->TheGui()->ProgressText("Stopped");
    }
    if(e.IsPvlFormat()) {
      std::ostringstream ostr;
      ostr << errors << std::endl;
      p_ui->TheGui()->LoadMessage(ostr.str());
    }
    else {
      p_ui->TheGui()->LoadMessage(e.Errors());
    }

    if(p_ui->TheGui()->ShowWarning()) exit(0);
    p_ui->TheGui()->ProgressText("Error");
    e.Clear();
  }

  /**
   * Returns the name of the application.  Returns 'Unknown' if the application
   * or gui equal NULL
   *
   * @return string The application name
   */
  QString Application::p_appName("Unknown");
  std::string Application::Name() {
    return p_appName.toStdString();
  }

  /**
   * Updates the progress bar in the gui.
   *
   * @param text Progress text
   *
   * @param print
   */
  void Application::UpdateProgress(const std::string &text, bool print) {
    if(HasParent()) {
      std::string msg = p_ui->ProgramName() + ": " + text;
      SendParentData(std::string("PROGRESSTEXT"), msg);
    }
    else if(p_ui->IsInteractive()) {
      p_ui->TheGui()->ProgressText(text);
    }
    else if(print) {
      std::string msg = p_ui->ProgramName() + ": " + text;
      std::cout << msg << std::endl;
    }
  }

  /**
   * Updates the progress bar percent.
   *
   * @param percent The percent of the application that is complete
   *
   * @param print
   */
  void Application::UpdateProgress(int percent, bool print) {
    if(HasParent()) {
      std::string data = iString(percent);
      SendParentData(std::string("PROGRESS"), data);
    }
    else if(p_ui->IsInteractive()) {
      p_ui->TheGui()->Progress(percent);
    }
    else if(print) {
      if(percent < 100) {
        std::cout << percent << "% Processed\r" << std::flush;
      }
      else {
        std::cout << percent << "% Processed" << std::endl;
      }
    }
  }

  /**
   * Processes the gui events. If the event is cancel, it throws a cancel
   * exception
   *
   * @throws Isis::iException::Cancel - The event was cancelled
   */
  void Application::ProcessGuiEvents() {
    if(p_ui->IsInteractive()) {
      bool cancel = p_ui->TheGui()->ProcessEvents();
      if(cancel) {

        // We need to catch our child if we are canceling, throw our own error if
        // the return status isn't zero, blocking wait.
        if(!p_childCaught) {
          int status = 0;
          waitpid(p_childPid, &status, 0);
          p_childCaught = true;
          if(status) {
            std::string msg = "Program execution canceled, child returned nonzero status ["
                              + (iString)status + "]";
            throw iException::Message(Isis::iException::System, msg, _FILEINFO_);
          }
        }
        throw iException::Message(Isis::iException::Cancel, "", _FILEINFO_);
      }
    }
  }

  /**
   * Executes the program passed in with the given string of parameters.
   *
   * @param &program The program name to be run
   *
   * @param &parameters A string of the parameters and their values to be run
   *                    with the application name passed in
   *
   * @throws Isis::iException::System - Unable to execute the command
   */
  void Application::Exec(const std::string &program, const std::string &parameters) {
    // Setup the command line
    Filename bin(program);
    if(!bin.Exists()) {
      bin = "$ISISROOT/bin/" + program;
    }
    //Isis::Filename bin("$ISISROOT/bin/"+program);
    std::string command = bin.Expanded() + " " + parameters;

    // If we are interactive we must do an asychronous execute
    // so we can watch our socket
    if(p_ui->IsInteractive()) {
      p_pid = getpid();
      p_childCaught = false;
      // fork and save off our child
      if((p_childPid = fork()) == -1) {
        std::string msg = "Unable to execute command [" + command + "]";
        throw iException::Message(iException::System, msg, _FILEINFO_);
      }

      // Parent code
      if(p_childPid != 0) {
        EstablishConnections();
        ParentFork(command, program);
        ProcessGuiEvents();

        // ParentFork only returns after the child has returned success
        // we should be safe to pick up a finished child at this point
        // `man 2 wait` for more info, this is a blocking wait for specific pid.
        // Odds are if we make it to here, we were successful, but, we'll
        // still be verifying that.
        if(!p_childCaught) {
          int status = 0;
          waitpid(p_childPid, &status, 0);
          p_childCaught = true;
          if(status) {
            std::string msg = "Child process return status was nonzero, something went wrong";
            throw iException::Message(Isis::iException::System, msg, _FILEINFO_);
          }
        }

        // Child code
      }
      else {
        ChildFork(command);
      }
    }
    // Handle command line programs invoked by GUI programs
    else if(HasParent()) {
      SendParentData("DISCONNECT", "");
      close(p_childSocket);
      p_childSocket = -1;
      command += " -pid=" + iString((int)p_ui->ParentId());
      System(command);
      SendParentData("RECONNECT", "");
    }
    // Otherwise just execute the command and wait for it to finish
    else {
      System(command);
    }
  }

  /**
   * Establishes a connection by creating a new socket, getting the process id
   * number and creating a unique filename that it binds to the socket.
   * It then sets up the socket to listen.
   *
   * @throws Isis::iException::System - Unable to create the socket
   * @throws Isis::iException::System - Unable to bind the socket to the filename
   * @throws Isis::iException::System - Unable to listen to the socket
   */
  void Application::EstablishConnections() {
    if(p_socket != -1) return;

    // Create a socket
    if((p_socket = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
      std::string msg = "Unable to create socket";
      throw iException::Message(Isis::iException::System, msg, _FILEINFO_);
    }

    // Get the process id and create a unique filename
    p_socketFile = "/tmp/isis_" + iString((int)p_pid);

    // Bind the file to the socket
    p_socketName.sun_family = AF_UNIX;
    strcpy(p_socketName.sun_path, p_socketFile.c_str());
    int len = strlen(p_socketName.sun_path) + sizeof(p_socketName.sun_family);
    if(bind(p_socket, (struct sockaddr *)&p_socketName, len) == -1) {
      std::string msg = "Unable to bind to socket [" + p_socketFile + "]";
      throw iException::Message(iException::System, msg, _FILEINFO_);
    }

    // Set up to listen to the socket
    if(listen(p_socket, 5) == -1) {
      std::string msg = "Unable to listen to socket [" + p_socketFile + "]";
      throw iException::Message(iException::System, msg, _FILEINFO_);
    }
  }

  /**
   * Runs the entered command line and returns the status to the parent
   *
   * @param commandLine The command line to be run
   */
  void Application::ChildFork(const std::string &commandLine) {
    // The child fork doesn't have the socket
    p_socket = -1;

    // Append the pid as an argument
    std::string command = commandLine + " -pid=" + iString((int)p_pid);

    // Run the command
    int status = system(command.c_str());

    int childSocket = socket(PF_UNIX, SOCK_STREAM, 0);
    if(childSocket == -1) {
      std::cout << "couldn't create socket to parent fork" << std::endl << std::flush;
      exit(255);
    }

    sockaddr_un socketName;
    socketName.sun_family = AF_UNIX;
    iString socketFile = "/tmp/isis_" + iString((int)p_pid);
    strcpy(socketName.sun_path, socketFile.c_str());

    int len = strlen(socketName.sun_path) + sizeof(socketName.sun_family);
    if(connect(childSocket, (struct sockaddr *)&socketName, len) == -1) {
      std::cout << "couldn't connect to parent fork" << std::endl << std::flush;
      exit(255);
    }

    if(status != 0) {
      char terminate[10] = "TERMINATE";
      terminate[9] = 27; // escape character
      if(send(childSocket, terminate, sizeof(terminate), 0) < 0) {
        std::cout << "couldn't send terminate to parent fork" << std::endl << std::flush;
        exit(255);
      }
    }
    else {
      char success[8] = "SUCCESS";
      success[7] = 27; // escape character
      if(send(childSocket, success, sizeof(success), 0) < 0) {
        std::cout << "couldn't send success to parent fork" << std::endl << std::flush;
        exit(255);
      }
    }

    close(childSocket);
    exit(0);
  }

  /**
   *
   *
   * @param &command The command to run
   *
   * @param &program The program name currently running
   *
   * @throws Isis::iException::System - Unable to execute the command
   * @throws Isis::iException::System - Unknown command on the socket
   */
  void Application::ParentFork(const std::string &command, const std::string &program) {
    // Try to accept a connection from the program our child fork executed.  If
    // the program we ran failed to start then we are waiting for the connection
    // in the child fork
    p_buffer = "";
    p_queue.clear();
    int status = -21; // Magic number hopefully not going to appear in a return status
    int childForkDone = false;
    while(!childForkDone) {
      int childSocket = -1;
      while(childSocket == -1) {

        // verify the child hasn't had any problems,
        // should only be exiting here if there were problems
        // nonblocking wait
        if(!p_childCaught) {
          waitpid(p_childPid, &status, WNOHANG);
          if(status != -21) {
            p_childCaught = true;
            if(status == 255) {
              std::string msg = "Error in connection between processes";
              throw iException::Message(iException::System, msg, _FILEINFO_);
            }
          }
        }
        errno = 0;
        socklen_t len = sizeof(p_socketName);
        childSocket = accept(p_socket, (struct sockaddr *)&p_socketName, &len);
        if(childSocket == -1) {
          if(errno != EINTR) {
            std::cout << "accept errno = " << errno << std::endl << std::flush;
            std::string msg = "Unable to accept socket connection [" +
                              p_socketFile + "] from child process";
            throw iException::Message(iException::System, msg, _FILEINFO_);
          }
        }
      }

      // Looks like we are the parent so we must wait for input to come from
      // the child.
      bool done = false;
      while(!done) {
        WaitForCommand(childSocket);

        // Again, check for problems, such as with socket communication
        // there is a possibility we could catch a successful run here
        // nonblocking wait
        if(!p_childCaught) {
          waitpid(p_childPid, &status, WNOHANG);
          if(status != -21) {
            p_childCaught = true;
            if(status == 255) {
              std::string msg = "Error in connection between processes";
              throw iException::Message(iException::System, msg, _FILEINFO_);
            }
          }
        }

        while(p_queue.size() > 0) {
          if(p_queue[0] == "DONE") {
            p_queue.erase(p_queue.begin());
            done = true;
          }
          else if(p_queue[0] == "DISCONNECT") {
            p_queue.erase(p_queue.begin());
            done = true;
          }
          else if(p_queue[0] == "RECONNECT") {
            p_queue.erase(p_queue.begin());
          }
          else if(p_queue[0] == "SUCCESS") {
            p_queue.erase(p_queue.begin());
            done = true;
            childForkDone = true;
          }
          else if(p_queue[0] == "TERMINATE") {
            // Should only happen if child fork could not fire off the process
            p_queue.erase(p_queue.begin());
            std::string msg = "Unable to execute command [" + command + "]";
            throw iException::Message(iException::System, msg, _FILEINFO_);
          }
          else if(p_queue[0] == "ERROR") {
            p_queue.erase(p_queue.begin());
            while(p_queue.size() == 0) WaitForCommand(childSocket);
            std::stringstream str;
            str << p_queue[0];
            p_queue.erase(p_queue.begin());

            Pvl error;
            str >> error;

            for(int i = 0; i < error.Groups(); i++) {
              PvlGroup &g = error.Group(i);
              std::string eclass = g["Class"];
              std::string emsg = g["Message"];
              int ecode = g["Code"];
              std::string efile = g["File"];
              int eline = g["Line"];

              iException::Message((iException::errType)ecode,
                                        emsg, (char *)efile.c_str(), eline);
            }
          }
          else if(p_queue[0] == "PROGRESSTEXT") {
            p_queue.erase(p_queue.begin());
            while(p_queue.size() == 0) WaitForCommand(childSocket);
            p_ui->TheGui()->ProgressText(p_queue[0]);
            p_ui->TheGui()->Progress(0);
            p_queue.erase(p_queue.begin());
          }
          else if(p_queue[0] == "PROGRESS") {
            p_queue.erase(p_queue.begin());
            while(p_queue.size() == 0) WaitForCommand(childSocket);
            p_ui->TheGui()->Progress(iString(p_queue[0]).ToInteger());
            p_queue.erase(p_queue.begin());
          }
          else if(p_queue[0] == "LOG") {
            p_queue.erase(p_queue.begin());
            while(p_queue.size() == 0) WaitForCommand(childSocket);

            std::stringstream str;
            str << p_queue[0];
            p_queue.erase(p_queue.begin());

            p_ui->TheGui()->Log(str.str());
          }
          else {
            std::string msg = "Unknown command [" + p_queue[0];
            msg += "] on socket [" + p_socketFile + "]";
            throw iException::Message(iException::System, msg, _FILEINFO_);
          }
          ProcessGuiEvents();
          // Last check for any bad return status from child
          if(!p_childCaught) {
            waitpid(p_childPid, &status, WNOHANG);
            if(status != -21) {
              p_childCaught = true;
              if(status == 255) {
                std::string msg = "Error in connection between processes";
                throw iException::Message(iException::System, msg, _FILEINFO_);
              }
            }
          }
        }
      }
      close(childSocket);
    }
  }

  /**
   * Trys to accept a connection from the child socket.  When it does accept the
   * connection it receives the command from the child and pushes it into a
   * buffer which is then pushed onto the queue.
   *
   * @param childSocket The child socket to establish connection to
   *
   * @throws Isis::iException::System - Unable to read from the socket
   */
  void Application::WaitForCommand(int childSocket) {

    // Now we are ready to receive commands from our child (like in real
    // life) so receive the command
    int bytes;
    char buf[1024*1024];
    if((bytes = recv(childSocket, &buf, 1024 * 1024, 0)) < 0) {
      std::string msg = "Unable to read from socket [" + p_socketFile + "]";
      throw iException::Message(iException::System, msg, _FILEINFO_);
    }

    // Push everything onto our string buffer
    for(int i = 0; i < bytes; i++) p_buffer += buf[i];

    // Move esc delimited strings to our command queue
    std::string::size_type pos;
    while((pos = p_buffer.find(27)) != std::string::npos) {
      p_queue.push_back(p_buffer.substr(0, pos));
      p_buffer.erase(0, pos + 1);
    }
  }

  /**
   * Returns the date and time as a string.
   *
   * @param *curtime
   *
   * @return string The date and time
   */
  iString Application::DateTime(time_t *curtime) {
    time_t startTime = time(NULL);
    if(curtime != 0) *curtime = startTime;
    struct tm *tmbuf = localtime(&startTime);
    char timestr[80];
    strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
    return(std::string) timestr;
  }

  /**
   * Returns the user name. Returns 'Unknown' if it cannot find the user name.
   *
   * @return string User Name
   */
  iString Application::UserName() {
    std::string user = "Unknown";
    char *userPtr = getenv("USER");
    if(userPtr != NULL) user = userPtr;
    return user;
  }

  /**
   * Returns the host name.  Returns 'Unknown' if it cannot find the host name.
   *
   * @return string Host Name
   */
  iString Application::HostName() {
    std::string host = "Unknown";
    char *hostPtr = getenv("HOST");
    if(hostPtr == NULL) hostPtr = getenv("HOSTNAME");
    if(hostPtr != NULL) host = hostPtr;
    return host;
  }

  iString Application::Version() {
    TextFile versionFile("$ISISROOT/version");
    iString versionString = versionFile.GetLine() + " | " +
                            versionFile.GetLine();

    return versionString;
  }
}  //end namespace isis

