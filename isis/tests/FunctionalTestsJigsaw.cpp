#include <QtMath>

#include "Pvl.h"
#include "PvlGroup.h"
#include "ControlNet.h"
#include "Statistics.h"
#include "CSVReader.h"

#include "jigsaw.h"

#include "TestUtilities.h"
#include "Fixtures.h"
#include "gmock/gmock.h"

using namespace Isis;
using namespace testing; 


static QString APP_XML = FileName("$ISISROOT/bin/xml/jigsaw.xml").expanded();

TEST_F(ObservationPair, FunctionalTestJigsawCamSolveAll) {
  // delete to remove old camera for when cam is updated
  delete cubeL;
  delete cubeR;      
  
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName, 
                           "observations=yes", "update=yes", "Cksolvedegree=3", 
                           "Camsolve=all", "twist=no", "Spsolve=none", "Radius=no", "imagescsv=on", "file_prefix="+prefix.path()+"/"};

  UserInterface options(APP_XML, args);
  
  Pvl log; 
  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Unable to bundle: " << e.what() << std::endl;
  }
  
  // images were updated 
  cubeL = new Cube(cubeLPath, "r");
  cubeR = new Cube(cubeRPath, "r");
  
  ControlNet oNet;
  oNet.ReadControl(outCnetFileName);

  EXPECT_NEAR(oNet.AverageResidual(), 0.123132, 0.00001); 
  EXPECT_NEAR(oNet.GetMaximumResidual(), 0.379967, 0.00001);
  ASSERT_EQ(oNet.GetNumIgnoredMeasures(), 0);
  ASSERT_EQ(oNet.GetNumValidPoints(), 46);
  
  QList<ControlPoint*> points = oNet.GetPoints(); 
  
  Statistics xstats;
  Statistics ystats;
  Statistics zstats;

  for (int i = 0; i < points.size(); i++) { 
      xstats.AddData(points.at(i)->GetAdjustedSurfacePoint().GetX().kilometers());
      ystats.AddData(points.at(i)->GetAdjustedSurfacePoint().GetY().kilometers());
      zstats.AddData(points.at(i)->GetAdjustedSurfacePoint().GetZ().kilometers());
  }
  
  EXPECT_NEAR(xstats.Average(),           1556.64806314499741, 0.00001);
  EXPECT_NEAR(xstats.StandardDeviation(), 10.663072757957551,  0.00001);
  EXPECT_NEAR(xstats.Minimum(),           1540.43360835455860, 0.00001);
  EXPECT_NEAR(xstats.Maximum(),           1574.6528854394717,  0.00001);

  EXPECT_NEAR(ystats.Average(),           98.326253648503553, 0.00001);
  EXPECT_NEAR(ystats.StandardDeviation(), 1.3218686492693708, 0.00001);
  EXPECT_NEAR(ystats.Minimum(),           96.795117686735381, 0.00001);
  EXPECT_NEAR(ystats.Maximum(),           100.04990583087032, 0.00001);

  EXPECT_NEAR(zstats.Average(),           763.0309515939565,  0.00001);
  EXPECT_NEAR(zstats.StandardDeviation(), 19.783664466904419, 0.00001);
  EXPECT_NEAR(zstats.Minimum(),           728.82827218510067, 0.00001);
  EXPECT_NEAR(zstats.Maximum(),           793.9672179283682,  0.00001); 

  Camera *cam = cubeL->camera(); 
  SpiceRotation *rot = cam->instrumentRotation();
  std::vector<double> a1; 
  std::vector<double> a2; 
  std::vector<double> a3;

  rot->GetPolynomial(a1, a2, a3); 
  
  EXPECT_NEAR(a1.at(0), 2.16338,    0.0001);
  EXPECT_NEAR(a1.at(1), -0.0264475, 0.0001);
  EXPECT_NEAR(a1.at(2), 0.00469675, 0.0001);
  EXPECT_NEAR(a1.at(3), 0.0210955,  0.0001);
  
  EXPECT_NEAR(a2.at(0), 1.83011,     0.0001);
  EXPECT_NEAR(a2.at(1), -0.0244244,  0.0001);
  EXPECT_NEAR(a2.at(2), -0.00456569, 0.0001);
  EXPECT_NEAR(a2.at(3), 0.00637157,  0.0001);

  QFile file(prefix.path() + "/bundleout_images.csv");
  if (!file.open(QIODevice::ReadOnly)) {
    FAIL() << file.errorString().toStdString();
  }
  
  // skip the first two lines, we don't want to compare the header. 
  file.readLine();
  file.readLine(); 

  QString line = file.readLine();
  QStringList elems = line.split(","); 
  
  // RA(t0) final
  EXPECT_NEAR(elems.at(21).toDouble(), 123.9524918, 0.00001); 
  // RA(t1) final
  EXPECT_NEAR(elems.at(26).toDouble(), -1.51532975, 0.00001); 
  // RA(t2) final
  EXPECT_NEAR(elems.at(31).toDouble(), 0.2691039,   0.00001); 
  // RA(t3) final
  EXPECT_NEAR(elems.at(36).toDouble(), 1.208684781, 0.00001); 

  // DEC(t0) final
  EXPECT_NEAR(elems.at(41).toDouble(), 104.8575294,       0.00001); 
  // DEC(t1) final
  EXPECT_NEAR(elems.at(46).toDouble(), -1.399416621,      0.00001); 
  // DEC(t2) final
  EXPECT_NEAR(elems.at(51).toDouble(), -0.26159502200533, 0.00001); 
  // DEC(t3) final
  EXPECT_NEAR(elems.at(56).toDouble(), 0.365064224,       0.00001); 
  
  
  line = file.readLine();
  elems = line.split(","); 
  
  // RA(t0) final
  EXPECT_NEAR(elems.at(21).toDouble(), 121.4164029, 0.00001); 
  // RA(t1) final
  EXPECT_NEAR(elems.at(26).toDouble(), -1.510464718, 0.00001); 
  // RA(t2) final
  EXPECT_NEAR(elems.at(31).toDouble(), 0.253046705,   0.00001); 
  // RA(t3) final
  EXPECT_NEAR(elems.at(36).toDouble(), 1.203832854, 0.00001); 

  // DEC(t0) final
  EXPECT_NEAR(elems.at(41).toDouble(), 106.11241033284,      0.00001); 
  // DEC(t1) final
  EXPECT_NEAR(elems.at(46).toDouble(), -1.4160602752902001,  0.00001); 
  // DEC(t2) final
  EXPECT_NEAR(elems.at(51).toDouble(), -0.26704142,          0.00001); 
  // DEC(t3) final
  EXPECT_NEAR(elems.at(56).toDouble(), 0.365717165,          0.00001); 
  
}


