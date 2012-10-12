#include "Environment.h"

#include <stdlib.h>

#include <QCoreApplication>

#include "IException.h"
#include "IString.h"
#include "TextFile.h"


namespace Isis {
  Environment Environment::automaticEnvironmentSetup;

  Environment::Environment() {
    // Set the Qt plugin directory
    QStringList pluginPaths;

    IString root = getEnvironmentValue("ISISROOT", "");

    if (root != "") {
      IString thirdPartyPluginPath = root + "/3rdParty/plugins";
      pluginPaths << thirdPartyPluginPath.ToQt();
      QCoreApplication::setLibraryPaths(pluginPaths);
    }
  }


  /**
   * @Returns the user name. Returns 'Unknown' if it cannot find the user name.
   */
  IString Environment::userName() {
    return getEnvironmentValue("USER", "Unknown");
  }
  

  /**
   * @returns the host name.  Returns 'Unknown' if it cannot find the host name.
   */
  IString Environment::hostName() {
    return getEnvironmentValue("HOST", "Unknown");
  }
  
  
  /**
   * @param variable The environment variable to get
   * @param defaultValue The returned value for variable if variable doesn't
   *                     exist.
   *
   * @returns The value for the environment variable requested.
   */
  IString Environment::getEnvironmentValue(IString variable,
      IString defaultValue) {
      
    IString value = defaultValue;
    
    char *envValue = getenv(variable.c_str());
    if (envValue)
      value = envValue;
      
    return value;
  }
  

  /**
   * @returns the Isis version in the format isis?.?.?.?qualifier | date
   */
  IString Environment::isisVersion() {
    TextFile versionFile("$ISISROOT/version");
    IString line1, line2, line3, line4;
    versionFile.GetLine(line1);
    versionFile.GetLine(line2);
    versionFile.GetLine(line3);
    versionFile.GetLine(line4);

    QRegExp validPartOfLine("[^ #]*");
    if (validPartOfLine.indexIn(line1) != -1) {
      line1 = validPartOfLine.cap();
    }
    else {
      IString msg = "$ISISROOT/version line 1, no valid text found";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (validPartOfLine.indexIn(line2) != -1) {
      line2 = validPartOfLine.cap();
    }
    else {
      IString msg = "$ISISROOT/version line 2, no valid text found";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (validPartOfLine.indexIn(line4) != -1) {
      line4 = validPartOfLine.cap();
    }
    else {
      IString msg = "$ISISROOT/version line 4, no valid text found";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return line1 + " " + line4 + " | " + line2;
  }
}
