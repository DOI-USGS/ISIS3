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
  char *myArgv[15];// = {"unitTest", "from=input.cub", "to=output.cub"};

  for(int i = 0; i < 15; i++)
    myArgv[i] = new char[128];

  try {
    cout << "Basic FROM/TO Test" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "unitTest");
      strcpy(myArgv[myArgc ++], "from=input.cub");
      strcpy(myArgv[myArgc ++], "to=output.cub");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing param= value Format" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "highpass");
      strcpy(myArgv[myArgc ++], "from=dog");
      strcpy(myArgv[myArgc ++], "to=biscuit");
      strcpy(myArgv[myArgc ++], "line=");
      strcpy(myArgv[myArgc ++], "3");
      strcpy(myArgv[myArgc ++], "samp=");
      strcpy(myArgv[myArgc ++], "3");

      Isis::UserInterface ui(highpass, myArgc, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing No Arguments (Defaults)" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "unitTest");

      int myArgcQ = myArgc;
      Isis::UserInterface ui(unitTestXml, myArgcQ, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing Basic Array Argument" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "highpass");
      strcpy(myArgv[myArgc ++], "from=dog");
      strcpy(myArgv[myArgc ++], "to=(biscuit,bread)");
      strcpy(myArgv[myArgc ++], "line=");
      strcpy(myArgv[myArgc ++], "3");
      strcpy(myArgv[myArgc ++], "samp=");
      strcpy(myArgv[myArgc ++], "3");

      Isis::UserInterface ui(highpass, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "highpass");
      strcpy(myArgv[myArgc ++], "from=dog");
      strcpy(myArgv[myArgc ++], "to=( \"dog\" , \"cat\", \" cow \", 'frog')");
      strcpy(myArgv[myArgc ++], "line=");
      strcpy(myArgv[myArgc ++], "3");
      strcpy(myArgv[myArgc ++], "samp=");
      strcpy(myArgv[myArgc ++], "3");

      Isis::UserInterface ui(highpass, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "highpass");
      strcpy(myArgv[myArgc ++], "from=dog");
      strcpy(myArgv[myArgc ++], "to=(biscuit\\\\,,'bread,',\",b,\\\\,iscuit2,\"\\,,)");
      strcpy(myArgv[myArgc ++], "line=");
      strcpy(myArgv[myArgc ++], "3");
      strcpy(myArgv[myArgc ++], "samp=");
      strcpy(myArgv[myArgc ++], "3");

      Isis::UserInterface ui(highpass, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "highpass");
      strcpy(myArgv[myArgc ++], "from");
      strcpy(myArgv[myArgc ++], "=");
      strcpy(myArgv[myArgc ++], "dog");
      strcpy(myArgv[myArgc ++], "to");
      strcpy(myArgv[myArgc ++], "=");
      strcpy(myArgv[myArgc ++], "bread");
      strcpy(myArgv[myArgc ++], "line");
      strcpy(myArgv[myArgc ++], "=");
      strcpy(myArgv[myArgc ++], "3");
      strcpy(myArgv[myArgc ++], "samp");
      strcpy(myArgv[myArgc ++], "=");
      strcpy(myArgv[myArgc ++], "3");

      Isis::UserInterface ui(highpass, myArgc, myArgv);
      vector<string> vals;
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing Space in Argument Value" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "unitTest");
      strcpy(myArgv[myArgc ++], "from=input file.cub");
      strcpy(myArgv[myArgc ++], "to=output.cub");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
      cout << "FROM:    " << ui.GetAsString("FROM") << endl;
      cout << "TO:      " << ui.GetAsString("TO") << endl;
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Testing unitTest v. ./unitTest for GUI" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "unitTest");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "./unitTest");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
      cout << "GUI:     " << ui.IsInteractive() << endl;
      cout << endl;
    }

    cout << "Starting Batchlist Test" << endl;
    {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "unitTest");
      strcpy(myArgv[myArgc ++], "from=$1");
      strcpy(myArgv[myArgc ++], "to=$2");
      strcpy(myArgv[myArgc ++], "-batchlist=unitTest.lis");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "unitTest");
      strcpy(myArgv[myArgc ++], "=input.cub");
      strcpy(myArgv[myArgc ++], "to");
      strcpy(myArgv[myArgc ++], "=output.cub");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "highpass");
      strcpy(myArgv[myArgc ++], "from=dog");
      strcpy(myArgv[myArgc ++], "to");
      strcpy(myArgv[myArgc ++], "=bread");
      strcpy(myArgv[myArgc ++], "line");
      strcpy(myArgv[myArgc ++], "=");
      strcpy(myArgv[myArgc ++], "3");
      strcpy(myArgv[myArgc ++], "samp");
      strcpy(myArgv[myArgc ++], "=");
      strcpy(myArgv[myArgc ++], "3");

      Isis::UserInterface ui(highpass, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "./unitTest");
      strcpy(myArgv[myArgc ++], "from=(\"hello)");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "./unitTest");
      strcpy(myArgv[myArgc ++], "from=(hello)\\");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "./unitTest");
      strcpy(myArgv[myArgc ++], "-restore=unitTest.par");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
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
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "$ISISROOT/src/base/apps/highpass/highpass");
      strcpy(myArgv[myArgc ++], "bogus=parameter");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
    }
    catch(Isis::iException &e) {
      e.Report(false);
      cout << endl;
    }

    try {
      int myArgc = 0;
      strcpy(myArgv[myArgc ++], "$ISISROOT/src/base/apps/highpass/highpass");
      strcpy(myArgv[myArgc ++], "-restore=junk.par");

      Isis::UserInterface ui(unitTestXml, myArgc, myArgv);
    }
    catch(Isis::iException &e) {
      e.Report(false);
    }
  }
  catch(Isis::iException &e) {
    e.Report(false);
  }
}
