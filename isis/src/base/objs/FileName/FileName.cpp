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

#include "FileName.h"

#include <cmath>

#include <QDate>
#include <QDebug>
#include <QDir>
#include <QPair>
#include <QString>
#include <QTemporaryFile>

#include "Preference.h"
#include "IException.h"
#include "IString.h"

using namespace std;

namespace Isis {

  //! Constructs an empty FileName object.
  FileName::FileName() {
    m_d = new Data;
  }


  FileName::FileName(const char *file) {
    m_d = new Data;
    m_d->setOriginal(file);
  }


  FileName::FileName(const std::string &file) {
    m_d = new Data;
    m_d->setOriginal(file);
  }


  FileName::FileName(const IString &file) {
    m_d = new Data;
    m_d->setOriginal(file);
  }


  FileName::FileName(const QString &file) {
    m_d = new Data;
    m_d->setOriginal(file);
  }


  FileName::FileName(const FileName &other) : m_d(other.m_d) {
  }


  //! Destroys the FileName object.
  FileName::~FileName() {
  }


  IString FileName::originalPath() const {
    return QFileInfo(m_d->original(false)).path();
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
   *   path() gives:
   *   "/home/me/img"
   * </pre>
   */
  IString FileName::path() const {
    return QFileInfo(expanded()).path();
  }


  IString FileName::attributes() const {
    QString result;
    QString fileNameWithAttribs = QFileInfo(m_d->original(true)).fileName();

    int attribStartPos = fileNameWithAttribs.indexOf("+");

    if (attribStartPos != -1)
      result = fileNameWithAttribs.mid(attribStartPos + 1);

    return result;
  }


  IString FileName::baseName() const {
    return QFileInfo(m_d->original(false)).completeBaseName();
  }


  IString FileName::name() const {
    return QFileInfo(m_d->original(false)).fileName();
  }


  IString FileName::extension() const {
    return QFileInfo(m_d->original(false)).suffix();
  }


  IString FileName::expanded() const {
    return m_d->expanded(false);
  }


  IString FileName::original() const {
    return m_d->original(true);
  }


  FileName FileName::addExtension(const IString &newExtension) const {
    FileName result = *this;

    if (result.extension() != newExtension) {
      IString attributesStr = result.attributes().ToQt();

      if (attributesStr == "")
        result = FileName(result.originalPath() + "/" + result.name() + "." + newExtension);
      else
        result = FileName(result.originalPath() + "/" + result.name() + "." + newExtension
                          + "+" + attributesStr);
    }

    return result;
  }


  FileName FileName::removeExtension() const {
    IString attributesStr = attributes().ToQt();

    FileName result;
    if (attributesStr == "")
      result = FileName(originalPath() + "/" + baseName());
    else
      result = FileName(originalPath() + "/" + baseName() + "+" + attributesStr);

    return result;
  }


  FileName FileName::setExtension(const IString &newExtension) const {
    FileName result = *this;

    if (extension() != newExtension) {
      result = result.removeExtension().addExtension(newExtension);
    }

    return result;
  }


  bool FileName::isVersioned() const {
    validateVersioningState();

    return isNumericallyVersioned() || isDateVersioned();
  }


  bool FileName::isNumericallyVersioned() const {
    return FileName(expanded()).name().ToQt().contains("?");
  }


  bool FileName::isDateVersioned() const {
    return FileName(expanded()).name().ToQt().contains(QRegExp("\\{.*\\}"));
  }


  FileName FileName::highestVersion() const {
    validateVersioningState();

    FileName result = *this;

    if (!isVersioned()) {
      throw IException(IException::Unknown,
                       QObject::tr("Asked for highest version of file named [%1] in [%2] but there "
                                   "are no version sequences in the name")
                         .arg(name().ToQt()).arg(originalPath().ToQt()),
                       _FILEINFO_);
    }

    // Look for dates
    if (isDateVersioned()) {
      result = result.version(result.highestVersionDate());
    }

    // Look for number's
    if (isNumericallyVersioned()) {
      result = result.version(result.highestVersionNum());
    }

    return result;
  }


  FileName FileName::newVersion() const {
    validateVersioningState();

    FileName result = *this;

    if (!isVersioned()) {
      throw IException(IException::Unknown,
                       QObject::tr("Asked for new version of file named [%1] in [%2] but there "
                                   "are no version sequences in the name")
                         .arg(name().ToQt()).arg(originalPath().ToQt()),
                       _FILEINFO_);
    }

    // Look for date
    if (isDateVersioned()) {
      result = result.version(QDate::currentDate());
    }

    // Look for #'s
    if (isNumericallyVersioned()) {
      try {
        result = result.version(result.highestVersionNum() + 1);
      }
      catch (IException &) {
        result = result.version(1);
      }
    }

    if (result.fileExists()) {
      throw IException(IException::Unknown,
                       QObject::tr("Could not generate unique new version of file named [%1] in "
                                   "[%2] because the file [%3] exists")
                         .arg(name().ToQt()).arg(originalPath().ToQt()).arg(result.name().ToQt()),
                       _FILEINFO_);

    }

    return result;
  }


  FileName FileName::version(long versionNumber) const {
    QString file = FileName(expanded()).name();

    int width = file.count("?");

    if (versionNumber < 0) {
      throw IException(IException::Unknown,
          QObject::tr("FileName does not support negative version numbers in the file name, "
                      "tried to get version [%1] in file named [%2]")
            .arg(versionNumber).arg(originalPath().ToQt() + "/" + file),
          _FILEINFO_);
    }

    if (versionNumber >= pow(10.0, width)) {
      throw IException(IException::Unknown,
          QObject::tr("FileName does not support version numbers greater than what would fit in "
                      "the file name, tried to get version [%1] in file named [%2]")
            .arg(versionNumber).arg(originalPath().ToQt() + "/" + file),
          _FILEINFO_);
    }

    QPair<QString, QString> splitName = splitNameAroundVersionNum();
    QString &before = splitName.first;
    QString &after = splitName.second;

    file = before + QString("%1").arg(QString::number(versionNumber), width, '0') + after;

    return FileName(originalPath().ToQt() + "/" + file);
  }


  FileName FileName::version(QDate versionDate) const {
    QString newName = versionDate.toString(fileNameQDatePattern());

    return FileName(originalPath().ToQt() + "/" + newName);
  }


  bool FileName::fileExists() const {
    return QFileInfo(expanded().ToQt()).exists();
  }


  QDir FileName::dir() const {
    return QFileInfo(expanded().ToQt()).dir();
  }


  FileName FileName::createTempFile(FileName templateFileName) {
    QString preppedFileName = QString("%1/%2XXXXXX.%3").arg(templateFileName.path().ToQt())
        .arg(templateFileName.baseName().ToQt()).arg(templateFileName.extension().ToQt());
    QTemporaryFile tempFile(preppedFileName);
    tempFile.setAutoRemove(false);

    if (!tempFile.open()) {
      throw IException(IException::Io,
          QObject::tr("Could not create a unique temporary file name based on [%1]")
            .arg(templateFileName.original().ToQt()),
          _FILEINFO_);
    }

    // We want to set the 'original' path as correctly as possible. So let's use the input original
    //   path with the output temp file's file name in our result.
    FileName result;
    QString newTempFileNameStr = templateFileName.originalPath().ToQt() + "/" +
        QFileInfo(tempFile.fileName()).fileName();
    result = FileName(newTempFileNameStr);

    return result;
  }


  IString FileName::toString() const {
    return expanded();
  }


  /**
   * Clears the current contents of the FileName object and reinitializes it with
   * the argument.
   *
   * @param file File name to replace the current contents of the object.
   */
  FileName &FileName::operator=(const FileName &rhs) {
    m_d = rhs.m_d;
    return *this;
  }


  bool FileName::operator==(const FileName &rhs) {
    QString expandedOfThis = expanded();
    QString canonicalOfThis = QFileInfo(expandedOfThis).canonicalFilePath();

    QString expandedOfRhs = rhs.expanded();
    QString canonicalOfRhs = QFileInfo(expandedOfRhs).canonicalFilePath();

    // Cononical file paths return empty strings if the file does not exist. Either both canonicals
    //   are valid and the same (equal is initialized to true), or neither canonical is valid but
    //   the expandeds are the same (equal is set to true when it isn't initialized to true).
    bool equal = (!canonicalOfThis.isEmpty() && canonicalOfThis == canonicalOfRhs);

    if (!equal) {
      equal = (canonicalOfThis.isEmpty() && canonicalOfRhs.isEmpty() &&
               expandedOfThis == expandedOfRhs);
    }

    return equal;
  }



  bool FileName::operator!=(const FileName &rhs) {
    return !(*this == rhs);
  }


  QDate FileName::highestVersionDate() const {
    QString fileQDatePattern = fileNameQDatePattern();

    QPair<int, int> truncateRange(-1, -1);
    if (fileQDatePattern.contains("?")) {
      QString trueLengthName = name().ToQt().replace(QRegExp("[{}]"), "");
      truncateRange.first = trueLengthName.indexOf("?");
      truncateRange.second = trueLengthName.lastIndexOf("?");
      fileQDatePattern = fileQDatePattern.replace("?", "");
    }

    QString file = name().ToQt();

    QDate result;
    QDate sputnikLaunch(1957, 10, 4);

    QString before = file.mid(0, file.indexOf("{"));
    QString after = file.mid(file.lastIndexOf("}") + 1);

    QStringList nameFilters;

    nameFilters.append(before + "*" + after);
    QStringList files = dir().entryList(nameFilters);

    // We can't sort the files to get our answer, so we need to go through every possible file.
    foreach (QString foundFile, files) {
      // Toss any numerical versioning sequence
      if (truncateRange.first >= 0 && truncateRange.second > truncateRange.first) {
        foundFile = foundFile.mid(0, truncateRange.first) +
                    foundFile.mid(truncateRange.second + 1);

      }

      QDate fileDate = QDate::fromString(foundFile, fileQDatePattern);

      if (fileDate.isValid()) {
        // No missions before Sputnik 1, so we must be in the new millenium
        if (fileDate < sputnikLaunch)
          fileDate = fileDate.addYears(100);

        if (!result.isValid() || fileDate > result) {
          result = fileDate;
        }
      }
    }

    if (!result.isValid()) {
      throw IException(IException::Unknown,
                       QObject::tr("No existing files found with a date version matching [%1] in "
                                   "[%2]")
                         .arg(FileName(expanded()).name().ToQt()).arg(path().ToQt()),
                       _FILEINFO_);
    }

    return result;
  }


  long FileName::highestVersionNum() const {
    QString file = FileName(expanded()).name();
    int result = 0;

    int width = file.count("?");

    QPair<QString, QString> splitName = splitNameAroundVersionNum();
    QString &before = splitName.first;
    QString &after = splitName.second;

    QStringList nameFilters;
    nameFilters.append(before + QString("%1").arg("", width, '?') + after);
    QStringList files = dir().entryList(nameFilters, QDir::NoFilter, QDir::Name);

    long foundValue = -1;
    bool success = false;

    for (int i = files.count() - 1; !success && i >= 0; i--) {
      foundValue = files[i].mid(before.count(), width).toLong(&success);
    }

    if (success) {
      result = foundValue;
    }
    else {
      throw IException(IException::Unknown,
                       QObject::tr("No existing files found with a numerial version matching [%1] "
                                   "in [%2]")
                         .arg(FileName(expanded()).name().ToQt()).arg(path().ToQt()),
                       _FILEINFO_);
    }

    return result;
  }


  /**
   * This verifies the class invariant when using versioning - that the FileName is in an acceptable
   *     state to find file version numbers.
   */
  void FileName::validateVersioningState() const {
    QString file = QFileInfo(expanded()).fileName();

    if (file.contains(QRegExp("\\?\\?*[^?][^?]*\\?"))) {
      throw IException(IException::Unknown,
          QObject::tr("Only one numerical version sequence is allowed in a filename; "
                      "there are multiple in [%1]").arg(file),
          _FILEINFO_);
    }

    if (isDateVersioned()) {
      QString fileDatePattern = FileName(expanded()).name();
      fileDatePattern = "'" + fileDatePattern.replace(QRegExp("[{}]"), "'") + "'";

      QString dated = QDate::currentDate().toString(fileDatePattern);
      if (file.contains("'")) {
        throw IException(IException::Unknown,
            QObject::tr("Date version sequenced file names cannot have single quotes in them; "
                        "the file named [%1] is not usable").arg(file),
            _FILEINFO_);
      }
      else if (dated == "") {
        throw IException(IException::Unknown,
            QObject::tr("The date version sequence is not usable in the file named [%1]").arg(file),
            _FILEINFO_);
      }
      else if (dated == fileDatePattern.replace(QRegExp("'"), "")) {
        throw IException(IException::Unknown,
            QObject::tr("The date version sequences are not recognized in the file named [%1]")
              .arg(file),
            _FILEINFO_);
      }
    }
  }

  QString FileName::fileNameQDatePattern() const {
    // We need to quote everything not in {} with single quotes.
    QString file = FileName(expanded()).name();

    // Current Text: {VAR}XXX{VAR}XXX{VAR} or XXX{VAR}XXX{VAR} or XXX{VAR}XXX or {VAR}XXX
    file = file.replace(QRegExp("[{}]"), "'");

    // Current Text: 'VAR'XXX'VAR'XXX'VAR' or XXX'VAR'XXX'VAR' or XXX'VAR'XXX or 'VAR'XXX
    if (file.startsWith("'"))
      file = file.mid(1);
    else
      file = "'" + file;

    // Current Text: VAR'XXX'VAR'XXX'VAR' or 'XXX'VAR'XXX'VAR' or 'XXX'VAR'XXX or VAR'XXX
    if (file.endsWith("'"))
      file = file.mid(0, file.length() - 1);
    else
      file = file + "'";

    // Current Text: VAR'XXX'VAR'XXX'VAR or 'XXX'VAR'XXX'VAR or 'XXX'VAR'XXX'  or VAR'XXX' -- VAR's
    //   are the only text not quoted; this is success.
    return file;
  }


  QPair<QString, QString> FileName::splitNameAroundVersionNum() const {
    QString file = FileName(expanded()).name();
    QString before;
    QString after;

    if (!isNumericallyVersioned()) {
      before = file;
    }
    else {
      before = file.mid(0, file.indexOf("?"));
      after = file.mid(file.lastIndexOf("?") + 1);
    }

    return QPair<QString, QString>(before, after);
  }


  FileName::Data::Data() {
    m_originalFileNameString = NULL;
    m_expandedFileNameString = NULL;

    m_originalFileNameString = new IString;
    m_expandedFileNameString = new IString;
  }


  FileName::Data::Data(const Data &other) : QSharedData(other) {
    m_originalFileNameString = NULL;
    m_expandedFileNameString = NULL;

    m_originalFileNameString = new IString(*other.m_originalFileNameString);
    m_expandedFileNameString = new IString(*other.m_expandedFileNameString);
  }


  FileName::Data::~Data() {
    delete m_originalFileNameString;
    m_originalFileNameString = NULL;

    delete m_expandedFileNameString;
    m_expandedFileNameString = NULL;
  }


  IString FileName::Data::original(bool includeAttributes) const {
    QString result = *m_originalFileNameString;


    if (!includeAttributes) {
      int attributesPos = result.indexOf("+");

      if (attributesPos != -1)
        result = result.left(attributesPos);
    }

    return result;
  }


  void FileName::Data::setOriginal(const IString &originalStr) {
    *m_originalFileNameString = originalStr;

    // Expand the file name and store that too.
    QString expandedStr = original(true);

    int varSearchStartPos = 0;
    int varStartPos = -1;
    // Loop while there are any "$" at the current position or after
    // Some "$" might be skipped if no translation can be found
    while((varStartPos = expandedStr.indexOf("$", varSearchStartPos)) != -1) {
      int varEndPos = expandedStr.indexOf(QRegExp("[^a-zA-Z{}0-9]"), varStartPos + 1);
      if (varEndPos == -1)
        varEndPos = expandedStr.length();

      bool variableValid = false;
      int varNameLength = varEndPos - varStartPos;

      if (varNameLength > 0) {
        QString varName = expandedStr.mid(varStartPos + 1, varEndPos - varStartPos - 1);

        if (varName.length()) {
          if (varName[0] =='{' && varName[varName.length() - 1] == '}')
            varName = varName.mid(1, varName.length() - 2);

          QString varValue;

          // Find the corresponding Isis Preference if one exists
          if(Preference::Preferences().HasGroup("DataDirectory")) {
            PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");
            if(dataDir.HasKeyword(varName.toStdString())) {
              varValue = ((IString)dataDir[varName.toStdString().c_str()][0]).ToQt();
            }
          }

          // Find the corresponding environment variable if one exists
          if (varValue.isEmpty()) {
            char *val;
            val = getenv(varName.toStdString().c_str());
            if(val != NULL) varValue = val;
          }

          // Replace the $xxxx with the pref/env, but don't move
          // the pointer. We may have replaced one $ for another.
          // Note: May need to put a test for circular replaces in here
          if (!varValue.isEmpty()) {
            expandedStr = expandedStr.replace(varStartPos, varNameLength, varValue);
            variableValid = true;
          }
        }
      }

      if (variableValid) {
        // We could expand multiple times...
        varSearchStartPos = varStartPos;
      }
      else {
        // We failed to understand this variable, move along
        varSearchStartPos = varStartPos + 1;
      }
    }

    *m_expandedFileNameString = expandedStr;
  }


  IString FileName::Data::expanded(bool includeAttributes) const {
    QString result = *m_expandedFileNameString;


    if (!includeAttributes) {
      int attributesPos = result.indexOf("+");

      if (attributesPos != -1)
        result = result.left(attributesPos);
    }

    return result;
  }
}

