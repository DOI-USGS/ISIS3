#include "Environment.h"

#include <stdlib.h>

#include "iString.h"
#include "TextFile.h"


namespace Isis {

  /**
   * @Returns the user name. Returns 'Unknown' if it cannot find the user name.
   */
  iString Environment::userName() {
    return getEnvironmentValue("USER", "Unknown");
  }
  

  /**
   * @returns the host name.  Returns 'Unknown' if it cannot find the host name.
   */
  iString Environment::hostName() {
    return getEnvironmentValue("HOST", "Unknown");
  }
  
  
  /**
   * @param variable The environment variable to get
   * @param defaultValue The returned value for variable if variable doesn't
   *                     exist.
   *
   * @returns The value for the environment variable requested.
   */
  iString Environment::getEnvironmentValue(iString variable,
      iString defaultValue) {
      
    iString value = defaultValue;
    
    char *envValue = getenv(variable.c_str());
    if (envValue)
      value = envValue;
      
    return value;
  }
  

  /**
   * @returns the Isis version in the format isis?.?.?.? | date
   */
  iString Environment::isisVersion() {
    TextFile versionFile("$ISISROOT/version");
    iString line1, line2;
    versionFile.GetLine(line1);
    versionFile.GetLine(line2);
    iString versionString = line1 + " | " + line2;
    return versionString;
  }
}
