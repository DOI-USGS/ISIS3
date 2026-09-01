/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <iostream>
#include <algorithm>

#include <QDir>
#include <QRegularExpression>

#include "SpiceDbGen.h"
#include "NaifStatus.h"
#include "spiceql.h"

using namespace std;
using namespace Isis;


const char *SpiceDbGen::calForm = "YYYY MON DD HR:MN:SC.###### TDB ::TDB";
/**
 * Constructs a new SpiceDbGen.
 *
 * @param type The type of kernel to be processed. Either, "SPK" or "CK.
*/
SpiceDbGen::SpiceDbGen(QString type) {
  p_type = type;
  m_coverageLevel = "SEGMENT"; // default
  //  calForm = "YYYY MON DD HR:MN:SC.### TDB ::TDB";
}


/**
 * Creates a Pvl object that stores all of the kernels under the condition
 * specified by the filter.
 *
 * @param quality   The quality of the kernels that are being filtered into
 *                  the database. For example, "Reconstructed".
 *
 * @param location  The directory in which the method searches for kernels.
 *
 * @param filter    Vector of QStrings containing regular expression used to
 *                  match kernels of a particular quality. This parameter is
 *                  used to distinguish between kernels of different qualities
 *                  and/or different missions that may be placed in the same
 *                  directory.
 *
 * @return PvlObject
 *
 * @internal
 *   @history 2010-04-27 Stuart Sides Changed to work with vector of filters
 *   instead of a single filter
 *
 *   @history 2015-03-27 Stuart Sides Changed the number of decimal places
 *   written to the Time keyword for kernel.###.db files from three to six
 *
 * @throws Isis::iException::Message
*/
PvlObject SpiceDbGen::Direct(QString quality, QString location,
                             std::vector<QString> &filter, double startOffset, double endOffset) {
  PvlObject result;

  QStringList files = FilterFiles(quality, location, filter);

  for (int fileNum = 0 ; fileNum < files.size() ; fileNum++) {
    FileName currFile(files[fileNum]);
    PvlGroup selection = AddSelection(currFile, startOffset, endOffset);
    selection += PvlKeyword("Type", quality);
    result.addGroup(selection);
  }

  return Finalize(result);
}


/**
 * Expands the given filters into a list of kernel files.
 *
 * @param quality  The quality of the kernels being filtered, used for error reporting.
 *
 * @param location The directory in which the method searches for kernels.
 *
 * @param filter   Vector of QStrings containing regular expressions used to match kernels.
 *
 * @return QStringList The matching files, each prefixed with the location
 *
 * @throws Isis::iException::Message
 */
QStringList SpiceDbGen::FilterFiles(QString quality, QString location,
                                    std::vector<QString> &filter) {
  QStringList results;

  for (unsigned int i = 0; i < filter.size(); ++i) {
    QStringList files = GetFiles(FileName(location), filter[i]);

    // Throw an error if no files are being added to this database for
    // this filter/regex
    if (files.size() == 0) {
      QString message = "Your filter [" + location + "/" + filter[i] + "]"
                       + "has not detected any " + quality + " kernels";
      throw IException(IException::User, message, _FILEINFO_);
    }

    for (int fileNum = 0 ; fileNum < files.size() ; fileNum++) {
      results.append(location + "/" + files[fileNum]);
    }
  }

  return results;
}


/**
 * Verifies every selection is of the expected kernel type, drops the selections with no time
 * coverage, and names the object after the kernel type.
 *
 * @param result The object of selections to finalize
 *
 * @return PvlObject
 *
 * @throws Isis::iException::Message
 */
