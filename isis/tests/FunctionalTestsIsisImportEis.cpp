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

TEST_F(TempTestingFiles, FunctionalTestIsisImportEisNacFrame){
  std::istringstream PvlInput(R"(
    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 445
        TileLines   = 958

        Group = Dimensions
          Samples = 1335
          Lines   = 3832
          Bands   = 1
        End_Group

        Group = Pixels
          Type       = SignedWord
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object

      Group = Instrument
        SpacecraftName   = "Europa Clipper"
        InstrumentId     = "EIS NAC FC"
        TargetName       = Didymos
        StartTime        = 2021-03-06T04:05:27.77
        ExposureDuration = 1.0 <seconds>
      End_Group

      Group = BandBin
        FilterName = CLEAR
        Center     = 702.5 <nm>
        Width      = 695 <nm>
      End_Group

      Group = Kernels
        NaifFrameCode = -159103
      End_Group
    End_Object

    Object = Label
      Bytes = 65536
    End_Object

    Object = History
      Name      = IsisCube
      StartByte = 10296977
      Bytes     = 657
    End_Object

    Object = OriginalXmlLabel
      Name      = IsisCube
      StartByte = 10297634
      Bytes     = 13238
      ByteOrder = Lsb
    End_Object
    End
  )");
  QString dataFilePath= "data/isisimport/eispds/nacFrame/nac000xxx_2022145t000000_0000000001_frame_raw02.xml";
  QString dataFileName = "nac000xxx_2022145t000000_0000000001_frame_raw02.xml";
  QString imageFileName = "nac000xxx_2022145t000000_0000000001_frame_raw02.img";
  QString templateFile = "../appdata/import/PDS4/ClipperEIS.tpl";
  QString cubeFileName = tempDir.path() + "/nacFrame.cub";

  int samples = 1335;
  int lines = 3832;
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

  if(!tempDataFile.open(QFile::ReadWrite | QFile::Text)){
      FAIL() << " Could not open file for writing";
  }

  // open xml to get data
  QFile realXmlFile(dataFilePath);
  if (!realXmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      FAIL() << "Failed to open file";
  }

  QTextStream xmlData(&tempDataFile);
  xmlData << realXmlFile.readAll();

  tempDataFile.close();
  realXmlFile.close();

  QFileInfo fileInfo(tempDataFile);

  // testing with template
  QVector<QString> args = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName, "template=" + templateFile};
  UserInterface options(APP_XML, args);
  isisimport(options);

  Pvl truthLabel;
  PvlInput >> truthLabel;

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

  // testing with out template
  QVector<QString> argsNoTemp = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName};
  UserInterface optionsNoTemp(APP_XML, argsNoTemp);
  isisimport(optionsNoTemp);

  Cube outCubeNoTemp(cubeFileName);
  Pvl *outLabelNoTemp = outCubeNoTemp.label();

  truthGroup = truthLabel.findGroup("Dimensions", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Pixels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Instrument", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("BandBin", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Kernels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Kernels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportEisNacPb){
  std::istringstream PvlInput(R"(
    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 445
        TileLines   = 958

        Group = Dimensions
          Samples = 1335
          Lines   = 3832
          Bands   = 1
        End_Group

        Group = Pixels
          Type       = SignedWord
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object

      Group = Instrument
        SpacecraftName   = "Europa Clipper"
        InstrumentId     = "EIS NAC PB"
        TargetName       = Didymos
        StartTime        = 2021-03-06T04:05:27.77
        ExposureDuration = 1.0 <seconds>
      End_Group

      Group = BandBin
        FilterName = CLEAR
        Center     = 702.5 <nm>
        Width      = 695 <nm>
      End_Group

      Group = Kernels
        NaifFrameCode = -159103
      End_Group
    End_Object

    Object = Label
      Bytes = 65536
    End_Object

    Object = History
      Name      = IsisCube
      StartByte = 10296977
      Bytes     = 659
    End_Object

    Object = OriginalXmlLabel
      Name      = IsisCube
      StartByte = 10297636
      Bytes     = 13244
      ByteOrder = Lsb
    End_Object
    End
  )");

  QString dataFilePath = "data/isisimport/eispds/nacPushb/nac000xxx_2022145t000000_0000000001_pushb_raw02.xml";
  QString dataFileName = "nac000xxx_2022145t000000_0000000001_pushb_raw02.xml";
  QString imageFileName = "nac000xxx_2022145t000000_0000000001_pushb_raw02.img";
  QString templateFile = "../appdata/import/PDS4/ClipperEIS.tpl";
  QString cubeFileName = tempDir.path() + "/NacPb.cub";

  int samples = 1335;
  int lines = 3832;
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

  if(!tempDataFile.open(QFile::ReadWrite | QFile::Text)){
      FAIL() << " Could not open file for writing";
  }

  // open xml to get data
  QFile realXmlFile(dataFilePath);
  if (!realXmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      FAIL() << "Failed to open file";
  }

  QTextStream xmlData(&tempDataFile);
  xmlData << realXmlFile.readAll();

  tempDataFile.close();
  realXmlFile.close();

  QFileInfo fileInfo(tempDataFile);

  // testing with template
  QVector<QString> args = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName, "template=" + templateFile};
  UserInterface options(APP_XML, args);
  isisimport(options);

  Pvl truthLabel;
  PvlInput >> truthLabel;

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

  // testing with out template
  QVector<QString> argsNoTemp = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName};
  UserInterface optionsNoTemp(APP_XML, argsNoTemp);
  isisimport(optionsNoTemp);

  Cube outCubeNoTemp(cubeFileName);
  Pvl *outLabelNoTemp = outCubeNoTemp.label();

  truthGroup = truthLabel.findGroup("Dimensions", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Pixels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Instrument", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("BandBin", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Kernels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Kernels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportEisWacFrame){
  std::istringstream PvlInput(R"(
    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 445
        TileLines   = 958

        Group = Dimensions
          Samples = 1335
          Lines   = 3832
          Bands   = 1
        End_Group

        Group = Pixels
          Type       = SignedWord
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object

      Group = Instrument
        SpacecraftName   = "Europa Clipper"
        InstrumentId     = "EIS WAC FC"
        TargetName       = Didymos
        StartTime        = 2021-03-06T04:05:27.77
        ExposureDuration = 1.0 <seconds>
      End_Group

      Group = BandBin
        FilterName = CLEAR
        Center     = 712.5 <nm>
        Width      = 675 <nm>
      End_Group

      Group = Kernels
        NaifFrameCode = -159104
      End_Group
    End_Object

    Object = Label
      Bytes = 65536
    End_Object

    Object = History
      Name      = IsisCube
      StartByte = 10296977
      Bytes     = 695
    End_Object

    Object = OriginalXmlLabel
      Name      = IsisCube
      StartByte = 10297672
      Bytes     = 13164
      ByteOrder = Lsb
    End_Object
    End
  )");
  QString dataFilePath = "data/isisimport/eispds/wacFrame/wac000xxx_2022126t000000_000000001_frame_raw02.xml";
  QString dataFileName = "wac000xxx_2022126t000000_000000001_frame_raw02.xml";
  QString imageFileName = "wac000xxx_2022126t000000_000000001_frame_raw02.img";
  QString templateFile = "../appdata/import/PDS4/ClipperEIS.tpl";
  QString cubeFileName = tempDir.path() + "/wacFrame.cub";

  int samples = 1335;
  int lines = 3832;
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

  if(!tempDataFile.open(QFile::ReadWrite | QFile::Text)){
      FAIL() << " Could not open file for writing";
  }

  // open xml to get data
  QFile realXmlFile(dataFilePath);
  if (!realXmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      FAIL() << "Failed to open file";
  }

  QTextStream xmlData(&tempDataFile);
  xmlData << realXmlFile.readAll();

  tempDataFile.close();
  realXmlFile.close();

  QFileInfo fileInfo(tempDataFile);

  // testing with template
  QVector<QString> args = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName, "template=" + templateFile};
  UserInterface options(APP_XML, args);
  isisimport(options);

  Pvl truthLabel;
  PvlInput >> truthLabel;

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

  // testing with out template
  QVector<QString> argsNoTemp = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName};
  UserInterface optionsNoTemp(APP_XML, argsNoTemp);
  isisimport(optionsNoTemp);

  Cube outCubeNoTemp(cubeFileName);
  Pvl *outLabelNoTemp = outCubeNoTemp.label();

  truthGroup = truthLabel.findGroup("Dimensions", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Pixels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Instrument", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("BandBin", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Kernels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Kernels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);
}

