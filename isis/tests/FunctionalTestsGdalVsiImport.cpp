#include <QTemporaryDir>
#include <QProcess>

#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "IEndian.h"
#include "PixelType.h"
#include "Histogram.h"

#include "lo2isis.h"
#include "isisimport.h"
#include "hi2isis.h"
#include "marci2isis.h"
#include "tgocassis2isis.h"
#include "pds2isis.h"
#include "chan1m32isis.h"

#include "gtest/gtest.h"

using namespace Isis;

// S3 test data base URL
const QString S3_TEST_DATA_URL = "https://asc-isisdata.s3.us-west-2.amazonaws.com/cnf_test_data/";

/**
 * Tests for GDAL VSI (Virtual File System) support in PDS import applications.
 * These tests verify that import applications use GDAL's VSI API for file I/O,
 * which enables both local file access and remote data access via /vsicurl/https://...
 *
 * Test Coverage:
 * - Local file imports (lo2isis, isisimport) - validates GDAL VSI integration
 * - Remote S3 imports with direct function calls (hi2isis, marci2isis, pds2isis, chan1m32isis)
 * - Remote S3 imports via subprocess (mroctx2isis, tgocassis2isis, mrf2isis, vims2isis, junocam2isis)
 * - PDS3 format (lo2isis local, mroctx2isis remote, hi2isis remote)
 * - PDS4/XML format (tgocassis2isis remote with companion .dat file)
 * - ASCII table companion files (chan1m32isis with .TAB file via ImportPdsTable)
 * - Error handling for non-existent files
 *
 * Implementation Notes:
 * - Apps with callable headers use direct function calls (lo2isis, isisimport, hi2isis, etc.)
 * - Apps without callable headers use subprocess execution via QProcess (mroctx2isis, mrf2isis, etc.)
 * - chan1m32isis test may skip if SPICE kernels are not available, but validates GDAL VSI file reading
 *
 */


// ========== Local File Tests ==========
// These tests validate GDAL VSI integration with local filesystem paths


TEST(GdalVsiImportTest, Lo2isisLocalFile) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/lo_local.cub";
  QString testDataPath = FileName("data/lo2isis/reimport/3133_h1.pds_cropped.img").expanded();

  QVector<QString> args = {"from=" + testDataPath, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/lo2isis.xml").expanded(), args);

  try {
    lo2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 151);
  EXPECT_EQ(cube.lineCount(), 5);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  // Verify Instrument group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("SpacecraftName"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "High Resolution Camera");
  EXPECT_EQ(inst["TargetName"][0].toStdString(), "Moon");
}


TEST(GdalVsiImportTest, IsisImportLocalFile) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/isisimport_local.cub";
  QString testDataPath = FileName("data/lo2isis/reimport/3133_h1.pds_cropped.img").expanded();

  QVector<QString> args = {"from=" + testDataPath, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/isisimport.xml").expanded(), args);

  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to import with isisimport: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 151);
  EXPECT_EQ(cube.lineCount(), 5);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";
}


// ========== Remote S3 Tests - Direct Function Calls ==========
// These tests use direct function calls for apps with callable headers


TEST(GdalVsiImportTest, IsisImportRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/isisimport_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "V13_084840_0996_XN_80S159W.IMG";

  // Explicitly specify template to avoid auto-detection issues
  QString templateFile = FileName("$ISISROOT/appdata/import/PDS3/MroCTX.tpl").expanded();

  QVector<QString> args = {"from=" + s3Url, "to=" + cubeFileName, "template=" + templateFile};
  UserInterface options(FileName("$ISISROOT/bin/xml/isisimport.xml").expanded(), args);

  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to import from S3 via vsicurl with isisimport: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 5000);
  EXPECT_EQ(cube.lineCount(), 7168);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  EXPECT_TRUE(isisLabel->hasObject("IsisCube"));
}


