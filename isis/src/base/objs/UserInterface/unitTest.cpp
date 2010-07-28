#include <iostream>
#include "UserInterface.h"
#include "iException.h"
#include "Preference.h"
#include "Filename.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::UserInterface ..." << endl;

  Isis::iString unitTestXml = Isis::Filename("unitTest.xml").Expanded();
  string highpass = Isis::Filename("$ISISROOT/src/base/apps/highpass/highpass.xml").Expanded();

  try {
    cout << "Basic FROM/TO Test" << endl;
    {
      const int myArgc = 3;
      char *myArgv[myArgc] = {"unitTest", "from=input.cub", "to=output.cub"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing param= value Format" << endl;
    {
      const int myArgc = 7;
      char *myArgv[myArgc] = {"highpass", "from=dog", "to=biscuit", "line=", "3", "samp=", "3"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing No Arguments (Defaults)" << endl;
    {
      const int myArgc = 1;
      char *myArgv[myArgc] = {"unitTest"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing Basic Array Argument" << endl;
    {
      const int myArgc = 7;
      char *myArgv[myArgc] = {"highpass", "from=dog", "to=(biscuit,bread)", "line=", "3", "samp=", "3"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
      vector<string> vals;
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      ";
      ui.GetAsString("TO", vals);

      for(unsigned int i = 0; i < vals.size(); i++) {
        if(i != 0) cout << ",";

        cout << vals[i];
      }

      cout << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing Common Array Argument" << endl;
    {
      const int myArgc = 7;
      char *myArgv[myArgc] = {"highpass", "from=dog",
                              "to=( \"dog\" , \"cat\", \" cow \", 'frog')",
                              "line=", "3", "samp=", "3"
                             };

      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
      vector<string> vals;
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << endl;
      ui.GetAsString("TO", vals);

      for(unsigned int i = 0; i < vals.size(); i++) {
        cout << " >> '" << vals[i] << "'" << endl;
      }

      cout << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing Complicated Array Argument" << endl;
    {
      const int myArgc = 7;
      char *myArgv[myArgc] = {"highpass", "from=dog",
                              "to=(biscuit\\\\,,'bread,',\",b,\\\\,iscuit2,\"\\,,)",
                              "line=", "3", "samp=", "3"
                             };

      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
      vector<string> vals;
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << endl;
      ui.GetAsString("TO", vals);

      for(unsigned int i = 0; i < vals.size(); i++) {
        cout << " >> " << vals[i] << endl;
      }

      cout << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing param = value Format" << endl;
    {
      const int myArgc = 13;
      char *myArgv[myArgc] = {"highpass", "from", "=", "dog", "to", "=", "bread", "line", "=", "3", "samp", "=", "3"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
      vector<string> vals;
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing Space in Argument Value" << endl;
    {
      const int myArgc = 3;
      char *myArgv[myArgc] = {"unitTest", "from=input file.cub", "to=output.cub"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing unitTest v. ./unitTest for GUI" << endl;
    {
      const int myArgc = 1;
      char *myArgv[myArgc] = {"unitTest"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    {
      const int myArgc = 1;
      char *myArgv[myArgc] = {"./unitTest"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    {
      cout << "Starting Batchlist Test" << endl;
      const int myArgc = 4;
      char *myArgv[myArgc] = {"unitTest", "from=$1", "to=$2", "-batchlist=unitTest.lis"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      for(int i = 0; i < ui.BatchListSize(); i++) {
        ui.SetBatchList(i);
        cout << "FROM:    " << ui.GetAsString("FROM") << endl;
        cout << "TO:      " << ui.GetAsString("TO") << endl;
        cout << "GUI:     " << ui.IsInteractive() << endl;
        cout << endl;
      }
      cout << "Finished Batchlist Test" << endl;
      cout << endl;
    }

    cout << "Testing =value" << endl;
    try {
      const int myArgc = 4;
      char *myArgv[myArgc] = {"unitTest", "=input.cub", "to", "=output.cub"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    cout << "Testing param =value" << endl;
    try {
      const int myArgc = 10;
      char *myArgv[myArgc] = {"highpass", "from=dog", "to", "=bread", "line", "=", "3", "samp", "=", "3"};

      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
      vector<string> vals;
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    cout << "Testing unterminated array-value quote" << endl;
    try {
      const int myArgc = 2;
      char *myArgv[myArgc] = {"./unitTest", "from=(\"hello)"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    cout << "Testing array-value ending in backslash" << endl;
    try {
      const int myArgc = 2;
      char *myArgv[myArgc] = {"./unitTest", "from=(hello)\\"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    try {
      const int myArgc = 2;
      char *myArgv[myArgc] = {"./unitTest", "-restore=unitTest.par"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    try {
      const int myArgc = 2;
      char *myArgv[myArgc] = {"$ISISROOT/src/base/apps/highpass/highpass", "bogus=parameter"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    try {
      const int myArgc = 2;
      char *myArgv[myArgc] = {"$ISISROOT/src/base/apps/highpass/highpass", "-restore=junk.par"};
      int myArgcQ = myArgc;
      Isis::UserInterface ui(highpass, myArgcQ, myArgv);
    }
    catch(Isis::iException &e) {
      e.Report(false);
    }
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
}
