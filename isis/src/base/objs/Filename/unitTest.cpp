#include <iostream>
#include <fstream>

#include <QDate>

#include "Filename.h"
#include "iException.h"
#include "Preference.h"
#include "ProgramLauncher.h"

using namespace std;
using namespace Isis;

void ReportError(iString err);
void TestHighestVersion(string name);

int main(int argc, char *argv[]) {
  void ReportError(iString err);
  Preference::Preferences(true);

  try {

    Preference::Preferences(true);

    Filename f("/path/base.ext+attr");

    cout << "Testing Basics ..." << endl;
    cout << "Original filename: " << "/path/base.ext+attr" << endl;
    cout << "Path:              " << f.Path() << endl;
    cout << "Name:              " << f.Name() << endl;
    cout << "Basename:          " << f.Basename() << endl;
    cout << "Extension:         " << f.Extension() << endl;
    cout << "Filename:          " << f.Expanded() << endl;
    cout << "Original path      " << f.OriginalPath() << endl;
    cout << endl;

    cout << "Testing Extension change ..." << endl;
    f.RemoveExtension();
    f.AddExtension("tmp");
    cout << "Filename:      " << f.Expanded() << endl;
    f.AddExtension("jpg");
    f.AddExtension("jpg");
    cout << "Filename:      " << f.Expanded() << endl;
    cout << endl;

    Filename fa("/path1/.path2/base.ext+attr");
    cout << "Testing path with a dot and extension ..." << endl;
    cout << "Original filename: " << "/path1/.path2/base.ext+attr" << endl;
    cout << "Path:              " << fa.Path() << endl;
    cout << "Name:              " << fa.Name() << endl;
    cout << "Basename:          " << fa.Basename() << endl;
    cout << "Extension:         " << fa.Extension() << endl;
    cout << "Filename:          " << fa.Expanded() << endl;
    cout << "Original path      " << fa.OriginalPath() << endl;
    cout << endl;

    Filename fb("/path1/pat.h2/base+attr");
    cout << "Testing path with dot and no extension ..." << endl;
    cout << "Original filename: " << "/path1/pat.h2/base+attr" << endl;
    cout << "Path:              " << fb.Path() << endl;
    cout << "Name:              " << fb.Name() << endl;
    cout << "Basename:          " << fb.Basename() << endl;
    cout << "Extension:         " << fb.Extension() << endl;
    cout << "Filename:          " << fb.Expanded() << endl;
    cout << "Original path      " << fb.OriginalPath() << endl;
    cout << endl;

    Filename fc("/.path1/path2/base");
    cout << "Testing path starting with a dot ..." << endl;
    cout << "Original filename: " << "/.path1/path2/base" << endl;
    cout << "Path:              " << fc.Path() << endl;
    cout << "Name:              " << fc.Name() << endl;
    cout << "Basename:          " << fc.Basename() << endl;
    cout << "Extension:         " << fc.Extension() << endl;
    cout << "Filename:          " << fc.Expanded() << endl;
    cout << "Original path      " << fc.OriginalPath() << endl;
    cout << endl;

    Filename fd("/.path1/path2/base.+attr");
    cout << "Testing file with a dot at the end ..." << endl;
    cout << "Original filename: " << "/.path1/path2/base.+attr" << endl;
    cout << "Path:              " << fd.Path() << endl;
    cout << "Name:              " << fd.Name() << endl;
    cout << "Basename:          " << fd.Basename() << endl;
    cout << "Extension:         " << fd.Extension() << endl;
    cout << "Filename:          " << fd.Expanded() << endl;
    cout << "Original path      " << fd.OriginalPath() << endl;
    cout << endl;

    Filename f2("/another/path/base.ex1.exten2.ext3");
    cout << "Testing file name with multiple extensions..." << endl;
    cout << "Original filename: " << "/path/base.ex1.exten2.ext3" << endl;
    cout << "Path:              " << f2.Path() << endl;
    cout << "Name:              " << f2.Name() << endl;
    cout << "Basename:          " << f2.Basename() << endl;
    cout << "Extension:         " << f2.Extension() << endl;
    cout << "Filename:          " << f2.Expanded() << endl;
    cout << "Original path      " << f2.OriginalPath() << endl;
    cout << endl;

    cout << "Testing environment variable expansion" << endl;
    Filename g("$base/testData/isisTruth.cub");
    cout << "Original filename: " << "$base/testData/isisTruth.cub" << endl;
    if(g.Exists()) {
      cout << "Filename was expanded correctly" << endl;
    }
    else {
      cout << "Filename was NOT expanded correctly" << endl;
    }
    cout << "Name:              " << g.Name() << endl;
    cout << "Basename:          " << g.Basename() << endl;
    cout << "Extension:         " << g.Extension() << endl;
    cout << "Original path      " << g.OriginalPath() << endl;
    cout << endl;

    cout << "Testing bad environment variable expansion" << endl;
    Filename h("/$BADENV/base.ext+attr");
    cout << "Original filename: " << "$BADENV/base.ext+attr" << endl;
    cout << "New filename:      " << h.Expanded() << endl;
    cout << "Path:              " << h.Path() << endl;
    cout << "Name:              " << h.Name() << endl;
    cout << "Basename:          " << h.Basename() << endl;
    cout << "Extension:         " << h.Extension() << endl;
    cout << "Original path      " << h.OriginalPath() << endl;
    cout << endl;

    cout << "Testing ISIS preference variable expansion" << endl;
    Filename g2("/$TEMPORARY/unitTest.cpp");
    cout << "Original filename: " << "/$TEMPORARY/unitTest.cpp" << endl;
    cout << "New filename:      " << g2.Expanded() << endl;
    cout << "Name:              " << g2.Name() << endl;
    cout << "Basename:          " << g2.Basename() << endl;
    cout << "Extension:         " << g2.Extension() << endl;
    cout << "Original path      " << g2.OriginalPath() << endl;
    cout << endl;

    // This code taken out because if a matching preference is not
    // found then it tries an environment variable
//    cout << "Testing bad ISIS preference variable expansion" << endl;
//    Filename h2("$BADPREF/name.ext+attr");
//    cout << "Original filename: " << "$BADPREF/name.ext+attr" << endl;
//    cout << "New filename:      " << h2.Expanded() << endl;
//    cout << "Path:              " << h2.Path() << endl;
//    cout << "Name:              " << h2.Name() << endl;
//    cout << "Basename:          " << h2.Basename() << endl;
//    cout << "Extension:         " << h2.Extension() << endl;
//    cout << "Original path      " << h2.OriginalPath() << endl;
//    cout << endl;

    cout << "Testing file name without a path" << endl;
    Filename i("unitTest.cpp");
    cout << "Original filename: " << "unitTest.cpp" << endl;
    if(i.Exists()) {
      cout << "Filename was expanded correctly" << endl;
    }
    else {
      cout << "Filename was NOT expanded correctly" << endl;
    }

// These two lines removed because the output from them contains
// the current working directory. Making the unitTest fail.
//    cout << "New filename:      " << i.Expanded() << endl;
//    cout << "Path:              " << i.Path() << endl;
    cout << "Name:              " << i.Name() << endl;
    cout << "Basename:          " << i.Basename() << endl;
    cout << "Extension:         " << i.Extension() << endl;
    cout << "Original path      " << i.OriginalPath() << endl;
    cout << endl;

    cout << "Testing file name with . as the path" << endl;
    Filename j("./unitTest.cpp");
    cout << "Original filename: " << "./unitTest.cpp" << endl;
    if(j.Exists()) {
      cout << "Filename was expanded correctly" << endl;
    }
    else {
      cout << "Filename was NOT expanded correctly" << endl;
    }

// These two lines removed because the output from them contains
// the current working directory. Making the unitTest fail.
//    cout << "New filename:      " << j.Expanded() << endl;
//    cout << "Path:              " << j.Path() << endl;
    cout << "Name:              " << j.Name() << endl;
    cout << "Basename:          " << j.Basename() << endl;
    cout << "Extension:         " << j.Extension() << endl;
    cout << "Original path      " << j.OriginalPath() << endl;
    cout << endl;

    {
      cout << "Testing file name with no path and no extension" << endl;
      Filename k("Makefile");
      cout << "Original filename: " << "Makefile" << endl;
      if(k.Exists()) {
        cout << "Filename was expanded correctly" << endl;
      }
      else {
        cout << "Filename was NOT expanded correctly" << endl;
      }
//      cout << "New filename:      " << k.Expanded() << endl;
//      cout << "Path:              " << k.Path() << endl;
      cout << "Name:              " << k.Name() << endl;
      cout << "Basename:          " << k.Basename() << endl;
      cout << "Extension:         " << k.Extension() << endl;
      cout << "Original path      " << k.OriginalPath() << endl;
      cout << endl;
    }

    {
      cout << "Testing file name with no path and only an extension" << endl;
      Filename k(".cub");
      cout << "Original filename: " << ".cub" << endl;
      cout << "Name:              " << k.Name() << endl;
      cout << "Basename:          " << k.Basename() << endl;
      cout << "Extension:         " << k.Extension() << endl;
      cout << "Original path      " << k.OriginalPath() << endl;
      cout << endl;
    }

    cout << "Testing filename operator= with a c++ string" << endl;
    Filename l("$temporary/uid/name.ext+16bit");
    string x = "/home/me/new.extension+0:255";
    l = x;
    cout << "Original filename: " << "/home/me/new.extension+0:255" << endl;
    cout << "New filename:      " << l.Expanded() << endl;
    cout << "Path:              " << l.Path() << endl;
    cout << "Name:              " << l.Name() << endl;
    cout << "Basename:          " << l.Basename() << endl;
    cout << "Extension:         " << l.Extension() << endl;
    cout << "Original path      " << l.OriginalPath() << endl;
    cout << endl;

    cout << "Testing filename operator= with a c string" << endl;
    Filename m("$temporary/uid/name.ext+16bit");
    m = "/home/me/new.extension+0:255";
    cout << "Original filename: " << "/home/me/new.extension+0:255" << endl;
    cout << "New filename:      " << m.Expanded() << endl;
    cout << "Path:              " << m.Path() << endl;
    cout << "Name:              " << m.Name() << endl;
    cout << "Basename:          " << m.Basename() << endl;
    cout << "Extension:         " << m.Extension() << endl;
    cout << "Original path      " << m.OriginalPath() << endl;
    cout << endl;

    cout << "Testing 1st temporary file name" << endl;
    Filename n("tttt", "tmp");
    cout << "Name and extension : " << "tttt, tmp" << endl;
    cout << "Name:                " << n.Name() << endl;
    cout << "Basename:            " << n.Basename() << endl;
    cout << "Extension:           " << n.Extension() << endl;
    cout << "Original path        " << n.OriginalPath() << endl;
    cout << endl;

    ofstream nstm;
    string n_str(n.Expanded());
    nstm.open(n_str.c_str(), std::ios::out);
    nstm.close();

    cout << "Testing 2nd temporary file name" << endl;
    Filename o("tttt", "tmp");
    cout << "Name and extension : " << "tttt, tmp" << endl;
    cout << "Name:                " << o.Name() << endl;
    cout << "Basename:            " << o.Basename() << endl;
    cout << "Extension:           " << o.Extension() << endl;
    cout << "Original path        " << o.OriginalPath() << endl;
    cout << endl;

    ofstream ostm;
    string o_str(o.Expanded());
    ostm.open(o_str.c_str(), std::ios::out);
    ostm.close();

    remove(n_str.c_str());
    remove(o_str.c_str());

    // Use the files created in the last two tests to
    // test the HighestVersion member
    ProgramLauncher::RunSystemCommand("touch tttt000001");
    ProgramLauncher::RunSystemCommand("touch tttt000001.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt000005.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt000006.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt000008.tmp");
    ProgramLauncher::RunSystemCommand("touch 1tttt000008.tmp");
    ProgramLauncher::RunSystemCommand("touch 2tttt000008.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt_0.tmp");

    Filename p("tttt??????.tmp");
    cout << "Testing HighestVersion for file " << p.Name() << endl;
    try {
      p.HighestVersion();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << "  " << p.Name() << endl;
    cout << endl;

    Filename q("tttt??????");
    cout << "Testing HighestVersion for file " << q.Name() << endl;
    try {
      q.HighestVersion();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << "  " << q.Name() << endl;
    cout << endl;

    Filename q2("?tttt000008.tmp");
    cout << "Testing HighestVersion for file " << q2.Name() << endl;
    try {
      q2.HighestVersion();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << "  " << q2.Name() << endl;
    cout << endl;

    TestHighestVersion("tttt_?.tmp");

    remove("tttt000001");
    remove("tttt000001.tmp");
    remove("tttt000005.tmp");
    remove("tttt000006.tmp");
    remove("tttt000008.tmp");
    remove("1tttt000008.tmp");
    remove("2tttt000008.tmp");
    remove("tttt_0.tmp");

    Filename r("tttt");
    cout << "Testing HighestVersion for file " << r.Name() << endl;
    try {
      r.HighestVersion();
    }
    catch(iException &error) {
      cout << "No version string in tttt" << endl;
      error.Clear();
    }
    cout << endl;

    Filename s("??tttt");
    cout << "Testing HighestVersion for file " << s.Name() << endl;
    try {
      s.HighestVersion();
    }
    catch(iException &error) {
      cout << "No version available for ??tttt" << endl;
      error.Clear();
    }
    cout << endl;
    ProgramLauncher::RunSystemCommand("touch junk06.tmp");
    ProgramLauncher::RunSystemCommand("touch junk09.tmp");

    Filename junk("junk?.tmp");
    junk.HighestVersion();
    cout << "Testing HighestVersion to expand 1 \"?\" into 2 digits" << endl;
    cout << junk.Name() << endl << endl;

    ProgramLauncher::RunSystemCommand("rm junk06.tmp");
    ProgramLauncher::RunSystemCommand("rm junk09.tmp");


    // Test Highest Version for date versioned files
    ProgramLauncher::RunSystemCommand("touch tttt05Sep2002.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt20Jan2010.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt14Apr2010.tmp");
    ProgramLauncher::RunSystemCommand("touch ttAPRtt22yy99.tmp");
    ProgramLauncher::RunSystemCommand("touch ttMARtt11yy00.tmp");
    ProgramLauncher::RunSystemCommand("touch ttFEBtt04yy01.tmp");
    ProgramLauncher::RunSystemCommand("touch ttMARtt072003.tmp");
    ProgramLauncher::RunSystemCommand("touch tt14ttNovember.tmp");
    ProgramLauncher::RunSystemCommand("touch tt2ttDecember.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt.tmp");

    TestHighestVersion("tttt{ddMMMyyyy}.tmp");
    TestHighestVersion("tt{MMM}tt{dd}yy{yy}.tmp");
    TestHighestVersion("tt{d}tt{MMM}.tmp");
    TestHighestVersion("tt{d}tt{MMMM}.tmp");
    TestHighestVersion("tt{dd}.tmp");
    TestHighestVersion("tttt{}.tmp");
    TestHighestVersion("tttt{dd}.tmp");

    remove("tttt05Sep2002.tmp");
    remove("tttt20Jan2010.tmp");
    remove("tttt14Apr2010.tmp");
    remove("ttAPRtt22yy99.tmp");
    remove("ttMARtt11yy00.tmp");
    remove("ttFEBtt04yy01.tmp");
    remove("ttMARtt072003.tmp");
    remove("tt14ttNovember.tmp");
    remove("tt2ttDecember.tmp");
    remove("tttt.tmp");


    // Use the files previously created to
    // test the NewVersion member
    ProgramLauncher::RunSystemCommand("touch tttt000001");
    ProgramLauncher::RunSystemCommand("touch tttt000001.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt000005.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt000006.tmp");
    ProgramLauncher::RunSystemCommand("touch tttt000008.tmp");
    ProgramLauncher::RunSystemCommand("touch 1tttt000008.tmp");
    ProgramLauncher::RunSystemCommand("touch 2tttt000008.tmp");

    Filename pNew("tttt??????.tmp");
    cout << "Testing NewVersion for file " << pNew.Name() << endl;
    try {
      pNew.NewVersion();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << "  " << pNew.Name() << endl;
    cout << endl;

    Filename qNew("tttt??????");
    cout << "Testing NewVersion for file " << qNew.Name() << endl;
    try {
      qNew.NewVersion();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << "  " << qNew.Name() << endl;
    cout << endl;

    Filename q2New("?tttt000008.tmp");
    cout << "Testing NewVersion for file " << q2New.Name() << endl;
    try {
      q2New.NewVersion();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << " " << q2New.Name() << endl;
    cout << endl;

    remove("tttt000001");
    remove("tttt000001.tmp");
    remove("tttt000005.tmp");
    remove("tttt000006.tmp");
    remove("tttt000008.tmp");
    remove("1tttt000008.tmp");
    remove("2tttt000008.tmp");

    // Test new version for date file.  Makes a file for today, which cannot be
    // printed as truth, so compare it with what we expect and print the result.
    Filename todayFilename("tttt{dd}tt{yyyy}tt{MMM}.tmp");
    cout << "Testing NewVersion for file " << todayFilename.Name() << endl;
    bool success = false;
    try {
      todayFilename.NewVersion();
      QDate today = QDate::currentDate();
      QString expected = today.toString("'tttt'dd'tt'yyyy'tt'MMM'.tmp'");
      success = todayFilename.Name() == expected.toStdString();
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << "Made today's filename: " << success << endl;
    cout << endl;

    Filename rNew("tttt");
    cout << "Testing NewVersion for file " << rNew.Name() << endl;
    try {
      rNew.NewVersion();
    }
    catch(iException &error) {
      cout << "No version string in tttt" << endl;
      error.Clear();
    }
    cout << endl;

    Filename sNew("??tttt");
    cout << "Testing NewVersion for file " << sNew.Name() << endl;
    sNew.NewVersion();
    cout << " " << sNew.Name() << endl;
    cout << endl;
//////////////////////////////////////////////////////////
    cout << "Testing Exists() for a file that should exist:" << endl;
    Filename t("$ISISROOT/src/Makefile");
    try {
      if(t.Exists()) {
        cout << "  The test file for \"Exists()\" was located" << endl;
      }
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << endl;

    cout << "Testing Exists() for a file that does NOT exist:" << endl;
    t.AddExtension("elifekaM");
    try {
      if(!t.Exists()) {
        cout << "  The test file for \"!Exists()\" was not located (this is correct)" << endl;
      }
    }
    catch(iException &error) {
      error.Report(false);
    }
    cout << endl;
  }
  catch(iException &e) {
    try {
      e.Report(true);
    }
    catch(...) {
      cout << "Unknown error in iException.Report" << endl;
    }
    exit(1);
  }
  catch(...) {
    cout << "Unknown error" << endl;
    exit(1);
  }

  // Test the makeDirectory method
  Filename d("$temporary/IsisFilenameTest");
  cout << "Testing MakeDirectory for " << d.OriginalPath() << d.Name() << endl;
  try {
    d.MakeDirectory();
    cout << "  The directory create succeed" << endl;
  }
  catch(iException &error) {
    error.Report(false);
  }
  cout << endl;

  // Test the makeDirectory method when the dir already exists
  cout << "Testing MakeDirectory for " << d.OriginalPath() << d.Name() << endl;
  try {
    d.MakeDirectory();
  }
  catch(iException &error) {
    ReportError(iString(error.Errors()) ); //shorten path
    error.Clear();
  }
  cout << endl;

  string d_str(d.Expanded());
  remove(d_str.c_str());

  return 0;
}

/**
 * Reports error messages from Isis:iException without full paths of filenames
 * @param err Error string of iException
 * @author Jeannie Walldren
 * @internal
 *   @history 2011-08-19 Jeannie Backer - Copied from Cube class.
 */
void ReportError(iString err) {
  iString report = ""; // report will be modified error message
  iString errorLine = ""; // read message one line at a time
  Filename expandedfile;
  while(err != "") {
    // pull off first line
    errorLine = err.Token("\n");
    while(errorLine != "") {
      size_t openBrace = errorLine.find('[');
      if(openBrace != string::npos) {
        // if open brace is found, look to see if a filename is inside (indicated by '/')
        if(errorLine.at(openBrace + 1) == '/') {
          // add message up to
          report += errorLine.Token("[");
          // and including [
          report += "[";
          // read entire path into Filename object
          expandedfile = errorLine.Token("]");
          // only keep the name of the immediate directory
          iString path = expandedfile.OriginalPath();
          while (path.find("/") != string::npos) {
            path.Token("/");
          }
          // only report immediate directory and file name, (rather than fully
          // expanded path since this may differ on each system
          report += "../" + path + "/" + expandedfile.Name();
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


void TestHighestVersion(string name) {
  Filename filename(name);
  cout << "Testing HighestVersion for file " << filename.Name() << endl;
  try {
    filename.HighestVersion();
    cout << "  " << filename.Name() << endl;
  }
  catch(iException &error) {
    cout << "No version available for " << name << endl;
    error.Clear();
  }
  cout << endl;
}

