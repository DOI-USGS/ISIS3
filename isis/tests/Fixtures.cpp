#include "Fixtures.h"
#include "LineManager.h"
#include "SpecialPixel.h"

namespace Isis {

  void TempTestingFiles::SetUp() {
    ASSERT_TRUE(tempDir.isValid());
  }


  void SmallCube::SetUp() {
    TempTestingFiles::SetUp();

    testCube = new Cube();
    testCube->setDimensions(10, 10, 10);
    testCube->create(tempDir.path() + "/small.cub");

    LineManager line(*testCube);
    double pixelValue = 0.0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) pixelValue++;
      }
      testCube->write(line);
    }

  }

  void SmallCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (testCube) {
      delete testCube;
    }
  }

  void SpecialSmallCube::SetUp() {
    TempTestingFiles::SetUp();

    testCube = new Cube();
    testCube->setDimensions(10, 10, 10);
    testCube->create(tempDir.path() + "/small.cub");

    // Use a line manager to update select lines with ISIS special pixel values
    LineManager line(*testCube);
    double pixelValue = 0.0;
    int lineNum = 0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        if (lineNum == 2) {
          line[i] = NULL8;
        }
        else if (lineNum == 3) {
          line[i] = LOW_REPR_SAT8;
        }
        else if (lineNum == 4) {
          line[i] = HIGH_REPR_SAT8;
        }
        else if (lineNum == 5) {
          line[i] = LOW_INSTR_SAT8;
        }
        else if (lineNum == 6) {
          line[i] = HIGH_INSTR_SAT8;
        }
        else {
          line[i] = (double) pixelValue++;
        }
      }
      lineNum++;
      testCube->write(line);
    }

  }

  void SpecialSmallCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (testCube) {
      delete testCube;
    }
  }


  void DefaultCube::SetUp() {
    TempTestingFiles::SetUp();
    std::ifstream isdFile("data/defaultImage/defaultCube.isd");
    std::ifstream cubeLabel("data/defaultImage/defaultCube.pvl");
    std::ifstream projCubeLabel("data/defaultImage/projDefaultCube.pvl");

    isdFile >> isd;
    cubeLabel >> label;
    projCubeLabel >> projLabel;

    testCube = new Cube();
    testCube->fromIsd(tempDir.path() + "/default.cub", label, isd, "rw");

    projTestCube = new Cube();
    projTestCube->fromIsd(tempDir.path() + "/default.level2.cub", projLabel, isd, "rw");
  }


  void DefaultCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (projTestCube->isOpen()) {
      projTestCube->close();
    }

    delete testCube;
    delete projTestCube;
  }

  void LineScannerCube::SetUp() {
    TempTestingFiles::SetUp();

    std::ifstream isdFile("data/LineScannerImage/defaultLineScanner.isd");
    std::ifstream cubeLabel("data/LineScannerImage/defaultLineScanner.pvl");
    std::ifstream projCubeLabel("data/LineScannerImage/projDefaultLineScanner.pvl");

    isdFile >> isd;
    cubeLabel >> label;
    projCubeLabel >> projLabel;

    testCube = new Cube();
    testCube->fromIsd(tempDir.path() + "/default.cub", label, isd, "rw");

    projTestCube = new Cube();
    projTestCube->fromIsd(tempDir.path() + "/default.level2.cub", projLabel, isd, "rw");
  }

  void LineScannerCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    if (projTestCube->isOpen()) {
      projTestCube->close();
    }

    delete testCube;
    delete projTestCube;
  }

  void Hayabusa2OncTSmallCube::SetUp() {
    TempTestingFiles::SetUp();

    std::ifstream cubeLabel("data/hayabusa2Image/hayabusa2OncTSmall.pvl");

    cubeLabel >> label;

    testCube = new Cube();
    testCube->fromLabel(tempDir.path() + "/hayabusa2OncTSmall.cub", label, "rw");

    LineManager line(*testCube);
    double pixelValue = 100.0;  // We need pixelValue * 4 > 300 (bias) for the calibration tests
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) pixelValue++;
      }
      testCube->write(line);
    }
  }

  void Hayabusa2OncTSmallCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    delete testCube;
  }

  void Hayabusa2OncTSmallCube::resetCube() {
    QString fileName = testCube->fileName();
    delete testCube;
    testCube = new Cube(fileName, "rw");
  }

  void Hayabusa2OncTCube::SetUp() {
    TempTestingFiles::SetUp();

    std::ifstream cubeLabel("data/hayabusa2Image/hayabusa2OncT.pvl");

    cubeLabel >> label;

    testCube = new Cube();
    testCube->fromLabel(tempDir.path() + "/hayabusa2OncT.cub", label, "rw");

    LineManager line(*testCube);
    double pixelValue = 100.0;  // We need pixelValue * 4 > 300 (bias) for the calibration tests
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) pixelValue;
      }
      testCube->write(line);
    }
  }

  void Hayabusa2OncTCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }

    delete testCube;
  }

  void ThreeImageNetwork::SetUp() {
    TempTestingFiles::SetUp();

    FileName labelPath1("data/threeImageNetwork/cube1.pvl");
    FileName labelPath2("data/threeImageNetwork/cube2.pvl");
    FileName labelPath3("data/threeImageNetwork/cube3.pvl");

    isdPath1 = new FileName("data/threeImageNetwork/cube1.isd");
    isdPath2 = new FileName("data/threeImageNetwork/cube2.isd");
    isdPath3 = new FileName("data/threeImageNetwork/cube3.isd");

    threeImageOverlapFile = new FileName("data/threeImageNetwork/threeImageOverlaps.lis");
    twoImageOverlapFile = new FileName("data/threeImageNetwork/twoImageOverlaps.lis");

    cube1 = new Cube();
    cube1->fromIsd(tempDir.path() + "/cube1.cub", labelPath1, *isdPath1, "rw");

    cube2 = new Cube();
    cube2->fromIsd(tempDir.path() + "/cube2.cub", labelPath2, *isdPath2, "rw");

    cube3 = new Cube();
    cube3->fromIsd(tempDir.path() + "/cube3.cub", labelPath3, *isdPath3, "rw");

    cubeList = new FileList();
    cubeList->append(cube1->fileName());
    cubeList->append(cube2->fileName());
    cubeList->append(cube3->fileName());

    cubeListFile = tempDir.path() + "/cubes.lis";
    cubeList->write(cubeListFile);

    network = new ControlNet();
    network->ReadControl("data/threeImageNetwork/controlnetwork.net");
  }

  void ThreeImageNetwork::TearDown() {
    delete cubeList;
    delete network;

    delete cube1;
    delete cube2;
    delete cube3;

    delete isdPath1;
    delete isdPath2;
    delete isdPath3; 

    delete threeImageOverlapFile; 
    delete twoImageOverlapFile; 
  }


  void Hayabusa2OncW2Cube::setInstrument(QString ikid, QString instrumentId, QString spacecraftName) {
    PvlGroup &kernels = testCube->label()->findObject("IsisCube").findGroup("Kernels");
    kernels.findKeyword("NaifFrameCode").setValue(ikid);    
    
    PvlGroup &inst = testCube->label()->findObject("IsisCube").findGroup("Instrument");
    std::istringstream iss(R"(
      Group = Instrument
      SpacecraftName                  = HAYABUSA-2
      InstrumentId                    = ONC-W2
      InstrumentName                  = "Optical Navigation Camera"
      TargetName                      = Mars
      StartTime                       = 2015-12-03T07:29:58.232
      StopTime                        = 2015-12-03T07:29:58.234
      ExposureDuration                = 0.00272 <seconds>
      RawSpacecraftClockCount         = 0x3C38845A <1/32 sec>
      Binning                         = 1
      SelectedImageAreaX1             = 1
      SelectedImageAreaY1             = 1
      SelectedImageAreaX2             = 1024
      SelectedImageAreaY2             = 1
      SelectedImageAreaX3             = 1
      SelectedImageAreaY3             = 1024
      SelectedImageAreaX4             = 1024
      SelectedImageAreaY4             = 1024
      SmearCorrection                 = NON
      OffsetCorrection                = N/A
      FlatCorrection                  = NON
      RadianceConversion              = NON
      PhotometricCorrection           = NON
      BandRegistration                = NON
      L2BFlatFileName                 = N/A
      L2BSystemEfficiencyFileName     = N/A
      L2CShapeModelFileName           = N/A
      L2DPhaseFunctionFileName        = N/A
      L2DShapeModelFileName           = N/A
      SubImageCount                   = 1
      BusLineVoltage                  = 49.28 <V>
      ONCCurrent                      = 0.52 <V>
      FLACCurrent                     = 0.00 <V>
      ONCAETemperature                = 1.53 <degC>
      ONCTOpticsTemperature           = 19.17 <degC>
      ONCTCCDTemperature              = -29.62 <degC>
      ONCTElectricCircuitTemperature  = -11.96 <degC>
      ONCW1OpticsTemperature          = 1.42 <degC>
      ONCW1CCDTemperature             = -24.98 <degC>
      ONCW1ElectricCircuitTemperature = -10.90 <degC>
      ONCW2OpticsTemperature          = 1.28 <degC>
      ONCW2CCDTemperature             = -24.67 <degC>
      ONCW2ElectricCircuitTemperature = -4.12 <degC>
      FLACTemperature                 = -15.27 <degC>
    End_Group
    )");
    
    PvlGroup newInstGroup; 
    iss >> newInstGroup; 

    newInstGroup.findKeyword("InstrumentId").setValue(instrumentId);
    newInstGroup.findKeyword("SpacecraftName").setValue(spacecraftName);
    inst = newInstGroup;

    PvlKeyword startcc("SpacecraftClockStartCount", "33322515");
    PvlKeyword stopcc("SpaceCraftClockStopCount", "33322516");
    inst += startcc;
    inst += stopcc; 

    PvlObject &naifKeywords = testCube->label()->findObject("NaifKeywords");
    
    json nk; 
    nk["INS"+ikid.toStdString()+"_FOCAL_LENGTH"] = 10.44;
    nk["INS"+ikid.toStdString()+"_PIXEL_PITCH"] = 0.013;
    nk["INS"+ikid.toStdString()+"_TRANSX"] = {0.0, 0.013, 0.0};
    nk["INS"+ikid.toStdString()+"_TRANSY"] = {0.0, 0.0, 0.013};
    nk["INS"+ikid.toStdString()+"_ITRANSS"] = {0.0, 76.923076923077, 0.0};
    nk["INS"+ikid.toStdString()+"_ITRANSL"] = {0.0, 0.0, 76.923076923077};
    nk["INS"+ikid.toStdString()+"_BORESIGHT_LINE"] = 490.5;
    nk["INS"+ikid.toStdString()+"_BORESIGHT_SAMPLE"] = 512.5;
    nk["INS"+ikid.toStdString()+"_OD_K"] = {1.014, 2.933e-07, -1.384e-13};
    nk["BODY499_RADII"] = {3396.19, 3396.19, 3376.2};
    nk["CLOCK_ET-37_33322515_COMPUTED"] = "8ed6ae8930f3bd41";
    nk["BODY_CODE"] = 499;
    nk["BODY_FRAME_CODE"] = 10014; 
    PvlObject newNaifKeywords("NaifKeywords", nk);
    naifKeywords = newNaifKeywords; 

    QString fileName = testCube->fileName();
    // need to remove old camera pointer 
    delete testCube;
    // This is now a Hayabusa cube
    testCube = new Cube(fileName, "rw");
  }
}
