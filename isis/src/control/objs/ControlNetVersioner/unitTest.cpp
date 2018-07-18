#include "ControlNetVersioner.h"

#include <QString>
#include <QTime>

#include "ControlNet.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "Progress.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void TestNetwork(const QString &filename, Progress *progress, bool printNetwork = true, bool pvlInput = false);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  Progress *testProgress = new Progress();
  std::cout << "Test ControlNetVersioner";

  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork2_PvlV0001.net", testProgress, false);     // No target
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork3_PvlV0001.net", testProgress);     // Really odd keywords with target
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork1_PvlV0001.net", testProgress);     // Another set of odd keywords
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork1_ProtoV0001.net", testProgress); // Binary V1
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_BadNetwork_ProtoV0001.net", testProgress);    // Corrupted (based off of oldNetwork2.net)
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork2_ProtoV0002.net", testProgress, false);  // Binary V2
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork8_PvlV0005.pvl", testProgress, true, true); // Network with rejected jigsaw points
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork5_PvlV0003.pvl", testProgress, false, false); // Network full of weird test cases (based on PvlNetwork4)
  TestNetwork("$control/testData/PvlNet_TestNetwork1_V2.net", testProgress, false, false); // Test Network 1 created for code coverage.
  TestNetwork("$control/testData/PvlNet_TestNetwork2_V3.net", testProgress, false, false); // Test Network 2 created for code coverage.

  // Re-test each version without progress
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork3_PvlV0001.net", 0, false);
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork1_ProtoV0001.net", 0, false);
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork2_ProtoV0002.net", 0, false);
  TestNetwork("$control/testData/unitTest_ControlNetVersioner_PvlNetwork4_PvlV0003.pvl", 0, false);

  std::cout << std::endl << "Test writing from ControlNet objects" << std::endl << std::endl;
  ControlNet *binaryV2Net = new ControlNet("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork2_ProtoV0002.net",
                                           testProgress);
  ControlNetVersioner *binV2Versioner = new ControlNetVersioner(binaryV2Net);
  binV2Versioner->write("./binaryV2tmp.net");
  remove("./binaryV2tmp.net");
  delete binV2Versioner;
  binV2Versioner = NULL;

  std::cout << std::endl << "Test reading version 1 protobuf network" << std::endl << std::endl;
  ControlNetVersioner *binV1Versioner = new ControlNetVersioner(FileName("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork1_ProtoV0001.net"), testProgress);
  std::cout << "Take all of the control points and delete them." << std::endl;
  int pointsTaken = 0;
  ControlPoint *readPoint = binV1Versioner->takeFirstPoint();
  while (readPoint != NULL) {
    pointsTaken++;
    std::cout << "  " << pointsTaken << (pointsTaken > 1 ? " points taken" : " point taken") << std::endl;
    delete readPoint;
    readPoint = binV1Versioner->takeFirstPoint();
  }
  delete binV1Versioner;
  binV1Versioner = NULL;

  std::cout << std::endl << "Test reading version 5 protobuf network" << std::endl << std::endl;
  ControlNetVersioner *binV5Versioner = new ControlNetVersioner(FileName("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork3_ProtoV0005.net"), testProgress);
  delete binV5Versioner;
  binV5Versioner = NULL;

  std::cout << std::endl << "Test writing with invalid target" << std::endl << std::endl;
  try {
    binaryV2Net->SetTarget("INVALID_TARGET_NAME");
    binV2Versioner = new ControlNetVersioner(binaryV2Net);
  }
  catch (IException &e) {
    e.print();
    if (binV2Versioner) {
      delete binV2Versioner;
      binV2Versioner = NULL;
    }
  }

  std::cout << std::endl << "Test reading a random PVL file" << std::endl << std::endl;
  try {
    ControlNetVersioner invalidVersioner("$base/templates/maps/equirectangular.map");
  }
  catch (IException &e) {
    e.print();
  }

  std::cout << std::endl << "Test reading a PVL files with missing header information" << std::endl << std::endl;
  try {
    ControlNetVersioner invalidVersionerV1("$control/testData/unitTest_ControlNetVersioner_PvlNetwork_BadHeaderV1.net");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    ControlNetVersioner invalidVersionerV2("$control/testData/unitTest_ControlNetVersioner_PvlNetwork_BadHeaderV2.net");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    ControlNetVersioner invalidVersionerV3("$control/testData/unitTest_ControlNetVersioner_PvlNetwork_BadHeaderV3.net");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    ControlNetVersioner invalidVersionerV4("$control/testData/unitTest_ControlNetVersioner_PvlNetwork_BadHeaderV4.net");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    ControlNetVersioner invalidVersionerV5("$control/testData/unitTest_ControlNetVersioner_PvlNetwork_BadHeaderV5.net");
  }
  catch (IException &e) {
    e.print();
  }

  std::cout << std::endl << "Test reading a protobuf file with a bad version number" << std::endl << std::endl;
  try {
    ControlNetVersioner invalidVersioner("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork_BadVersion.net");
  }
  catch (IException &e) {
    e.print();
  }

  std::cout << std::endl << "Test reading a protobuf file with no version number" << std::endl << std::endl;
  try {
    ControlNetVersioner invalidVersioner("$control/testData/unitTest_ControlNetVersioner_ProtoNetwork_NoVersion.net");
  }
  catch (IException &e) {
    e.print();
  }

  delete binaryV2Net;
}

