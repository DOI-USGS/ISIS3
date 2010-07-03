/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/01/07 18:33:38 $
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
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Preference.h"
#include "iString.h"
#include "iException.h"
#include "Filename.h"

using namespace std;
namespace Isis {

  //! Constructs an empty Filename object.
  Filename::Filename () {
    p_original.clear();
  }

/**
 * Constructs a Filename object and expands environment variables and
 * Preferences from the DataDirectory group embedded in the filename.
 *
 * @param file A string containing a filename.
 */
  Filename::Filename (const std::string &file)
      : QFileInfo () {
    p_original = file;
    QFileInfo::setFile ((iString)Expand (file));
  }

/**
 * Creates a temporary filename from the arguments. The new filename is checked
 * to make sure it doesn't exist, but race conditions could still occur between
 * the time the file is checked for existance and the application attempts to
 * create the file.
 *
 * @param name Name of the temporary file.
 *
 * @param extension Extension of the temporary file.
 */
  Filename::Filename (const std::string &name, const std::string &extension) {
    Temporary (name, extension);
  }

  //! Destroys the Filename object.
  Filename::~Filename () {}

/**
 * Clears the current contents of the Filename object and reinitializes it with
 * the argument.
 *
 * @param file File name to replace the current contents of the object.
 */
  void Filename::operator=(const std::string &file) {
    p_original = file;
    QFileInfo::setFile ((iString)Expand (file));
  }

/**
 * Clears the current contents of the Filename object and reinitializes it with
 * the argument.
 *
 * @param file File name to replace the current contents of the object.
 */
  void Filename::operator=(const char *file) {
    p_original = file;
    QFileInfo::setFile ((iString)Expand (file));
  }

/**
 * Returns the path
 * Returns the path portion of a filename. For *nix operating
 * systems this includes everything upto but not including the
 * last slash "/". For file names created without any slashes
 * the current working directory will be returned.
 * <pre>
 *   for a full file specification of:
 *   "/home/me/img/picture.jpg"
 *   Path() gives:
 *   "/home/me/img"
 * </pre>
 */
  std::string Filename::Path() const {
    return QFileInfo::absolutePath().toStdString();
  }

/**
 * Returns the basename
 * Returns the file name only. This excludes any path and the last extension.
 * For *nix operating systems this includes everything following the last slash
 * "/" and upto the last dot ".". If a file name contains multiple extensions,
 * all but the last one will be returned. If a file name has no extension,
 * Basename returns the same as Name
 */
  std::string Filename::Basename() const {
    if (QFileInfo::suffix().length() == 0) {
      return QFileInfo::baseName().toStdString();
    }
    else {
      return QFileInfo::completeBaseName().toStdString();
    }
  }

/**
 * Returns the filename.
 * Returns the file name and all extensions of the filename. For
 * *nix operating systems this includes everything following the
 * last slash "/".
 */
  std::string Filename::Name() const {
    return QFileInfo::fileName().toStdString();
  }

/**
 * Returns the extension (Does not include .)
 * Returns the extension of the filename. If multiple extensions
 * exist, then only the last one will be returned.
 */
  std::string Filename::Extension () const {
    return QFileInfo::suffix().toStdString();
  }

/**
 * Returns the full filename (path, basename and extension(s))
 * Returns a fully expanded version of the file name. This will
 * include the expansion of any Isis Preference variables,
 * environment variables and operating system shortcuts such as
 * ".", "..", or "~"
 */
  std::string Filename::Expanded () const {
    return QFileInfo::absoluteFilePath().toStdString();
  }

/**
 * Returns whether the file exists or not.
 *
 * @return True if the file exists, false if it doesn't.
 */
  bool Filename::Exists () {
    return QFileInfo::exists();
  }

/**
 * Returns the path used to initialize the Filename object, if any.
 */
  std::string Filename::OriginalPath() const {
    QFileInfo fi(p_original.c_str());
    return fi.path().toStdString();
  }

/**
 * Adds an extension to the filename. If the existing extension is the same
 * nothing is added. If it is different then the existing
 * extensions is added to the basename.  For example,
 * base="temp", ext="cub" and invoking AddExtension("jpg") gives
 * base="temp.cub" and ext="jpg".  If the existing extension is
 * null then it is replaced.
 *
 * @param ext Extention to add.
 */
  void Filename::AddExtension (const std::string &ext) {
    // Don't modify the extension if it is already there
    if (Extension() == ext) return;

    // Add the argument as an extension to the current filename
    if (ext.length() > 0) {
      QFileInfo::setFile (iString(Expanded() + "." + ext));
    }
  }

