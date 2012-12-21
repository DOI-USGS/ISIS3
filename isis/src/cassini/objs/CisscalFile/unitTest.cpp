/**
 * @file
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
#include <stdio.h>
#include <vector>

#include <QFile>

#include "CisscalFile.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

/** 
 *  
 * @internal 
 * @history 2012-03-12 Tracie Sucharski - Replaced /tmp with $temporary 
 *  
 */
int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << endl << "Unit test for CisscalFile" << endl;
  cout << "--------------------------------------------" << endl;


// ----------------------------------------------------------------------------------

  QString testFile = "$temporary/CisscalFile.tmp";
  // setup test data
  QString testLines[8];

  // setup line test data
  testLines[0]  = "PDS_VERSION_ID    = PDS3";
  testLines[1]  = "RECORD_TYPE       = STREAM";
  testLines[2]  = "  ";
  testLines[3]  = "\\begindata";
  testLines[4]  = "data line 1";
  testLines[5]  = "data line 2";
  testLines[6]  = "data line 3: last line of data, next line (last in file) is empty";
  testLines[7] = "";

  // calc cumulative byte counts for each line - to check CisscalFile.Size()
  streamsize numBytes = 0;
  streamsize numBytesFiltered = 0;
  vector<QString> testLinesVector;

  for(int i = 0; i <= 7; i++) {
    numBytes += testLines[i].length() + strlen("\n");
    testLinesVector.push_back(testLines[i]);
  }

  numBytesFiltered = numBytes - numBytesFiltered;
  Isis::TextFile p(testFile, "overwrite", testLinesVector);
  p.Close();

// ----------------------------------------------------------------------------------

  cout << "1) Read valid data from " << testFile << endl;
  try {
    Isis::CisscalFile f(testFile);
    bool lineFound = false;
    QString line;
    // data line 1
    lineFound = f.GetLine(line);
    if (!lineFound) {
      cout << "First Line Not Found" << endl;
      return 0;
    }
    if (line != testLines[4]) {
      cout << " *** Failed to Find \"\\begindata\" Tag *** " << endl;
      cout << "   First line of data should be:   -> " << testLines[4] << " <-" << endl;
      cout << "   returned is: -> " << line << " <-" << endl;
      return 0;
    }
    cout << line << endl;
    // data line 2
    lineFound = f.GetLine(line);
    if (!lineFound) {
      cout << "Second Line Not Found" << endl;
      return 0;
    }
    if (line != testLines[5]) {
      cout << " *** Failed to Match Second Line *** " << endl;
      cout << "   First line of data should be:   -> " << testLines[5] << " <-" << endl;
      cout << "   returned is: -> " << line << " <-" << endl;
      return 0;
    }
    cout << line << endl;
    // data line 3
    lineFound = f.GetLine(line);
    if (!lineFound) {
      cout << "Third Line Not Found" << endl;
      return 0;
    }
    if (line != testLines[6]) {
      cout << " *** Failed to Match Third Line *** " << endl;
      cout << "   First line of data should be:   -> " << testLines[6] << " <-" << endl;
      cout << "   returned is: -> " << line << " <-" << endl;
      return 0;
    }
    cout << line << endl;
    // last line, empty
    lineFound = f.GetLine(line);
    if (!lineFound) {
      cout << "Last Line Not Found" << endl;
      return 0;
    }
    if (line != testLines[7]) {
      cout << " *** Failed to Match Last Line *** " << endl;
      cout << "   First line of data should be:   -> " << testLines[7] << " <-" << endl;
      cout << "   returned is: -> " << line << " <-" << endl;
      return 0;
    }
    cout << line << endl;
    // grab line beyond end of file
    lineFound = f.GetLine(line);
    if (lineFound) {
      cout << "Extra Line Found:   -> " << line << " <-" << endl;
      lineFound = f.GetLine(line);
      cout << "Next Line True?   -> " << lineFound << " <-" << endl;
      return 0;
    }
    f.Close();
    cout << "--------------------------------------------" << endl;
  }
  catch(Isis::IException &e) {
    e.print();
  }
//-----------------------------------------------------------------------------------
  cout << "2) Remove temp file -> " << testFile << " <-" << endl;
  if (!QFile::remove(FileName(testFile).expanded())) {
    cout << "*** Failed to remove tmp file: " << testFile << endl;
  }
  return 0;
}

