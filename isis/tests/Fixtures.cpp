#include <QTextStream>

#include "Fixtures.h"
#include "LineManager.h"
#include "SpecialPixel.h"
#include "ControlNet.h"

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


  void LargeCube::SetUp() {
    TempTestingFiles::SetUp();

    testCube = new Cube();
    testCube->setDimensions(1000, 1000, 10);
    testCube->create(tempDir.path() + "/large.cub");

    LineManager line(*testCube);
    double pixelValue = 0.0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = pixelValue;
      }

      pixelValue++;
      testCube->write(line);
    }
  }

  void LargeCube::TearDown() {
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

  void DemCube::SetUp() {
    DefaultCube::SetUp();
    testCube->label()->object(4)["SolarLongitude"] = "294.73518831328";
    testCube->reopen("rw");

    std::ifstream cubeLabel("data/defaultImage/demCube.pvl");

    Pvl demLabel;
    cubeLabel >> demLabel;
    demLabel.findObject("IsisCube").findObject("Core").findGroup("Pixels")["Type"] = "Real";

    demCube = new Cube();
    demCube->fromLabel(tempDir.path() + "/demCube.cub", demLabel, "rw");

    TableField minRadius("MinimumRadius", TableField::Double);
    TableField maxRadius("MaximumRadius", TableField::Double);

    TableRecord record;
    record += minRadius;
    record += maxRadius;

    Table shapeModelStatistics("ShapeModelStatistics", record);

    record[0] = 3376.2;
    record[1] = 3396.19;
    shapeModelStatistics += record;

    demCube->write(shapeModelStatistics);

    int xCenter = int(demCube->lineCount()/2);
    int yCenter = int(demCube->sampleCount()/2);
    double radius = std::min(xCenter, yCenter);
    double depth = 30;
    double pointRadius;

    LineManager line(*demCube);
    double pixelValue;
    double base = demCube->label()->findObject("IsisCube").findObject("Core").findGroup("Pixels")["Base"];
    double xPos = 0.0;

    for(line.begin(); !line.end(); line++) {
      for(int yPos = 0; yPos < line.size(); yPos++) {
        pointRadius = pow(pow((xPos - xCenter), 2) + pow((yPos - yCenter), 2), 0.5);
        if (pointRadius < radius) {
          pixelValue = ((sin(((M_PI*pointRadius)/(2*radius))) * depth) + depth) + base;
        }
        else {
          pixelValue = base + (depth * 2);
        }
        line[yPos] = (double) pixelValue;
      }
      xPos++;
      demCube->write(line);
    }

    demCube->reopen("rw");
  }

  void DemCube::TearDown() {
    if (demCube->isOpen()) {
      demCube->close();
    }

    delete demCube;
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

    LineManager line(*testCube);
    int pixelValue = 1;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) (pixelValue % 255);
        pixelValue++;
      }
      testCube->write(line);
    }

    projTestCube = new Cube();
    projTestCube->fromIsd(tempDir.path() + "/default.level2.cub", projLabel, isd, "rw");

    line = LineManager(*projTestCube);
    pixelValue = 1;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) (pixelValue % 255);
        pixelValue++;
      }
      projTestCube->write(line);
    }
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

    ImagePolygon poly;
    coords = {{30, 0},
              {30, 10},
              {35, 10},
              {35, 0},
              {30, 0}};
    poly.Create(coords);
    cube1->write(poly);

    cube2 = new Cube();
    cube2->fromIsd(tempDir.path() + "/cube2.cub", labelPath2, *isdPath2, "rw");

    coords = {{31, 1},
              {31, 11},
              {36, 11},
              {36, 1},
              {31, 1}};
    poly.Create(coords);
    cube2->write(poly);

    cube3 = new Cube();
    cube3->fromIsd(tempDir.path() + "/cube3.cub", labelPath3, *isdPath3, "rw");

    LineManager line(*cube1);
    LineManager line2(*cube2);
    LineManager line3(*cube3);
    int pixelValue = 1;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) (pixelValue %255);
        pixelValue++;
      }
      cube1->write(line);
    }

    for(line2.begin(); !line2.end(); line2++) {
      for(int i = 0; i < line.size(); i++) {
        line2[i] = (double) (pixelValue %255);
        pixelValue++;
      }
      cube2->write(line2);
    }

    for(line3.begin(); !line3.end(); line3++) {
      for(int i = 0; i < line3.size(); i++) {
        line3[i] = (double) (pixelValue %255);
        pixelValue++;
      }
      cube3->write(line3);
    }

    cube1->reopen("rw");
    cube2->reopen("rw");
    cube3->reopen("rw");

    cubeList = new FileList();
    cubeList->append(cube1->fileName());
    cubeList->append(cube2->fileName());
    cubeList->append(cube3->fileName());

    cubeListFile = tempDir.path() + "/cubes.lis";
    cubeList->write(cubeListFile);

    networkFile = "data/threeImageNetwork/controlnetwork.net";

    network = new ControlNet();
    network->ReadControl(networkFile);
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

  void ApolloNetwork::SetUp() {
    TempTestingFiles::SetUp();

    cubes.fill(nullptr, 7);

    cubeList = new FileList();

    for(int i = 0; i < cubes.size(); i++) {
      int n = i+1; // filenames use 1 based indexing
      isdFiles.push_back(FileName("data/apolloNetwork/apolloImage"+QString::number(n)+".isd"));
      labelFiles.push_back(FileName("data/apolloNetwork/apolloImage"+QString::number(n)+".pvl"));
      cubes[i] = new Cube();
      cubes[i]->fromIsd(tempDir.path() + "/cube"+QString::number(n)+".cub", labelFiles[i], isdFiles[i], "rw");
      cubeList->append(cubes[i]->fileName());
    }

    cubeListFile = tempDir.path() + "/cubes.lis";
    cubeList->write(cubeListFile);

    network = new ControlNet("data/apolloNetwork/apolloNet.pvl");
    controlNetPath = tempDir.path() + "/apolloNet.net";
    network->Write(controlNetPath);
  }

  void ApolloNetwork::TearDown() {
    for(int i = 0; i < cubes.size(); i++) {
      if(cubes[i] && cubes[i]->isOpen()) {
        delete cubes[i];
      }
    }

    if (cubeList) {
      delete cubeList;
    }
  }

  void ObservationPair::SetUp() {
      FileName labelPathL = FileName("data/observationPair/observationImageL.pvl");
      FileName labelPathR = FileName("data/observationPair/observationImageR.pvl");

      isdPathL = new FileName("data/observationPair/observationImageL.isd");
      isdPathR = new FileName("data/observationPair/observationImageR.isd");

      cubeL = new Cube();
      cubeR = new Cube();

      cubeLPath = tempDir.path() + "observationPairL.cub";
      cubeRPath = tempDir.path() + "/observationPairR.cub";

      cubeL->fromIsd(cubeLPath, labelPathL, *isdPathL, "rw");
      Pvl originalPdsLabL("data/observationPair/observationImageLOriginalLabel.pvl");
      OriginalLabel origLabel(originalPdsLabL);
      cubeL->write(origLabel);
      cubeL->reopen("rw");

      cubeR->fromIsd(cubeRPath, labelPathR, *isdPathR, "rw");

      cubeList = new FileList();
      cubeList->append(cubeL->fileName());
      cubeList->append(cubeR->fileName());

      cubeListFile = tempDir.path() + "/cubes.lis";
      cubeList->write(cubeListFile);

      cnetPath = "data/observationPair/observationPair.net";
      network = new ControlNet();
      network->ReadControl(cnetPath);
  }


  void ObservationPair::TearDown() {
    delete cubeList;
    delete network;

    if (cubeL) {
      delete cubeL;
    }

    if (cubeR) {
      delete cubeR;
    }

    delete isdPathL;
    delete isdPathR;
  }

  void MroHiriseCube::SetUp() {
    DefaultCube::SetUp();
    dejitteredCube.open("data/mroKernels/mroHiriseProj.cub");

    // force real DNs
    QString fname = testCube->fileName();

    PvlObject &core = label.findObject("IsisCube").findObject("Core");
    PvlGroup &pixels = core.findGroup("Pixels");
    pixels.findKeyword("Type").setValue("Real");

    delete testCube;
    testCube = new Cube();

    FileName newCube(tempDir.path() + "/testing.cub");

    testCube->fromIsd(newCube, label, isd, "rw");
    PvlGroup &kernels = testCube->label()->findObject("IsisCube").findGroup("Kernels");
    kernels.findKeyword("NaifFrameCode").setValue("-74999");
    PvlGroup &inst = testCube->label()->findObject("IsisCube").findGroup("Instrument");
    std::istringstream iss(R"(
      Group = Instrument
        SpacecraftName              = "MARS RECONNAISSANCE ORBITER"
        InstrumentId                = HIRISE
        TargetName                  = Mars
        StartTime                   = 2008-05-17T09:37:24.7300819
        StopTime                    = 2008-05-17T09:37:31.0666673
        ObservationStartCount       = 895484264:44383
        SpacecraftClockStartCount   = 895484264:57342
        SpacecraftClockStopCount    = 895484272:12777
        ReadoutStartCount           = 895484659:31935
        CalibrationStartTime        = 2006-11-08T04:49:13.952
        CalibrationStartCount       = 847428572:51413
        AnalogPowerStartTime        = 2006-11-08T04:48:34.478
        AnalogPowerStartCount       = 847428533:20297
        MissionPhaseName            = "PRIMARY SCIENCE PHASE"
        LineExposureDuration        = 95.0625 <MICROSECONDS>
        ScanExposureDuration        = 95.0625 <MICROSECONDS>
        DeltaLineTimerCount         = 337
        Summing                     = 1
        Tdi                         = 128
        FocusPositionCount          = 2020
        PoweredCpmmFlag             = (On, On, On, On, On, On, On, On, On, On, On,
                                      On, On, On)
        CpmmNumber                  = 8
        CcdId                       = RED5
        ChannelNumber               = 0
        LookupTableType             = Stored
        LookupTableNumber           = 19
        LookupTableMinimum          = -9998
        LookupTableMaximum          = -9998
        LookupTableMedian           = -9998
        LookupTableKValue           = -9998
        StimulationLampFlag         = (Off, Off, Off)
        HeaterControlFlag           = (On, On, On, On, On, On, On, On, On, On, On,
                                      On, On, On)
        OptBnchFlexureTemperature   = 19.5881 <C>
        OptBnchMirrorTemperature    = 19.6748 <C>
        OptBnchFoldFlatTemperature  = 19.9348 <C>
        OptBnchFpaTemperature       = 19.5015 <C>
        OptBnchFpeTemperature       = 19.2415 <C>
        OptBnchLivingRmTemperature  = 19.4148 <C>
        OptBnchBoxBeamTemperature   = 19.5881 <C>
        OptBnchCoverTemperature     = 19.6748 <C>
        FieldStopTemperature        = 17.9418 <C>
        FpaPositiveYTemperature     = 18.8082 <C>
        FpaNegativeYTemperature     = 18.6349 <C>
        FpeTemperature              = 18.0284 <C>
        PrimaryMirrorMntTemperature = 19.5015 <C>
        PrimaryMirrorTemperature    = 19.6748 <C>
        PrimaryMirrorBafTemperature = 2.39402 <C>
        MsTrussLeg0ATemperature     = 19.6748 <C>
        MsTrussLeg0BTemperature     = 19.8482 <C>
        MsTrussLeg120ATemperature   = 19.3281 <C>
        MsTrussLeg120BTemperature   = 20.1949 <C>
        MsTrussLeg240ATemperature   = 20.2816 <C>
        MsTrussLeg240BTemperature   = 20.7151 <C>
        BarrelBaffleTemperature     = -13.8299 <C>
        SunShadeTemperature         = -33.9377 <C>
        SpiderLeg30Temperature      = 17.5087 <C>
        SpiderLeg120Temperature     = -9999
        SpiderLeg240Temperature     = -9999
        SecMirrorMtrRngTemperature  = 20.6284 <C>
        SecMirrorTemperature        = 20.455 <C>
        SecMirrorBaffleTemperature  = -11.1761 <C>
        IeaTemperature              = 25.4878 <C>
        FocusMotorTemperature       = 21.4088 <C>
        IePwsBoardTemperature       = 16.3696 <C>
        CpmmPwsBoardTemperature     = 17.6224 <C>
        MechTlmBoardTemperature     = 34.7792 <C>
        InstContBoardTemperature    = 34.4121 <C>
        DllLockedFlag               = (YES, YES)
        DllResetCount               = 0
        DllLockedOnceFlag           = (YES, YES)
        DllFrequenceCorrectCount    = 4
        ADCTimingSetting            = -9999
        Unlutted                    = TRUE
      End_Group
    )");

    PvlGroup newInstGroup;
    iss >> newInstGroup;

    newInstGroup.findKeyword("InstrumentId").setValue("HIRISE");
    newInstGroup.findKeyword("SpacecraftName").setValue("MARS RECONNAISSANCE ORBITER");

    inst = newInstGroup;
    PvlObject &naifKeywords = testCube->label()->findObject("NaifKeywords");

    PvlKeyword startcc("SpacecraftClockStartCount", "33322515");
    PvlKeyword stopcc("SpaceCraftClockStopCount", "33322516");
    inst += startcc;
    inst += stopcc;

    json nk;
    nk["INS-74999_FOCAL_LENGTH"] = 11994.9988;
    nk["INS-74999_PIXEL_PITCH"] = 0.012;
    nk["INS-74605_TRANSX"] = {-89.496, -1.0e-06, 0.012};
    nk["INS-74605_TRANSY"] = {-12.001, -0.012, -1.0e-06};
    nk["INS-74605_ITRANSS"] = {-1000.86, -0.0087, -83.333};
    nk["INS-74605_ITRANSL"] = {7457.9, 83.3333, -0.0087};
    nk["INS-74999_OD_K"] = {-0.0048509, 2.41312e-07, -1.62369e-13};
    nk["BODY499_RADII"] = {3396.19, 3396.19, 3376.2};
    nk["CLOCK_ET_-74999_895484264:57342_COMPUTED"] = "8ed6ae8930f3bd41";

    nk["BODY_CODE"] = 499;
    nk["BODY_FRAME_CODE"] = 10014;
    PvlObject newNaifKeywords("NaifKeywords", nk);
    naifKeywords = newNaifKeywords;

    QString fileName = testCube->fileName();

    LineManager line(*testCube);
    for(line.begin(); !line.end(); line++) {
        for(int i = 0; i < line.size(); i++) {
          line[i] = (double)(i+1);
        }
        testCube->write(line);
    }
    testCube->reopen("rw");

    // need to remove old camera pointer
    delete testCube;
    // This is now a MRO cube

    testCube = new Cube(fileName, "rw");

    // create a jitter file
    QString jitter = R"(# Sample                 Line                   ET
-0.18     -0.07     264289109.96933
-0.11     -0.04     264289109.97
-0.05     -0.02     264289109.98
1.5     0.6     264289110.06
    )";

    jitterPath = tempDir.path() + "/jitter.txt";
    QFile jitterFile(jitterPath);

    if (jitterFile.open(QIODevice::WriteOnly)) {
      QTextStream out(&jitterFile);
      out << jitter;
      jitterFile.close();
    }
    else {
      FAIL() << "Failed to create Jitter file" << std::endl;
    }
  }


  void NewHorizonsCube::setInstrument(QString ikid, QString instrumentId, QString spacecraftName) {
    PvlObject &isisCube = testCube->label()->findObject("IsisCube");

    label = Pvl();
    label.addObject(isisCube);

    PvlGroup &kernels = label.findObject("IsisCube").findGroup("Kernels");
    kernels.findKeyword("NaifFrameCode").setValue(ikid);
    kernels["ShapeModel"] = "Null";

    PvlGroup &dim = label.findObject("IsisCube").findObject("Core").findGroup("Dimensions");
    dim.findKeyword("Samples").setValue("10");
    dim.findKeyword("Lines").setValue("10");
    dim.findKeyword("Bands").setValue("2");

    PvlGroup &pixels = label.findObject("IsisCube").findObject("Core").findGroup("Pixels");
    pixels.findKeyword("Type").setValue("Real");

    PvlGroup &inst = label.findObject("IsisCube").findGroup("Instrument");
    std::istringstream iss(R"(
      Group = Instrument
        SpacecraftName            = "NEW HORIZONS"
        InstrumentId              = LEISA
        TargetName                = Jupiter
        SpacecraftClockStartCount = 1/0034933739:00000
        ExposureDuration          = 0.349
        StartTime                 = 2007-02-28T01:57:01.3882862
        StopTime                  = 2007-02-28T02:04:53.3882861
        FrameRate                 = 2.86533 <Hz>
      End_Group
    )");

    PvlGroup newInstGroup;
    iss >> newInstGroup;

    newInstGroup.findKeyword("InstrumentId").setValue(instrumentId);
    newInstGroup.findKeyword("SpacecraftName").setValue(spacecraftName);

    inst = newInstGroup;

    PvlGroup &bandBin = label.findObject("IsisCube").findGroup("BandBin");
    std::istringstream bss(R"(
      Group = BandBin
        Center       = (2.4892, 1.2204)
        Width        = (0.011228, 0.005505)
        OriginalBand = (1, 200)
      End_Group
    )");

    PvlGroup newBandBin;
    bss >> newBandBin;
    bandBin = newBandBin;

    std::istringstream alphaSS(R"(
      Group = AlphaCube
        AlphaSamples        = 256
        AlphaLines          = 1354
        AlphaStartingSample = 0.5
        AlphaStartingLine   = 229.5
        AlphaEndingSample   = 100.5
        AlphaEndingLine     = 329.5
        BetaSamples         = 100
        BetaLines           = 100
      End_Group
    )");

    PvlGroup alphaGroup;
    alphaSS >> alphaGroup;
    label.findObject("IsisCube").addGroup(alphaGroup);

    std::ifstream isdFile("data/leisa/nh_leisa.isd");
    isdFile >> isd;

    QString fileName = tempDir.path() + "/leisa.cub";
    delete testCube;
    testCube = new Cube();
    testCube->fromIsd(fileName, label, isd, "rw");

    LineManager line(*testCube);
    double pixelValue = 0.0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = (double) pixelValue++;
      }
      testCube->write(line);
    }
  }


  void ApolloCube::SetUp() {
    TempTestingFiles::SetUp();

    testCube = new Cube();
    testCube->setDimensions(22900, 22900, 1);
    testCube->create(tempDir.path() + "/large.cub");

    LineManager line(*testCube);
    double pixelValue = 0.0;
    for(line.begin(); !line.end(); line++) {
      for(int i = 0; i < line.size(); i++) {
        line[i] = pixelValue;
      }

      pixelValue++;
      testCube->write(line);
    }

    PvlGroup reseaus("Reseaus");
    PvlKeyword samples = PvlKeyword("Sample", "200");
    samples += "400";
    samples += "600";

    PvlKeyword lines = PvlKeyword("Line", "200");
    lines += "400";
    lines += "600";

    PvlKeyword types = PvlKeyword("Type", "5");
    types += "5";
    types += "5";

    PvlKeyword valid = PvlKeyword("Valid", "1");
    valid += "1";
    valid += "1";

    reseaus += lines;
    reseaus += samples;
    reseaus += types;
    reseaus += valid;
    reseaus += PvlKeyword("Status", "Nominal");

    std::istringstream instStr (R"(
      Group = Instrument
          SpacecraftName = "APOLLO 15"
          InstrumentId   = METRIC
          TargetName     = MOON
          StartTime      = 1971-08-01T14:58:03.78
      End_Group
    )");

    PvlGroup instGroup;
    instStr >> instGroup;

    Pvl *lab = testCube->label();
    lab->findObject("IsisCube").addGroup(reseaus);
    lab->findObject("IsisCube").addGroup(instGroup);

    testCube->reopen("r");
  }

  void RingsCube::SetUp() {
    TempTestingFiles::SetUp();

    ring1 = new Cube("data/rings/rings1proj.cub", "r");
    ring2 = new Cube("data/rings/rings2proj.cub", "r");

    cubeListPath = tempDir.path() + "/filelist.txt";
    cubeFileList.append("data/rings/rings1proj.cub");
    cubeFileList.append("data/rings/rings2proj.cub");
    cubeFileList.write(cubeListPath);
  }

}
