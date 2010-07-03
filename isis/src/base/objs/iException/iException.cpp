/**                                                                       
 * @file                                                                  
 * $Revision: 1.13 $                                                             
 * $Date: 2009/07/29 21:16:39 $                                                                 
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

#include <sstream>

#include "iException.h"
#include "iString.h"
#include "Pvl.h"
#include "Preference.h"
#include "Application.h"
#include "TextFile.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a blank iException. Checks if file and line number should be
   * output, and whether the exception should be reported in PVL format.
   */
  iException::iException () {
    // See if we should output the file and line number  
    Isis::PvlGroup &ef = Isis::Preference::Preferences().FindGroup("ErrorFacility");
    Isis::iString fileline = (std::string) ef["FileLine"];
    p_reportFileLine = (fileline.UpCase() == "ON");

    // Should we report in pvl format??
    p_pvlFormat = false;
    Isis::iString pvlForm = (std::string) ef["Format"];
    p_pvlFormat = (pvlForm.UpCase() == "PVL");
    atexit(Shutdown);
  }

  //! Exception object pointer (Default is NULL)
  iException *iException::p_exception = NULL;
  //! The list of exception information (From Info class)
  QList<iException::Info> iException::p_list;

  /**
   * Adds a message to an existing iException object (or creates a new one if
   * it doesn't exist)
   * @param t An errType value of what type of exception is thrown.
   * @param m The message to add
   * @param f The filename
   * @param l The line number
   * @return The modified exception object.
   */
  iException &iException::Message(iException::errType t, const std::string &m, 
                                  const char *f, int l) {
    if (p_exception == NULL) {
      p_exception = new iException();
    }

    PvlGroup &errPref = Preference::Preferences().FindGroup("ErrorFacility");
    bool printTrace = false;

    if (errPref.HasKeyword("StackTrace")) {
      printTrace = (((iString)errPref["StackTrace"][0]).UpCase() == "ON");
    }

    if (printTrace && p_list.empty()) {
      createStackTrace();
    }

    Info i;
    i.type = t;
    i.message = m;
    i.filename = f;
    i.lineNumber = l;

    p_list.push_back(i);

    p_exception->describe();

    return *p_exception;
  }

  //! Throws and destroys the iException object.
  iException::~iException() throw () {
  }

  /**
   * Stores what happened in a member std::string.
   */
  void iException::describe() {
    if (p_list.size() > 0) {
      std::string message;
      for (int i=p_list.size()-1; i>=0; i--) {
        message += "**" + enumString(p_list[i].type) + "** " + 
                   p_list[i].message;
        if (p_reportFileLine) {
          message += " in " + p_list[i].filename + 
                     " at " + Isis::iString(p_list[i].lineNumber);
        }
        if (i != 0) message += "\n";
      }

      p_what = message;
    }
    else {
      p_what = "";
    }
  }

  /**
   * Returns what happened in output format. This pointer is valid as 
   * long as no new iException objects are created (iException::Message is not 
   * called again). 
   *  
   * @return The message stating what happened and where.
   */
  const char *iException::what() const throw () {
    if(p_what != "") {
      return p_what.c_str();
    }
    else {
      return NULL;
    }
  }

  /**
   * Returns the type of exception.
   * @return The type of exception (None if no type).
   */
  iException::errType iException::Type() const {
    if (p_list.size() > 0) {
      int i = p_list.size() - 1;
      return p_list[i].type;
    }
    else {
      return iException::None;
    }
  }

  /**
   * Returns true or false based on whether the exception should output in PVL
   * format.
   * @return True if PVL format is enabled, false if not.
   */
  bool iException::IsPvlFormat() {
    return p_pvlFormat;
  }

  void iException::Shutdown() {
    if(p_exception) {
      ASSERT_PTR(p_exception);
      delete p_exception;
      p_exception = NULL;
    }
  }

  /**
   * Reports the exception to output. 
   * @param fileinfo Allows the caller to overide the users preference setting 
   *                 for file name and line number where the error was thrown
   * @return The type of exception.
   */
  int iException::Report (bool fileinfo) {
    // Loop and present each message
    bool saveFileLine = p_reportFileLine;
    p_reportFileLine = fileinfo & p_reportFileLine;

    if (IsPvlFormat()) {
      Isis::Pvl errors = PvlErrors();
      std::cerr << errors << std::endl;
    }
    else {
      std::cerr << Errors() << std::endl;
    }

    p_reportFileLine = saveFileLine;
    int type = (int) Type(); // Must get type before clear!!
    Clear();
    return type;
  }

  /**
   * Adds a new PvlGroup called "Error" and appends it to the file (if PVL 
   * format output is enabled)
   */
  Pvl iException::PvlErrors() {
    Isis::Pvl errors;

    for (int i=p_list.size()-1; i>=0; i--) {
      PvlGroup errGroup("Error");

      errGroup += Isis::PvlKeyword ("Program",Isis::Application::Name());
      errGroup += Isis::PvlKeyword ("Class",enumString(p_list[i].type));
      errGroup += Isis::PvlKeyword ("Code",(int)p_list[i].type);
      errGroup += Isis::PvlKeyword ("Message",p_list[i].message);
      errGroup += Isis::PvlKeyword ("File",p_list[i].filename);
      errGroup += Isis::PvlKeyword ("Line",p_list[i].lineNumber);

      errors.AddGroup(errGroup);
    }

    return errors;
  }

  /**
   * Returns the Exception message to be output (non-PVL)
   */
  std::string iException::Errors() {
    std::string message;
    for (int i=p_list.size()-1; i>=0; i--) {
      // Construct the line-based message
      message += "**" + enumString(p_list[i].type) + "** " + 
                 p_list[i].message;
      if (p_reportFileLine) {
        message += " in " + p_list[i].filename + 
                   " at " + Isis::iString(p_list[i].lineNumber);
      }
      if (i != 0) message += "\n";
    }
    return message;
  }

  //! Clears the list of exceptions
  void iException::Clear () {
    p_list.clear();
  }

  /**
   * Returns the type of exception in string format.
   * @return "USER ERROR" if user error type, etc.
   */
  std::string iException::enumString(iException::errType t) const {
    switch (t) {
      case User:
        return "USER ERROR";

      case Programmer:
        return "PROGRAMMER ERROR";

      case Pvl:
        return "PVL ERROR";

      case Io:
        return "I/O ERROR";

      case Camera:
        return "CAMERA ERROR";

      case Projection:
        return "PROJECTION ERROR";

      case Parse:
        return "PARSE ERROR";

      case Math:
        return "MATH ERROR";

      case Spice:
        return "SPICE ERROR";

      case Cancel:
        return "CANCEL";

      case System:
        return "SYSTEM ERROR";

      default:
        return "UNKNOWN ERROR";
    }
  }

  /**
   * Given a string, returns the error type associated with it.
   * @return If the string reads "USER ERROR", will return type User.
   */
  iException::errType iException::enumString(const std::string &s) const {
    if (s == "USER ERROR") return User;
    if (s == "PROGRAMMER ERROR") return Programmer;
    if (s == "PVL ERROR") return Pvl;
    if (s == "I/O ERROR") return Io;
    if (s == "CAMERA ERROR") return Camera;
    if (s == "PROJECTION ERROR") return Projection;
    if (s == "PARSE ERROR") return Parse;
    if (s == "MATH ERROR") return Math;
    if (s == "CANCEL") return Cancel;
    if (s == "SYSTEM ERROR") return System;
    return None;
  }

  void iException::createStackTrace() {
    Info stackTraceInfo;
    stackTraceInfo.type = iException::Programmer;
    stackTraceInfo.filename = "N/A";
    stackTraceInfo.lineNumber = 0;

    std::vector<std::string> theStack;
    StackTrace::GetStackTrace(&theStack);
    stackTraceInfo.message = "\n";

    for(unsigned int i = 1; i < theStack.size(); i++) {
      stackTraceInfo.message += theStack[i] + "\n";
    }

    if(theStack.size() != 0) {
      p_list.push_back(stackTraceInfo);
    }
  }
}

