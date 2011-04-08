#include "ControlNetVersioner.h"

#include <string>

#include <QTime>

#include "ControlNet.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void TestNetwork(const string &filename, bool printNetwork = true);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cerr << "Test ControlNetVersioner" << endl;
  TestNetwork("./reallyOldNetwork.net"); // No target
  TestNetwork("./reallyOldNetwork2.net"); // Really odd keywords with target
  TestNetwork("./oldNetwork.net"); // Another set of odd keywords
  TestNetwork("./oldNetwork2.net"); // Binary V1
  TestNetwork("./badNetwork.net"); // Corrupted (based off of oldNetwork2.net)
  TestNetwork("./semilarge.net", false);
}


void TestNetwork(const string &filename, bool printNetwork) {
  cerr << "Reading: " << filename << "...\n\n";
  Filename networkFilename(filename);

  LatestControlNetFile * test = NULL;
  LatestControlNetFile * test2 = NULL;
  try {

    // If we're reading in a Pvl file, this will call the Pvl update cycle, then
    //   convert to binary, then convert back to Pvl.
    // If we're reading in a binary file, this will call the binary read, then
    //   convert to Pvl, then update, then convert to binary, and back to pvl.
    //   The reason for the intermediate Pvl is described in
    //   ControlNetVersioner.h.
    cerr << "Read network..." << endl;
    QTime timer;
    timer.start();
    test = ControlNetVersioner::Read(networkFilename);
//     cerr << "Elapsed time to read: " << timer.elapsed() << "ms" << endl;

    if(printNetwork) {
      cerr << "Converted directly to Pvl:" << endl;
      Pvl pvlVersion(test->ToPvl());
      cerr << pvlVersion << endl;
      pvlVersion.Write("./tmp.pvl");
    }

    // Test the latest binary read/write and Pvl conversion
    cerr << "Write the network and re-read it..." << endl;
    ControlNetVersioner::Write(Filename("./tmp"), *test);

    try {
      QTime timer2;
      timer2.start();
      test2 = ControlNetVersioner::Read(Filename("./tmp"));
//       cerr << "Elapsed time to read: " << timer2.elapsed() << "ms" << endl;
    }
    catch(iException &e) {
      remove("./tmp");
      throw;
    }

    cerr << "After reading and writing to a binary form does Pvl match?"
         << endl;
    if(printNetwork) {
      Pvl pvlVersion2(test2->ToPvl());
      pvlVersion2.Write("./tmp2.pvl");
      if(system("cmp ./tmp.pvl ./tmp2.pvl")) {
        cerr << "Reading/Writing results in Pvl differences!" << endl;
      }
      else {
        cerr << "Conversion to Pvl stays consistent" << endl;
      }
    }

    ControlNetVersioner::Write(Filename("./tmp2"), *test2);
    if(system("cmp ./tmp ./tmp2")) {
      cerr << "Reading/Writing control network results in binary differences!"
           << endl;
    }
    else {
      cerr << "Reading/Writing control network is consistent" << endl;
    }

    remove("./tmp");
    remove("./tmp2");

    if(printNetwork) {
      remove("./tmp.pvl");
      remove("./tmp2.pvl");
    }
  }
  catch(iException &e) {
    e.Report(false);
    e.Clear();
  }

  if(test) {
    delete test;
    test = NULL;
  }

  if(test2) {
    delete test2;
    test2 = NULL;
  }

  cerr << endl;
}