TEST(GdalVsiImportTest, Hi2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/hi_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "ESP_075499_1945_BG12_0.IMG";

  QVector<QString> args = {"from=" + s3Url, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/hi2isis.xml").expanded(), args);

  try {
    hi2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest HiRISE image from S3 via vsicurl: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 512);
  EXPECT_EQ(cube.lineCount(), 20000);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  // Verify Instrument group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("InstrumentId"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "HIRISE");
}


TEST(GdalVsiImportTest, Marci2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/marci_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "G21_026226_0772_MA_00N247W.IMG";

  QVector<QString> args = {"from=" + s3Url, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/marci2isis.xml").expanded(), args);
  Pvl appLog;

  try {
    marci2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest MARCI image from S3 via vsicurl: " << e.toString().toStdString().c_str() << std::endl;
  }

  // marci2isis creates two output cubes: .even.cub and .odd.cub
  QString cubeFileNameEven = prefix.path() + "/marci_s3.even.cub";
  QString cubeFileNameOdd = prefix.path() + "/marci_s3.odd.cub";

  Cube cubeEven(cubeFileNameEven);
  Cube cubeOdd(cubeFileNameOdd);
  Pvl *evenLabel = cubeEven.label();
  Pvl *oddLabel = cubeOdd.label();

  // Verify cube dimensions
  EXPECT_EQ(cubeEven.sampleCount(), 1024);
  EXPECT_EQ(cubeEven.lineCount(), 21280);
  EXPECT_EQ(cubeEven.bandCount(), 5);
  EXPECT_EQ(cubeOdd.sampleCount(), 1024);
  EXPECT_EQ(cubeOdd.lineCount(), 21280);
  EXPECT_EQ(cubeOdd.bandCount(), 5);

  // Verify cubes have valid pixel data
  std::unique_ptr<Histogram> histEven (cubeEven.histogram());
  EXPECT_GT(histEven->ValidPixels(), 0) << "Even cube should contain valid pixel data";
  std::unique_ptr<Histogram> histOdd (cubeOdd.histogram());
  EXPECT_GT(histOdd->ValidPixels(), 0) << "Odd cube should contain valid pixel data";

  // Verify Instrument groups
  PvlGroup &instEven = evenLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(instEven.hasKeyword("InstrumentId"));
  EXPECT_EQ(instEven["InstrumentId"][0].toStdString(), "Marci");
  EXPECT_EQ(instEven["Framelets"][0].toStdString(), "Even");

  PvlGroup &instOdd = oddLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(instOdd.hasKeyword("InstrumentId"));
  EXPECT_EQ(instOdd["InstrumentId"][0].toStdString(), "Marci");
  EXPECT_EQ(instOdd["Framelets"][0].toStdString(), "Odd");
}


TEST(GdalVsiImportTest, Pds2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/pds2isis_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "ui73n262.img";

  QVector<QString> args = {"from=" + s3Url, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/pds2isis.xml").expanded(), args);
  Pvl appLog;

  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest PDS image from S3 via vsicurl with pds2isis: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 1562);
  EXPECT_EQ(cube.lineCount(), 2127);
  EXPECT_EQ(cube.bandCount(), 5);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  EXPECT_TRUE(isisLabel->hasObject("IsisCube"));
}


TEST(GdalVsiImportTest, Chan1m32isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/chan1m3_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "M3G20081118T222604_V02_L1B.LBL";

  QVector<QString> args = {"from=" + s3Url, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/chan1m32isis.xml").expanded(), args);

  try {
    chan1m32isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest M3 image from S3 via vsicurl: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 304);
  EXPECT_EQ(cube.lineCount(), 1182);
  EXPECT_EQ(cube.bandCount(), 85);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  // Verify Instrument group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("InstrumentId"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "M3");
  EXPECT_TRUE(inst.hasKeyword("SpacecraftName"));
  EXPECT_EQ(inst["SpacecraftName"][0].toStdString(), "CHANDRAYAAN-1");
}


// ========== Remote S3 Tests - Subprocess Execution ==========
// These tests use subprocess execution for apps without callable headers


TEST(GdalVsiImportTest, Mroctx2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/ctx_s3.cub";
  QString s3Url = "/vsicurl/https://asc-isisdata.s3.us-west-2.amazonaws.com/isis_testData/isis/src/mro/apps/mroctx2isis/tsts/default/input/P01_001473_2479_XI_67N185W.img";

  QString mroctx2isisApp = FileName("$ISISROOT/bin/mroctx2isis").expanded();
  QStringList args;
  args << "from=" + s3Url << "to=" + cubeFileName;

  QProcess mroctx2isis;
  mroctx2isis.setProgram(mroctx2isisApp);
  mroctx2isis.setArguments(args);
  mroctx2isis.start();
  bool finished = mroctx2isis.waitForFinished();

  ASSERT_TRUE(finished) << "mroctx2isis did not finish within timeout";
  EXPECT_EQ(mroctx2isis.exitCode(), 0) << "mroctx2isis failed with exit code: " << mroctx2isis.exitCode()
                                        << "\nStderr: " << mroctx2isis.readAllStandardError().toStdString();

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 4080);
  EXPECT_EQ(cube.lineCount(), 4096);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  // Verify Instrument group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("InstrumentId"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "CTX");
}


TEST(GdalVsiImportTest, TgoCassis2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/tgo_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "cas_cal_sc_20200119T141457-20200119T141501-9621-12-NIR-556575436-0-2__4_0.xml";

  QString tgocassis2isisApp = FileName("$ISISROOT/bin/tgocassis2isis").expanded();
  QStringList args;
  args << "from=" + s3Url << "to=" + cubeFileName;

  QProcess tgocassis2isis;
  tgocassis2isis.setProgram(tgocassis2isisApp);
  tgocassis2isis.setArguments(args);
  tgocassis2isis.start();
  bool finished = tgocassis2isis.waitForFinished();

  ASSERT_TRUE(finished) << "tgocassis2isis did not finish within timeout";
  EXPECT_EQ(tgocassis2isis.exitCode(), 0) << "tgocassis2isis failed with exit code: " << tgocassis2isis.exitCode()
                                           << "\nStderr: " << tgocassis2isis.readAllStandardError().toStdString();

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 1408);
  EXPECT_EQ(cube.lineCount(), 256);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  // Verify Instrument group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("InstrumentId"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "CaSSIS");
  EXPECT_TRUE(inst.hasKeyword("SpacecraftName"));
}


TEST(GdalVsiImportTest, Mrf2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/mrf_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "lsb_00222_1cd_xiu_79s202_v1.lbl";

  QString mrf2isisApp = FileName("$ISISROOT/bin/mrf2isis").expanded();
  QStringList args;
  args << "from=" + s3Url << "to=" + cubeFileName;

  QProcess mrf2isis;
  mrf2isis.setProgram(mrf2isisApp);
  mrf2isis.setArguments(args);
  mrf2isis.start();
  bool finished = mrf2isis.waitForFinished();

  ASSERT_TRUE(finished) << "mrf2isis did not finish within timeout";
  EXPECT_EQ(mrf2isis.exitCode(), 0) << "mrf2isis failed with exit code: " << mrf2isis.exitCode()
                                     << "\nStderr: " << mrf2isis.readAllStandardError().toStdString();

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 266);
  EXPECT_EQ(cube.lineCount(), 5373);
  EXPECT_EQ(cube.bandCount(), 4);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  // Verify Instrument group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("InstrumentId"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "MRFLRO");
}