TEST_F(ApolloNetwork, FunctionalTestJigsawHeldList) {
  QTemporaryDir prefix;
  
  QString heldlistpath = prefix.path() + "/heldlist.lis"; 
  FileList heldList; 
  heldList.append(cube6->fileName());
  heldList.write(heldlistpath); 

  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName, "heldlist="+heldlistpath,  
                           "radius=yes", "errorpropagation=yes", "spsolve=position", "Spacecraft_position_sigma=1000", 
                           "Residuals_csv=off", "Camsolve=angles", "Twist=yes", "Camera_angles_sigma=2", 
                           "Output_csv=off", "imagescsv=on", "file_prefix="+prefix.path()+"/"};

  UserInterface options(APP_XML, args);
  
  Pvl log; 
  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Unable to bundle: " << e.what() << std::endl;
  }

  CSVReader::CSVAxis csvLine;
  CSVReader header = CSVReader(prefix.path()+"/bundleout_images.csv",
                               false, 0, ',', false, true);

  csvLine = header.getRow(7);

  // assert corrections are very small 
  // X Correction
  EXPECT_LE(std::abs(csvLine[5].toDouble()), 1e-10); 
  // Y Correction
  EXPECT_LE(std::abs(csvLine[10].toDouble()), 1e-10); 
  // Z Correction
  EXPECT_LE(std::abs(csvLine[15].toDouble()), 1e-10); 
  // RA Correction
  EXPECT_LE(std::abs(csvLine[20].toDouble()), 1e-10); 
  // DEC Correction
  EXPECT_LE(std::abs(csvLine[25].toDouble()), 1e-10); 
  // TWIST Correction
  EXPECT_LE(std::abs(csvLine[30].toDouble()), 1e-10); 
}


TEST_F(ApolloNetwork, FunctionalTestJigsawHeldList) {
  QTemporaryDir prefix;
  
  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName,
                           ""
                           "Output_csv=off", "imagescsv=on", "file_prefix="+prefix.path()+"/"};

  UserInterface options(APP_XML, args);
  
  Pvl log; 
  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Unable to bundle: " << e.what() << std::endl;
  }

  CSVReader::CSVAxis csvLine;
  CSVReader header = CSVReader(prefix.path()+"/bundleout_images.csv",
                               false, 0, ',', false, true);

 
}
