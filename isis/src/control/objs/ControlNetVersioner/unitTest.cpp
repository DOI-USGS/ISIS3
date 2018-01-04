#include "ControlNetVersioner.h"

#include <QDebug>
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
  qDebug() << "Test ControlNetVersioner";
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_reallyOldNetwork_PvlV0001.net"); // No target
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_reallyOldNetwork2_PvlV0001.net"); // Really odd keywords with target
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_oldNetwork_PvlV0001.net"); // Another set of odd keywords
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_oldNetwork2_ProtoV0001.net"); // Binary V1
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_badNetwork_ProtoV0001.net"); // Corrupted (based off of oldNetwork2.net)
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_semilarge_ProtoV0002.net", false);
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_smallPvlTest_PvlV0003.pvl", true, true); // network with rejected jigsaw points
}

void TestNetwork(const QString &filename, bool printNetwork, bool pvlInput) {
  qDebug() << "Reading: " << filename << "...\n";
  FileName networkFileName(filename);

  ControlNetVersioner *test = NULL;
  ControlNetVersioner *test2 = NULL;

  try {

    // If we're reading in a Pvl file, this will call the Pvl update cycle, then
    //   convert to binary, then convert back to Pvl.
    // If we're reading in a binary file, this will call the binary read, then
    //   convert to Pvl, then update, then convert to binary, and back to pvl.
    //   The reason for the intermediate Pvl is described in
    //   ControlNetVersioner.h.
    qDebug() << "Read network...";
    test = new ControlNetVersioner(networkFileName);

    if(printNetwork) {
      qDebug() << "Converted directly to Pvl:";
      Pvl pvlVersion(test->toPvl());

      // qDebug() does not support this operation on a pvl
      std::cout << pvlVersion << std::endl;
      pvlVersion.write("./tmp.pvl");
    }

    // Test the latest binary read/write and Pvl conversion
    qDebug() << "Write the network and re-read it...";
    test->write( FileName("./tmp") );
    try {
      test2 = new ControlNetVersioner( FileName("./tmp") );
    }
    catch(IException &e) {
      remove("./tmp");
      throw;
    }

    qDebug() << "After reading and writing to a binary form does Pvl match?";

    if(printNetwork) {
      Pvl pvlVersion2(test2->toPvl());
      pvlVersion2.write("./tmp2.pvl");
      if(system("cmp ./tmp.pvl ./tmp2.pvl")) {
        qDebug() << "Reading/Writing results in Pvl differences!";
      }
      else {
        qDebug() << "Conversion to Pvl stays consistent";
      }
    }

    test2->write(FileName("./tmp2"));
    if(system("cmp ./tmp ./tmp2")) {
      qDebug() << "Reading/Writing control network results in binary differences!";
    }
    else {
      qDebug() << "Reading/Writing control network is consistent";
    }

    if (pvlInput) {

      ControlNetVersioner *cNet2 = NULL;

      qDebug() << "Check conversions between the binary format and the pvl format.";
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
      cNet2 = new ControlNetVersioner(FileName("./tmp.pvl"));

      cNet2->write( FileName("./tmpCNet2") );

      //if there are differences between the pvls.
      QString cmd = "diff -EbB --suppress-common-lines " + filename + " ./tmp.pvl";
      if(system(cmd.toStdString().c_str())) {

        //if the binary files are different.
        if(system("diff -EbB --suppress-common-lines ./tmp ./tmpCNet2")){
          qDebug() << "The conversion from binary to pvl is incorrect.";
        }
        else {
          qDebug() << "The conversion from pvl to binary is incorrect.";
        }
      }
      else {
        qDebug() << "The conversion methods for pvl->bin and bin->pvl are correct.";
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
    qDebug() << errors.join("\n");
  }

  if(test) {
    delete test;
    test = NULL;
  }

  if(test2) {
    delete test2;
    test2 = NULL;
  }

  qDebug();
}