PvlObject SpiceDbGen::Finalize(PvlObject &result) {
  // Check each group to make sure it is the same type as others
  PvlObject::PvlGroupIterator grp = result.beginGroup();

  while (grp != result.endGroup()) {
    // The kernel did not have any time coverage, so ignore it
    if (grp->name() == "No coverage" || grp->name() == "Null") {
      result.deleteGroup(grp->name());
    }
    else if (grp->name() == p_type) {
      grp->setName("Selection");
      grp++;
    }
    else {
      QString message = "A kernel of type [" + grp->name() + "] has been found in a directory for type [" + p_type + "]" ;
      throw IException(IException::Programmer, message, _FILEINFO_);
      break;
    }
  }

  if (p_type == "SPK") {
    result.setName("SpacecraftPosition");
  }
  else if (p_type == "CK") {
    result.setName("SpacecraftPointing");
  }

  return result;
}


/**
 * Creates a Pvl object that stores all of the kernels specified by a list
 *
 * @param quality   The quality of the kernels that are being filtered into
 *                  the database. For example, "Reconstructed".
 *
 * @param fileList  The list of files to create a database for. The files must
 *                  be ordered in ascending priority. That is, if two kernels
 *                  cover the same time period, the kernel later in the file
 *                  will be used.
 *
 * @return PvlObject
 *
 * @internal
 *   @history 2019-08-13 Jesse Mapel Created method that takes a file list.
 *
 * @throws Isis::iException::Message
*/
PvlObject SpiceDbGen::Direct(QString quality, FileList fileList,
                             double startOffset, double endOffset) {
  PvlObject result;

  // Throw an error if no files are being added to this database for
  // this filter/regex
  if (fileList.empty()) {
    QString message = "Input filelist is empty!";
    throw IException(IException::User, message, _FILEINFO_);
  }

  for (int fileNum = 0 ; fileNum < fileList.size() ; fileNum++) {
    FileName currFile = fileList[fileNum];
    PvlGroup selection = AddSelection(currFile, startOffset, endOffset);
    selection += PvlKeyword("Type", quality);
    result.addGroup(selection);
  }

  return Finalize(result);
}


/**
 * Creates a Pvl object in which every kernel matching the filters is split into a reconstructed
 * and a predicted selection at its delivery date.
 *
 * Some projects deliver a single kernel that holds reconstructed data up to the date the kernel
 * was generated and predicted data after it. Such a kernel cannot be assigned a single quality,
 * so each one contributes two selections with complementary time coverage.
 *
 * @param location    The directory in which the method searches for kernels.
 *
 * @param filter      Vector of QStrings containing regular expressions used to match kernels.
 *
 * @param margin      Seconds subtracted from the delivery date to place the boundary between the
 *                    reconstructed and predicted coverage.
 *
 * @return PvlObject
 *
 * @throws Isis::iException::Message
 */
PvlObject SpiceDbGen::DirectReconSplit(QString location, std::vector<QString> &filter,
                                       double startOffset, double endOffset, double margin) {
  PvlObject result;

  QStringList files = FilterFiles("Reconstructed", location, filter);

  for (int fileNum = 0 ; fileNum < files.size() ; fileNum++) {
    AddSplitSelections(result, FileName(files[fileNum]), startOffset, endOffset, margin);
  }

  return Finalize(result);
}


/**
 * Creates a Pvl object in which every kernel in the list is split into a reconstructed and a
 * predicted selection at its delivery date.
 *
 * @param fileList  The list of files to create a database for, ordered in ascending priority.
 *
 * @param margin    Seconds subtracted from the delivery date to place the boundary between the
 *                  reconstructed and predicted coverage.
 *
 * @return PvlObject
 *
 * @throws Isis::iException::Message
 */
PvlObject SpiceDbGen::DirectReconSplit(FileList fileList, double startOffset, double endOffset,
                                       double margin) {
  PvlObject result;

  if (fileList.empty()) {
    QString message = "Input filelist is empty!";
    throw IException(IException::User, message, _FILEINFO_);
  }

  for (int fileNum = 0 ; fileNum < fileList.size() ; fileNum++) {
    AddSplitSelections(result, fileList[fileNum], startOffset, endOffset, margin);
  }

  return Finalize(result);
}


