/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/05/28 17:55:36 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "UserInterface.h"

#include <sstream>
#include <vector>

#include "Application.h"
#include "iException.h"
#include "Filename.h"
#include "iString.h"
#include "Message.h"
#include "Gui.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "TextFile.h"

using namespace std;
namespace Isis {

  /**
   * Constructs an UserInterface object.
   *
   * @param xmlfile Name of the Isis application xml file to open.
   *
   * @param argc Number of arguments on the command line.  Must be pass by
   *             reference!!
   *
   * @param argv[] Array of arguments
   */
  UserInterface::UserInterface(const std::string &xmlfile, int &argc,
                               char *argv[]) :
    IsisAml::IsisAml(xmlfile) {
    p_interactive = false;
    p_info = false;
    p_infoFileName = "";
    p_gui = NULL;
    p_errList = "";
    p_saveFile = "";
    p_abortOnError = true;
    p_parentId = 0;

    // Make sure the user has a .Isis and .Isis/history directory
    try {
      Isis::Filename setup("$HOME/.Isis");
      if(!setup.Exists()) {
        setup.MakeDirectory();
      }

      setup = "$HOME/.Isis/history";
      if(!setup.Exists()) {
        setup.MakeDirectory();
      }
    }
    catch(iException &e) {
      e.Clear();
    }

    // Parse the user input
    LoadCommandLine(argc, argv);

    // See if we need to create the gui
    if(p_interactive) {
      p_gui = Gui::Create(*this, argc, argv);
    }
  }

  //! Destroys the UserInterface object
  UserInterface::~UserInterface() {
    if(p_gui) {
      delete p_gui;
      p_gui = NULL;
    }
  }

  /**
   * This is used to load the command line into p_cmdline and the Aml object
   * using information contained in argc and argv.
   *
   * @param argc Number of arguments on the command line
   *
   * @param argv[] Array of arguments
   *
   * @throws Isis::iException::User - Invalid command line
   * @throws Isis::iException::System - -GUI and -PID are incompatible arguments
   * @throws Isis::iException::System - -BATCHLIST & -GUI are incompatible
   *                                    arguments
   */
  void UserInterface::LoadCommandLine(int argc, char *argv[]) {
    // The program will be interactive if it has no arguments or
    // if it has the name unitTest
    p_progName = argv[0];
    Isis::Filename file(p_progName);
    if((argc == 1) && (file.Name() != "unitTest")) {
      p_interactive = true;
    }

    p_cmdline.clear();
    for(int i = 0; i < argc; i ++) {
      p_cmdline.push_back(argv[i]);
    }

    // Check for special tokens (those beginning with a dash)
    vector< string > options;
    options.push_back("-GUI");
    options.push_back("-NOGUI");
    options.push_back("-BATCHLIST");
    options.push_back("-LAST");
    options.push_back("-RESTORE");
    options.push_back("-WEBHELP");
    options.push_back("-HELP");
    options.push_back("-ERRLIST");
    options.push_back("-ONERROR");
    options.push_back("-SAVE");
    options.push_back("-INFO");
    options.push_back("-PREFERENCE");
    options.push_back("-LOG");
    options.push_back("-VERBOSE");
    options.push_back("-PID");

    bool usedDashLast = false;

    // Use an initial pass to find conflicting parameters
    for(unsigned int currArgument = 1; currArgument < (unsigned) argc; currArgument ++) {
      string paramName;
      vector< string > paramValue;

      GetNextParameter(currArgument, paramName, paramValue);

      paramName = ((iString) paramName).UpCase();

      // -LAST needs to be evaluated first to prevent incorrect errors from IsisAml
      if(paramName == "-LAST") {
        EvaluateOption(paramName, "");
        break;
      }
    }

    for(unsigned int currArgument = 1; currArgument < (unsigned) argc; currArgument ++) {
      string paramName;
      vector< string > paramValue;

      GetNextParameter(currArgument, paramName, paramValue);

      // we now have a name,value pair
      if(paramName[0] == '-') {
        paramName = ((iString) paramName).UpCase();

        // Prevent double handling of conflicting parameters handled in first pass
        if(paramName == "-LAST") {
          continue;
        }

        if(paramValue.size() > 1) {
          string msg = "Invalid value for reserve parameter ["
                       + paramName + "]";
          throw iException::Message(iException::User, msg,
                                    _FILEINFO_);
        }

        // We have an option (not a parameter)...
        int matchOption = -1;

        for(int option = 0; option < (int) options.size(); option ++) {
          // If our option starts with the parameter name so far, this is it
          if(options[option].find(paramName) == 0) {
            if(matchOption >= 0) {
              string msg = "Ambiguous Reserve Parameter ["
                           + paramName + "]. Please clarify.";
              throw iException::Message(iException::User, msg,
                                        _FILEINFO_);
            }

            matchOption = option;
          }
        }

        if(matchOption < 0) {
          string msg = "Invalid Reserve Parameter Option ["
                       + paramName + "]. Choices are ";

          iString msgOptions;
          for(int option = 0; option < (int) options.size() - 1; option ++) {
            // Make sure not to show -PID as an option
            if(options[option].compare("-PID") != 0) {
              continue;
            }

            if(!options.empty()) {
              msgOptions += ",";
            }

            msgOptions += options[option];
          }

          msg += " [" + msgOptions + "]";

          throw iException::Message(iException::User, msg,
                                    _FILEINFO_);
        }

        paramName = options[matchOption];

        if(paramName == "-LAST") {
          usedDashLast = true;
        }

        iString realValue = "";

        if(paramValue.size())
          realValue = paramValue[0];

        EvaluateOption(paramName, realValue);
        continue;
      }

      try {
        Clear(paramName);
        PutAsString(paramName, paramValue);
      }
      catch(Isis::iException &e) {
        throw Isis::iException::Message(Isis::iException::User,
                                        "Invalid command line", _FILEINFO_);
      }
    }

    // Can't use the batchlist with the gui, save, last or restore option
    if(BatchListSize() != 0 && (p_interactive || usedDashLast
                                || p_saveFile != "")) {
      string msg =
        "-BATCHLIST cannot be used with -GUI, -SAVE, -RESTORE, ";
      msg += "or -LAST";
      throw Isis::iException::Message(Isis::iException::System, msg,
                                      _FILEINFO_);
    }

    // Must use batchlist if using errorlist or onerror=continue
    if((BatchListSize() == 0) && (!p_abortOnError || p_errList != "")) {
      string msg =
        "-ERRLIST and -ONERROR=continue cannot be used without ";
      msg += " the -BATCHLIST option";
      throw Isis::iException::Message(Isis::iException::System, msg,
                                      _FILEINFO_);
    }
  }

