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

#include "Filename.h"

#include <QDate>
#include <QDir>

#include "Preference.h"
#include "iException.h"
#include "iString.h"

using namespace std;

namespace Isis {

  //! Constructs an empty Filename object.
  Filename::Filename() {
    p_original.clear();
  }


  /**
   * Constructs a Filename object and expands environment variables and
   * Preferences from the DataDirectory group embedded in the filename.
   *
   * @param file A string containing a filename.
   */
  Filename::Filename(const iString &file)
    : QFileInfo() {
    p_original = file;
    QFileInfo::setFile((iString)Expand(file));
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
  Filename::Filename(const iString &name, const iString &extension) {
    Temporary(name, extension);
  }


  //! Destroys the Filename object.
  Filename::~Filename() {
  }


  /**
   * Clears the current contents of the Filename object and reinitializes it with
   * the argument.
   *
   * @param file File name to replace the current contents of the object.
   */
  void Filename::operator=(const iString &file) {
    p_original = file;
    QFileInfo::setFile((iString)Expand(file));
  }


  /**
   * Clears the current contents of the Filename object and reinitializes it with
   * the argument.
   *
   * @param file File name to replace the current contents of the object.
   */
  void Filename::operator=(const char *file) {
    p_original = file;
    QFileInfo::setFile((iString)Expand(file));
  }


  /**
   * @brief Returns the path. 
   * @returns the path portion of a filename. For *nix operating
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
  iString Filename::Path() const {
    return QFileInfo::absolutePath().toStdString();
  }


  /**
   * @brief Returns the basename. 
   * @returns the file name only. This excludes any path and the last extension.
   * For *nix operating systems this includes everything following the last slash
   * "/" and upto the last dot ".". If a file name contains multiple extensions,
   * all but the last one will be returned. If a file name has no extension,
   * Basename returns the same as Name
   */
  iString Filename::Basename() const {
    if(QFileInfo::suffix().length() == 0) {
      return QFileInfo::baseName().toStdString();
    }
    else {
      return QFileInfo::completeBaseName().toStdString();
    }
  }


  /**
   * @brief Returns the filename. 
   * @returns the file name and all extensions of the filename. For
   * *nix operating systems this includes everything following the
   * last slash "/".
   */
  iString Filename::Name() const {
    return QFileInfo::fileName().toStdString();
  }


  /**
   * @brief Returns the extension (Does not include .) 
   * @returns the extension of the filename. If multiple extensions
   * exist, then only the last one will be returned.
   */
  iString Filename::Extension() const {
    return QFileInfo::suffix().toStdString();
  }


  /**
   * @brief Returns the full filename (path, basename and 
   * extension(s)). 
   * @returns a fully expanded version of the file name. This will
   * include the expansion of any Isis Preference variables,
   * environment variables and operating system shortcuts such as
   * ".", "..", or "~"
   */
  iString Filename::Expanded() const {
    return QFileInfo::absoluteFilePath().toStdString();
  }


  /**
   * Returns whether the file exists or not.
   *
   * @return True if the file exists, false if it doesn't.
   */
  bool Filename::Exists() {
    return QFileInfo::exists();
  }


  /**
   * @returns the path used to initialize the Filename object, if any.
   */
  iString Filename::OriginalPath() const {
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
  void Filename::AddExtension(const iString &ext) {
    // Don't modify the extension if it is already there
    if(Extension() == ext) return;

    // Add the argument as an extension to the current filename
    if(ext.length() > 0) {
      QFileInfo::setFile(iString(Expanded() + "." + ext));
    }
  }


  /**
  * Removes all extensions from the file name.
  */
  void Filename::RemoveExtension() {
    QFileInfo::setFile(iString(this->Path() + "/" + this->Basename()));
  }


  /**
   * Get the directory contents of the path to this filename.
   */
  QDir Filename::GetDirectory() const {
    // Get the path of the current file and make sure it exists
    QDir dir((QString)(iString)Path());
    if (!dir.exists()) {
      string msg = "The path [" + Path() + "] does not exist";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }
    return dir;
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
   * @throws iException::Io - The path does not exist
   * @throws iException::Programmer - No versions available for the file
   */
  void Filename::HighestVersion() {
    CheckVersion();
    if (IsNumericallyVersioned())
      SetHighestNumericalVersion();
    else if (IsDateVersioned())
      SetHighestDateVersion();
  }


  /**
   * Checks that the filename is a valid versioning pattern, and throws an
   * exception if it is not.
   *
   * @throws iException::Programmer - File does not contain a version
   * @throws iException::Programmer - File has too many version sequences,
   *                                        only one is allowed
   */
  void Filename::CheckVersion() const {
    if (!IsVersioned()) {
      string msg = "Filename [" + Expanded() +
                   "] does not contain a version sequence";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Returns true if the filename is a versioning pattern.  Such patterns are
   * denoted by a sequence of '?' symbols for numerical versions or a pair of
   * curly braces '{}' for date versions.
   */
  bool Filename::IsVersioned() const {
    return IsNumericallyVersioned() || IsDateVersioned();
  }


  /**
   * Returns true if the filename is a numerical versioning pattern.  These
   * patterns are denoted by a series of '?'.  Only one such continuous pattern
   * is allowed in a filename.
   */
  bool Filename::IsNumericallyVersioned() const {
    // Make sure there was at least one "?" for a version number
    return Name().ToQt().contains("?");
  }


  /**
   * Returns true if the filename is a Qt date versioning pattern.  These
   * patterns are denoted by a series of 'd', 'M', and 'y' surrounded by curly
   * braces '{}', and follow Qt's formatting guidelines for constructing dates
   * from strings, except that instead of enclosing non-date patterns in
   * single-quotes, Isis encloses the date patterns themselves in curly braces.
   */
  bool Filename::IsDateVersioned() const {
    QString filePattern = Name().ToQt();
    return filePattern.contains("{") && filePattern.contains("}");
  }


  /**
   * Sets this filename to be that of the highest numerically versioned file in
   * the pattern filename's directory.
   */
  void Filename::SetHighestNumericalVersion() {
    QString highestVersion = GetHighestVersionNumber();

    // Make sure we got a version number
    if (highestVersion.isEmpty()) {
      string msg = "No versions available for file [" + Expanded() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    QString name = ReplacePattern(Name().ToQt(), highestVersion);
    string temp = Path() + "/" + name.toStdString();
    QFileInfo::setFile(temp.c_str());
  }


  /**
   * Replace the numeric '?' pattern with the given version number.
   */
  QString Filename::ReplacePattern(QString name, QString version) {
    // Pad each extra '?' with a '0'. This maintains the length of the filename
    // so that "file.???.ext" will be "file.001.ext" instead of "file.1.ext".
    int minLength = name.lastIndexOf("?") - name.indexOf("?") + 1;
    version = PadFront(version, "0", minLength);

    return name.replace(QRegExp("\\?+"), version);
  }


  /**
   * Pad the front of the string with the given padding symbols until the
   * minimum length is reached.
   */
  QString Filename::PadFront(QString string, QString padding, int minLength) {
    while (string.size() < minLength) string = padding + string;
    return string;
  }


  /**
   * Retrieves the version number of the highest numerically versioned filename
   * in the pattern filename's directory.
   */
  QString Filename::GetHighestVersionNumber() const {
    iString highestVersion = "-1";

    // Find the beginning and end of the "?"s in the versioned filename
    string name = Name();
    unsigned int start = name.find_first_of("?");
    unsigned int end = name.find_last_of("?");
    unsigned int charsAfterVersion = name.length() - end - 1;

    // Make sure all chars between start and end are "?"
    for (unsigned int pos = start; pos <= end; pos++) {
      if (name[pos] != '?') {
        string msg = "Only one version sequence is allowed per filename [" +
                     Expanded() + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }

    // Loop through all files in the dir and see if they match our name
    QDir dir = GetDirectory();
    for (unsigned int indx = 0; indx < dir.count(); indx++) {
      string file = dir[indx].toStdString();
      bool leftSide = file.substr(0, start) == name.substr(0, start);
      bool rightSide = ((int)file.length() - (int)charsAfterVersion) >= 0;
      if (rightSide) {
        rightSide = rightSide &&
                    (file.substr(file.length() - charsAfterVersion) == name.substr(end + 1));
      }

      if (leftSide && rightSide) {
        iString version = file.substr(start, file.length() - charsAfterVersion - start);
        if ((version.length() > 0) &&
            (version.find_first_not_of("0123456789") == string::npos) &&
            (version.ToInteger() > highestVersion.ToInteger())) {
          highestVersion = version;
        }
      }
    }

    return highestVersion.ToInteger() != -1 ? highestVersion.ToQt() : QString();
  }


  /**
   * Sets this filename to be that of the highest date versioned file in the
   * pattern filename's directory.
   */
  void Filename::SetHighestDateVersion() {
    QDir dir = GetDirectory();
    QString filePattern = GetDatePattern();

    QString latestFilename = "";
    QDate latestDate(1900, 1, 1);
    QDate sputnikLaunch(1957, 10, 4);
    for (unsigned int i = 0; i < dir.count(); i++) {
      QString filename = dir[i];
      QDate fileDate = QDate::fromString(filename, filePattern);

      if (fileDate.isValid()) {
        // No missions before Sputnik 1, so we must be in the new millenium
        if (fileDate < sputnikLaunch) fileDate = fileDate.addYears(100);

        if (fileDate > latestDate) {
          latestFilename = filename;
          latestDate = fileDate;
        }
      }
    }

    if (latestFilename == "") {
      string msg = "No versions available for file [" + Expanded() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    string fullFilename = Path() + "/" + latestFilename.toStdString();
    QFileInfo::setFile(fullFilename.c_str());
  }


  /**
   * Converts the Isis date pattern to a standard Qt pattern and returns the
   * result, excluding the path.
   */
  QString Filename::GetDatePattern() const {
    QString filePattern = Name().ToQt();
    filePattern = filePattern.replace("{", "'");
    filePattern = filePattern.replace("}", "'");
    filePattern = "'" + filePattern + "'";
    return filePattern;
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
   * @throws iException::Io - The path does not exist
   * @throws iException::Programmer - No versions available for the file
   */
  void Filename::NewVersion() {
    CheckVersion();
    if (IsNumericallyVersioned())
      SetNewNumericalVersion();
    else if (IsDateVersioned())
      SetNewDateVersion();
  }


  /**
   * Sets this filename to be a new, latest version of the numerical filename
   * pattern.  This new version is constructed by incrementing the version
   * number of the current highest version filename.
   */
  void Filename::SetNewNumericalVersion() {
    // Create a string with the new version number
    QString highestVersion = GetHighestVersionNumber();
    if (!highestVersion.isEmpty()) {
      // Increment the version number, but preserve the length of the string
      // (for expanding '?' symbols when the true filename has more digits than
      // the pattern
      int length = highestVersion.size();
      highestVersion = QString::number(highestVersion.toInt() + 1);
      PadFront(highestVersion, "0", length);
    }
    else {
      // It has been decided that versioning starts at 1, but we must still
      // account for existing files versioned starting at 0
      highestVersion = QString::number(1);
    }

    QString name = ReplacePattern(Name().ToQt(), highestVersion);
    string temp = Path() + "/" + name.toStdString();
    QFileInfo::setFile(temp.c_str());
  }


  /**
   * Sets this filename to be a new, latest version of the date filename
   * pattern.  This new version is constructed by formatting the current date to
   * the filename pattern.
   */
  void Filename::SetNewDateVersion() {
    QDate today = QDate::currentDate();
    QString filePattern = GetDatePattern();

    QString filename = today.toString(filePattern);
    string fullFilename = Path() + "/" + filename.toStdString();
    QFileInfo::setFile(fullFilename.c_str());
  }


  /**
   * Makes a directory.
   *
   * @throws iException::Programmer - Unable to create the directory
   */
  void Filename::MakeDirectory() {
    QDir dir;
    if(!dir.mkdir(iString(Expanded()))) {
      string msg = "Unable to create directory [" + Expanded() + "]";
      throw iException::Message(iException::Programmer,
                                      msg, _FILEINFO_);
    }

  }


  /**
   * Create a temporary file.
   *
   * If an absolute path is specified, then the file will be placed in
   *   the given folder and the DataDirectory-&gt;Temporary preference
   *   will be ignored.
   *
   * If only a relative path is specified, and the DataDirectory-&gt;Temporary
   *   preference is set, then the temporary file will be placed relative
   *   to the folder specified by DataDirectory-&gt;Temporary.
   *
   * If a relative path is specified, and the DataDirectory-&gt;Temporary
   *   preference is not set, then the file will be created relative to the
   *   current directory. 
   *
   * The temporary file's filename will be modified to be unique; the final
   *   filename will consist of the current name with a number between 100,000
   *   and 999,999 appended. An exception will be thrown if a unique name cannot
   *   be found. This does not guarantee O_EXCL (see man open).
   *
   * @param name The name of the temporary file.
   * @param extension The extension of the temporary file.
   */
  void Filename::Temporary(const iString &name, const iString &extension) {
    string tempDir;
    tempDir.clear();
    // If the IsisPreference exists use it otherwise just use name as is
    if(!(name.at(0) == '/') && Preference::Preferences().HasGroup("DataDirectory")) {
      PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");
      if(dataDir.HasKeyword("Temporary")) {
        tempDir = (string) dataDir["Temporary"];
      }
    }

    if (!tempDir.empty())
      tempDir += "/";

    // Start off by appending "100000" to the name if that file exists
    // increment it and try again
    int add = 100000;
    string tfile;

    do {
      iString num(add);
      tfile = tempDir + name + num + "." + extension;
      QFileInfo f(tfile.c_str());
      if(f.exists()) {
        add++;
      }
      else {
        p_original = tfile;
        QFileInfo::setFile(tfile.c_str());
        return;
      }
    }
    while(add < 1000000);

    string msg = "No temporary files available for [" + name + extension + "]";
    throw iException::Message(iException::Io, msg, _FILEINFO_);
  }


  /**
   * Expand any $xxxxx into Isis preference and environment variables.
   * Note: "DataDirectory" is the only group search in Preferences.
   *
   * @param file The file to be expanded.
   *
   * @return @b string Expanded file name
   */
  iString Filename::Expand(const iString &file) {

    // Setup an index for searching strings
    std::string::size_type pos, pos2;

    // Work with a tempory copy
    string temp = file;

    // Strip off any cube attributes
    if((pos = temp.find("+")) != std::string::npos) temp.erase(pos);


    // Expand any $xxxxx into ISIS preferences and environment variables
    // *** NOTE *** This may be very operating system dependent
    string prefVar;
    string var;
    pos = 0;

    // Loop while there are any "$" at the current position or after
    // Some "$" might be skipped if no translation can be found
    while((pos = temp.find("$", pos)) != std::string::npos) {
      pos2 = temp.find("/", pos);
      var = temp.substr(pos + 1, pos2 - pos - 1);
      string value;
      value.clear();

      // Find the corresponding Isis Preference if one exists
      if(Preference::Preferences().HasGroup("DataDirectory")) {
        PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");
        if(dataDir.HasKeyword(var)) {
          value = (string) dataDir[var];
        }
      }

      // Find the corresponding environment variable if one exists
      if(value.length() == 0) {
        char *val;
        val = getenv(var.c_str());
        if(val != NULL) value = val;
      }

      // Replace the $xxxx with the pref/env, but don't move
      // the pointer. We may have replaced one $ for another.
      // Note: May need to put a test for circular replaces in here
      if(value.length() > 0) {
        temp.replace(pos, pos2 - pos, value);
      }
      // No pref or env was available so ignore this "$" and move on
      else {
        pos++;
      }
    }
    return temp;
  }
}

