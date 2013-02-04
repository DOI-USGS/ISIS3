#include "Isis.h"
#include "ProcessImportPds.h"
#include "Application.h"
#include "IString.h"
#include "OriginalLabel.h"
#include "Statistics.h"

using namespace std;
using namespace Isis;
/**
 * @internal
 *   @history 2012-05-08 Tracie Sucharski - Moved test data to /usgs/cpks/mer/testData and
 *                         /usgs/cpkgs/clementine1/testData.  Added test for invalid label.
 */
void IsisMain() {

  Isis::Preference::Preferences(true);
  void ReportError(IString err);

  // Test an IMAGE file
  try {
    cout << "Testing PDS file containing an ^IMAGE pointer" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("$mer/testData/mer.img", "$mer/testData/mer.img", plab);
    p.SetOutputCube("TO");
    p.StartProcess();
    p.EndProcess();

    cout << plab << endl;
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    string file = Isis::Application::GetUserInterface().GetFileName("TO");
    Isis::Cube *cube = p2.SetInputCube(file, att);
    Isis::Statistics *stat = cube->getStatistics();
    cout << stat->Average() << endl;
    cout << stat->Variance() << endl;
    p2.EndProcess();
    Isis::OriginalLabel ol(file);
    Isis::Pvl label = ol.ReturnLabels();
    cout << label << endl;
    remove(file.c_str());
  }
  catch(Isis::IException &e) {
    e.print();
  }

  // Test a QUBE file
  try {
    cout << endl;
    cout << "Testing PDS file containing a ^QUBE pointer" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("$mer/testData/mer.lab", "", plab);
    p.SetOutputCube("TO");
    p.StartProcess();
    p.OmitOriginalLabel();
    p.EndProcess();

    cout << plab << endl;
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    string file = Isis::Application::GetUserInterface().GetFileName("TO");
    Isis::Cube *cube = p2.SetInputCube(file, att);
    Isis::Statistics *stat = cube->getStatistics();
    cout << stat->Average() << endl;
    cout << stat->Variance() << endl;
    p2.EndProcess();

    // Check input file error
    try {
      Isis::OriginalLabel ol(file);
    }
    catch(Isis::IException &e) {
      ReportError(e.toString());
    }
    remove(file.c_str());
  }
  catch(Isis::IException &e) {
    e.print();
  }

  // Test an Isis2 file
  try {
    cout << endl;
    cout << "Testing Isis2 file" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("$clementine1/testData/clemuvvis_Isis2.cub",
                 "$clementine1/testData/clemuvvis_Isis2.cub", plab);
    p.SetOutputCube("TO");
    p.StartProcess();
    Isis::Pvl ilab;
    p.TranslateIsis2Labels(ilab);
    p.EndProcess();

    cout << ilab << endl;
    string file = Isis::Application::GetUserInterface().GetFileName("TO");
    remove(file.c_str());
  }
  catch(Isis::IException &e) {
    e.print();
  }

  try{// this file is saved locally since it is not needed in the data area for 
      // the rest of isis
    cout << "Testing PDS file containing an ^IMAGE pointer and ^TABLE pointer" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("data/pdsImageWithTables.lbl", "data/pdsImageWithTables.img", plab);
    p.SetOutputCube("TO");
    p.ImportTable("SUN_POSITION_TABLE");
    p.StartProcess();
    p.EndProcess();
    
    cout << plab << endl;
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    string file = Isis::Application::GetUserInterface().GetFileName("TO");
    Isis::Cube *cube = p2.SetInputCube(file, att);
    Pvl isisCubeLab = *(cube->getLabel());
    (isisCubeLab.FindObject("IsisCube").FindObject("Core")["StartByte"]).SetValue("");
    (isisCubeLab.FindObject("Table")["StartByte"]).SetValue("");
    (isisCubeLab.FindObject("Table")["Bytes"]).SetValue("");
    (isisCubeLab.FindObject("History")["StartByte"]).SetValue("");
    (isisCubeLab.FindObject("History")["Bytes"]).SetValue("");
    (isisCubeLab.FindObject("OriginalLabel")["StartByte"]).SetValue("");
    (isisCubeLab.FindObject("OriginalLabel")["Bytes"]).SetValue("");
    cout << isisCubeLab << endl;
    Isis::Statistics *stat = cube->getStatistics();
    cout << stat->Average() << endl;
    cout << stat->Variance() << endl;
    p2.EndProcess();
    Isis::OriginalLabel ol(file);
    Isis::Pvl label = ol.ReturnLabels();
    cout << label << endl;
    remove(file.c_str());
  }
  catch(Isis::IException &e) {
    e.print();
  }

  //  Test an invalid label file
  try {
    cout << endl;
    cout << "Testing file with invalid Pds label" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("$galileo/testData/1213r.img", "$galileo/testData/1213r.img", plab);
  }
  catch (Isis::IException &e) {
    e.print();
  }
}

/**
 * Reports error messages from Isis:iException without full paths of filenames
 * @param err Error string of iException
 * @author Jeannie Walldren
 * @internal
 *   @history 2011-08-05 Jeannie Backer - Copied from Cube class.
 */
void ReportError(IString err) {
  IString report = ""; // report will be modified error message
  IString errorLine = ""; // read message one line at a time
  FileName expandedfile;
  while(err != "") {
    // pull off first line
    errorLine = err.Token("\n");
    while(errorLine != "") {
      size_t openBrace = errorLine.find('[');
      if(openBrace != string::npos) {
        // if open brace is found, look to see if a filename is inside (indicated by '/')
        if(errorLine.at(openBrace + 1) == '/') {
          // add message up to and including [
          report += errorLine.Token("[");
          report += "[";
          // read entire path into FileName object
          expandedfile = errorLine.Token("]");
          report += expandedfile.name(); // only report base name, rather than fully expanded path
          report += "]";
        }
        else {
          // not a filename inside braces, add message up to and including ]
          report += errorLine.Token("]");
          report += "]";
          continue;
        }
      }
      else {
        // no more braces are found, add rest of error message
        report += errorLine;
        break;
      }
    }
    report += "\n";
  }
  cout << report << endl;
}