void TestNetwork(const QString &filename, Progress *progress, bool printNetwork, bool pvlInput) {
  std::cout << "\nReading: " << filename << "...\n";
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
    std::cout << "\nRead network..." << std::endl;
    test = new ControlNetVersioner(networkFileName, progress);

    if(printNetwork) {
      std::cout << "Converted directly to Pvl:" << std::endl;
      Pvl pvlVersion(test->toPvl());

      // std::cout does not support this operation on a pvl
      std::cout << pvlVersion << std::endl;
      pvlVersion.write("./tmp.pvl");
    }

    // Test the latest binary read/write and Pvl conversion
    std::cout << "Write the network and re-read it..." << std::endl;
    test->write( FileName("./tmp") );
    try {
      test2 = new ControlNetVersioner( FileName("./tmp") );
    }
    catch(IException &e) {
      remove("./tmp");
      throw;
    }

    std::cout << "After reading and writing to a binary form does Pvl match?" << std::endl;

    if(printNetwork) {
      Pvl pvlVersion2(test2->toPvl());
      pvlVersion2.write("./tmp2.pvl");
      if(system("cmp ./tmp.pvl ./tmp2.pvl")) {
        std::cout << "Reading/Writing results in Pvl differences!" << std::endl;
      }
      else {
        std::cout << "Conversion to Pvl stays consistent" << std::endl;
      }
    }

    test2->write(FileName("./tmp2"));
    if(system("cmp ./tmp ./tmp2")) {
      std::cout << "Reading/Writing control network results in binary differences!" << std::endl;
    }
    else {
      std::cout << "Reading/Writing control network is consistent" << std::endl;
    }

    if (pvlInput) {

      ControlNetVersioner *cNet2 = NULL;

      std::cout << "Check conversions between the binary format and the pvl format." << std::endl;
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
      QString cmd = "diff -EbB --suppress-common-lines -I 'Version.*' " + networkFileName.expanded() + " ./tmp.pvl";
      if(system(cmd.toStdString().c_str())) {

        //if the binary files are different.
        if(system("diff -EbB --suppress-common-lines ./tmp ./tmpCNet2")){
          std::cout << "The conversion from binary to pvl is incorrect." << std::endl;
        }
        else {
          std::cout << "The conversion from pvl to binary is incorrect." << std::endl;
        }
      }
      else {
        std::cout << "The conversion methods for pvl->bin and bin->pvl are correct." << std::endl;
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
    std::cout << errors.join("\n") << std::endl;
  }

  if(test) {
    delete test;
    test = NULL;
  }

  if(test2) {
    delete test2;
    test2 = NULL;
  }

}
