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
#include "IsisDebug.h"

#include <cstdio>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

extern int errno;

#include <fstream>
//#include <stdlib.h>
//#include <string>

#include <iostream>
#include <sstream>
#include <locale.h>

#include <QCoreApplication>
#include <QLocalSocket>
#include <QString>

#include "Application.h"
#include "Constants.h"    //is this still used in this class?
#include "CubeManager.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Gui.h"  //is this still used?
#include "Message.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "SessionLog.h"
#include "TextFile.h"
#include "UserInterface.h"

using namespace std;

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
    p_ui = NULL;
    p_connectionToParent = NULL;

    // Save the application name
    p_appName = argv[0];

    // Get the starting wall clock time
    // p_datetime = DateTime(&p_startTime);
        
    // Init
    p_startClock = 0;
    p_startDirectIO = 0;
    p_startPageFaults = 0;
    p_startProcessSwaps = 0;
    p_BatchlistPass = 0;

    // try to use US locale for numbers so we don't end up printing "," instead
    //   of "." where it might count.
    setlocale(LC_ALL, "en_US");

    char env[1024];
    strncpy(env, "LANG=en_US", 1023);
    putenv(env);

    // Verify ISISROOT was set
    if (getenv("ISISROOT") == NULL || iString(getenv("ISISROOT")) == "") {
      string message = "Please set ISISROOT before running any Isis "
          "applications";
      cerr << message << endl;
      abort();
    }

    // Get the starting cpu time, direct I/Os, page faults, and swaps
    //p_startClock = clock();
    p_startDirectIO = DirectIO();
    p_startPageFaults = PageFaults();
    p_startProcessSwaps = ProcessSwaps();

    // Create user interface and log
    try {
      Filename f(string(argv[0]) + ".xml");

      // Create preferences
      Preference::Preferences(f.Name() == "unitTest.xml");

      if (!f.Exists()) {
        f = "$ISISROOT/bin/xml/" + f.Name();
        if (!f.Exists()) {
          string message = Message::FileOpen(f.Expanded());
          throw iException::Message(iException::Io, message, _FILEINFO_);
        }
      }
      string xmlfile = f.Expanded();

      p_ui = new UserInterface(xmlfile, argc, argv);
      if (!p_ui->IsInteractive()) {
        // Get the starting wall clock time
        p_datetime = DateTime(&p_startTime);
        p_startClock = clock();
        //cerr << "Application NonGUI start clock=" << (long)p_startClock << " time=" << p_startTime << endl;
        new QCoreApplication(argc, argv);

        // Add the Qt plugin directory to the library path
        Filename qtpluginpath("$ISISROOT/3rdParty/plugins");
        QCoreApplication::addLibraryPath(qtpluginpath.Expanded().c_str());
      }
    }
    catch (iException &e) {
      exit(e.Report());
    }

    iApp = this;

    // If we were run by another Isis app, connect to it
    if (GetUserInterface().ParentId()) {
      p_connectionToParent = new QLocalSocket;

      iString serverName = "isis_" + UserName() +
          "_" + iString(iApp->GetUserInterface().ParentId());

      p_connectionToParent->connectToServer(serverName);
      if (!p_connectionToParent->waitForConnected()) {
        delete p_connectionToParent;
        p_connectionToParent = NULL;
      }
    }
  }

  //! Destroys the Application object
  Application::~Application() {
    if (p_ui) {
      delete p_ui;
    }
  }

  /**
   * Runs the program defined in the function funct.
   *
   * @param *funct
   *
   * @return int Status of the function execution
   */
  int Application::Run(void (*funct)()) {
    int status = 0;
    try {
      if (p_ui->IsInteractive()) {        
        p_ui->TheGui()->Exec(funct);
      }
      else {        
        if (p_ui->BatchListSize() > 0) {
          for (int i = 0; i < p_ui->BatchListSize(); i++) {
            try {
              p_ui->SetBatchList(i);

              if (i != 0) {
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
            catch (iException &e) {
              p_ui->SetErrorList(i);
              status = Application::FunctionError(e);
              if (p_ui->AbortOnError()) {
                for (int j = (i + 1); j < p_ui->BatchListSize(); j++) {
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
    catch (iException &e) {
      status = Application::FunctionError(e);
    }

#if 0
    catch (exception &e) {
      string message = e.what();
      Isis::iExceptionSystem i(message, _FILEINFO_);
      status = i.Report();
    }
    catch (...) {
      string message = "Unknown error expection";
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
    if (p_ui->IsInteractive()) {
      p_startClock = clock();
      p_datetime = DateTime(&p_startTime);
      //cerr << "History GUI start clock=" << p_startClock << " time=" << p_startTime << endl;
    }
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
    string conTime = temp;

    //cerr << "Accounting GUI end time=" << endTime <<  " start time=" << p_startTime << "  total=" << seconds << endl;
    
    // Grab the ending cpu time to compute total cpu time
    clock_t endClock = clock();
    seconds = (double(endClock) - (double)p_startClock) / CLOCKS_PER_SEC;
    minutes = (int)(seconds / 60.0);
    seconds = seconds - minutes * 60.0;
    hours = minutes / 60;
    minutes = minutes - hours * 60;
    sprintf(temp, "%02d:%02d:%04.1f", hours, minutes, seconds);
    string cpuTime = temp;
    
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
    if (SessionLog::TheLog().TerminalOutput()) return;

    // See if we should write the info to our parents gui
    if (HasParent()) {
      ostringstream ostr;
      if (blankLine) ostr << endl;
      ostr << results << endl;
      string data = ostr.str();
      iApp->SendParentData(string("LOG"), data);
    }

    // Otherwise see if we need to write to our gui
    else if (iApp->GetUserInterface().IsInteractive()) {
      ostringstream ostr;
      if (blankLine) ostr << endl;
      ostr << results << endl;
      iApp->GetUserInterface().TheGui()->Log(ostr.str());
      iApp->GetUserInterface().TheGui()->ShowLog();
    }

    // Otherwise its command line mode
    else {
      if (blankLine) cout << endl;
      cout << results << endl;
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
    if (HasParent()) {
      ostringstream ostr;
      ostr << results << endl;
      string data = ostr.str();
      iApp->SendParentData(string("GUILOG"), data);
    }

    // Otherwise see if we need to write to our gui
    else if (iApp->GetUserInterface().IsInteractive()) {
      ostringstream ostr;
      ostr << results << endl;
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
    if (HasParent()) {
      ostringstream ostr;
      ostr << results << endl;
      string data = ostr.str();
      iApp->SendParentData(string("GUILOG"), data);
    }

    // Otherwise see if we need to write to our gui
    else if (iApp->GetUserInterface().IsInteractive()) {
      ostringstream ostr;
      ostr << results << endl;
      iApp->GetUserInterface().TheGui()->Log(ostr.str());
      iApp->GetUserInterface().TheGui()->ShowLog();
    }
  }

  /**
   * Writes the results to the sessionlog, but not to the printfile
   *
   * @param results string containing the results to add to the session log
   */
  void Application::GuiLog(string &results) {
    // See if we should write the info to our parents gui
    if (HasParent()) {
      iApp->SendParentData(string("GUILOG"), results);
    }

    // Otherwise see if we need to write to our gui
    else if (iApp->GetUserInterface().IsInteractive()) {
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
    if (iApp == NULL) return false;
    if (iApp->p_ui == NULL) return false;
    if (iApp->p_ui->ParentId() == 0) return false;
    return true;
  }

  /**
   * Sends errors to the parent
   *
   * @param &errors A PvlObject of the errors
   */
  void Application::SendParentErrors(Isis::PvlObject &errors) {
    if (!HasParent()) return;

    for (int i = 0; i < errors.Groups(); i++) {
      ostringstream ostr;
      ostr << errors.Group(i) << endl;
      string data = ostr.str();
      iApp->SendParentData(string("ERROR"), data);
    }
  }


  /**
   * @param code 
   * @param message 
   */
  void Application::SendParentData(const string code,
      const string &message) {
    // See if we need to connect to the parent
    if (p_connectionToParent == NULL) {
      iString msg = "Unable to communicate with parent process with pid [" +
          iString(iApp->GetUserInterface().ParentId()) + "]";
      throw iException::Message(iException::System, msg, _FILEINFO_);
    }

    // Have connection so build data string and send it
    string data = code;
    data += char(27);
    data += message;
    data += char(27);
    data += '\n';

    if (p_connectionToParent->write(data.c_str(), data.size()) == -1) {
      string msg = "Unable to send data to parent [" +
          iString(iApp->GetUserInterface().ParentId()) + "]";
      throw iException::Message(iException::System, msg, _FILEINFO_);
    }

    p_connectionToParent->waitForBytesWritten(-1);
  }


  /**
   * Cleans up after the function by writing the log, saving the history, and
   * either sending the log to the parent if it has one, printing the log data
   * to the terminal or showing the log in the gui.
   */
  void Application::FunctionCleanup() {
    CubeManager::CleanUp();

    SessionLog::TheLog().Write();

    if (SessionLog::TheLog().TerminalOutput()) {
      if (HasParent()) {
        ostringstream ostr;
        ostr << SessionLog::TheLog() << endl;
        string data = ostr.str();
        iApp->SendParentData(string("LOG"), data);
      }
      else if (p_ui->IsInteractive()) {
        ostringstream ostr;
        ostr << SessionLog::TheLog() << endl;
        p_ui->TheGui()->Log(ostr.str());
        p_ui->TheGui()->ShowLog();
      }
      else {
        cout << SessionLog::TheLog() << endl;
      }
    }

    // If debugging flag on write debugging log
    if (p_ui->GetInfoFlag()) {
      string filename = p_ui->GetInfoFileName();
      Pvl log;
      iString app = (iString)QCoreApplication::applicationDirPath() + "/" + p_appName;
      if (p_BatchlistPass == 0) {
        stringstream ss ;
        ss << SessionLog::TheLog();
        ss.clear();
        ss >> log;
        PvlGroup uname = GetUnameInfo();
        PvlGroup env = GetEnviromentInfo();
        log.AddGroup(uname);
        log.AddGroup(env);
      }

      // Write to file
      if (filename.compare("") != 0) {

        if (p_BatchlistPass == 0) {
          ofstream debugingLog(filename.c_str());
          if (!debugingLog.good()) {
            string msg = "Error opening debugging log file [" + filename + "]";
            throw iException::Message(Isis::iException::System, msg, _FILEINFO_);
          }
          debugingLog << log << endl;
          debugingLog << "\n############### User Preferences ################\n" << endl;
          debugingLog << Preference::Preferences();
          debugingLog << "\n############## System Disk Space ################\n" << endl;
          debugingLog << GetSystemDiskSpace() << endl;
          debugingLog << "\n############ Executable Information #############\n" << endl;
          debugingLog << GetLibraryDependencies(app) << endl;
          debugingLog.close();
        }
        else {
          ofstream debugingLog(filename.c_str(), ios_base::app);
          debugingLog << SessionLog::TheLog() << endl;
          debugingLog.close();
        }
      }
      else {   // Write to std out
        if (p_BatchlistPass == 0) {
          cout << log << endl;
          cout << "\n############### User Preferences ################\n" << endl;
          cout << Preference::Preferences();
          cout << "\n############## System Disk Space ################\n" << endl;
          cout << GetSystemDiskSpace() << endl;
          cout << "\n############ Executable Information #############\n" << endl;
          cout << GetLibraryDependencies(app) << endl;
        }
        else {
          cout << SessionLog::TheLog() << endl;
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

    if (HasParent()) {
      SendParentErrors(errors);
    }
    else if (p_ui->IsInteractive()) {
      if (e.IsPvlFormat()) {
        ostringstream ostr;
        ostr << errors << endl;
        p_ui->TheGui()->LoadMessage(ostr.str());
      }
      else {
        p_ui->TheGui()->LoadMessage(e.Errors());
      }
    }
    else if (SessionLog::TheLog().TerminalOutput()) {
      cerr << SessionLog::TheLog() << endl;
    }
    else if (e.IsPvlFormat()) {
      cerr << errors << endl;
    }
    else {
      cerr << e.Errors() << endl;
    }

    // If debugging flag on write debugging log
    if (p_ui->GetInfoFlag()) {
      string filename = p_ui->GetInfoFileName();
      Pvl log;
      iString app = (iString)QCoreApplication::applicationDirPath() + "/" + p_appName;
      if (p_BatchlistPass == 0) {
        stringstream ss ;
        ss << SessionLog::TheLog();
        ss.clear();
        ss >> log;
        PvlGroup uname = GetUnameInfo();
        PvlGroup env = GetEnviromentInfo();
        log.AddGroup(uname);
        log.AddGroup(env);
      }

      // Write to file
      if (filename.compare("") != 0) {
        if (p_BatchlistPass == 0) {
          ofstream debugingLog(filename.c_str());
          if (!debugingLog.good()) {
            string msg = "Error opening debugging log file [" + filename + "]";
            throw iException::Message(iException::System, msg, _FILEINFO_);
          }
          debugingLog << log << endl;
          debugingLog << "\n############### User Preferences ################\n" << endl;
          debugingLog << Preference::Preferences();
          debugingLog << "\n############ System Disk Space #############\n" << endl;
          debugingLog << GetSystemDiskSpace() << endl;
          debugingLog << "\n############ Executable Information #############\n" << endl;
          debugingLog << GetLibraryDependencies(app) << endl;
          debugingLog.close();
        }
        else {
          ofstream debugingLog(filename.c_str(), ios_base::app);
          debugingLog << SessionLog::TheLog() << endl;
          debugingLog.close();
        }
      }
      else {   // Write to std out
        if (p_BatchlistPass == 0) {
          cout << log << endl;
          cout << "\n############### User Preferences ################\n" << endl;
          cout << Preference::Preferences();
          cout << "\n############ System Disk Space #############\n" << endl;
          cout << GetSystemDiskSpace() << endl;
          cout << "\n############ Executable Information #############\n" << endl;
          cout << GetLibraryDependencies(app) << endl;
        }
        else {
          cout << SessionLog::TheLog() << endl;
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
    if (e.Type() == iException::Cancel) {
      e.Clear();
      p_ui->TheGui()->ProgressText("Stopped");
    }
    if (e.IsPvlFormat()) {
      ostringstream ostr;
      ostr << errors << endl;
      p_ui->TheGui()->LoadMessage(ostr.str());
    }
    else {
      p_ui->TheGui()->LoadMessage(e.Errors());
    }

    if (p_ui->TheGui()->ShowWarning()) exit(0);
    p_ui->TheGui()->ProgressText("Error");
    e.Clear();
  }

  iString Application::p_appName("Unknown"); //!< 
  /**
   * Returns the name of the application.  Returns 'Unknown' if the application
   * or gui equal NULL
   *
   * @return string The application name
   */
  iString Application::Name() {
    return p_appName;
  }

  /**
   * Updates the progress bar in the gui.
   *
   * @param text Progress text
   *
   * @param print
   */
  void Application::UpdateProgress(const string &text, bool print) {
    if (HasParent() && print) {
      iApp->SendParentData(string("PROGRESSTEXT"), text);
    }
    else if (p_ui->IsInteractive()) {
      p_ui->TheGui()->ProgressText(text);
    }
    else if (print) {
      string msg = p_ui->ProgramName() + ": " + text;
      cout << msg << endl;
    }

    ProcessGuiEvents();
  }

  /**
   * Updates the progress bar percent.
   *
   * @param percent The percent of the application that is complete
   *
   * @param print
   */
  void Application::UpdateProgress(int percent, bool print) {
    if (HasParent() && print) {
      string data = iString(percent);
      iApp->SendParentData(string("PROGRESS"), data);
    }
    else if (p_ui->IsInteractive()) {
      p_ui->TheGui()->Progress(percent);
    }
    else if (print) {
      if (percent < 100) {
        cout << percent << "% Processed\r" << flush;
      }
      else {
        cout << percent << "% Processed" << endl;
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
    if (p_ui->IsInteractive()) {
      if (p_ui->TheGui()->ProcessEvents()) {
        throw iException::Message(iException::Cancel, "", _FILEINFO_);
      }
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
    if (curtime != 0) *curtime = startTime;
    struct tm *tmbuf = localtime(&startTime);
    char timestr[80];
    strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
    return(string) timestr;
  }

  /**
   * Returns the user name. Returns 'Unknown' if it cannot find the user name.
   *
   * @return string User Name
   */
  iString Application::UserName() {
    return userName();
  }

  /**
   * Returns the host name.  Returns 'Unknown' if it cannot find the host name.
   *
   * @return string Host Name
   */
  iString Application::HostName() {
    return hostName();
  }

  /**
   * The Isis Version for this application.
   * @return @b iString 
   * 
   */
  iString Application::Version() {
    return isisVersion();
  }


  /**
   * Runs various system specific uname commands and returns the results
   *
   * @return PvlGroup containing uname information
   */
  PvlGroup Application::GetUnameInfo() {
    // Create a temporary file to store console output to
    Filename temp;
    temp.Temporary("$temporary/UnameConsoleInfo", "txt");
    iString tempFile = temp.Expanded();

    // Uname commands output to temp file with each of the following
    // values on its own line in this order:
    // machine hardware name, processor, hardware platform name,
    // operating system, kernel name, kernel version, kernel release, all
    PvlGroup unameGroup("UNAME");
    ifstream readTemp;

#if defined(__linux__)
    // Write uname outputs to temp file
    ProgramLauncher::RunSystemCommand("uname -m > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -p > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -i > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -o > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -s > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -v > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -r > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -a > " + tempFile);
    // Read data from temp file
    char value[256];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("MachineHardware", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("Processor", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("HardwarePlatform", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("OperatingSystem", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("KernelName", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("KernelVersion", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("KernelRelease", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("FullUnameString", value));

#elif defined(__APPLE__)
    // Write uname outputs to temp file
    ProgramLauncher::RunSystemCommand("uname -m > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -p > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -s > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -v > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -r > " + tempFile);
    ProgramLauncher::RunSystemCommand("uname -a > " + tempFile);

    // Read data from temp file
    char value[256];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("MachineHardware", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("Processor", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("OperatingSystem", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("OperatingSystemVersion", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("OperatingSystemRelease", value));
    readTemp.getline(value, 256);
    unameGroup.AddKeyword(PvlKeyword("FullUnameString", value));
#endif

    // remove temp file and return
    remove(tempFile.c_str());
    return unameGroup;
  }

  /**
   * Runs some printenv commands that return Isis related Enviroment Variables.
   *
   * @return PvlGroup containing Enviroment information
   * @todo Replace printenv commands with c library getenv
   * @todo
   */
  PvlGroup Application::GetEnviromentInfo() {
    // Create a temporary file to store console output to
    Filename temp;
    temp.Temporary("$temporary/EnviromentInfo", "txt");
    iString tempFile = temp.Expanded();
    PvlGroup envGroup("EnviromentVariables");
    ifstream readTemp;

    string env1 = "printenv SHELL > " + tempFile;
    string env2 = "printenv HOME >> " + tempFile;
    string env3 = "printenv PWD >> " + tempFile;
    string env5 = "printenv ISISROOT >> " + tempFile;
    string env6 = "printenv ISIS3DATA >> " + tempFile;
    ProgramLauncher::RunSystemCommand(env1);
    ProgramLauncher::RunSystemCommand(env2);
    ProgramLauncher::RunSystemCommand(env3);
    ProgramLauncher::RunSystemCommand(env5);
    ProgramLauncher::RunSystemCommand(env6);
    // Read data from temp file
    char value[511];
    readTemp.open(tempFile.c_str(), ifstream::in);
    readTemp.getline(value, 255);
    envGroup.AddKeyword(PvlKeyword("Shell", value));
    readTemp.getline(value, 255);
    envGroup.AddKeyword(PvlKeyword("Home", value));
    readTemp.getline(value, 255);
    envGroup.AddKeyword(PvlKeyword("Pwd", value));
    readTemp.getline(value, 255);
    envGroup.AddKeyword(PvlKeyword("ISISROOT", value));
    readTemp.getline(value, 255);
    envGroup.AddKeyword(PvlKeyword("ISIS3DATA", value));

    // remove temp file and return
    iString cleanup = "rm -f " + tempFile;
    ProgramLauncher::RunSystemCommand(cleanup);
    return envGroup;
  }

  /**
   * Runs df to see the disk space availability
   *
   * @return iString containing df results
   */
  iString Application::GetSystemDiskSpace() {
    Filename temp;
    temp.Temporary("$temporary/SystemDiskSpace", "txt");
    iString tempFile = temp.Expanded();
    ifstream readTemp;
    string diskspace = "df > " + tempFile;
    ProgramLauncher::RunSystemCommand(diskspace);
    readTemp.open(tempFile.c_str(), ifstream::in);

    iString results = "";
    char tmp[512];
    while (!readTemp.eof()) {
      readTemp.getline(tmp, 512);
      results += tmp;
      results += "\n";
    }

    // remove temp file and return
    iString cleanup = "rm -f " + tempFile;
    ProgramLauncher::RunSystemCommand(cleanup);
    return results;
  }

  /**
   * Runs ldd on linux and sun and otool on macs to get information about the applicaiton run
   *
   * @return iString containing application information
   */
  iString Application::GetLibraryDependencies(iString file) {
    Filename temp;
    temp.Temporary("$temporary/LibraryDependencies", "txt");
    iString tempFile = temp.Expanded();
    ifstream readTemp;
    string dependencies = "";
#if defined(__linux__)
    dependencies = "ldd -v " + file + " > " + tempFile;
#elif defined(__APPLE__)
    dependencies = "otool -L " + file + " > " + tempFile;
#elif defined (__sun__)
    dependencies = "ldd -v " + file + " > " + tempFile;
#endif
    ProgramLauncher::RunSystemCommand(dependencies);
    readTemp.open(tempFile.c_str(), ifstream::in);

    iString results = "";
    char tmp[512];
    while (!readTemp.eof()) {
      readTemp.getline(tmp, 512);
      results += tmp;
      results += "\n";
    }

    // remove temp file and return
    iString cleanup = "rm -f " + tempFile;
    ProgramLauncher::RunSystemCommand(cleanup);
    return results;
  }
}  //end namespace isis

