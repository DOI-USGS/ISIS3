#include "pointreg.h"

#include "Fixtures.h"
#include "TestUtilities.h"
#include "UserInterface.h"
#include "ControlNet.h"
#include "LineManager.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/pointreg.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestPointregDefault) {
  // Populate cubes
  LineManager line(*cube1);
  double pixelValue = 0.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      line[i] = pixelValue++;
    }
    cube1->write(line);
    cube2->write(line);
    cube3->write(line);
  }
  cube1->close();
  cube2->close();
  cube3->close();

  Pvl log;
  QTemporaryDir prefix;
  QString flatFilePath = prefix.path() + "/flatfile.csv";
  QString outNetPath = prefix.path() + "/outNet.net";
  QVector<QString> args = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath };
  UserInterface options(APP_XML, args);

  try {
    pointreg(options, &log);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  // Check app log
  PvlGroup points = log.findGroup("Points");
  PvlGroup measures = log.findGroup("Measures");
  ASSERT_EQ(int(points["Total"]), 16);
  ASSERT_EQ(int(measures["Registered"]), 24);
  
  // Check flatFile
  QFile flatFile(flatFilePath);
  ASSERT_TRUE(flatFile.size() > 0) ;

  // Check output control network
  ControlNet outNet(outNetPath);
  ASSERT_EQ(int(outNet.GetNumPoints()), 16);
  ASSERT_EQ(int(outNet.GetNumMeasures()), 41);
}

TEST_F(ThreeImageNetwork, FunctionalTestPointregFailOptions) {
  // Populate cubes
  LineManager line(*cube1);
  double pixelValue = 0.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      line[i] = pixelValue++;
    }
    cube1->write(line);
    cube2->write(line);
    cube3->write(line);
  }
  cube1->close();
  cube2->close();
  cube3->close();

  Pvl log;
  QTemporaryDir prefix;
  QString flatFilePath = prefix.path() + "/flatfile.csv";
  QString outNetPath = prefix.path() + "/outNet.net";
  QVector<QString> args = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputignored=no",
                            "outputfailed=no",
                            "points=all",
                            "measures=all" };
  UserInterface options(APP_XML, args);

  try {
    pointreg(options, &log);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  // Check app log
  PvlGroup points = log.findGroup("Points");
  PvlGroup measures = log.findGroup("Measures");
  ASSERT_EQ(int(points["Total"]), 15);
  ASSERT_EQ(int(measures["Registered"]), 24);
  
  // Check flatFile
  QFile flatFile(flatFilePath);
  ASSERT_TRUE(flatFile.size() > 0) ;

  // Check output control network
  ControlNet outNet(outNetPath);
  ASSERT_EQ(int(outNet.GetNumPoints()), 15);
  ASSERT_EQ(int(outNet.GetNumMeasures()), 39);
}

