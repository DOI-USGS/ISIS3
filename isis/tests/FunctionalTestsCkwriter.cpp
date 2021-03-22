#include "ckwriter.h"

#include "Fixtures.h"
#include "Pvl.h"
#include "TextFile.h"
#include "TestUtilities.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;

static QString APP_XML = FileName("$ISISROOT/bin/xml/ckwriter.xml").expanded();

TEST_F(DefaultCube, FunctionalTestCkwriterDefault) {
  Pvl appLog;
  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + tempDir.path() + "/newKernel.bc"};

  UserInterface options(APP_XML, args);
  try {
   ckwriter(options, &appLog);
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
  instrumentPointing += options.GetFileName("TO");
  instrumentPointing += "$viking1/kernels/fk/vo1_v10.tf";
  kernels.addKeyword(instrumentPointing, PvlContainer::InsertMode::Replace);

  PvlKeyword instrumentPosition("InstrumentPosition");
  instrumentPosition += kernels["InstrumentPosition"][1];
  kernels.addKeyword(instrumentPosition, PvlContainer::InsertMode::Replace);

  newKernelCube.reopen("rw");

  Camera *newCamera = nullptr;

  try {
    newCamera = newKernelCube.camera();
  } catch(IException &e) {
    FAIL() << "Unable to generate camera with new ck kernel: " << e.toString().toStdString().c_str() << std::endl;
  }

  SpiceRotation *newKernelRotation = newCamera->instrumentRotation();
  SpiceRotation *originalRotation = testCube->camera()->instrumentRotation();

  Table instPointingTable = testCube->readTable("InstrumentPointing");
  double startTime = double(instPointingTable.Label()["CkTableStartTime"]);

  newKernelRotation->SetEphemerisTime(startTime);
  originalRotation->SetEphemerisTime(startTime);

  ASSERT_EQ(newKernelRotation->cacheSize(), originalRotation->cacheSize());

  for (int i = 0; i < newKernelRotation->TimeBasedMatrix().size(); i++) {
    ASSERT_DOUBLE_EQ(newKernelRotation->TimeBasedMatrix()[i],
                     originalRotation->TimeBasedMatrix()[i]);
  }

  for (int i = 0; i < newKernelRotation->AngularVelocity().size(); i++) {
    ASSERT_DOUBLE_EQ(newKernelRotation->AngularVelocity()[i],
                     originalRotation->AngularVelocity()[i]);
  }
}

TEST_F(DefaultCube, FunctionalTestCkwriterFromlist) {
  Pvl appLog;
  FileList cubeList;
  cubeList.append(testCube->fileName());

  QString cubeListFile = tempDir.path() + "/cubes.lis";
  cubeList.write(cubeListFile);
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bc"};

  UserInterface options(APP_XML, args);
  try {
   ckwriter(options, &appLog);
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
  instrumentPointing += options.GetFileName("TO");
  instrumentPointing += kernels["InstrumentPointing"][2];
  kernels.addKeyword(instrumentPointing, PvlContainer::InsertMode::Replace);

  PvlKeyword instrumentPosition("InstrumentPosition");
  instrumentPosition += kernels["InstrumentPosition"][1];
  kernels.addKeyword(instrumentPosition, PvlContainer::InsertMode::Replace);

  newKernelCube.reopen("rw");

  Camera *newCamera = nullptr;
  Camera *origCamera = testCube->camera();

  try {
    newCamera = newKernelCube.camera();
  } catch(IException &e) {
    FAIL() << "Unable to generate camera with new ck kernel: " << e.toString().toStdString().c_str() << std::endl;
  }

  SpiceRotation *newKernelRotation = newCamera->instrumentRotation();
  SpiceRotation *originalRotation = origCamera->instrumentRotation();

  Table instPointingTable = testCube->readTable("InstrumentPointing");
  double startTime = double(instPointingTable.Label()["CkTableStartTime"]);

  newKernelRotation->SetEphemerisTime(startTime);
  originalRotation->SetEphemerisTime(startTime);

  ASSERT_EQ(newKernelRotation->cacheSize(), originalRotation->cacheSize());

  for (int i = 0; i < newKernelRotation->TimeBasedMatrix().size(); i++) {
    ASSERT_DOUBLE_EQ(newKernelRotation->TimeBasedMatrix()[i],
                     originalRotation->TimeBasedMatrix()[i]);
  }

  for (int i = 0; i < newKernelRotation->AngularVelocity().size(); i++) {
    ASSERT_DOUBLE_EQ(newKernelRotation->AngularVelocity()[i],
                     originalRotation->AngularVelocity()[i]);
  }
}

TEST_F(ObservationPair, FunctionalTestCkwriterCantValidate) {
  Pvl appLog;
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bc"};

  UserInterface options(APP_XML, args);
  try {
    ckwriter(options, &appLog);
    FAIL() << "Should not have been able to generate new CK" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Time overlap conflicts are present in segment (image) list."));
  }
}

TEST_F(ObservationPair, FunctionalTestCkwriterWarnValidate) {
  Pvl appLog;
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bc",
                           "overlap=warn"};

  UserInterface options(APP_XML, args);
  try {
   ckwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " << e.toString().toStdString().c_str() << std::endl;
  }

  EXPECT_TRUE(appLog.hasGroup("Overlaps"));
}

TEST_F(DefaultCube, FunctionalTestCkwriterComSum) {
  Pvl appLog;

  QString comFilePath = tempDir.path() + "/commfile.txt";
  TextFile comFile(comFilePath, "output");
  comFile.PutLine("This is a comment");
  comFile.Close();

  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + tempDir.path() + "/newKernel.bc",
                           "comfile=" + comFilePath,
                           "summary=" + tempDir.path() + "/summary.txt"};

  UserInterface options(APP_XML, args);
  try {
   ckwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " << e.toString().toStdString().c_str() << std::endl;
  }

  TextFile kernelFile(options.GetFileName("summary"));

  // Skip over the default comment in the summary file to get to
  // the user comment
  QString line;
  for (int i = 0; i < 69; i++) {
    kernelFile.GetLineNoFilter(line);
  }

  EXPECT_EQ("This is a comment", line);
}
