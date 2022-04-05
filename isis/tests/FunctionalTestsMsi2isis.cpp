#include <QTemporaryDir>

#include "msi2isis.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();

TEST(Msi2isis, Msi2isisTestGblIngestDefault) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/m0157063318f4_2p_iof_gbl.cub";
  QVector<QString> args = {"from=data/near/msi2isis/m0157063318f4_2p_iof_gbl.lbl",
                            "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
    std::cerr << "Running msi2isis..." << std::endl;
    msi2isis(options, &appLog);
    std::cerr << "Success!" << std::endl;

  }
  catch (IException &e) {
    FAIL() << "Unable to ingest NEAR/MSI GBL file: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  std::cerr << "Got labels... "<< std::endl;

  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 537);
  ASSERT_EQ(cube.lineCount(), 412);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "MSI" );
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "EROS" );
  ASSERT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "157063318856" );
  ASSERT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "157063318919" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStartCount"][0].toStdString(), "157063318.856" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStopCount"][0].toStdString(), "157063318.919" );
  ASSERT_DOUBLE_EQ(double(inst["ExposureDuration"]), 63.0);
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2001-02-08T17:24:42.217" );
  ASSERT_EQ(inst["StopTime"][0].toStdString(), "2001-02-08T17:24:42.280" );
  ASSERT_DOUBLE_EQ(double(inst["DpuDeckTemperature"]), 286.5);
  ASSERT_EQ(inst["DpuDeckTemperature"].unit().toStdString(), "K");

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_EQ(archive["DataSetId"][0].toStdString(), "NEAR-A-MSI-3-EDR-EROS/ORBIT-V1.0");
  ASSERT_EQ(archive["ProductId"][0].toStdString(), "M0157063318F4_2P_IOF_GBL.FIT");
  ASSERT_EQ(archive["InstrumentHostName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(archive["InstrumentName"][0].toStdString(), "MULTI-SPECTRAL IMAGER");
  ASSERT_EQ(archive["InstrumentId"][0].toStdString(), "MSI");
  ASSERT_EQ(archive["TargetName"][0].toStdString(), "EROS");

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(int(bandbin["FilterNumber"]), 4);
  ASSERT_EQ(int(bandbin["Center"]), 950);
  ASSERT_EQ(bandbin["Center"].unit().toStdString(), "nm");

 // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kernel["NaifFrameCode"]), -93001);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 0.042633522648316, .000001);
  ASSERT_NEAR(hist->Sum(), 6947.8146519054, .0001);
  ASSERT_EQ(hist->ValidPixels(), 162966);
  ASSERT_EQ(hist->NullPixels(), 58278);
  ASSERT_NEAR(hist->StandardDeviation(), 0.016649588425538, .0001);
}



TEST(Msi2isis, Msi2isisTestGblIngestNoTrim) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/m0157063318f4_2p_iof_gbl_notrim.cub";
  QVector<QString> args = {"from=data/near/msi2isis/m0157063318f4_2p_iof_gbl.lbl",
                            "to=" + cubeFileName,
                            "trim=false" };

  UserInterface options(APP_XML, args);
  try {
   std::cerr << "Running msi2isis..." << std::endl;
   msi2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest NEAR/MSI GBL file: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 537);
  ASSERT_EQ(cube.lineCount(), 412);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "MSI" );
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "EROS" );
  ASSERT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "157063318856" );
  ASSERT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "157063318919" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStartCount"][0].toStdString(), "157063318.856" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStopCount"][0].toStdString(), "157063318.919" );
  ASSERT_DOUBLE_EQ(double(inst["ExposureDuration"]), 63.0);
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2001-02-08T17:24:42.217" );
  ASSERT_EQ(inst["StopTime"][0].toStdString(), "2001-02-08T17:24:42.280" );
  ASSERT_DOUBLE_EQ(double(inst["DpuDeckTemperature"]), 286.5);
  ASSERT_EQ(inst["DpuDeckTemperature"].unit().toStdString(), "K");

  // Archive Group
 PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_EQ(archive["DataSetId"][0].toStdString(), "NEAR-A-MSI-3-EDR-EROS/ORBIT-V1.0");
  ASSERT_EQ(archive["ProductId"][0].toStdString(), "M0157063318F4_2P_IOF_GBL.FIT");
  ASSERT_EQ(archive["InstrumentHostName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(archive["InstrumentName"][0].toStdString(), "MULTI-SPECTRAL IMAGER");
  ASSERT_EQ(archive["InstrumentId"][0].toStdString(), "MSI");
  ASSERT_EQ(archive["TargetName"][0].toStdString(), "EROS");

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(int(bandbin["FilterNumber"]), 4);
  ASSERT_EQ(int(bandbin["Center"]), 950);
  ASSERT_EQ(bandbin["Center"].unit().toStdString(), "nm");

 // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kernel["NaifFrameCode"]), -93001);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 0.040669880452177, .000001);
  ASSERT_NEAR(hist->Sum(), 8997.9670307614, .0001);
  ASSERT_EQ(hist->ValidPixels(), 221244);
  ASSERT_EQ(hist->NullPixels(), 0);
  ASSERT_NEAR(hist->StandardDeviation(), 0.017675184374796, .0001);
}

TEST(Msi2isis, Msi2isisTestCubicConvolution) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/m0126865998f4_2p_iof.cubic.cub";
  QVector<QString> args = {"from=data/near/msi2isis/m0126865998f4_2p_iof.lbl",
                            "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   std::cerr << "Running msi2isis..." << std::endl;
   msi2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest NEAR/MSI file: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 537);
  ASSERT_EQ(cube.lineCount(), 412);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "MSI" );
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "EROS" );
  ASSERT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "126865998830" );
  ASSERT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "126865998919" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStartCount"][0].toStdString(), "126865998.830" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStopCount"][0].toStdString(), "126865998.919" );
  ASSERT_DOUBLE_EQ(double(inst["ExposureDuration"]), 89.0);
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2000-02-25T05:16:12.656" );
  ASSERT_EQ(inst["StopTime"][0].toStdString(), "2000-02-25T05:16:12.745" );
  ASSERT_DOUBLE_EQ(double(inst["DpuDeckTemperature"]), 286.5);
  ASSERT_EQ(inst["DpuDeckTemperature"].unit().toStdString(), "K");

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_EQ(archive["DataSetId"][0].toStdString(), "NEAR-A-MSI-3-EDR-EROS/ORBIT-V1.0");
  ASSERT_EQ(archive["ProductId"][0].toStdString(), "M0126865998F4_2P_IOF.FIT");
  ASSERT_EQ(archive["InstrumentHostName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(archive["InstrumentName"][0].toStdString(), "MULTI-SPECTRAL IMAGER");
  ASSERT_EQ(archive["InstrumentId"][0].toStdString(), "MSI");
  ASSERT_EQ(archive["TargetName"][0].toStdString(), "EROS");

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(int(bandbin["FilterNumber"]), 4);
  ASSERT_EQ(int(bandbin["Center"]), 950);
  ASSERT_EQ(bandbin["Center"].unit().toStdString(), "nm");

 // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kernel["NaifFrameCode"]), -93001);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 0.00909104981804087, .000001);
  ASSERT_NEAR(hist->Sum(), 1481.5320246468, .0001);
  ASSERT_EQ(hist->ValidPixels(), 162966);
  ASSERT_EQ(hist->NullPixels(), 58278);
  ASSERT_NEAR(hist->StandardDeviation(), 0.013555951402431, .0001);
}

TEST(Msi2isis, Msi2isisTestNearestNeighbor) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/m0126865998f4_2p_iof.nearestneighbor.cub";
  QVector<QString> args = {"from=data/near/msi2isis/m0126865998f4_2p_iof.lbl",
                            "to=" + cubeFileName,
                            "interp=nearestneighbor" };

  UserInterface options(APP_XML, args);
  try {
   std::cerr << "Running msi2isis..." << std::endl;
   msi2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest NEAR/MSI file: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 537);
  ASSERT_EQ(cube.lineCount(), 412);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "MSI" );
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "EROS" );
  ASSERT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "126865998830" );
  ASSERT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "126865998919" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStartCount"][0].toStdString(), "126865998.830" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStopCount"][0].toStdString(), "126865998.919" );
  ASSERT_DOUBLE_EQ(double(inst["ExposureDuration"]), 89.0);
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2000-02-25T05:16:12.656" );
  ASSERT_EQ(inst["StopTime"][0].toStdString(), "2000-02-25T05:16:12.745" );
  ASSERT_DOUBLE_EQ(double(inst["DpuDeckTemperature"]), 286.5);
  ASSERT_EQ(inst["DpuDeckTemperature"].unit().toStdString(), "K");

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_EQ(archive["DataSetId"][0].toStdString(), "NEAR-A-MSI-3-EDR-EROS/ORBIT-V1.0");
  ASSERT_EQ(archive["ProductId"][0].toStdString(), "M0126865998F4_2P_IOF.FIT");
  ASSERT_EQ(archive["InstrumentHostName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(archive["InstrumentName"][0].toStdString(), "MULTI-SPECTRAL IMAGER");
  ASSERT_EQ(archive["InstrumentId"][0].toStdString(), "MSI");
  ASSERT_EQ(archive["TargetName"][0].toStdString(), "EROS");

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(int(bandbin["FilterNumber"]), 4);
  ASSERT_EQ(int(bandbin["Center"]), 950);
  ASSERT_EQ(bandbin["Center"].unit().toStdString(), "nm");

 // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kernel["NaifFrameCode"]), -93001);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 0.0090919593904032, .000001);
  ASSERT_NEAR(hist->Sum(), 1481.6802540165, .0001);
  ASSERT_EQ(hist->ValidPixels(), 162966);
  ASSERT_EQ(hist->NullPixels(), 58278);
  ASSERT_NEAR(hist->StandardDeviation(), 0.013550997159207, .0001);
}

TEST(Msi2isis, Msi2isisTestBilinear) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/m0126865998f4_2p_iof.bilinear.cub";
  QVector<QString> args = {"from=data/near/msi2isis/m0126865998f4_2p_iof.lbl",
                            "to=" + cubeFileName,
                            "interp=bilinear" };

  UserInterface options(APP_XML, args);
  try {
   std::cerr << "Running msi2isis..." << std::endl;
   msi2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest NEAR/MSI file: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();


  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 537);
  ASSERT_EQ(cube.lineCount(), 412);
  ASSERT_EQ(cube.bandCount(), 1);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "MSI" );
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "EROS" );
  ASSERT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "126865998830" );
  ASSERT_EQ(inst["SpacecraftClockStopCount"][0].toStdString(), "126865998919" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStartCount"][0].toStdString(), "126865998.830" );
  ASSERT_EQ(inst["OriginalSpacecraftClockStopCount"][0].toStdString(), "126865998.919" );
  ASSERT_DOUBLE_EQ(double(inst["ExposureDuration"]), 89.0);
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2000-02-25T05:16:12.656" );
  ASSERT_EQ(inst["StopTime"][0].toStdString(), "2000-02-25T05:16:12.745" );
  ASSERT_DOUBLE_EQ(double(inst["DpuDeckTemperature"]), 286.5);
  ASSERT_EQ(inst["DpuDeckTemperature"].unit().toStdString(), "K");

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_EQ(archive["DataSetId"][0].toStdString(), "NEAR-A-MSI-3-EDR-EROS/ORBIT-V1.0");
  ASSERT_EQ(archive["ProductId"][0].toStdString(), "M0126865998F4_2P_IOF.FIT");
  ASSERT_EQ(archive["InstrumentHostName"][0].toStdString(), "NEAR EARTH ASTEROID RENDEZVOUS");
  ASSERT_EQ(archive["InstrumentName"][0].toStdString(), "MULTI-SPECTRAL IMAGER");
  ASSERT_EQ(archive["InstrumentId"][0].toStdString(), "MSI");
  ASSERT_EQ(archive["TargetName"][0].toStdString(), "EROS");

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(int(bandbin["FilterNumber"]), 4);
  ASSERT_EQ(int(bandbin["Center"]), 950);
  ASSERT_EQ(bandbin["Center"].unit().toStdString(), "nm");

 // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kernel["NaifFrameCode"]), -93001);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 0.009091271356228, .000001);
  ASSERT_NEAR(hist->Sum(), 1481.568127839, .0001);
  ASSERT_EQ(hist->ValidPixels(), 162966);
  ASSERT_EQ(hist->NullPixels(), 58278);
  ASSERT_NEAR(hist->StandardDeviation(), 0.013542099831273, .0001);
}


