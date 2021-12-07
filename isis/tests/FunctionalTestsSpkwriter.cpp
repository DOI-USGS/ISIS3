#include "spkwriter.h"

#include "Fixtures.h"
#include "Pvl.h"
#include "TextFile.h"
#include "TestUtilities.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;

static QString APP_XML = FileName("$ISISROOT/bin/xml/spkwriter.xml").expanded();

TEST_F(DefaultCube, FunctionalTestSpkwriterDefault) {
  Pvl appLog;
  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + tempDir.path() + "/newKernel.bsp"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube newKernelCube;
  newKernelCube.fromLabel(tempDir.path() + "/newKernelCube.cub", label, "rw");

  PvlGroup &kernels = newKernelCube.label()->findObject("IsisCube").findGroup("Kernels");
  PvlKeyword targetPosition("TargetPosition");
  targetPosition += kernels["TargetPosition"][1];
  targetPosition += kernels["TargetPosition"][2];
  kernels.addKeyword(targetPosition, PvlContainer::InsertMode::Replace);

  PvlKeyword instrumentPointing("InstrumentPointing");
  instrumentPointing += kernels["InstrumentPointing"][1];
  instrumentPointing += kernels["InstrumentPointing"][2];
  kernels.addKeyword(instrumentPointing, PvlContainer::InsertMode::Replace);

  PvlKeyword instrumentPosition("InstrumentPosition");
  instrumentPosition += options.GetFileName("TO");
  kernels.addKeyword(instrumentPosition, PvlContainer::InsertMode::Replace);

  newKernelCube.reopen("rw");

  Camera *newCamera = nullptr;

  try {
    newCamera = newKernelCube.camera();
  } catch(IException &e) {
    FAIL() << "Unable to generate camera with new spk kernel: " << e.toString().toStdString().c_str() << std::endl;
  }

  Table oldInstPositionTable = testCube->readTable("InstrumentPosition");

  Table newInstPositionTable = newKernelCube.camera()->instrumentPosition()->Cache("InstrumentPosition");

  ASSERT_EQ(oldInstPositionTable.Records(), 1);

  for (int i = 0; i<oldInstPositionTable.Records(); i++) {
    for (int j = 0; j<oldInstPositionTable[i].Fields(); j++){
      EXPECT_DOUBLE_EQ(double(oldInstPositionTable[i][j]), double(newInstPositionTable[i][j]));
    }
  }
}

TEST_F(DefaultCube, FunctionalTestSpkwriterFromlist) {
  Pvl appLog;
  FileList cubeList;
  cubeList.append(testCube->fileName());

  QString cubeListFile = tempDir.path() + "/cubes.lis";
  cubeList.write(cubeListFile);
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bsp"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube newKernelCube;
  newKernelCube.fromLabel(tempDir.path() + "/newKernelCube.cub", label, "rw");

  PvlGroup &kernels = newKernelCube.label()->findObject("IsisCube").findGroup("Kernels");
  PvlKeyword targetPosition("TargetPosition");
  targetPosition += kernels["TargetPosition"][1];
  targetPosition += kernels["TargetPosition"][2];
  kernels.addKeyword(targetPosition, PvlContainer::InsertMode::Replace);

  PvlKeyword instrumentPointing("InstrumentPointing");
  instrumentPointing += kernels["InstrumentPointing"][1];
  instrumentPointing += kernels["InstrumentPointing"][2];
  kernels.addKeyword(instrumentPointing, PvlContainer::InsertMode::Replace);

  PvlKeyword instrumentPosition("InstrumentPosition");
  instrumentPosition += options.GetFileName("TO");
  kernels.addKeyword(instrumentPosition, PvlContainer::InsertMode::Replace);

  newKernelCube.reopen("rw");

  Camera *newCamera = nullptr;

  try {
    newCamera = newKernelCube.camera();
  } catch(IException &e) {
    FAIL() << "Unable to generate camera with new spk kernel: " << e.toString().toStdString().c_str() << std::endl;
  }

  Table oldInstPositionTable = testCube->readTable("InstrumentPosition");

  Table newInstPositionTable = newKernelCube.camera()->instrumentPosition()->Cache("InstrumentPosition");

  ASSERT_EQ(oldInstPositionTable.Records(), 1);

  for (int i = 0; i<oldInstPositionTable.Records(); i++) {
    for (int j = 0; j<oldInstPositionTable[i].Fields(); j++){
      EXPECT_DOUBLE_EQ(double(oldInstPositionTable[i][j]), double(newInstPositionTable[i][j]));
    }
  }
}

TEST_F(ObservationPair, FunctionalTestSpkwriterCantValidate) {
  Pvl appLog;
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bsp"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
   FAIL() << "Should not have been able to generate new SPK" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Time/body overlap conflicts are present in segment (image) list."));
  }
}

TEST_F(ObservationPair, FunctionalTestSpkwriterWarnValidate) {
  Pvl appLog;
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bsp",
                           "overlap=warn"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " << e.toString().toStdString().c_str() << std::endl;
  }

  EXPECT_TRUE(appLog.hasGroup("Overlaps"));
}

TEST_F(DefaultCube, FunctionalTestSpkwriterComSum) {
  Pvl appLog;

  QString comFilePath = tempDir.path() + "/commfile.txt";
  TextFile comFile(comFilePath, "output");
  comFile.PutLine("This is a comment");
  comFile.Close();

  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + tempDir.path() + "/newKernel.bsp",
                           "comfile=" + comFilePath,
                           "summary=" + tempDir.path() + "/summary.txt"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " << e.toString().toStdString().c_str() << std::endl;
  }

  TextFile kernelFile(options.GetFileName("summary"));

  // Skip over the default comment in the summary file to get to
  // the user comment
  QString line;
  for (int i = 0; i < 72; i++) {
    kernelFile.GetLineNoFilter(line);
  }

  EXPECT_EQ("This is a comment", line);
}