  /**
  * Removes all extensions from the file name.
  */
  void Filename::RemoveExtension () {
    QFileInfo::setFile (iString(this->Path() + "/" + this->Basename()));
  }

/**
 * Searches for a filename with the highest integer version number. Version
 * numbers are defined as a sequence of question marks "?" in the filename.
 * Only one sequence is allowed per filename. The already internalized filename
 * is used as the directory to be searched, and it is used as the template for
 * where the version number is located. The search is not recursive. The
 * original name stored in the Filename object will be overwritten with the
 * results of the search.
 *
 * @throws Isis::iException::Io - The path does not exist
 * @throws Isis::iException::Programmer - No versions available for the file
 */
  void Filename::HighestVersion (){

    CheckVersion();

    int highestVersion = -1;
    Isis::iString highestVersionStr;

    // Get the path of the current file and make sure it exists
    QDir dir((QString)(iString)Path());
    if (!dir.exists()) {
      string msg = "The path [" + Path() + "] does not exist";
      throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
    }

    // Find the beginning and end of the "?"s in the versioned filename
    unsigned int start = Name().find_first_of("?");
    unsigned int end = Name().find_last_of("?");
    unsigned int charsAfterVersion = Name().length() - end - 1;


    // Loop through all files in the dir and see if they match our name
    for (unsigned int indx=0; indx < dir.count(); indx++) {

      string file = dir[indx].toStdString();
      bool leftSide = file.substr(0, start) == Name().substr(0, start);
      bool rightSide = ((int)file.length()-(int)charsAfterVersion) >= 0;
      if (rightSide) {
        rightSide = rightSide &&
        (file.substr(file.length()-charsAfterVersion) == Name().substr(end+1));
      }

      if (leftSide && rightSide) {

        Isis::iString version = file.substr(start, file.length()-charsAfterVersion-start);

        if ((version.length() > 0) &&
            (version.find_first_not_of("0123456789") == string::npos) &&
            (version.ToInteger() > highestVersion)) {
          highestVersion = version.ToInteger();
          highestVersionStr = version;
        }
      }
    }

    // Make sure we got a version number
    if (highestVersion == -1) {
      string msg = "No versions available for file [" + Expanded() + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }

    string temp = Path() + "/" + Name().substr(0, start) +
                  highestVersionStr + Name().substr(end+1);
    QFileInfo::setFile(temp.c_str());
  }

/**
 * Creates a filename with a version number one higher than the previous highest
 * integer version number. Version numbers are defined as a sequence of question
 * marks "?" in the filename. Only one sequence is allowed per filename. The
 * already internalized filename is used as the directory to be searched, and
 * it is used as the template for where the version number is located. The
 * search is not recursive. As is the case with HighestVersion, The original
 * name stored in the Filename object will be overwritten with the results of
 * the search. Thus, a user may operate on a Filename to change the sequence of
 * question marks to the highest version, or the new version, but may not do
 * both operations on the same Filename.
 *
 * @throws Isis::iException::Io - The path does not exist
 * @throws Isis::iException::Programmer - No versions available for the file
 */
  void Filename::NewVersion (){

    CheckVersion();

    // Get the path of the current file and make sure it exists
    QDir dir((QString)(iString)Path());
    if (!dir.exists()) {
      string msg = "The path [" + Path() + "] does not exist";
      throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
    }

    // Find the beginning and end of the "?"s in the versioned filename
    unsigned int start = Name().find_first_of("?");
    unsigned int end = Name().find_last_of("?");
    unsigned int charsAfterVersion = Name().length() - end - 1;

    int highestVersion = 0;
    // Loop through all files in the dir and see if they match our name
    for (unsigned int indx=0; indx < dir.count(); indx++) {

      string file = dir[indx].toStdString();
      bool leftSide = file.substr(0, start) == Name().substr(0, start);
      bool rightSide = ((int)file.length()-(int)charsAfterVersion) >= 0;
      if (rightSide) {
        rightSide = rightSide &&
        (file.substr(file.length()-charsAfterVersion) == Name().substr(end+1));
      }

      if (leftSide && rightSide) {

        Isis::iString version = file.substr(start, file.length()-charsAfterVersion-start);

        if ((version.length() > 0) &&
            (version.find_first_not_of("0123456789") == string::npos) &&
            (version.ToInteger() > highestVersion)) {
          highestVersion = version.ToInteger();
        }
      }
    }
    //create a string with the new version number
    Isis::iString newVersion = ++highestVersion;



    //pad each extra "?" with a 0. This maintains the length of the Filename
    //so that "file.???.ext" will be "file.001.ext" instead of "file.1.ext"
    int zeroesNeeded = end - start - newVersion.length();
    for (int i = 0 ; i <= zeroesNeeded; i++){
      newVersion = "0" + newVersion;
    }

    string temp = Path() + "/" + Name().substr(0, start) +
                  newVersion + Name().substr(end+1);
    QFileInfo::setFile(temp.c_str());
  }

/**
 * Makes a directory.
 *
 * @throws Isis::iException::Programmer - Unable to create the directory
 */
  void Filename::MakeDirectory() {

    QDir dir;
    if (!dir.mkdir(iString(Expanded()))) {
      string msg = "Unable to create directory [" + Expanded() + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg, _FILEINFO_);
    }

  }

