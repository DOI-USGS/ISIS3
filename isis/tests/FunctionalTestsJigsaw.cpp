#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "ControlNet.h"
#include "Statistics.h"

#include "jigsaw.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/jigsaw.xml").expanded();

TEST_F(ObservationPair, FunctionalTestJigsawCamSolveAll) {
  // delete to remove old camera for when cam is updated
  delete cubeL;
  delete cubeR;      
  
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName, 
                           "observations=yes", "update=yes", "Cksolvedegree=3", 
                           "Camsolve=all", "twist=no", "Spsolve=none", "Radius=no", "imagescsv=on", "file_prefix="+prefix.path()};

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
  
  QVector<double> x(points.size()); 
  QVector<double> y(points.size()); 
  QVector<double> z(points.size()); 
  
  for (int i = 0; i < points.size(); i++) { 
      x.append(points.at(i)->GetAdjustedSurfacePoint().GetX().kilometers());
      y.append(points.at(i)->GetAdjustedSurfacePoint().GetY().kilometers());
      z.append(points.at(i)->GetAdjustedSurfacePoint().GetZ().kilometers());
  }
  
  Statistics xstats; 
  xstats.AddData(x.data(), x.size());
  EXPECT_NEAR(xstats.Average(),           778.32403157284591, 0.00001);
  EXPECT_NEAR(xstats.StandardDeviation(), 782.62477485560999, 0.00001);
  EXPECT_NEAR(xstats.Minimum(),           0,                  0.00001);
  EXPECT_NEAR(xstats.Maximum(),           1574.6528854394717, 0.00001);

  Statistics ystats; 
  ystats.AddData(y.data(), y.size());
  EXPECT_NEAR(ystats.Average(),           49.163126822565225, 0.00001);
  EXPECT_NEAR(ystats.StandardDeviation(), 49.441254933802206, 0.00001);
  EXPECT_NEAR(ystats.Minimum(),           0,                  0.00001);
  EXPECT_NEAR(ystats.Maximum(),           100.04990583087032, 0.00001);

  Statistics zstats; 
  zstats.AddData(z.data(), z.size());
  EXPECT_NEAR(zstats.Average(),           381.51547579648877, 0.00001);
  EXPECT_NEAR(zstats.StandardDeviation(), 383.85817640746325, 0.00001);
  EXPECT_NEAR(zstats.Minimum(),           0,                  0.00001);
  EXPECT_NEAR(zstats.Maximum(),           793.9672179283682, 0.00001); 

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
  
  EXPECT_NEAR(a3.at(0), 2.08085,     0.0001);
  EXPECT_NEAR(a3.at(1), -0.00813174, 0.0001);
  EXPECT_NEAR(a3.at(2), 0.000123724, 0.0001);
  EXPECT_NEAR(a3.at(3), 0.0,         0.0001);


}