#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Histogram.h"
#include "TestUtilities.h"

#include "thm2isis.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/thm2isis.xml").expanded();

TEST_F(TempTestingFiles, FunctionalTestThm2isisVis) {
  // tempDir exists if the fixture subclasses TempTestingFiles, which most do
  QString outCubeFileName = tempDir.path() + "/test.cub";
  QVector<QString> args = {"from=data/thm2isis/V00821003RDR.QUB",  "to="+outCubeFileName};

  UserInterface options(APP_XML, args);
  try {
    thm2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  
  // open even cube
  Cube evenCube( tempDir.path() + "/test.even.cub");
  Pvl *isisLabel = evenCube.label();

  // Dimensions Group
  EXPECT_EQ(evenCube.sampleCount(), 1024);
  EXPECT_EQ(evenCube.lineCount(), 576);
  EXPECT_EQ(evenCube.bandCount(), 5);

  // Pixels Group
  EXPECT_EQ(PixelTypeName(evenCube.pixelType()), "Real");
  EXPECT_EQ(ByteOrderName(evenCube.byteOrder()), "Lsb");
  EXPECT_DOUBLE_EQ(evenCube.base(), 0.0);
  EXPECT_DOUBLE_EQ(evenCube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(inst["SpacecraftName"][0].toStdString(), "MARS_ODYSSEY");
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "THEMIS_VIS" );
  EXPECT_EQ(inst["TargetName"][0].toStdString(), "MARS" );
  EXPECT_EQ(inst["SpacecraftClockCount"][0].toStdString(), "698642092.025" );
  EXPECT_EQ(inst["ExposureDuration"][0].toStdString(), "6.000" );
  EXPECT_EQ(inst["StartTime"][0].toStdString(), "2002-02-20T03:14:02.471" );
  EXPECT_EQ(inst["StopTime"][0].toStdString(), "2002-02-20T03:14:09.471" );
  EXPECT_EQ(inst["Framelets"][0].toStdString(), "Even" );
  EXPECT_EQ(inst["InterframeDelay"][0].toStdString(), "1.000" );
  EXPECT_EQ(int(inst["NumFramelets"]), 3);

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_EQ(archive["DataSetId"][0].toStdString(), "ODY-M-THM-3-VISRDR-V1.0" );
  EXPECT_EQ(archive["ProductId"][0].toStdString(), "V00821003RDR" );
  EXPECT_EQ(archive["ProductCreationTime"][0].toStdString(), "2003-07-08T03:07:17" );
  EXPECT_EQ(double(archive["ProductVersionId"]), 1.3);

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(bandbin["OriginalBand"].size(), 5);
  EXPECT_EQ(bandbin["Center"].size(), 5);
  EXPECT_EQ(bandbin["Width"].size(), 5);
  EXPECT_EQ(bandbin["FilterNumber"].size(), 5);

  // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_EQ(int(kernel["NaifFrameCode"]), -53032);

  std::unique_ptr<Histogram> hist (evenCube.histogram());

  EXPECT_NEAR(hist->Average(), 0.0012095900723426705, 0.0001);
  EXPECT_NEAR(hist->Sum(), 227.52389260765631, .00001);
  EXPECT_EQ(hist->ValidPixels(), 188100);
  EXPECT_NEAR(hist->StandardDeviation(), 2.241887e-05, .00001);

  // open odd cube
  Cube oddCube( tempDir.path() + "/test.odd.cub");
  isisLabel = oddCube.label();

  // Dimensions Group
  EXPECT_EQ(oddCube.sampleCount(), 1024);
  EXPECT_EQ(oddCube.lineCount(), 576);
  EXPECT_EQ(oddCube.bandCount(), 5);

  // Pixels Group
  EXPECT_EQ(PixelTypeName(oddCube.pixelType()), "Real");
  EXPECT_EQ(ByteOrderName(oddCube.byteOrder()), "Lsb");
  EXPECT_DOUBLE_EQ(oddCube.base(), 0.0);
  EXPECT_DOUBLE_EQ(oddCube.multiplier(), 1.0);

  // Instrument Group
  inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(inst["SpacecraftName"][0].toStdString(), "MARS_ODYSSEY");
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "THEMIS_VIS" );
  EXPECT_EQ(inst["TargetName"][0].toStdString(), "MARS" );
  EXPECT_EQ(inst["SpacecraftClockCount"][0].toStdString(), "698642092.025" );
  EXPECT_EQ(inst["ExposureDuration"][0].toStdString(), "6.000" );
  EXPECT_EQ(inst["StartTime"][0].toStdString(), "2002-02-20T03:14:02.471" );
  EXPECT_EQ(inst["StopTime"][0].toStdString(), "2002-02-20T03:14:09.471" );
  EXPECT_EQ(inst["Framelets"][0].toStdString(), "Odd" );
  EXPECT_EQ(inst["InterframeDelay"][0].toStdString(), "1.000" );
  EXPECT_EQ(int(inst["NumFramelets"]), 3);

  // Archive Group
  archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_EQ(archive["DataSetId"][0].toStdString(), "ODY-M-THM-3-VISRDR-V1.0" );
  EXPECT_EQ(archive["ProductId"][0].toStdString(), "V00821003RDR" );
  EXPECT_EQ(archive["ProductCreationTime"][0].toStdString(), "2003-07-08T03:07:17" );
  EXPECT_EQ(double(archive["ProductVersionId"]), 1.3);

  // BandBin Group
  bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(bandbin["OriginalBand"].size(), 5);
  EXPECT_EQ(bandbin["Center"].size(), 5);
  EXPECT_EQ(bandbin["Width"].size(), 5);
  EXPECT_EQ(bandbin["FilterNumber"].size(), 5);

  // Kernels Group
  kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_EQ(int(kernel["NaifFrameCode"]), -53032);

  hist.reset(oddCube.histogram());

  EXPECT_NEAR(hist->Average(), 0.0012095900723426705, 0.0001);
  EXPECT_NEAR(hist->Sum(), 457.42227419186383, .00001);
  EXPECT_EQ(hist->ValidPixels(), 376200);
  EXPECT_NEAR(hist->StandardDeviation(), 2.241887e-05, .00001);
}