/**
 * Create a temporary file. If the Preference "DataDirectory-&gt;Temporary"
 * exists then the file will be created there. If not the file will be created
 * using only what is specified in the current filename. The filename will
 * consist of the current name with a number between 100000 and 999999 appended
 * to the name portion.
 *
 * @param name The name of the temporary file.
 *
 * @param extension The extension of the temporary file.
 *
 * @throws Isis::iException::Io - No temporary files available for the given
 *                                name and extension
 */
  void Filename::Temporary (const std::string &name, const std::string &extension) {
    string tempDir;
    tempDir.clear();
    // If the IsisPreference exists use it otherwise just use name as is
    if (!(name.at(0)=='/') && Isis::Preference::Preferences().HasGroup("DataDirectory")) {
      Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
      if (dataDir.HasKeyword("Temporary")) {
        tempDir = (string) dataDir["Temporary"];
      }
    }

    // Start off by appending "100000" to the name if that file exists
    // increment it and try again
    int add = 100000;
    string tfile;

    do {
      Isis::iString num(add);
      tfile = tempDir + "/" + name + num + "." + extension;
      QFileInfo f(tfile.c_str());
      if (f.exists()) {
        add++;
      }
      else {
        p_original = tfile;
        QFileInfo::setFile (tfile.c_str());
        return;
      }
    } while (add < 1000000);

    string msg = "No temporary files available for [" + name + extension + "]";
    throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);

  }

/**
 * Expand any $xxxxx into Isis preference and environment variables.
 * Note: "DataDirectory" is the only group search in Preferences.
 *
 * @param file The file to be expanded.
 *
 * @return string
 */
  string Filename::Expand (const std::string &file) {

    // Setup an index for searching strings
    std::string::size_type pos, pos2;

    // Work with a tempory copy
    string temp = file;

    // Strip off any cube attributes
    if ((pos = temp.find("+")) != std::string::npos) temp.erase(pos);


    // Expand any $xxxxx into ISIS preferences and environment variables
    // *** NOTE *** This may be very operating system dependent
    string prefVar;
    string var;
    pos = 0;

    // Loop while there are any "$" at the current position or after
    // Some "$" might be skipped if no translation can be found
    while ((pos = temp.find("$", pos)) != std::string::npos) {
      pos2 = temp.find ("/", pos);
      var = temp.substr(pos+1, pos2-pos-1);
      string value;
      value.clear();

      // Find the corresponding Isis Preference if one exists
      if (Isis::Preference::Preferences().HasGroup("DataDirectory")) {
        Isis::PvlGroup &dataDir = Isis::Preference::Preferences().FindGroup("DataDirectory");
        if (dataDir.HasKeyword(var)) {
          value = (string) dataDir[var];
        }
      }

      // Find the corresponding environment variable if one exists
      if (value.length() == 0) {
        char *val;
        val = getenv(var.c_str());
        if (val != NULL) value = val;
      }

      // Replace the $xxxx with the pref/env, but don't move
      // the pointer. We may have replaced one $ for another.
      // Note: May need to put a test for circular replaces in here
      if (value.length() > 0) {
        temp.replace (pos, pos2-pos, value);
      }
      // No pref or env was available so ignore this "$" and move on
      else {
        pos++;
      }
    }
    return temp;
  }

  /**
   *  Check the current filename for a valid version sequence of "?"s
   *
   * @throws Isis::iException::Programmer - File does not contain a version
   * @throws Isis::iException::Programmer - File has too many version sequences,
   *                                        only one is allowed
   */
  void Filename::CheckVersion () const {

    // Find the series of "?"
    string name = Expanded();

    std::string::size_type start = name.find_first_of("?");
    std::string::size_type end = name.find_last_of("?");

    // Make sure there was at least one "?" for a version number
    if (start == string::npos || end == string::npos) {
      string msg = "Filename [" + Expanded() +
                   "] does not contain a version sequence";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }

    // Make sure all chars between start and end are "?"
    for (unsigned int pos=start; pos<=end; ++pos) {
      if (name[pos] != '?') {
        string msg = "Only one version sequence is allowed per filename [" +
                     Expanded() + "]";
        throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
      }
    }
  }
} // end namespace Isis
