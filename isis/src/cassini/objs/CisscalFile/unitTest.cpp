#include <iostream>
#include <stdio.h>
#include <vector>
#include "CisscalFile.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main (int argc, char *argv[]) {
  Preference::Preferences(true);
  cout << endl << "Unit test for CisscalFile" << endl;
  cout << "--------------------------------------------" << endl;


// ----------------------------------------------------------------------------------
  
  string testFile = "/tmp/CisscalFile.tmp";
                                                    // setup test data
  string testLines[8];

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
  streamsize testLineBytes[8];
  streamsize numBytes = 0;
  streamsize numBytesFiltered = 0;
  vector<string> testLinesVector;
  
  for (int i=0; i<=7; i++) {
    numBytes += testLines[i].length() + strlen("\n");
    testLineBytes[i] = numBytes;
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
    string line;
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
    if(!lineFound) {
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
    if(!lineFound) {
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
    if(!lineFound) {
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
    cout << "--------------------------------------------" << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
//-----------------------------------------------------------------------------------
  cout << "2) Remove temp file -> " << testFile << " <-" << endl;
  if (remove(testFile.c_str())) {
    cout << "*** Failed to remove tmp file: " << testFile << endl;
  }
  return 0;
}