TEST_F(TempTestingFiles, FunctionalTestThm2isisOutAttributes) {
  // tempDir exists if the fixture subclasses TempTestingFiles, which most do
  QString outCubeFileName = tempDir.path() + "/test.cub+msb+8bit+0:1";
  QVector<QString> args = {"from=data/thm2isis/V00821003RDR.QUB",  "to="+outCubeFileName};

  UserInterface options(APP_XML, args);
  try {
    thm2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }
  
  // open even cube
  Cube evenCube( tempDir.path() + "/test.even.cub");

  // Pixels Group
  EXPECT_EQ(PixelTypeName(evenCube.pixelType()).toStdString(), "UnsignedByte");
  EXPECT_EQ(ByteOrderName(evenCube.byteOrder()).toStdString(), "Msb");
  EXPECT_DOUBLE_EQ(evenCube.base(), -0.0039525691699605001);
  EXPECT_DOUBLE_EQ(evenCube.multiplier(), 0.0039525691699605001);

  std::unique_ptr<Histogram> hist (evenCube.histogram());
   
  EXPECT_NEAR(hist->Minimum(), 0.0, 0.0001);
  EXPECT_NEAR(hist->Maximum(), 0.0, 0.0001); 
  EXPECT_NEAR(hist->Average(), 0.0, 0.0001);
  EXPECT_NEAR(hist->Sum(), 0.0, .00001);
  EXPECT_EQ(hist->ValidPixels(), 188100);
  EXPECT_NEAR(hist->StandardDeviation(), 0, .00001);
}