/**
 * Adds the reconstructed and predicted selections for a single kernel to the given object.
 *
 * @param result      The object to add the selections to
 *
 * @param currFile    The kernel to split
 *
 * @param margin      Seconds subtracted from the delivery date to place the boundary
 *
 * @throws Isis::iException::Message
 */
void SpiceDbGen::AddSplitSelections(PvlObject &result, FileName currFile,
                                    double startOffset, double endOffset, double margin) {
  double boundary = DeliveryDate(currFile) - margin;

  PvlGroup recon = AddSelection(currFile, startOffset, endOffset, -DBL_MAX, boundary);
  if (recon.name() != "No coverage" && recon.name() != "Null") {
    recon += PvlKeyword("Type", "Reconstructed");
    result.addGroup(recon);
  }

  PvlGroup predicted = AddSelection(currFile, startOffset, endOffset, boundary, DBL_MAX);
  if (predicted.name() != "No coverage" && predicted.name() != "Null") {
    predicted += PvlKeyword("Type", "Predicted");
    result.addGroup(predicted);
  }
}


/**
 * Reads the delivery date out of a kernel's comment area.
 *
 * Projects that deliver mixed reconstructed and predicted data in a single kernel record the date
 * the source ephemeris was provided by the flight dynamics team in the comments. The data before
 * that date is reconstructed and the data after it is predicted. This is meant to support TGO 
 * Casis which has a delivery date in the comments of its SPK kernels. Anything after that 
 * delivery date is predicted and anything before it is reconstructed.
 *
 * @param fileIn The kernel to read
 *
 * @return double The delivery date as an ephemeris time
 *
 * @throws Isis::iException::Message
 */
double SpiceDbGen::DeliveryDate(FileName fileIn) {
  NaifStatus::CheckErrors();

  QString tmp = fileIn.expanded();
  SpiceQL::load(tmp.toLatin1().data());

  SpiceChar fileType[32], source[2048];
  SpiceInt handle;
  SpiceBoolean found;
  kinfo_c(tmp.toLatin1().data(), 32, 2048, fileType, source, &handle, &found);

  QString dateString = "";

  if (found == SPICETRUE) {
    // The date follows the phrase, which itself may be broken across two comment lines, so each
    // line is tested joined to the one before it as well as on its own
    QRegularExpression re("Flight Dynamics team on\\s+"
                          "([A-Z][a-z]+\\s+\\d{1,2},\\s*\\d{4})");
    SpiceChar commnt[1001];
    SpiceBoolean done(SPICEFALSE);
    SpiceInt n;
    QString prev = "";

    while (!done) {
      dafec_c(handle, 1, sizeof(commnt), &n, commnt, &done);
      QString cmmt(commnt);

      if (dateString.isEmpty()) {
        QRegularExpressionMatch match = re.match(prev.simplified() + " " + cmmt.simplified());
        if (match.hasMatch()) {
          dateString = match.captured(1);
        }
      }
      prev = cmmt;
    }
  }

  unload_c(tmp.toLatin1().data());
  NaifStatus::CheckErrors();

  if (dateString.isEmpty()) {
    QString message = "Could not find a delivery date in the comments of ["
                      + fileIn.original() + "]. A kernel cannot be split into reconstructed and "
                      "predicted coverage without one.";
    throw IException(IException::User, message, _FILEINFO_);
  }

  SpiceDouble et;
  str2et_c(dateString.toLatin1().data(), &et);
  NaifStatus::CheckErrors();

  return et;
}


/**
  * Essenetially a method call to the underlying QDir class which will filter
  * the files needed by Direct(). Files are returned in order of the time they
  * were most recently modified, the oldest file being first and the most
  * recently modified being last.
  *
  * @param location  The directory in which the method searches for files.
  *
  * @param filter    The regular expression used to match files of a
  *                  particular quality. This parameter is important, since it
  *                  is necessary to distinguish between kernels of different
  *                  qualities and/or different missions that may be placed in
  *                  the same directory.
  *
  * @return QStringList
  *
  */
