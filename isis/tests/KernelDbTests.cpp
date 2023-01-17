#include <fstream>

#include <QList>
#include <QString>
#include <QStringList>

#include "FileName.h"
#include "KernelDb.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Preference.h"

#include "gmock/gmock.h"

using namespace Isis;

class KernelDbFixture : public ::testing::Test {
  protected:
  Pvl cubeLabel;
  Pvl dbPvl;


  void SetUp() {
    std::istringstream cubeLabelStr(R"(
      Object = IsisCube
        Object = Core
          StartByte   = 65537
          Format      = Tile
          TileSamples = 128
          TileLines   = 128

          Group = Dimensions
            Samples = 126
            Lines   = 126
            Bands   = 2
          End_Group

          Group = Pixels
            Type       = Real
            ByteOrder  = Lsb
            Base       = 0.0
            Multiplier = 1.0
          End_Group
        End_Object

        Group = Instrument
          SpacecraftName = IdealSpacecraft
          InstrumentId   = IdealCamera
          StartTime      = "2005 JUN 15 12:00:00.000 TDB"
          StopTime       = "2005 DEC 15 12:00:00.000 TDB"
        End_Group

        Group = TestGroup
          TestKeyword = TestValue
        End_Group

        Group = Test
          Keyword = "This is a test"
        End_Group
      End_Object

      Object = Label
        Bytes = 65536
      End_Object

      Object = History
        Name      = IsisCube
        StartByte = 196609
        Bytes     = 695
      End_Object
      End
    )");

    std::istringstream dbStr(R"(
      Object = LeapSecond
        Group = Selection
          File = ("base", "lsTest")
        EndGroup
      EndObject

      Object = TargetAttitudeShape
        Group = Selection
          File = ("base", "pckIncorrect")
        EndGroup
        Group = Selection
          File = ("base", "pckTest")
        EndGroup
      EndObject

      Object = TargetPosition
        Group = Selection
          File = ("base", "spkTest1")
          File = ("base", "spkTest2")
        EndGroup
      EndObject

      Object = SpacecraftPointing
        Group = Selection
          Time = ("2005 JAN 01 01:00:00.000 TDB", "2006 JAN 01 01:00:00.000 TDB")
          File = ("base", "ckIncorrect1")
          Type = Reconstructed
        EndGroup

        Group = Selection
          Time = ("2005 JAN 01 01:00:00.000 TDB", "2006 JAN 01 01:00:00.000 TDB")
          File = ("base", "ckTest1")
          Type = Reconstructed
        EndGroup

        Group = Selection
          Time = ("2004 JAN 01 01:00:00.000 TDB", "2005 JAN 01 01:00:00.000 TDB")
          File = ("base", "ckIncorrect2")
          Type = Reconstructed
        EndGroup

        Group = Selection
          Time = ("2005 JUN 15 12:00:00.000 TDB", "2005 JUN 15 12:05:00.000 TDB")
          Time = ("2005 JUN 16 12:00:00.000 TDB", "2005 JUN 16 12:05:00.000 TDB")
          File = ("base", "ckTest2.1")
          Type = Reconstructed
        EndGroup

        Group = Selection
          Time = ("2005 JUN 15 12:04:00.000 TDB", "2005 JUN 15 12:15:00.000 TDB")
          Time = ("2005 JUN 16 12:04:00.000 TDB", "2005 JUN 16 12:15:00.000 TDB")
          File = ("base", "ckTest2.2")
          Type = Reconstructed
        EndGroup
      EndObject

      Object = Instrument
        Group = Selection
          Match = ("TestGroup","TestKeyword","TestValue")
          File  = ("base", "ikTest1")
          File  = ("base", "ikTest2")
          CameraVersion = "2"
        EndGroup

        Group = Selection
          Match = ("TestGroup","TestKeyword","TestValue")
          File  = ("base", "ikTest3")
          File  = ("base", "ikTest4")
          CameraVersion = "1"
        EndGroup

        Group = Selection
          Match = ("TestGroup","TestKeyword","TestValue")
          File  = ("base", "ikTest5")
          File  = ("base", "ikTest6")
          CameraVersion = "3"
        EndGroup
      EndObject

      Object = SpacecraftClock
        Group = Selection
          File = ("base", "sclkTest")
        EndGroup
      EndObject

      Object = SpacecraftPosition
        Group = Selection
          Time = ("2005 JAN 01 01:00:00.000 TDB", "2006 JAN 01 01:00:00.000 TDB")
          File = ("base", "spkTest1")
          File = ("base", "spkTest2")
          Type = Reconstructed
        EndGroup
      EndObject

      Object = Frame
        Group = Selection
          File = ("base", "fkTest")
        EndGroup
      EndObject

      Object = InstrumentAddendum
        Group = Selection
          Match = ("TestGroup","TestKeyword","TestValue")
          File  = ("base", "iakTest")
        EndGroup

        Group = Selection
          Match = ("TestGroup","TestKeyword","IncorrectValue")
          File  = ("base", "iakIncorrect")
        EndGroup

        Group = Selection
          Match = ("TestGroup","IncorrectKeyword","TestValue")
          File  = ("base", "iakIncorrect")
        EndGroup

        Group = Selection
          Match = ("IncorrectGroup","TestKeyword","TestValue")
          File  = ("base", "iakIncorrect")
        EndGroup
      EndObject

      Object = Dem
        Group = Selection
          Match = ("TestGroup","TestKeyword","TestValue")
          File  = ("base", "demTest1")
          File  = ("base", "demTest2")
        EndGroup
      EndObject
    )");

    cubeLabelStr >> cubeLabel;

    dbStr >> dbPvl;
  }
};

