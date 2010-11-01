/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/05/02 18:39:28 $
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

#include <iostream>
#include <algorithm>
#include "SpiceDbGen.h"
#include "NaifStatus.h"

using namespace std;
using namespace Isis;


const char *SpiceDbGen::calForm = "YYYY MON DD HR:MN:SC.### TDB ::TDB";
/**
 * Constructs a new SpiceDbGen.
 *
 * @param type The type of kernel to be processed. Either, "SPK" or "CK.
*/
SpiceDbGen::SpiceDbGen(iString type) {
  p_type = type;
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
 * @param filter    Vector of strings containing regular expression used to
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
 * @throws Isis::iException::Message
*/
PvlObject SpiceDbGen::Direct(iString quality, iString location,
                             std::vector<std::string> &filter) {
  PvlObject result;
  iString type = "none";

  for(unsigned int i = 0; i < filter.size(); ++i) {
    //Create a list of all of the files matching the current filter
    QStringList files = GetFiles(Filename(location), filter[i]);
    PvlObject pvlKernel("change");

    // Throw an error if no files are being added to this database for
    // this filter/regex
    if(files.size() == 0) {
      string message = "Your filter [" + location + "/" + filter[i] + "]"
                       + "has not detected any " + quality + " kernels";
      throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    }

    for(int fileNum = 0 ; fileNum < files.size() ; fileNum++) {
      Filename currFile = ((string) location + "/" + files[fileNum].toStdString());
      PvlGroup selection = AddSelection(currFile);
      selection += PvlKeyword("Type", quality);
      result.AddGroup(selection);
    }
  }

  //check each group to make sure it is the same type as others
  PvlObject::PvlGroupIterator grp = result.BeginGroup();
  while(grp != result.EndGroup()) {
    // The kernel did not have any time coverage, so ignore it
    if(grp->Name() == "No coverage" || grp->Name() == "Null") {
      result.DeleteGroup(grp->Name());
    }
    // This is used for the first time thru the while loop
    // DO NOT increment grp here
    else if(type == "none") {
      type = grp->Name();
    }
    else if(grp->Name() == type) {
      grp->SetName("Selection");
      grp++;
    }
    else {
      string message = "A kernel of type [" + grp->Name() + "] has been found in a directory for type [" + type + "]" ;
      throw Isis::iException::Message(Isis::iException::Programmer, message, _FILEINFO_);
      break;
    }
  }

  if(type == "SPK") {
    result.SetName("SpacecraftPosition");
  }
  else if(type == "CK") {
    result.SetName("SpacecraftPointing");
  }

  return result;
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
QStringList SpiceDbGen::GetFiles(Filename location, iString filter) {
  filter.Remove("\\");
  QDir dir(location.Expanded().c_str(), filter.c_str(),
           QDir::Name, QDir::Files);
  return dir.entryList();
}


/**
  * Format a single kernel file to include the file.
  *
  * @param fileIn   The file name being added
  *
  * @return PvlGroup
  *
  * @throws Isis::iException::Message
  */
PvlGroup SpiceDbGen::AddSelection(Filename fileIn) {
  NaifStatus::CheckErrors();

  //finalize the filename so that it may be used in spice routines
  std::string tmp = fileIn.Expanded();
//  const char* file = fileIn.Expanded().c_str();
  furnsh_c(tmp.c_str());
  SpiceChar fileType[32], source[2048];
  SpiceInt handle;

  SpiceBoolean found;
  kinfo_c(tmp.c_str(), 32, 2048, fileType, source, &handle, &found);
  iString currFile = fileType;

  //create a spice cell capable of containing all the objects in the kernel.
  SPICEINT_CELL(currCell, 1000);
  //this resizing is done because otherwise a spice cell will append new data
  //to the last "currCell"
  ssize_c(0, &currCell);
  ssize_c(1000, &currCell);

  //select which spice coverage routine to use. If a text kernel is detected, it
  //will be returned here and weeded out at the end of Direct(). This helps
  //to protect the user from inadvertently adding "." and ".." to their filters
  if(currFile == "SPK") {
    spkobj_c(tmp.c_str(), &currCell);
  }
  else if(currFile == "CK") {
    ckobj_c(tmp.c_str(), &currCell);
  }
  else if(currFile == "TEXT") {
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
    if(body < 0) {
      NaifStatus::CheckErrors();

      //find the correct coverage window
      if(currFile == "SPK") {
        //  2000 is the max coverage window size for an SPK kernel
        SPICEDOUBLE_CELL(cover, 2000);
        ssize_c(0, &cover);
        ssize_c(2000, &cover);
        spkcov_c(tmp.c_str(), body, &cover);

        NaifStatus::CheckErrors();

        result = FormatIntervals(cover, currFile);
      }
      else if(currFile == "CK") {
        //  200,000 is the max coverage window size for a CK kernel
        SPICEDOUBLE_CELL(cover, 200000);
        ssize_c(0, &cover);
        ssize_c(200000, &cover);
        ckcov_c(tmp.c_str(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);

        NaifStatus::CheckErrors();

        result = FormatIntervals(cover, currFile);
      }
    }
  }

  iString outFile = fileIn.OriginalPath();
  result += PvlKeyword("File", outFile + "/" + fileIn.Name());

  NaifStatus::CheckErrors();
  return result;
}


PvlGroup SpiceDbGen::FormatIntervals(SpiceCell &coverage, string type) {
  NaifStatus::CheckErrors();

  PvlGroup result(type);
  SpiceChar begStr[32], endStr[32];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;
  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  for(int j = 0;  j < niv;  j++) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, j, &begin, &end);
    //Convert the endpoints to TDB calendar
    timout_c(begin, calForm, 32, begStr);
    timout_c(end, calForm, 32, endStr);
    result += PvlKeyword("Time", "(\"" + (string)begStr +
                         "\", \"" + (string)endStr + "\")");
  }

  NaifStatus::CheckErrors();

  return result;
}


void SpiceDbGen::FurnishDependencies(string sclk, string lsk) {
  NaifStatus::CheckErrors();

  //furnish the lsk file
  furnsh_c(lsk.c_str());

  //get the sclk, if such a file was specified
  if(sclk != "") {
    furnsh_c(sclk.c_str());
  }

  NaifStatus::CheckErrors();
}