QStringList SpiceDbGen::GetFiles(FileName location, QString filter) {
  filter.remove("\\");
  QDir dir(location.expanded(), filter,
           QDir::Name, QDir::Files);
  return dir.entryList();
}


/**
 * Sets the desired time coverage level of the Spice database.
 *
 * @param level The desired time coverage level. May be either Segment or
 *              Interval.
 */
void SpiceDbGen::setCoverageLevel(QString level) {
  m_coverageLevel = level;
}


/**
  * Format a single kernel file to include the file.
  *
  * @param fileIn     The file name being added
  *
  * @param clipStart  Coverage before this ephemeris time is excluded
  *
  * @param clipStop   Coverage after this ephemeris time is excluded
  *
  * @return PvlGroup
  *
  * @throws Isis::iException::Message
  */
PvlGroup SpiceDbGen::AddSelection(FileName fileIn, double startOffset, double endOffset,
                                  double clipStart, double clipStop) {
  NaifStatus::CheckErrors();

  //finalize the filename so that it may be used in spice routines
  QString tmp = fileIn.expanded();
  //  const char* file = fileIn.expanded().c_str();
  SpiceQL::load(tmp.toLatin1().data());
  SpiceChar fileType[32], source[2048];
  SpiceInt handle;
  QString instrument = "";
  QString startoffset = "";
  QString endoffset = "";

  SpiceBoolean found;
  kinfo_c(tmp.toLatin1().data(), 32, 2048, fileType, source, &handle, &found);
  QString currFile = fileType;

  if (found == SPICETRUE) {
    SpiceChar commnt[1001];
    SpiceBoolean done(SPICEFALSE);
    SpiceInt n;

    // extract all comments of kernel
    while (!done) {
      dafec_c(handle, 1, sizeof(commnt), &n, commnt, &done);
      QString cmmt(commnt);

      // Grab Instrument and Offset information, if exists
      int instPos = 0;
      if ( (instPos = cmmt.indexOf("Instrument:", instPos, Qt::CaseInsensitive)) != -1 ) {
        instrument = cmmt.remove(" ").split(":")[1];
      }
      int startPos = 0;
      if ( (startPos = cmmt.indexOf("StartOffset:", startPos, Qt::CaseInsensitive)) != -1 ) {
        startoffset = cmmt.remove(" ").split(":")[1];
      }
      int endPos = 0;
      if ( (endPos = cmmt.indexOf("EndOffset:", endPos, Qt::CaseInsensitive)) != -1 ) {
        endoffset = cmmt.remove(" ").split(":")[1];
      }
    }
  }

  //create a spice cell capable of containing all the objects in the kernel.
  SPICEINT_CELL(currCell, 1000);
  //this resizing is done because otherwise a spice cell will append new data
  //to the last "currCell"
  ssize_c(0, &currCell);
  ssize_c(1000, &currCell);

  //select which spice coverage routine to use. If a text kernel is detected, it
  //will be returned here and weeded out at the end of Direct(). This helps
  //to protect the user from inadvertently adding "." and ".." to their filters
  if (currFile == "SPK") {
    spkobj_c(tmp.toLatin1().data(), &currCell);
  }
  else if (currFile == "CK") {
    ckobj_c(tmp.toLatin1().data(), &currCell);
  }
  else if (currFile == "TEXT") {
    return PvlGroup("No coverage");
  }

  PvlGroup result;
  //iterate through every body in the kernel
  for(int bodyCount = 0 ; bodyCount < card_c(&currCell) ; bodyCount++) {
    //get the NAIF body code
    int body = SPICE_CELL_ELEM_I(&currCell, bodyCount);

    //only provide coverage for negative NAIF codes
    //(Positive codes indicate planetary bodies, negatives indicate
    // spacecraft and instruments)
    if (body < 0) {
      NaifStatus::CheckErrors();

      //find the correct coverage window
      if (currFile == "SPK") {
        SPICEDOUBLE_CELL(cover, 200000);
        ssize_c(0, &cover);
        ssize_c(200000, &cover);
        spkcov_c(tmp.toLatin1().data(), body, &cover);

        NaifStatus::CheckErrors();

        result = FormatIntervals(cover, currFile, startOffset, endOffset, clipStart, clipStop);
      }
      else if (currFile == "CK") {
        //  200,000 is the max coverage window size for a CK kernel
        SPICEDOUBLE_CELL(cover, 200000);
        ssize_c(0, &cover);
        ssize_c(200000, &cover);

        // A SPICE SEGMENT is composed of SPICE INTERVALS
        if (QString::compare(m_coverageLevel, "SEGMENT", Qt::CaseInsensitive) == 0 ) {
          ckcov_c(tmp.toLatin1().data(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);
        }
        else {
          ckcov_c(tmp.toLatin1().data(), body, SPICEFALSE, "INTERVAL", 0.0, "TDB", &cover);
        }

        NaifStatus::CheckErrors();
        result = FormatIntervals(cover, currFile, startOffset, endOffset, clipStart, clipStop);
      }
    }
  }

  // add instrument and timing offsets only if timing offsets found in comments
  if (!startoffset.isEmpty() || !endoffset.isEmpty()) {
    result += PvlKeyword("Instrument", instrument);
    if(!startoffset.isEmpty()){
      result += PvlKeyword("StartOffset", startoffset);
    }
    if(!endoffset.isEmpty()){
      result += PvlKeyword("EndOffset", endoffset);
    }
  }

  QString outFile = fileIn.originalPath();
  result += PvlKeyword("File", outFile + "/" + fileIn.name());

  NaifStatus::CheckErrors();

  // Unfurnishes tmp file to prevent file table overflow
  unload_c(tmp.toLatin1().data());

  return result;
}


PvlGroup SpiceDbGen::FormatIntervals(SpiceCell &coverage, QString type,
                                     double startOffset, double endOffset,
                                     double clipStart, double clipStop) {
  NaifStatus::CheckErrors();

  bool clipped = (clipStart > -DBL_MAX) || (clipStop < DBL_MAX);

  // Restrict the coverage to the requested interval, which is how a kernel holding more than one
  // quality of data is divided into a selection per quality
  SPICEDOUBLE_CELL(clip, 200000);
  SPICEDOUBLE_CELL(bounds, 2);
  if (clipped) {
    ssize_c(0, &bounds);
    ssize_c(2, &bounds);
    wninsd_c(clipStart, clipStop, &bounds);

    ssize_c(0, &clip);
    ssize_c(200000, &clip);
    wnintd_c(&coverage, &bounds, &clip);
    NaifStatus::CheckErrors();
  }
  SpiceCell &window = clipped ? clip : coverage;

  PvlGroup result(type);
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&window) / 2;
  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  for(int j = 0;  j < niv;  j++) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&window, j, &begin, &end);
    //Convert the endpoints to TDB calendar
    begin -= startOffset;
    end += endOffset;
    timout_c(begin, calForm, 35, begStr);
    timout_c(end, calForm, 35, endStr);

    result += PvlKeyword("Time", "(\"" + (QString)begStr +
                         "\", \"" + (QString)endStr + "\")");
  }

  NaifStatus::CheckErrors();

  // A selection with no Time keyword matches every image, so a kernel clipped down to nothing
  // must not produce one
  if (clipped && (niv == 0)) {
    return PvlGroup("No coverage");
  }

  return result;
}


void SpiceDbGen::FurnishDependencies(QList<FileName> sclks, QList<FileName> lsks,
                                     QList<FileName> extras) {
  NaifStatus::CheckErrors();

  // furnish the lsk files
  foreach (FileName lsk, lsks) {
    SpiceQL::load(lsk.expanded().toLatin1().data());
  }

  // furnish the sclk files
  foreach (FileName sclk, sclks) {
    SpiceQL::load(sclk.expanded().toLatin1().data());
  }

  // furnish the extra files
  foreach (FileName extra, extras) {
    SpiceQL::load(extra.expanded().toLatin1().data());
  }

  NaifStatus::CheckErrors();
}