TEST_F(KernelDbFixture, TestKernelsFromDb) {
  std::stringstream dbStr;
  dbStr << dbPvl;
  KernelDb db(dbStr, Kernel::Predicted|Kernel::Nadir|Kernel::Reconstructed|Kernel::Smithed);

  QStringList lsk = db.leapSecond(cubeLabel).kernels();
  ASSERT_EQ(lsk.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, lsk[0], "$base/lsTest");

  QStringList pcks = db.targetAttitudeShape(cubeLabel).kernels();
  ASSERT_EQ(pcks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, pcks[0], "$base/pckTest");

  QStringList tspks = db.targetPosition(cubeLabel).kernels();
  ASSERT_EQ(tspks.size(), 2);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, tspks[0], "$base/spkTest1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, tspks[1], "$base/spkTest2");

  QList< std::priority_queue<Kernel> > cklist = db.spacecraftPointing(cubeLabel);
  ASSERT_EQ(cklist.size(), 1);
  ASSERT_EQ(cklist[0].size(), 4);
  Kernel cKernels(cklist[0].top());
  QStringList cks = cKernels.kernels();
  ASSERT_EQ(cks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cks[0], "$base/ckTest1");

  QStringList iks = db.instrument(cubeLabel).kernels();
  ASSERT_EQ(iks.size(), 2);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, iks[0], "$base/ikTest3");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, iks[1], "$base/ikTest4");

  QStringList sclk = db.spacecraftClock(cubeLabel).kernels();
  ASSERT_EQ(sclk.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, sclk[0], "$base/sclkTest");

  QStringList spks = db.spacecraftPosition(cubeLabel).kernels();
  ASSERT_EQ(spks.size(), 2);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, spks[0], "$base/spkTest1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, spks[1], "$base/spkTest2");

  QStringList fks = db.frame(cubeLabel).kernels();
  ASSERT_EQ(fks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, fks[0], "$base/fkTest");

  QStringList iaks = db.instrumentAddendum(cubeLabel).kernels();
  ASSERT_EQ(iaks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, iaks[0], "$base/iakTest");

  QStringList dems = db.dem(cubeLabel).kernels();
  ASSERT_EQ(dems.size(), 2);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, dems[0], "$base/demTest1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, dems[1], "$base/demTest2");
}

TEST_F(KernelDbFixture, TwoCks) {
  PvlGroup &instGroup = cubeLabel.findObject("IsisCube").findGroup("Instrument");
  instGroup.findKeyword("StopTime") = "2005 JUN 15 12:14:00.000 TDB";
  std::stringstream dbStr;
  dbStr << dbPvl;
  KernelDb db(dbStr, Kernel::Predicted|Kernel::Nadir|Kernel::Reconstructed|Kernel::Smithed);

  QList< std::priority_queue<Kernel> > cklist = db.spacecraftPointing(cubeLabel);
  ASSERT_EQ(cklist.size(), 1);
  ASSERT_EQ(cklist[0].size(), 5);
  Kernel cKernels(cklist[0].top());
  QStringList cks = cKernels.kernels();
  ASSERT_EQ(cks.size(), 2);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cks[0], "$base/ckTest2.1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cks[1], "$base/ckTest2.2");
}