TEST_F(TempTestingFiles, FunctionalTestIsisImportEisWacPb){
  std::istringstream PvlInput(R"(
    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 1024
        TileLines   = 1024

        Group = Dimensions
          Samples = 4096
          Lines   = 4096
          Bands   = 1
        End_Group

        Group = Pixels
          Type       = SignedWord
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object

      Group = Instrument
        SpacecraftName   = "Europa Clipper"
        InstrumentId     = "EIS WAC PB"
        TargetName       = Didymos
        StartTime        = 2021-03-06T04:05:27.77
        ExposureDuration = 1.0 <seconds>
      End_Group

      Group = BandBin
        FilterName = CLEAR
        Center     = 712.5 <nm>
        Width      = 675 <nm>
      End_Group

      Group = Kernels
        NaifFrameCode = -159104
      End_Group
    End_Object

    Object = Label
      Bytes = 65536
    End_Object

    Object = History
      Name      = IsisCube
      StartByte = 33619969
      Bytes     = 583
    End_Object

    Object = OriginalXmlLabel
      Name      = IsisCube
      StartByte = 33620552
      Bytes     = 11704
      ByteOrder = Lsb
    End_Object
    End
  )");

  QString dataFilePath = "data/isisimport/eispds/wacPushb/wac000xxx_2022126t000000_000000002_pushb_raw02.xml";
  QString dataFileName = "wac000xxx_2022126t000000_000000002_pushb_raw02.xml";
  QString imageFileName = "wac000xxx_2022126t000000_000000002_pushb_raw02.img";
  QString templateFile = "../appdata/import/PDS4/ClipperEIS.tpl";
  QString cubeFileName = tempDir.path() + "/WacPb.cub";


  int samples = 4096;
  int lines = 4096;
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

  if(!tempDataFile.open(QFile::ReadWrite | QFile::Text)){
      FAIL() << " Could not open file for writing";
  }

  // open xml to get data
  QFile realXmlFile(dataFilePath);
  if (!realXmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      FAIL() << "Failed to open file";
  }

  QTextStream xmlData(&tempDataFile);
  xmlData << realXmlFile.readAll();

  tempDataFile.close();
  realXmlFile.close();

  QFileInfo fileInfo(tempDataFile);

  // testing with template
  QVector<QString> args = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName, "template=" + templateFile};
  UserInterface options(APP_XML, args);
  isisimport(options);

  Pvl truthLabel;
  PvlInput >> truthLabel;

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

  // testing with out template
  QVector<QString> argsNoTemp = {"from=" + fileInfo.absoluteFilePath(), "to=" + cubeFileName};
  UserInterface optionsNoTemp(APP_XML, argsNoTemp);
  isisimport(optionsNoTemp);

  Cube outCubeNoTemp(cubeFileName);
  Pvl *outLabelNoTemp = outCubeNoTemp.label();

  truthGroup = truthLabel.findGroup("Dimensions", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Pixels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Instrument", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("BandBin", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("BandBin", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);

  truthGroup = truthLabel.findGroup("Kernels", Pvl::Traverse);
  outGroup = outLabelNoTemp->findGroup("Kernels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, outGroup, truthGroup);
}
