#include "ControlNetVersioner.h"

#include <string>

#include <QString>
#include <QTime>

#include "ControlNet.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void TestNetwork(const QString &filename, bool printNetwork = true, bool pvlInput = false);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cerr << "Test ControlNetVersioner" << endl;
  TestNetwork("./reallyOldNetwork.net"); // No target
  TestNetwork("./reallyOldNetwork2.net"); // Really odd keywords with target
  TestNetwork("./oldNetwork.net"); // Another set of odd keywords
  TestNetwork("./oldNetwork2.net"); // Binary V1
  TestNetwork("./badNetwork.net"); // Corrupted (based off of oldNetwork2.net)
  TestNetwork("./semilarge.net", false);
  TestNetwork("./smallPvlTest.pvl", true, true); // network with rejected jigsaw points
}

void TestNetwork(const QString &filename, bool printNetwork, bool pvlInput) {
  cerr << "Reading: " << filename << "...\n\n";
  FileName networkFileName(filename);

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
    test = ControlNetVersioner::Read(networkFileName);

    if(printNetwork) {
      cerr << "Converted directly to Pvl:" << endl;
      Pvl pvlVersion(test->toPvl());
      cerr << pvlVersion << endl;
      pvlVersion.write("./tmp.pvl");
    }

    // Test the latest binary read/write and Pvl conversion
    cerr << "Write the network and re-read it..." << endl;
    ControlNetVersioner::Write(FileName("./tmp"), *test);
          
    try {
      test2 = ControlNetVersioner::Read(FileName("./tmp"));
    }
    catch(IException &e) {
      remove("./tmp");
      throw;
    }

    cerr << "After reading and writing to a binary form does Pvl match?"
         << endl;
    if(printNetwork) {
      Pvl pvlVersion2(test2->toPvl());
      pvlVersion2.write("./tmp2.pvl");
      if(system("cmp ./tmp.pvl ./tmp2.pvl")) {
        cerr << "Reading/Writing results in Pvl differences!" << endl;
      }
      else {
        cerr << "Conversion to Pvl stays consistent" << endl;
      }
    }

    ControlNetVersioner::Write(FileName("./tmp2"), *test2);
    if(system("cmp ./tmp ./tmp2")) {
      cerr << "Reading/Writing control network results in binary differences!"
           << endl;
    }
    else {
      cerr << "Reading/Writing control network is consistent" << endl;
    }

    if (pvlInput) {

      LatestControlNetFile * cNet2 = NULL;
      
      cerr << "Check conversions between the binary format and the pvl format." << endl;
      /*
       * When the input is a pvl, ./tmp is the binary form of the initial input. (pvl1->bin1)
       * Furthermore, ./tmp.pvl is the first binary conversion reverted back to pvl.
       * (pvl1->bin1->pvl2)
       * cNet1 is the binary version of the second pvl. (pvl1->bin1->pvl2->bin2)
       *
       *                                  a       b       c
       *                            (pvl1 -> bin1 -> pvl2 -> bin2)
       * 
       * if (pvl1 != pvl2)
       *        a or b is broken but we don't know which yet
       *        if(bin1 != bin2)
       *                bin->pvl is broken (b) because the error happened after bin1 was created.
       *        else
       *                pvl-bin is broken (a/c) because the error happened before bin1 was created
       *                        and was propagated to bin2 via c.
       * else
       *        The conversions are up to date and correct because neither a nor b broke.
       *
       * 
       */
      cNet2 = ControlNetVersioner::Read(FileName("./tmp.pvl"));

      ControlNetVersioner::Write(FileName("./tmpCNet2"), *cNet2);

      //if there are differences between the pvls.
      QString cmd = "diff -EbB --suppress-common-lines " + filename + " ./tmp.pvl";
      if(system(cmd.toStdString().c_str())) {
        
        //if the binary files are different.
        if(system("diff -EbB --suppress-common-lines ./tmp ./tmpCNet2")){
          cerr << "The conversion from binary to pvl is incorrect." << endl;
        }
        else {
          cerr << "The conversion from pvl to binary is incorrect." << endl;
        }
      }
      else {
        cerr << "The conversion methods for pvl->bin and bin->pvl are correct." << endl;
      }

      remove("./tmpCNet2");
      delete cNet2;
      cNet2 = NULL;
    }

    remove("./tmp");
    remove("./tmp2");

    if(printNetwork) {
      remove("./tmp.pvl");
      remove("./tmp2.pvl");
    }
  }
  catch(IException &e) {
    QStringList errors = e.toString().split("\n");
    errors.removeLast();
    cerr << errors.join("\n") << endl;
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