TEST_F(ThreeImageNetwork, FunctionalTestPointregOutputOptions) {
  // Populate cubes
  LineManager line(*cube1);
  double pixelValue = 0.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      line[i] = pixelValue++;
    }
    cube1->write(line);
    cube2->write(line);
    cube3->write(line);
  }
  cube1->close();
  cube2->close();
  cube3->close();

  QTemporaryDir prefix;
  QString flatFilePath = prefix.path() + "/flatfile.csv";
  QString outNetPath = prefix.path() + "/outNet.net";
  QVector<QString> argsA = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputfailed=no",
                            "points=ignored" };
  QVector<QString> argsB = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "measures=candidates",
                            "points=ignored" };
  QVector<QString> argsC = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputfailed=no",
                            "points=nonignored" };
  QVector<QString> argsD = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputignored=no",
                            "points=nonignored" };
  QVector<QString> argsE = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputignored=no",
                            "points=ignored" };
  UserInterface optionsA(APP_XML, argsA);  // Output ignored, register ignored
  UserInterface optionsB(APP_XML, argsB);  // Output ignored, unmeasured, register ignored
  UserInterface optionsC(APP_XML, argsC);  // Output ignored only
  UserInterface optionsD(APP_XML, argsD);  // Output unmeasured only
  UserInterface optionsE(APP_XML, argsE);  // Output unmeasured, register ignored
  Pvl logA, logB, logC, logD, logE;

  try {
    pointreg(optionsA, &logA);
    pointreg(optionsB, &logB);
    pointreg(optionsC, &logC);
    pointreg(optionsD, &logD);
    pointreg(optionsE, &logE);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  // Check app log
  PvlGroup pointsA = logA.findGroup("Points");
  PvlGroup pointsB = logB.findGroup("Points");
  PvlGroup pointsC = logC.findGroup("Points");
  PvlGroup pointsD = logD.findGroup("Points");
  PvlGroup pointsE = logE.findGroup("Points");
  PvlGroup measuresA = logA.findGroup("Measures");
  PvlGroup measuresB = logB.findGroup("Measures");
  PvlGroup measuresC = logC.findGroup("Measures");
  PvlGroup measuresD = logD.findGroup("Measures");
  PvlGroup measuresE = logE.findGroup("Measures");
  PvlGroup surfaceA= logA.findGroup("SurfaceModelFailures");
  PvlGroup surfaceB= logB.findGroup("SurfaceModelFailures");
  PvlGroup surfaceC= logC.findGroup("SurfaceModelFailures");
  PvlGroup surfaceD= logD.findGroup("SurfaceModelFailures");
  PvlGroup surfaceE= logE.findGroup("SurfaceModelFailures");

  ASSERT_EQ(int(pointsA["Total"]), 16);
  ASSERT_EQ(int(pointsB["Total"]), 16);
  ASSERT_EQ(int(pointsC["Total"]), 16);
  ASSERT_EQ(int(pointsD["Total"]), 15);
  ASSERT_EQ(int(pointsE["Total"]), 16);
  ASSERT_EQ(int(measuresA["Registered"]), 0);
  ASSERT_EQ(int(measuresB["Registered"]), 0);
  ASSERT_EQ(int(measuresC["Registered"]), 24);
  ASSERT_EQ(int(measuresD["Registered"]), 24);
  ASSERT_EQ(int(measuresE["Registered"]), 0);
  ASSERT_EQ(int(surfaceA["SurfaceModelNotEnoughValidData"]), 0);
  ASSERT_EQ(int(surfaceB["SurfaceModelNotEnoughValidData"]), 0);
  ASSERT_EQ(int(surfaceC["SurfaceModelNotEnoughValidData"]), 1);
  ASSERT_EQ(int(surfaceD["SurfaceModelNotEnoughValidData"]), 1);
  ASSERT_EQ(int(surfaceE["SurfaceModelNotEnoughValidData"]), 0);
}

TEST_F(ThreeImageNetwork, FunctionalTestPointregRegisterOptions) {
  // Populate cubes
  LineManager line(*cube1);
  double pixelValue = 0.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      line[i] = pixelValue++;
    }
    cube1->write(line);
    cube2->write(line);
    cube3->write(line);
  }
  cube1->close();
  cube2->close();
  cube3->close();

  QTemporaryDir prefix;
  QString flatFilePath = prefix.path() + "/flatfile.csv";
  QString outNetPath = prefix.path() + "/outNet.net";
  QVector<QString> argsA = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputfailed=no",
                            "outputignored=no",
                            "points=ignored" };
  QVector<QString> argsB = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "outputfailed=no",
                            "outputignored=no" };

  UserInterface optionsA(APP_XML, argsA);  // Register ignored only
  UserInterface optionsB(APP_XML, argsB);  // Register valid only
  Pvl logA, logB;

  try {
    pointreg(optionsA, &logA);
    pointreg(optionsB, &logB);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  // Check app log
  PvlGroup pointsA = logA.findGroup("Points");
  PvlGroup pointsB = logB.findGroup("Points");
  PvlGroup measuresA = logA.findGroup("Measures");
  PvlGroup measuresB = logB.findGroup("Measures");

  ASSERT_EQ(int(pointsA["Total"]), 16);
  ASSERT_EQ(int(pointsB["Total"]), 15);
  ASSERT_EQ(int(measuresA["Registered"]), 0);
  ASSERT_EQ(int(measuresB["Registered"]), 24);
}

