#include <iostream>
#include <fstream>

#include "Preference.h"
#include "iException.h"
#include "Filename.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {

    Isis::Preference::Preferences(true);

    Isis::Filename f("/path/base.ext+attr");

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

    Isis::Filename fa("/path1/.path2/base.ext+attr");
    cout << "Testing path with a dot and extension ..." << endl;
    cout << "Original filename: " << "/path1/.path2/base.ext+attr" << endl;
    cout << "Path:              " << fa.Path() << endl;
    cout << "Name:              " << fa.Name() << endl;
    cout << "Basename:          " << fa.Basename() << endl;
    cout << "Extension:         " << fa.Extension() << endl;
    cout << "Filename:          " << fa.Expanded() << endl;
    cout << "Original path      " << fa.OriginalPath() << endl;
    cout << endl;

    Isis::Filename fb("/path1/pat.h2/base+attr");
    cout << "Testing path with dot and no extension ..." << endl;
    cout << "Original filename: " << "/path1/pat.h2/base+attr" << endl;
    cout << "Path:              " << fb.Path() << endl;
    cout << "Name:              " << fb.Name() << endl;
    cout << "Basename:          " << fb.Basename() << endl;
    cout << "Extension:         " << fb.Extension() << endl;
    cout << "Filename:          " << fb.Expanded() << endl;
    cout << "Original path      " << fb.OriginalPath() << endl;
    cout << endl;

    Isis::Filename fc("/.path1/path2/base");
    cout << "Testing path starting with a dot ..." << endl;
    cout << "Original filename: " << "/.path1/path2/base" << endl;
    cout << "Path:              " << fc.Path() << endl;
    cout << "Name:              " << fc.Name() << endl;
    cout << "Basename:          " << fc.Basename() << endl;
    cout << "Extension:         " << fc.Extension() << endl;
    cout << "Filename:          " << fc.Expanded() << endl;
    cout << "Original path      " << fc.OriginalPath() << endl;
    cout << endl;

    Isis::Filename fd("/.path1/path2/base.+attr");
    cout << "Testing file with a dot at the end ..." << endl;
    cout << "Original filename: " << "/.path1/path2/base.+attr" << endl;
    cout << "Path:              " << fd.Path() << endl;
    cout << "Name:              " << fd.Name() << endl;
    cout << "Basename:          " << fd.Basename() << endl;
    cout << "Extension:         " << fd.Extension() << endl;
    cout << "Filename:          " << fd.Expanded() << endl;
    cout << "Original path      " << fd.OriginalPath() << endl;
    cout << endl;

    Isis::Filename f2("/another/path/base.ex1.exten2.ext3");
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
    Isis::Filename g("$base/testData/isisTruth.cub");
    cout << "Original filename: " << "$base/testData/isisTruth.cub" << endl;
    if (g.Exists()) {
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
    Isis::Filename h("/$BADENV/base.ext+attr");
    cout << "Original filename: " << "$BADENV/base.ext+attr" << endl;
    cout << "New filename:      " << h.Expanded() << endl;
    cout << "Path:              " << h.Path() << endl;
    cout << "Name:              " << h.Name() << endl;
    cout << "Basename:          " << h.Basename() << endl;
    cout << "Extension:         " << h.Extension() << endl;
    cout << "Original path      " << h.OriginalPath() << endl;
    cout << endl;

    cout << "Testing ISIS preference variable expansion" << endl;
    Isis::Filename g2("/$TEMPORARY/unitTest.cpp");
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
//    Isis::Filename h2("$BADPREF/name.ext+attr");
//    cout << "Original filename: " << "$BADPREF/name.ext+attr" << endl;
//    cout << "New filename:      " << h2.Expanded() << endl;
//    cout << "Path:              " << h2.Path() << endl;
//    cout << "Name:              " << h2.Name() << endl;
//    cout << "Basename:          " << h2.Basename() << endl;
//    cout << "Extension:         " << h2.Extension() << endl;
//    cout << "Original path      " << h2.OriginalPath() << endl;
//    cout << endl;

    cout << "Testing file name without a path" << endl;
    Isis::Filename i("unitTest.cpp");
    cout << "Original filename: " << "unitTest.cpp" << endl;
    if (i.Exists()) {
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
    Isis::Filename j("./unitTest.cpp");
    cout << "Original filename: " << "./unitTest.cpp" << endl;
    if (j.Exists()) {
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
      Isis::Filename k("Makefile");
      cout << "Original filename: " << "Makefile" << endl;
      if (k.Exists()) {
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
      Isis::Filename k(".cub");
      cout << "Original filename: " << ".cub" << endl;
      cout << "Name:              " << k.Name() << endl;
      cout << "Basename:          " << k.Basename() << endl;
      cout << "Extension:         " << k.Extension() << endl;
      cout << "Original path      " << k.OriginalPath() << endl;
      cout << endl;
    }

    cout << "Testing filename operator= with a c++ string" << endl;
    Isis::Filename l("/tmp/uid/name.ext+16bit");
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
    Isis::Filename m("/tmp/uid/name.ext+16bit");
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
    Isis::Filename n("tttt", "tmp");
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
    Isis::Filename o("tttt", "tmp");
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
    system ("touch tttt000001");
    system ("touch tttt000001.tmp");
    system ("touch tttt000005.tmp");
    system ("touch tttt000006.tmp");
    system ("touch tttt000008.tmp");
    system ("touch 1tttt000008.tmp");
    system ("touch 2tttt000008.tmp");

    Isis::Filename p ("tttt??????.tmp");
    cout << "Testing HighestVersion for file " << p.Name() << endl;
    try {
      p.HighestVersion ();
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << "  " << p.Name() << endl;
    cout << endl;

    Isis::Filename q ("tttt??????");
    cout << "Testing HighestVersion for file " << q.Name() << endl;
    try {
      q.HighestVersion ();
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << "  " << q.Name() << endl;
    cout << endl;

    Isis::Filename q2 ("?tttt000008.tmp");
    cout << "Testing HighestVersion for file " << q2.Name() << endl;
    try {
      q2.HighestVersion ();
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << "  " << q2.Name() << endl;
    cout << endl;


    remove ("tttt000001");
    remove ("tttt000001.tmp");
    remove ("tttt000005.tmp");
    remove ("tttt000006.tmp");
    remove ("tttt000008.tmp");
    remove ("1tttt000008.tmp");
    remove ("2tttt000008.tmp");

    Isis::Filename r ("tttt");
    cout << "Testing HighestVersion for file " << r.Name() << endl;
    try {
      r.HighestVersion ();
    }
    catch (Isis::iException &error) {
      cout << "No version string in tttt" << endl;
      error.Clear();
    }
    cout << endl;

    Isis::Filename s ("??tttt");
    cout << "Testing HighestVersion for file " << s.Name() << endl;
    try {
      s.HighestVersion ();
    }
    catch (Isis::iException &error) {
      cout << "No version available for ??tttt" << endl;
      error.Clear();
    }
    cout << endl;
    system ("touch junk06.tmp");
    system ("touch junk09.tmp");

    Isis::Filename junk("junk?.tmp");
    junk.HighestVersion();
    cout << "Testing HighestVersion to expand 1 \"?\" into 2 digits" <<endl;
    cout << junk.Name() <<endl <<endl;

    system("rm junk06.tmp");
    system("rm junk09.tmp");

// Use the files previously created to
// test the NewVersion member

    system ("touch tttt000001");
    system ("touch tttt000001.tmp");
    system ("touch tttt000005.tmp");
    system ("touch tttt000006.tmp");
    system ("touch tttt000008.tmp");
    system ("touch 1tttt000008.tmp");
    system ("touch 2tttt000008.tmp");

    Isis::Filename pNew ("tttt??????.tmp");
    cout << "Testing NewVersion for file " << pNew.Name() << endl;
    try {
      pNew.NewVersion ();
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << "  " << pNew.Name() << endl;
    cout << endl;

    Isis::Filename qNew ("tttt??????");
    cout << "Testing NewVersion for file " << qNew.Name() << endl;
    try {
      qNew.NewVersion ();
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << "  " << qNew.Name() << endl;
    cout << endl;

    Isis::Filename q2New ("?tttt000008.tmp");
    cout << "Testing NewVersion for file " << q2New.Name() << endl;
    try {
      q2New.NewVersion ();
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << " " << q2New.Name() << endl;
    cout << endl;


    remove ("tttt000001");
    remove ("tttt000001.tmp");
    remove ("tttt000005.tmp");
    remove ("tttt000006.tmp");
    remove ("tttt000008.tmp");
    remove ("1tttt000008.tmp");
    remove ("2tttt000008.tmp");

    Isis::Filename rNew ("tttt");
    cout << "Testing NewVersion for file " << rNew.Name() << endl;
    try {
      rNew.NewVersion ();
    }
    catch (Isis::iException &error) {
      cout << "No version string in tttt" << endl;
      error.Clear();
    }
    cout << endl;

    Isis::Filename sNew ("??tttt");
    cout << "Testing NewVersion for file " << sNew.Name() << endl;
    sNew.NewVersion();
    cout<< " " << sNew.Name() << endl;
    cout << endl;
//////////////////////////////////////////////////////////
    cout << "Testing Exists() for a file that should exist:" << endl;
    Isis::Filename t ("$ISISROOT/src/Makefile");
    try {
      if (t.Exists()) {
        cout << "  The test file for \"Exists()\" was located" << endl;
      }
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << endl;

    cout << "Testing Exists() for a file that does NOT exist:" << endl;
    t.AddExtension("elifekaM");
    try {
      if (!t.Exists()) {
        cout << "  The test file for \"!Exists()\" was not located (this is correct)" << endl;
      }
    }
    catch (Isis::iException &error) {
      error.Report(false);
    }
    cout << endl;
  }
  catch (Isis::iException &e) {
    try {
      e.Report(true);
    }
    catch (...) {
      cout << "Unknown error in Isis::iException.Report" << endl;
    }
    exit(1);
  }
  catch (...) {
    cout << "Unknown error" << endl;
    exit(1);
  }

  // Test the makeDirectory method
  Isis::Filename d ("/tmp/IsisFilenameTest");
  cout << "Testing MakeDirctory for " << d.Expanded() << endl;
  try {
    d.MakeDirectory ();
    cout << "  The directory create succeed" << endl;
  }
  catch (Isis::iException &error) {
    error.Report(false);
  }
  cout << endl;

  // Test the makeDirectory method when the dir already exists
  cout << "Testing MakeDirectory for " << d.Expanded() << endl;
  try {
    d.MakeDirectory ();
  }
  catch (Isis::iException &error) {
    error.Report(false);
  }
  cout << endl;

  string d_str(d.Expanded());
  remove (d_str.c_str());

  return 0;
}
