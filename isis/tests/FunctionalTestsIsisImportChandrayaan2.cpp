#include <iostream>
#include <time.h>

#include <QRegExp>
#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDataStream>
#include <QTextStream>
#include <QByteArray>
#include <QDataStream>

#include <nlohmann/json.hpp>
#include "TempFixtures.h"
#include "Histogram.h"
#include "md5wrapper.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "isisimport.h"
#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;
using json = nlohmann::json;

static QString APP_XML = FileName("$ISISROOT/bin/xml/isisimport.xml").expanded();

TEST_F(TempTestingFiles, FunctionalTestIsisImportChandrayaan2TmcMinimalLabel){

  QString pref = "ch2_tmc_nca_20191128T0035389755_b_brw_d18";
  QString dataFilePath= "data/isisimport/chan2/" + pref + ".xml";
  QString dataFileName = pref + ".xml";
  QString imageFileName = pref + ".img";
  QString minPvlFileName = "data/isisimport/chan2/ch2_tmc_minimal.pvl";
  QString cubeFileName = tempDir.path() + "/output.cub";

  int samples = 400;
  int lines = 17891;
  int bytes = 2;

  // create a temp img file and write data to it
  QFile tempImgFile(tempDir.path() + "/" + imageFileName);

  if(!tempImgFile.open(QFile::WriteOnly | QFile::Text)){
      FAIL() << " Could not open file for writing";
  }
  QDataStream out(&tempImgFile);

  // generate lines
  QByteArray writeToFile = QByteArray();
  short int fill = 0;
  for(int i=-1; i<(samples * bytes); i++){
    writeToFile.append(fill);
  }

  // write the lines to the temp file
  for(int i=0; i<lines; i++){
    QDataStream out(&tempImgFile);
    out << writeToFile;
  }
  tempImgFile.flush();
  tempImgFile.close();

  // create a temp data file and copy the contents of the xml in to it
  QFile tempDataFile(tempDir.path() + "/" + dataFileName);
  if (!tempDataFile.open(QFile::ReadWrite | QFile::Text))
      FAIL() << " Could not open file for writing";

  // open xml to get data
  QFile realXmlFile(dataFilePath);
  if (!realXmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
      FAIL() << "Failed to open file";

  QTextStream xmlData(&tempDataFile);
  xmlData << realXmlFile.readAll();

  tempDataFile.close();
  realXmlFile.close();

  QFileInfo fileInfo(tempDataFile);

  // testing with template
  QVector<QString> args = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName};
  UserInterface options(APP_XML, args);
  isisimport(options);

  Pvl truthLabel;
  truthLabel.read(minPvlFileName);

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup truthGroup = truthLabel.findGroup("Dimensions", Pvl::Traverse);
  PvlGroup &outGroup = outLabel->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Pixels", Pvl::Traverse);
  outGroup = outLabel->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Instrument", Pvl::Traverse);
  outGroup = outLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("BandBin", Pvl::Traverse);
  outGroup = outLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Kernels", Pvl::Traverse);
  outGroup = outLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);
}

void testChanImport(QString const& pref, 
                    QString const& truthFile, 
                    QString const& cubFile, 
                    QString const& tempPath,
                    int samples, int lines) {

  QString dataFilePath= "data/isisimport/chan2/" + pref + ".xml";
  QString dataFileName = pref + ".xml";
  QString imageFileName = pref + ".img";
  int bytes = 2;

  // Create a temp img file and write data to it
  QFile tempImgFile(tempPath + "/" + imageFileName);

  // Write binary data 
  if (!tempImgFile.open(QIODevice::WriteOnly))
      FAIL() << " Could not open file for writing";
  QByteArray writeToFile = QByteArray(samples * bytes, 0);
  for (int i=0; i<lines; i++) {
    qint64 bytesWritten = tempImgFile.write(writeToFile);
    if (bytesWritten == -1 || bytesWritten != writeToFile.size())
        FAIL() << "Failed to write all data for line " << i << " to image file.";
  }
  tempImgFile.flush();
  tempImgFile.close();

  // create a temp data file and copy the contents of the xml in to it
  QFile tempDataFile(tempPath + "/" + dataFileName);

  if (!tempDataFile.open(QFile::ReadWrite | QFile::Text))
      FAIL() << " Could not open file for writing";

  // open xml to get data
  QFile realXmlFile(dataFilePath);
  if (!realXmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
      FAIL() << "Failed to open file";

  QTextStream xmlData(&tempDataFile);
  xmlData << realXmlFile.readAll();

  tempDataFile.close();
  realXmlFile.close();

  QFileInfo fileInfo(tempDataFile);

  // testing with template
  QVector<QString> args = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubFile};
  UserInterface options(APP_XML, args);
  isisimport(options);

  Pvl truthLabel;
  truthLabel.read(truthFile);

  Cube outCube(cubFile);
  Pvl *outLabel = outCube.label();

  PvlGroup truthGroup = truthLabel.findGroup("Dimensions", Pvl::Traverse);
  PvlGroup &outGroup = outLabel->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Pixels", Pvl::Traverse);
  outGroup = outLabel->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Instrument", Pvl::Traverse);
  outGroup = outLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("BandBin", Pvl::Traverse);
  outGroup = outLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Kernels", Pvl::Traverse);
  outGroup = outLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Archive", Pvl::Traverse);
  outGroup = outLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportChandrayaan2TmcFullLabel){

  QString pref = "ch2_tmc_ncn_20240808T0532596974_d_img_d18";
  QString truthFile = "data/isisimport/chan2/ch2_tmc_full.pvl";
  QString tempPath = tempDir.path();
  QString cubFile = tempPath + "/output_tmc.cub";
  int samples = 4000;
  int lines = 2000; // Decreased for speed, also in the input files

  testChanImport(pref, truthFile, cubFile, tempPath, samples, lines);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportChandrayaan2OhrcFullLabel){

  QString pref = "ch2_ohr_ncp_20240229T0921593215_d_img_d18";
  QString truthFile = "data/isisimport/chan2/ch2_ohrc_full.pvl";
  QString tempPath = tempDir.path();
  QString cubFile = tempPath + "/output_ohrc.cub";
  int samples = 3000;
  int lines = 2000; // Decreased for speed, also in the input files
  
  testChanImport(pref, truthFile, cubFile, tempPath, samples, lines);
}