TEST_F(ThreeImageNetwork, FunctionalTestPointregValidation) {
  // Populate cubes
  LineManager line(*cube1);
  double pixelValue = 0.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      line[i] = pixelValue++;
    }
    cube1->write(line);
    cube2->write(line);
    cube3->write(line);
  }
  cube1->close();
  cube2->close();
  cube3->close();

  QTemporaryDir prefix;
  QString flatFilePath = prefix.path() + "/flatfile.csv";
  QString outNetPath = prefix.path() + "/outNet.net";

  QString falsePosPathA = prefix.path() + "/falsePosA.csv";
  QString falsePosPathB = prefix.path() + "/falsePosB.csv";
  QString falsePosPathC = prefix.path() + "/falsePosC.csv";

  QVector<QString> argsA = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "validate=after",
                            "falsepositives=" + falsePosPathA,
                            "revert=no", "shift=0.1",
                            "points=nonignored" };
  QVector<QString> argsB = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "validate=only",
                            "falsepositives=" + falsePosPathB,
                            "search=7", "shift=0.1",
                            "points=nonignored" };
  QVector<QString> argsC = { "fromlist=" + cubeListFile,
                            "cnet=data/threeImageNetwork/controlnetwork.net",
                            "deffile=data/threeImageNetwork/autoRegTemplate.def",
                            "flatfile=" + flatFilePath,
                            "onet=" + outNetPath,
                            "validate=only",
                            "falsepositives=" + falsePosPathC,
                            "search=7", "shift=0.1", "restolerance=0.0",
                            "points=nonignored" };
  UserInterface optionsA(APP_XML, argsA);  // Test the validation without reverting failed registration
  UserInterface optionsB(APP_XML, argsB);  // Test the validation with reverting failed registration
  // Test the validation with everything being skipped due restolerance=0.0
  UserInterface optionsC(APP_XML, argsC);
  Pvl logA, logB, logC;

  try {
    pointreg(optionsA, &logA);
    pointreg(optionsB, &logB);
    pointreg(optionsC, &logC);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  // Check app log
  PvlGroup pointsA = logA.findGroup("Points");
  PvlGroup pointsB = logB.findGroup("Points");
  PvlGroup pointsC = logC.findGroup("Points");
  PvlGroup measuresA = logA.findGroup("Measures");
  PvlGroup measuresB = logB.findGroup("Measures");
  PvlGroup measuresC = logC.findGroup("Measures");
  PvlGroup validA= logA.findGroup("ValidationStatistics");
  PvlGroup validB= logB.findGroup("ValidationStatistics");
  PvlGroup validC= logC.findGroup("ValidationStatistics");

  ASSERT_EQ(int(pointsA["Total"]), 16);
  ASSERT_EQ(int(pointsB["Total"]), 16);
  ASSERT_EQ(int(pointsC["Total"]), 16);
  ASSERT_EQ(int(measuresA["Registered"]), 24);
  ASSERT_EQ(int(measuresB["Registered"]), 0);
  ASSERT_EQ(int(measuresC["Registered"]), 0);
  ASSERT_EQ(int(validA["Total"]), 24);
  ASSERT_EQ(int(validB["Total"]), 0);
  ASSERT_EQ(int(validC["Total"]), 0);
  
  QFile falsePosA(falsePosPathA);  // Should be populated
  QFile falsePosB(falsePosPathB);  // Should be empty
  QFile falsePosC(falsePosPathC);  // Should be empty

  ASSERT_TRUE(falsePosA.size() > falsePosB.size());
  ASSERT_EQ(falsePosB.size(), falsePosC.size());
}