TEST(GdalVsiImportTest, Vims2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString visCubeFileName = prefix.path() + "/vims_vis_s3.cub";
  QString irCubeFileName = prefix.path() + "/vims_ir_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "v1884113035_1.qub";

  QString vims2isisApp = FileName("$ISISROOT/bin/vims2isis").expanded();
  QStringList args;
  args << "from=" + s3Url << "vis=" + visCubeFileName << "ir=" + irCubeFileName;

  QProcess vims2isis;
  vims2isis.setProgram(vims2isisApp);
  vims2isis.setArguments(args);
  vims2isis.start();
  bool finished = vims2isis.waitForFinished();

  ASSERT_TRUE(finished) << "vims2isis did not finish within timeout";
  EXPECT_EQ(vims2isis.exitCode(), 0) << "vims2isis failed with exit code: " << vims2isis.exitCode()
                                      << "\nStderr: " << vims2isis.readAllStandardError().toStdString();

  // Verify VIS output cube
  Cube visCube(visCubeFileName);
  Pvl *visLabel = visCube.label();

  // Verify VIS cube dimensions
  EXPECT_EQ(visCube.sampleCount(), 64);
  EXPECT_EQ(visCube.lineCount(), 64);
  EXPECT_EQ(visCube.bandCount(), 96);

  PvlGroup &visInst = visLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(visInst.hasKeyword("InstrumentId"));
  EXPECT_EQ(visInst["InstrumentId"][0].toStdString(), "VIMS");

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> visHist (visCube.histogram());
  EXPECT_GT(visHist->ValidPixels(), 0) << "VIS cube should contain valid pixel data";

  // Verify IR output cube
  Cube irCube(irCubeFileName);

  // Verify IR cube dimensions
  EXPECT_EQ(irCube.sampleCount(), 64);
  EXPECT_EQ(irCube.lineCount(), 64);
  EXPECT_EQ(irCube.bandCount(), 256);

  // Verify IR cube has valid pixel data
  std::unique_ptr<Histogram> irHist (irCube.histogram());
  EXPECT_GT(irHist->ValidPixels(), 0) << "IR cube should contain valid pixel data";
}


