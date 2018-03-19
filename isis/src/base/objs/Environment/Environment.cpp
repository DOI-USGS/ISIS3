#include "Environment.h"

#include <iostream>
#include <stdlib.h>

#include <QString>
#include <QStringList>
#include <QCoreApplication>

#ifndef __APPLE__
#include <QtDBus>
#endif

#include "IException.h"
#include "IString.h"
#include "TextFile.h"

 /**
  *
  *
  */
  static void QStartup() {
    // Set the Qt plugin directory
    QStringList pluginPaths;

    Isis::IString root = Isis::Environment::getEnvironmentValue("ISISROOT", "");
    QCoreApplication *core = QCoreApplication::instance();
    if ( !core ) {
      std::cout << "****  Qt Plugin Path is not set because no instance of QCoreApplication ****\n";
    }

    return;
  }

// Add this to ensure Qt is set up properly
Q_COREAPP_STARTUP_FUNCTION(QStartup);

namespace Isis {


  Environment Environment::automaticEnvironmentSetup;

  Environment::Environment() {

#if 0
    // Set the Qt plugin directory
    QStringList pluginPaths;

    IString root = getEnvironmentValue("ISISROOT", "");
    std::cout << "ISISROOT=" << root << "\n";

    if (root != "") {

      QCoreApplication *core = QCoreApplication::instance();
      if ( !core ) {
        std::cout << "QCoreApplication doesn't exist yet!\n";
      }
      IString thirdPartyPluginPath = root + "/3rdParty/plugins";
      pluginPaths << thirdPartyPluginPath.ToQt();
      std::cout << "PluginPaths=" << pluginPaths.join(";") << "\n";
      QCoreApplication::setLibraryPaths(pluginPaths);
    }
#endif

#ifndef __APPLE__
    // We need to force the correct QDBus library to be loaded... to do that, just
    //   use a symbol in it's library. This only applies to linux and fixes #1228.

    // Long explanation:
    //   When we run GUI apps, the system (and Qt) work together to figure out
    //   which runtime libraries are necessary on-the-fly. When QApplication is
    //   instantiated, it goes into QtGui's style code. The styles ignore our plugin
    //   path setting (above) on all OS's. So Qt GUI grabs a style from the OS's styles,
    //   which is a shared library in the kde area. These styles require a version (any version)
    //   of QtDBus loaded. If QtDBus is not yet loaded, then the style library will grab it.
    //   However, on Ubuntu 12.04, the style library grabs the system (OS) QDBus library. QDBus
    //   detects that you've already loaded Isis' QtCore, so the library versions mismatch, and
    //   it crashes. The problem becomes more interesting because sometimes it picks up the system
    //   QDBus, and sometimes it picks up Isis' QDBus, and I have no good reason why we pick up
    //   one versus another; currently, installed apps pick up the system and locally built apps
    //   pick up Isis' (even when the executables are made to be identical). The end result is no
    //   installed GUI applications will run and our automated tests fail to catch it. This solution
    //   bypasses the entire issue by forcing QDBus to be loaded long before any styles are loaded,
    //   so the style plugins do not need to go and get their own QDBus library.
    //
    //   The root cause is that Ubuntu's run time loader is failing to respect
    //   our executable's rpaths when loading a style library. However, when we link against the
    //   QBus library directly, we get the right one.
    QDBusArgument();
#endif
  }


  /**
   * @Returns the user name. Returns 'Unknown' if it cannot find the user name.
   */
  QString Environment::userName() {
    return getEnvironmentValue("USER", "Unknown");
  }


  /**
   * @returns the host name.  Returns 'Unknown' if it cannot find the host name.
   */
  QString Environment::hostName() {
    return getEnvironmentValue("HOST", "Unknown");
  }


  /**
   * @param variable The environment variable to get
   * @param defaultValue The returned value for variable if variable doesn't
   *                     exist.
   *
   * @returns The value for the environment variable requested.
   */
  QString Environment::getEnvironmentValue(QString variable,
      QString defaultValue) {

    QString value = defaultValue;

    char *envValue = getenv(variable.toLatin1().data());
    if (envValue)
      value = envValue;

    return value;
  }


  /**
   * @returns the Isis version in the format isis?.?.?.?qualifier | date
   */
  QString Environment::isisVersion() {
    TextFile versionFile("$ISISROOT/version");
    QString line1, line2, line3, line4;
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
