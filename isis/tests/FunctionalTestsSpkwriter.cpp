#include "spkwriter.h"

#include <SpiceUsr.h>

#include "Camera.h"
#include "CameraFixtures.h"
#include "NaifStatus.h"
#include "NetworkFixtures.h"
#include "Pvl.h"
#include "Table.h"
#include "TextFile.h"
#include "TestUtilities.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;

static QString APP_XML = QString::fromStdString(FileName("$ISISROOT/bin/xml/spkwriter.xml").expanded());

TEST_F(DefaultCube, FunctionalTestSpkwriterDefault) {
  Pvl appLog;
  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + tempDir.path() + "/newKernel.bsp"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " <<  e.toString().c_str() << std::endl;
  }

  Cube newKernelCube;
  newKernelCube.fromLabel(tempDir.path().toStdString() + "/newKernelCube.cub", label, "rw");

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
  instrumentPosition += options.GetFileName("TO").toStdString();
  kernels.addKeyword(instrumentPosition, PvlContainer::InsertMode::Replace);

  newKernelCube.reopen("rw");

  try {
    newKernelCube.camera();
  } catch(IException &e) {
    FAIL() << "Unable to generate camera with new spk kernel: " <<  e.toString().c_str() << std::endl;
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
  cubeList.append(testCube->fileName().toStdString());

  QString cubeListFile = tempDir.path() + "/cubes.lis";
  cubeList.write(cubeListFile.toStdString());
  QVector<QString> args = {"fromlist=" + cubeListFile,
                           "to=" + tempDir.path() + "/newKernel.bsp"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " <<  e.toString().c_str() << std::endl;
  }

  Cube newKernelCube;
  newKernelCube.fromLabel(tempDir.path().toStdString() + "/newKernelCube.cub", label, "rw");

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
  instrumentPosition += options.GetFileName("TO").toStdString();
  kernels.addKeyword(instrumentPosition, PvlContainer::InsertMode::Replace);

  newKernelCube.reopen("rw");

  try {
    newKernelCube.camera();
  } catch(IException &e) {
    FAIL() << "Unable to generate camera with new spk kernel: " <<  e.toString().c_str() << std::endl;
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
                           "to=" + tempDir.path() + "/newKernel.bsp",
                           "type=9"};

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
                           "type=9",
                           "overlap=warn"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " <<  e.toString().c_str() << std::endl;
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
    FAIL() << "Unable to write kernel file: " <<  e.toString().c_str() << std::endl;
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


TEST(Spkwriter, FunctionalTestSpkwriterOffsets) {
  QTemporaryDir prefix;

  QVector<QString> args = {"from=data/kernelWriterOffset/thmIR.cub",
                           "to=" + prefix.path() + "/newKernel.bc"};

  UserInterface options(APP_XML, args);
  try {
   spkwriter(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to write kernel file: " <<  e.toString().c_str() << std::endl;
  }
  QString tmp = options.GetFileName("TO");
  furnsh_c(tmp.toLatin1().data());
  SpiceChar fileType[32], source[2048];
  SpiceInt handle;
  QString instrument = "";
  QString startoffset = "";
  QString endoffset = "";

  SpiceBoolean found;
  kinfo_c(tmp.toLatin1().data(), 32, 2048, fileType, source, &handle, &found);

  if (found == SPICETRUE) {
    SpiceChar commnt[1001];
    SpiceBoolean done(SPICEFALSE);
    SpiceInt n;

    while (!done) {
      dafec_c(handle, 1, sizeof(commnt), &n, commnt, &done);
      QString cmmt(commnt);

      int instPos = 0;
      if ( (instPos = cmmt.indexOf("Instrument:", instPos, Qt::CaseInsensitive)) != -1 ) {
        instrument = cmmt.remove(" ").split(":")[1];
      }
      int startPos = 0;
      if ( (startPos = cmmt.indexOf("StartOffset:", startPos, Qt::CaseInsensitive)) != -1 ) {
        startoffset = cmmt.remove(" ").split(":")[1];
      }
      int endPos = 0;
      if ( (endPos = cmmt.indexOf("EndOffset:", endPos, Qt::CaseInsensitive)) != -1 ) {
        endoffset = cmmt.remove(" ").split(":")[1];
      }
    }
  }

  EXPECT_EQ(instrument, "THEMIS_IR");
  EXPECT_EQ(startoffset, "0.263");
  EXPECT_EQ(endoffset, "171.871");
}
