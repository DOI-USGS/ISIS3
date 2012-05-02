#include "Isis.h"
#include "ProcessImportPds.h"
#include "Application.h"
#include "iString.h"
#include "OriginalLabel.h"
#include "Statistics.h"

using namespace std;
using namespace Isis;
void IsisMain() {

  Isis::Preference::Preferences(true);
  void ReportError(iString err);

  // Test an IMAGE file
  try {
    cout << "Testing PDS file containing an ^IMAGE pointer" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("unitTest.img", "unitTest.img", plab);
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
    p.SetPdsFile("unitTest.lab", "", plab);
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
    p.SetPdsFile("clemuvvis.cub", "clemuvvis.cub", plab);
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
}

/**
 * Reports error messages from Isis:iException without full paths of filenames
 * @param err Error string of iException
 * @author Jeannie Walldren
 * @internal
 *   @history 2011-08-05 Jeannie Backer - Copied from Cube class.
 */
void ReportError(iString err) {
  iString report = ""; // report will be modified error message
  iString errorLine = ""; // read message one line at a time
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

