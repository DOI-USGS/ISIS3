/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/03/16 18:45:00 $
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
#include "Plugin.h"

#include <ostream>

#include <QCoreApplication>
#include <QLibrary>
#include <QStringList>

#include "FileName.h"
#include "IException.h"

using namespace std;

namespace Isis {
  //! Constructs a Plugin object.
  Plugin::Plugin() : Pvl() {
  }

  /**
   * This method returns a void pointer to a C function (i.e., the plugin) It
   * does this by looking in itself (Pvl) for the keyword LIBRARY and the
   * keyword ROUTINE.  When you write this function make sure to place extern
   * "C" infront of it to eliminate C++ symbol mangling.
   *
   * @param group The group name.
   *
   * @return A void pointer to a C function (i.e., the plugin)
   *
   * @history 2010-03-16  Tracie Sucharski,  Added paths to plugin.  First
   *                            try to load from current working directory,
   *                            then from $ISISROOT/lib.
   */
  QFunctionPointer Plugin::GetPlugin(const QString &group) {
    // Get the library and plugin to load
    PvlGroup &g = findGroup(group);
    QString library = g["Library"];

    QString path = "./";
    Isis::FileName libraryFile(path + library);

    QString pluginName = g["Routine"];

    // Open the library, resolve the routine name, and return the function
    // address. The function will stay in memory until the application exists
    // so the scope of lib does not matter.
    QLibrary lib(libraryFile.expanded());
    bool loadedOk = lib.load();

    if(!loadedOk) {
      path = "$ISISROOT/lib/";
      libraryFile = path + library;
    }

    lib.setFileName(libraryFile.expanded());

    QFunctionPointer plugin = lib.resolve(pluginName.toLatin1().data());
    if (plugin == 0) {
      QString msg = "Unable to find plugin [" + pluginName +
                    "] in shared library [" + lib.fileName() + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return plugin;
  }
} // end namespace isis