// Error tests
TEST(Msi2isis, Msi2isisTestAMissingLabelFile) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/image.cub";
   QVector<QString> args = {"from=data/near/msi2isis/image.fit",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST A: pass in a fits file without label
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}


TEST(Msi2isis, Msi2isisTestBInvalidInstrumentId) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label1.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST B: Invalid Instrument ID
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}

TEST(Msi2isis, Msi2isisTestCInvalidSize) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label2.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST C: Invalid size
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}

TEST(Msi2isis, Msi2isisTestDInvalidSize2) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label3.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST D: Invalid size
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}

TEST(Msi2isis, Msi2isisTestEInvalidSampleDirection) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label4.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST E: Invalid Sample Display Direction
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}

TEST(Msi2isis, Msi2isisTestFInvalidLineDirection) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label5.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST F: Invalid Line Display Direction
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}

TEST(Msi2isis, Msi2isisTestGInvalidProjectedImage) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label6.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST G: Unable to import projected images
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}

TEST(Msi2isis, Msi2isisTestHLabelTranslateError) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString dAPP_XML = FileName("$ISISROOT/bin/xml/msi2isis.xml").expanded();
   QString cubeFileName = prefix.path() + "/bad_msi.cub";
   QVector<QString> args = {"from=data/near/msi2isis/label7.lbl",
                            "to=" + cubeFileName };

   UserInterface options(dAPP_XML, args);

   // TEST H: Unable to translate labels
   ASSERT_ANY_THROW(msi2isis(options, &appLog));
}