TEST(GdalVsiImportTest, Junocam2isisRemoteS3Vsicurl) {
  QTemporaryDir prefix;
  QString baseCubeFileName = prefix.path() + "/junocam_s3.cub";
  QString s3Url = "/vsicurl/" + S3_TEST_DATA_URL + "JNCE_2022186_43C00001_V01.LBL";

  QString junocam2isisApp = FileName("$ISISROOT/bin/junocam2isis").expanded();
  QStringList args;
  args << "from=" + s3Url << "to=" + baseCubeFileName;

  QProcess junocam2isis;
  junocam2isis.setProgram(junocam2isisApp);
  junocam2isis.setArguments(args);
  junocam2isis.start();
  bool finished = junocam2isis.waitForFinished();

  ASSERT_TRUE(finished) << "junocam2isis did not finish within timeout";

  QString stdoutStr = junocam2isis.readAllStandardOutput();
  QString stderrStr = junocam2isis.readAllStandardError();

  EXPECT_EQ(junocam2isis.exitCode(), 0) << "junocam2isis failed with exit code: " << junocam2isis.exitCode()
                                         << "\nStdout: " << stdoutStr.toStdString()
                                         << "\nStderr: " << stderrStr.toStdString();

  // junocam2isis creates multiple output cubes (one per filter/framelet)
  QDir tempDir(prefix.path());
  QStringList createdFiles = tempDir.entryList(QDir::Files);
  QStringList cubFiles = tempDir.entryList(QStringList() << "*.cub", QDir::Files);

  ASSERT_TRUE(cubFiles.size() > 0) << "No .cub files created. Files created: " << createdFiles.join(", ").toStdString()
                                    << "\nStdout: " << stdoutStr.toStdString();

  // Verify first created cube
  QString firstCube = prefix.path() + "/" + cubFiles.first();
  Cube cube(firstCube);
  Pvl *label = cube.label();

  // Verify cube dimensions
  EXPECT_EQ(cube.sampleCount(), 1648);
  EXPECT_EQ(cube.lineCount(), 128);
  EXPECT_EQ(cube.bandCount(), 1);

  // Verify cube has valid pixel data
  std::unique_ptr<Histogram> hist (cube.histogram());
  EXPECT_GT(hist->ValidPixels(), 0) << "Cube should contain valid pixel data";

  PvlGroup &inst = label->findGroup("Instrument", Pvl::Traverse);
  EXPECT_TRUE(inst.hasKeyword("InstrumentId"));
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "JNC");
}


// ========== Error Handling Tests ==========


TEST(GdalVsiImportTest, NonExistentFile) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/nonexistent.cub";
  QString nonexistentPath = "/tmp/nonexistent_test_file_12345.img";

  QVector<QString> args = {"from=" + nonexistentPath, "to=" + cubeFileName};
  UserInterface options(FileName("$ISISROOT/bin/xml/lo2isis.xml").expanded(), args);

  EXPECT_THROW({
    try {
      lo2isis(options);
    }
    catch (const IException &e) {
      QString errorMsg = e.toString();
      EXPECT_TRUE(errorMsg.contains("Unable") || errorMsg.contains("Failed") ||
                  errorMsg.contains("not") || errorMsg.contains("open") ||
                  errorMsg.contains("does not exist"));
      throw;
    }
  }, IException);
}