  /**
   * This gets the next parameter in the list of arguments. curPos will be changed
   * to be the end of the current argument (still needs incremented to get the
   * next argument).
   *
   * @param curPos End of previous argument
   * @param name Resulting parameter name
   * @param value Resulting array of parameter values (usually just 1 element)
   */
  void UserInterface::GetNextParameter(unsigned int &curPos,
                                       std::string &name, std::vector< std::string > &value) {
    iString paramName = p_cmdline[curPos];
    iString paramValue = "";

    // we need to split name and value, they can either be in 1, 2 or 3 arguments,
    //   try to see if "=" is end of argument to distinguish. Some options have no
    //   "=" and they are value-less (-gui for example).
    if(paramName.find("=") == string::npos) {
      // This looks value-less, but lets make sure
      //   the next argument is not an equals sign by
      //   itself
      if(curPos < p_cmdline.size() - 2) {
        if(string(p_cmdline[curPos + 1]).compare("=") == 0) {
          paramValue = p_cmdline[curPos + 2];

          // increment extra to skip 2 elements next time around
          curPos += 2;
        }
      }
    }
    // = is end of parameter, next item must be value
    else if(paramName.find("=") == paramName.size() - 1) {
      paramName = paramName.substr(0, paramName.size() - 1);

      if(curPos + 1 < p_cmdline.size()) {
        paramValue = p_cmdline[curPos + 1];
      }

      // increment extra to skip next element next time around
      curPos ++ ;
    }
    // we found "=" in the middle
    else if(paramName.find("=") != 0) {
      string parameterLiteral = p_cmdline[curPos];
      paramName
      = parameterLiteral.substr(0, parameterLiteral.find("="));
      paramValue = parameterLiteral.substr(parameterLiteral.find("=")
                                           + 1);
    }
    // We found "=" at the beginning - did we find "appname param =value" ?
    else {
      // parameters can not start with "="
      string msg = "Unknown parameter [" + string(p_cmdline[curPos])
                   + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    name = paramName;
    value.clear();

    // read arrays out of paramValue
    paramValue = paramValue.Trim(" ");

    if(paramValue.length() > 0 && paramValue[0] != '(') {
      // We dont have an array... if they escaped
      //  an open paren, undo their escape

      // escape: \( result: (
      if(paramValue.length() > 1 && paramValue.substr(0, 2).compare(
            "\\(") == 0) {
        paramValue = paramValue.substr(1);
      }
      // escape: \\( result: \(
      else if(paramValue.length() > 2
              && paramValue.substr(0, 4).compare("\\\\(") == 0) {
        paramValue = paramValue.substr(1);
      }

      value.push_back(paramValue);
    }
    else if(paramValue.length()) {
      // We have an array...
      value = ReadArray(paramValue);
    }
  }

  /**
   * This interprets an array value from the command line.
   *
   * @param arrayString Parameter value containing an array of
   *   format (a,b,c)
   *
   * @return std::vector<std::string> Values in the array string
   */
  std::vector< std::string > UserInterface::ReadArray(iString arrayString) {
    std::vector< std::string > values;

    bool inDoubleQuotes = false;
    bool inSingleQuotes = false;
    bool arrayClosed = false;
    bool nextElementStarted = false;
    iString currElement = "";

    for(unsigned int strPos = 0; strPos < arrayString.size(); strPos ++) {
      if(strPos == 0) {
        if(arrayString[strPos] != '(') {
          string msg = "Invalid array format [" + arrayString + "]";
          throw iException::Message(iException::User, msg,
                                    _FILEINFO_);
        }

        continue;
      }

      // take literally anything that is escaped and not quoted
      if(arrayString[strPos] == '\\' && strPos + 1 < arrayString.size()) {
        currElement += arrayString[strPos+1];
        strPos ++;
        continue;
      }
      // ends in a backslash??
      else if(arrayString[strPos] == '\\') {
        string msg = "Invalid array format [" + arrayString + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      // not in quoted part of string
      if(!inDoubleQuotes && !inSingleQuotes) {
        if(arrayClosed) {
          string msg = "Invalid array format [" + arrayString + "]";
          throw iException::Message(iException::User, msg,
                                    _FILEINFO_);
        }

        nextElementStarted = (nextElementStarted || arrayString[strPos] != ' ');

        if(!nextElementStarted) {
          continue;
        }

        if(arrayString[strPos] == '"') {
          inDoubleQuotes = true;
        }
        else if(arrayString[strPos] == '\'') {
          inSingleQuotes = true;
        }
        else if(arrayString[strPos] == ',') {
          values.push_back(currElement);
          currElement = "";
          nextElementStarted = false;
        }
        else if(arrayString[strPos] == ')') {
          values.push_back(currElement);
          currElement = "";
          arrayClosed = true;
          nextElementStarted = false;
        }
        else if(nextElementStarted && arrayString[strPos] == ' ') {
          // Make sure there's something before the next ',' or ')'
          bool onlyWhite = true;
          int closingPos = strPos + 1;

          for(unsigned int pos = strPos;
              onlyWhite && arrayString[pos] != ',' && arrayString[pos] != ')' &&
              pos < arrayString.size(); pos++) {
            closingPos ++;
            onlyWhite &= (arrayString[pos] == ' ');
          }

          if(!onlyWhite) {
            currElement += arrayString[strPos];
          }
        }
        else if(nextElementStarted) {
          currElement += arrayString[strPos];
        }
      }
      else if(inSingleQuotes) {
        if(arrayString[strPos] == '\'') {
          inSingleQuotes = false;
        }
        else {
          currElement += arrayString[strPos];
        }
      }
      // in double quotes
      else {
        if(arrayString[strPos] == '"') {
          inDoubleQuotes = false;
        }
        else {
          currElement += arrayString[strPos];
        }
      }
    }


    if(!arrayClosed || currElement != "") {
      string msg = "Invalid array format [" + arrayString + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    return values;
  }

  /**
   * This interprets the "-" options for reserved parameters
   *
   * @param name "-OPTIONNAME"
   * @param value Value of the option, if supplied (-name=value)
   */
  void UserInterface::EvaluateOption(const std::string name,
                                     const std::string value) {
    Preference &p = Preference::Preferences();

    if(name == "-GUI") {
      p_interactive = true;
    }
    else if(name == "-NOGUI") {
      p_interactive = false;
    }
    else if(name == "-BATCHLIST") {
      LoadBatchList(value);
    }
    else if(name == "-LAST") {
      PvlGroup &grp = p.FindGroup("UserInterface", Isis::Pvl::Traverse);
      iString histFile = grp["HistoryPath"][0] + "/" + Filename(
                           p_progName).Name() + ".par";

      LoadHistory(histFile);
    }
    else if(name == "-RESTORE") {
      LoadHistory(value);
    }
    else if(name == "-WEBHELP") {
      Isis::PvlGroup &pref = Isis::Preference::Preferences().FindGroup(
                               "UserInterface");
      string command = pref["GuiHelpBrowser"];
      command += " $ISISROOT/doc/Application/presentation/Tabbed/";
      command += Filename(p_progName).Name() + "/" + Filename(
                   p_progName).Name() + ".html";
      ProgramLauncher::RunSystemCommand(command);
      exit(0);
    }
    else if(name == "-INFO") {
      p_info = true;

      // check for filename and set value
      if(value.size() != 0) {
        p_infoFileName = value;
      }
    }
    else if(name == "-HELP") {
      if(value.size() == 0) {
        Pvl params;
        params.SetTerminator("");
        for(int k = 0; k < NumGroups(); k ++) {
          for(int j = 0; j < NumParams(k); j ++) {
            if(ParamListSize(k, j) == 0) {
              params += PvlKeyword(ParamName(k, j),
                                   ParamDefault(k, j));
            }
            else {
              PvlKeyword key(ParamName(k, j));
              string def = ParamDefault(k, j);
              for(int l = 0; l < ParamListSize(k, j); l ++) {
                if(ParamListValue(k, j, l) == def)
                  key.AddValue("*" + def);
                else
                  key.AddValue(ParamListValue(k, j, l));
              }
              params += key;
            }
          }
        }
        cout << params;
      }
      else {
        Pvl param;
        param.SetTerminator("");
        string key = value;
        for(int k = 0; k < NumGroups(); k ++) {
          for(int j = 0; j < NumParams(k); j ++) {
            if(ParamName(k, j) == key) {
              param += PvlKeyword("ParameterName", key);
              param += PvlKeyword("Brief", ParamBrief(k, j));
              param += PvlKeyword("Type", ParamType(k, j));
              if(PixelType(k, j) != "") {
                param += PvlKeyword("PixelType", PixelType(k,
                                    j));
              }
              if(ParamInternalDefault(k, j) != "") {
                param += PvlKeyword("InternalDefault",
                                    ParamInternalDefault(k, j));
              }
              else
                param += PvlKeyword("Default", ParamDefault(
                                      k, j));
              if(ParamMinimum(k, j) != "") {
                if(ParamMinimumInclusive(k, j) == "YES") {
                  param += PvlKeyword("GreaterThanOrEqual",
                                      ParamMinimum(k, j));
                }
                else {
                  param += PvlKeyword("GreaterThan",
                                      ParamMinimum(k, j));
                }
              }
              if(ParamMaximum(k, j) != "") {
                if(ParamMaximumInclusive(k, j) == "YES") {
                  param += PvlKeyword("LessThanOrEqual",
                                      ParamMaximum(k, j));
                }
                else {
                  param += PvlKeyword("LessThan",
                                      ParamMaximum(k, j));
                }
              }
              if(ParamLessThanSize(k, j) > 0) {
                PvlKeyword key("LessThan");
                for(int l = 0; l < ParamLessThanSize(k, j); l ++) {
                  key.AddValue(ParamLessThan(k, j, l));
                }
                param += key;
              }
              if(ParamLessThanOrEqualSize(k, j) > 0) {
                PvlKeyword key("LessThanOrEqual");
                for(int l = 0; l < ParamLessThanOrEqualSize(
                      k, j); l ++) {
                  key.AddValue(
                    ParamLessThanOrEqual(k, j, l));
                }
                param += key;
              }
              if(ParamNotEqualSize(k, j) > 0) {
                PvlKeyword key("NotEqual");
                for(int l = 0; l < ParamNotEqualSize(k, j); l ++) {
                  key.AddValue(ParamNotEqual(k, j, l));
                }
                param += key;
              }
              if(ParamGreaterThanSize(k, j) > 0) {
                PvlKeyword key("GreaterThan");
                for(int l = 0; l
                    < ParamGreaterThanSize(k, j); l ++) {
                  key.AddValue(ParamGreaterThan(k, j, l));
                }
                param += key;
              }
              if(ParamGreaterThanOrEqualSize(k, j) > 0) {
                PvlKeyword key("GreaterThanOrEqual");
                for(int l = 0; l
                    < ParamGreaterThanOrEqualSize(k, j); l ++) {
                  key.AddValue(ParamGreaterThanOrEqual(k,
                                                       j, l));
                }
                param += key;
              }
              if(ParamIncludeSize(k, j) > 0) {
                PvlKeyword key("Inclusions");
                for(int l = 0; l < ParamIncludeSize(k, j); l ++) {
                  key.AddValue(ParamInclude(k, j, l));
                }
                param += key;
              }
              if(ParamExcludeSize(k, j) > 0) {
                PvlKeyword key("Exclusions");
                for(int l = 0; l < ParamExcludeSize(k, j); l ++) {
                  key.AddValue(ParamExclude(k, j, l));
                }
                param += key;
              }
              if(ParamOdd(k, j) != "") {
                param += PvlKeyword("Odd", ParamOdd(k, j));
              }
              if(ParamListSize(k, j) != 0) {
                for(int l = 0; l < ParamListSize(k, j); l ++) {
                  PvlGroup grp(ParamListValue(k, j, l));
                  grp += PvlKeyword("Brief", ParamListBrief(
                                      k, j, l));
                  if(ParamListIncludeSize(k, j, l) != 0) {
                    PvlKeyword include("Inclusions");
                    for(int m = 0; m
                        < ParamListIncludeSize(k, j, l); m ++) {
                      include.AddValue(ParamListInclude(
                                         k, j, l, m));
                    }
                    grp += include;
                  }
                  if(ParamListExcludeSize(k, j, l) != 0) {
                    PvlKeyword exclude("Exclusions");
                    for(int m = 0; m
                        < ParamListExcludeSize(k, j, l); m ++) {
                      exclude.AddValue(ParamListExclude(
                                         k, j, l, m));
                    }
                    grp += exclude;
                  }
                  param.AddGroup(grp);
                }
              }
              cout << param;
            }
          }
        }
      }
      exit(0);
    }
    else if(name == "-PID") {
      p_parentId = iString(value).ToInteger();
    }
    else if(name == "-ERRLIST") {
      p_errList = value;

      if(value == "") {
        string msg = "-ERRLIST expects a file name";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      if(Filename(p_errList).Exists()) {
        remove(p_errList.c_str());
      }
    }
    else if(name == "-ONERROR") {
      if(iString(value).UpCase() == "CONTINUE") {
        p_abortOnError = false;
      }

      else if(iString(value).UpCase() == "ABORT") {
        p_abortOnError = true;
      }

      else {
        string
        msg =
          "[" + value
          + "] is an invalid value for -ONERROR, options are ABORT or CONTINUE";
        throw Isis::iException::Message(Isis::iException::User, msg,
                                        _FILEINFO_);
      }
    }
    else if(name == "-SAVE") {
      if(value.size() == 0) {
        p_saveFile = ProgramName() + ".par";
      }
      else {
        p_saveFile = value;
      }
    }
    else if(name == "-PREFERENCE") {
      p.Load(value);
    }
    else if(name == "-LOG") {
      if(value.empty()) {
        p.FindGroup("SessionLog")["FileOutput"].SetValue("On");
      }
      else {
        p.FindGroup("SessionLog")["FileOutput"].SetValue("On");
        p.FindGroup("SessionLog")["FileName"].SetValue(value);
      }
    }
    else if(name == "-VERBOSE") {
      p.FindGroup("SessionLog")["TerminalOutput"].SetValue("On");
    }

    // Can't have a parent id and the gui
    if(p_parentId > 0 && p_interactive) {
      string msg = "-GUI and -PID are incompatible arguments";
      throw Isis::iException::Message(Isis::iException::System, msg,
                                      _FILEINFO_);
    }
  }

  /**
   * Loads the user entered batchlist file into a private variable for later use
   *
   * @param file The batchlist file to load
   *
   *  @history 2010-03-26 Sharmila Prasad - Remove the restriction of the number of
   *           columns in the batchlist file to 10.
   * @throws Isis::iException::User - The batchlist does not contain any data
   */
  void UserInterface::LoadBatchList(const std::string file) {
    // Read in the batch list
    TextFile temp;
    try {
      temp.Open(file);
    }
    catch(iException &e) {
      string msg = "The batchlist file [" + file
                   + "] could not be opened";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    p_batchList.resize(temp.LineCount());

    for(int i = 0; i < temp.LineCount(); i ++) {
      iString t;
      temp.GetLine(t);

      // Convert tabs to spaces but leave tabs inside quotes alone
      t.Replace("\t", " ", true);

      t.Compress();
      t.Trim(" ");
      // Allow " ," " , " or ", " as a valid single seperator
      t.Replace(" ,", ",", true);
      t.Replace(", ", ",", true);
      // Convert all spaces to "," the use "," as delimiter
      t.Replace(" ", ",", true);
      int j = 0;
      iString token = t.Token(",");

      while(token != "") {
        // removes quotes from tokens. NOTE: also removes escaped quotes.
        token = token.Remove("\"'");
        p_batchList[i].push_back(token);
        token = t.Token(",");
        j ++ ;
      }
      p_batchList[i].resize(j);
      // Every row in the batchlist must have the same number of columns
      if(i == 0)
        continue;
      if(p_batchList[i - 1].size() != p_batchList[i].size()) {
        string msg =
          "The number of columns must be constant in batchlist";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    // The batchlist cannot be empty
    if(p_batchList.size() < 1) {
      string msg = "The list file [" + file
                   + "] does not contain any data";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  /**
   * Loads the previous history for the program
   *
   * @param file Filename to get the history entry from
   *
   * @throws Isis::iException::User - The file does not contain any parameters
   *                                  to restore
   * @throws Isis::iException::User - The file does not contain a valid parameter
   *                                  history file
   * @throws Isis::iException::User - Parameter history file does not exist
   */
  void UserInterface::LoadHistory(const std::string file) {
    Isis::Filename hist(file);
    if(hist.Exists()) {
      try {
        Isis::Pvl lab(hist.Expanded());

        int g = lab.Groups() - 1;
        if(g >= 0 && lab.Group(g).IsNamed("UserParameters")) {
          Isis::PvlGroup &up = lab.Group(g);
          for(int k = 0; k < up.Keywords(); k ++) {
            string keyword = up[k].Name();

            vector<string> values;

            for(int i = 0; i < up[k].Size(); i++) {
              values.push_back(up[k][i]);
            }

            PutAsString(keyword, values);
          }
          return;
        }

        for(int o = lab.Objects() - 1; o >= 0; o --) {
          if(lab.Object(o).IsNamed(ProgramName())) {
            Isis::PvlObject &obj = lab.Object(o);
            for(int g = obj.Groups() - 1; g >= 0; g --) {
              Isis::PvlGroup &up = obj.Group(g);
              if(up.IsNamed("UserParameters")) {
                for(int k = 0; k < up.Keywords(); k ++) {
                  string keyword = up[k].Name();
                  string value = up[k][0];
                  PutAsString(keyword, value);
                }
              }
              return;
            }
          }
        }

        /*string msg = "[" + hist.Expanded() +
         "] does not contain any parameters to restore";
         throw Isis::iException::Message( Isis::iException::User, msg, _FILEINFO_ );*/
      }
      catch(...) {
        string msg = "The history file [" + file
                     + "] is corrupt, please fix or delete this file";
        throw Isis::iException::Message(Isis::iException::User, msg,
                                        _FILEINFO_);
      }
    }
    else {
      string msg = "The history file [" + file + "] does not exist";
      throw Isis::iException::Message(Isis::iException::User, msg,
                                      _FILEINFO_);
    }
  }

  /**
   * Saves the user parameter information in the history of the program for later
   * use
   */
  void UserInterface::SaveHistory() {

    // If history recording is off, return
    Preference &p = Preference::Preferences();
    PvlGroup &grp = p.FindGroup("UserInterface", Isis::Pvl::Traverse);
    if(grp["HistoryRecording"][0] == "Off")
      return;

    // Get the current history file
    Isis::Filename histFile(grp["HistoryPath"][0] + "/" + ProgramName()
                            + ".par");

    // If a save file is specified, override the default file path
    if(p_saveFile != "")
      histFile = p_saveFile;

    // Get the current command line
    Isis::Pvl cmdLine;
    CommandLine(cmdLine);

    Isis::Pvl hist;

    // If the history file's Pvl is corrupted, then
    // leave hist empty such that the history gets
    // overwriten with the new entry.
    try {
      if(histFile.Exists()) {
        hist.Read(histFile.Expanded());
      }
    }
    catch(iException e) {
      e.Clear();
    }

    // Add it
    hist.AddGroup(cmdLine.FindGroup("UserParameters"));

    // See if we have exceeded history length
    while(hist.Groups() > (int) grp["HistoryLength"][0]) {
      hist.DeleteGroup("UserParameters");
    }

    // Write it
    try {
      hist.Write(histFile.Expanded());
    }
    catch(iException &e) {
      e.Clear();
    }

  }

  /**
   * Clears the gui parameters and sets the batch list information at line i as
   * the new parameters
   *
   * @param i The line number to retrieve parameter information from
   */
  void UserInterface::SetBatchList(int i) {
    //Clear all parameters currently in the gui
    for(int k = 0; k < NumGroups(); k ++) {
      for(int j = 0; j < NumParams(k); j ++) {
        Clear(ParamName(k, j));
      }
    }

    //Load the new parameters into the gui
    cout << p_progName << " ";

    for(unsigned int currArgument = 1; currArgument < p_cmdline.size(); currArgument ++) {
      string paramName;
      vector< string > paramValue;

      try {
        GetNextParameter(currArgument, paramName, paramValue);

        if(paramName[0] == '-')
          continue;

        for(unsigned int value = 0; value < paramValue.size(); value ++) {
          iString thisValue = paramValue[value];

          string token = thisValue.Token("$");
          iString newValue;

          while(thisValue != "") {
            newValue += token;
            int j = iString(thisValue.substr(0, 1)).ToInteger()
                    - 1;
            newValue += p_batchList[i][j];
            thisValue.replace(0, 1, "");
            token = thisValue.Token("$");
          }

          if(token != "")
            newValue += token;

          paramValue[value] = newValue;
        }
      }
      catch(Isis::iException &e) {
        throw Isis::iException::Message(Isis::iException::User,
                                        "Invalid command line", _FILEINFO_);
      }

      PutAsString(paramName, paramValue);

      cout << paramName;

      if(paramValue.size() == 1) {
        cout << "=" << paramValue[0] << " ";
      }
      else if(paramValue.size() > 1) {
        cout << "=(";

        for(unsigned int value = 0; value < paramValue.size(); value ++) {
          if(value != 0)
            cout << ",";

          cout << paramValue[value] << endl;
        }

        cout << ") ";
      }
    }
    cout << endl;

    // Verify the command line
    VerifyAll();
  }

  /**
   * This method adds the line specified in the BatchList that the error occured
   * on.  The BatchList line is added exactly as it is seen, so the BatchList
   * command can be run on the errorlist file created.
   *
   * @param i The line of the batchlist to write to the error file
   */
  void UserInterface::SetErrorList(int i) {
    if(p_errList != "") {
      std::ofstream os;
      string fileName(Filename(p_errList).Expanded());
      os.open(fileName.c_str(), std::ios::app);

      if(!os.good()) {
        string
        msg =
          "Unable to create error list [" + p_errList
          + "] Disk may be full or directory permissions not writeable";
        throw Isis::iException::Message(Isis::iException::User, msg,
                                        _FILEINFO_);
      }

      for(int j = 0; j < (int) p_batchList[i].size(); j ++) {
        os << p_batchList[i][j] << " ";
      }

      os << endl;
      os.close();
    }
  }

  /**
   * This method returns the flag state of info. This returns if
   * its in debugging mode(the -info tag was specified).
   */
  bool UserInterface::GetInfoFlag() {
    return p_info;
  }

  /**
   * This method returns the filename where the debugging info is
   * stored when the "-info" tag is used.
   */
  std::string UserInterface::GetInfoFileName() {
    return p_infoFileName;
  }
} // end namespace isis