TEST_F(KernelDbFixture, SystemKernels) {
  PvlGroup &instGroup = cubeLabel.findObject("IsisCube").findGroup("Instrument");
  instGroup.findKeyword("StartTime") = "2008 JAN 12 00:00:00.0";
  instGroup.findKeyword("StopTime") = "2008 JAN 12 00:00:00.0";
  instGroup.findKeyword("SpacecraftName") = "MarsReconnaissanceOrbiter";
  instGroup.findKeyword("InstrumentId") = "HiRISE";
  KernelDb db(Kernel::Reconstructed);

  db.loadSystemDb("Mro", cubeLabel);
  QList<FileName> dbFiles = db.kernelDbFiles();
  ASSERT_EQ(dbFiles.size(), 10);

  QStringList tspks = db.targetPosition(cubeLabel).kernels();
  ASSERT_EQ(tspks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, tspks[0], "$base/kernels/spk/de430.bsp");

  QList< std::priority_queue<Kernel> > cklist = db.spacecraftPointing(cubeLabel);
  ASSERT_EQ(cklist.size(), 1);
  ASSERT_EQ(cklist[0].size(), 1);
  Kernel cKernels(cklist[0].top());
  QStringList cks = cKernels.kernels();
  ASSERT_EQ(cks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cks[0], "$mro/kernels/ck/mro_sc_psp_080108_080114.bc");

  QStringList spks = db.spacecraftPosition(cubeLabel).kernels();
  ASSERT_EQ(spks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, spks[0], "$mro/kernels/spk/mro_psp6_ssd_mro110c.bsp");
}

TEST_F(KernelDbFixture, SystemCKConfig) {
  PvlGroup &instGroup = cubeLabel.findObject("IsisCube").findGroup("Instrument");
  instGroup.findKeyword("StartTime") = "2008 JAN 12 00:00:00.0";
  instGroup.findKeyword("StopTime") = "2008 JAN 12 00:00:00.0";
  instGroup.findKeyword("SpacecraftName") = "MarsReconnaissanceOrbiter";
  instGroup.findKeyword("InstrumentId") = "CRISM";
  KernelDb db(Kernel::Reconstructed);

  db.loadSystemDb("Mro", cubeLabel);
  QList<FileName> dbFiles = db.kernelDbFiles();
  ASSERT_EQ(dbFiles.size(), 11);

  QList< std::priority_queue<Kernel> > cklist = db.spacecraftPointing(cubeLabel);
  ASSERT_EQ(cklist.size(), 2);
  ASSERT_EQ(cklist[0].size(), 1);
  Kernel cKernels(cklist[0].top());
  QStringList cks = cKernels.kernels();
  ASSERT_EQ(cks.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cks[0], "$mro/kernels/ck/mro_sc_psp_080108_080114.bc");
  ASSERT_EQ(cklist[1].size(), 1);
  Kernel cKernels2(cklist[1].top());
  QStringList cks2 = cKernels2.kernels();
  ASSERT_EQ(cks2.size(), 1);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cks2[0], "$mro/kernels/ck/mro_crm_psp_080101_080131.bc");
}



TEST_F(KernelDbFixture, TestKernelsSmithOffset) {
  PvlGroup &instGroup = cubeLabel.findObject("IsisCube").findGroup("Instrument");
  instGroup.findKeyword("StartTime") = "2002-02-20T22:57:57.253";
  instGroup.findKeyword("StopTime") = "2002-02-20T23:00:56.983";
  instGroup.findKeyword("SpacecraftName") = "MARS_ODYSSEY";
  instGroup.findKeyword("InstrumentId") = "THEMIS_IR";
  KernelDb db(Kernel::Smithed);

  PvlGroup &dataDir = Preference::Preferences(true).findGroup("DataDirectory");

  QString testDir = "data/kernelDB";

  db.loadKernelDbFiles(dataDir, testDir + "/ck", cubeLabel);
  db.loadKernelDbFiles(dataDir, testDir + "/spk", cubeLabel);
  db.readKernelDbFiles();

  QStringList spk = db.spacecraftPosition(cubeLabel).kernels();
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, spk[0], "data/kerneldbgen/thmIR.bsp");

  QList<std::priority_queue<Kernel>> ck = db.spacecraftPointing(cubeLabel);
  Kernel cKernels(ck[0].top());
  QStringList cklist = cKernels.kernels();
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cklist[0], "data/kerneldbgen/thmIR.bc");
}
