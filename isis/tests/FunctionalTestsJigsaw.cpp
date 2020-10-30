#include <iostream>

#include <QFile>
#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "jigsaw.h"
#include "Angle.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "Distance.h"
#include "FileName.h"
#include "Fixtures.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SurfacePoint.h"
#include "TestUtilities.h"
#include "gmock/gmock.h"
#include "UserInterface.h"
#include "CSVReader.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Statistics.h"

#include "jigsaw.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/jigsaw.xml").expanded();

TEST_F(ApolloNetwork, FunctionalTestJigsawApollo) {
  QVector<QString> args = {"radius=yes",
                            "errorpropagation=yes",
                            "spsolve=position",
                            "spacecraft_position_sigma=1000.0",
                            "camsolve=angles",
                            "twist=yes",
                            "camera_angles_sigma=2.",
                            "update=no",
                            "bundleout_txt=no",
                            "cnet="+controlNetPath,
                            "fromlist="+tempDir.path() + "/cubes.lis",
                            "onet="+tempDir.path()+"/apollo_out.net",
                            "file_prefix="+tempDir.path()+"/"};

  UserInterface ui(APP_XML, args);

  jigsaw(ui);

  // Test points.csv, images.csv, residuals.csv 

  QString pointsOutput = tempDir.path() + "/bundleout_points.csv";
  QString imagesOutput = tempDir.path() + "/bundleout_images.csv";
  QString residualsOutput = tempDir.path() + "/residuals.csv";
  ControlNet outputNet(tempDir.path()+"/apollo_out.net");

  // Check for the correct header output format and csv file structure for the points.csv file
  CSVReader::CSVAxis csvLine;
  CSVReader line = CSVReader(pointsOutput,
                               false, 0, ',', false, true);

  int numColumns = line.columns();
  int numRows = line.rows();

  ASSERT_EQ(numColumns, 12);
  ASSERT_EQ(numRows, 398);

  // Validate the line information is correct
  csvLine = line.getRow(0);
  compareCsvLine(csvLine, "3-d,3-d,3-d,Sigma,Sigma,Sigma,Correction,Correction,Correction,Coordinate,Coordinate,Coordinate");

  csvLine = line.getRow(1);
  compareCsvLine(csvLine, "Point,Point,Accepted,Rejected,Residual,Latitude,Longitude,Radius,Latitude,Longitude,Radius,Latitude,Longitude,Radius,X,Y,Z");

  csvLine = line.getRow(2);
  compareCsvLine(csvLine, "Label,Status,Measures,Measures,RMS,(dd),(dd),(km),(m),(m),(m),(m),(m),(m),(km),(km),(km)");

  // Compare all of the values from the network against the values in the CSV 
  QList<ControlPoint*> points = outputNet.GetPoints();

  EXPECT_EQ(numRows-3, points.length());

  ControlPoint* outputPoint;
  for (int i=3; i < numRows; i++) {
    csvLine = line.getRow(i);
    EXPECT_NO_THROW({
      outputPoint = outputNet.GetPoint(csvLine[0]); 
    }) << "Point in points.csv file is not present in the output network.";
    EXPECT_EQ(outputPoint->GetPointTypeString().toUpper().toStdString(), QString(csvLine[1]).toStdString());
    EXPECT_EQ(outputPoint->GetNumMeasures() - outputPoint->GetNumberOfRejectedMeasures(), csvLine[2].toInt());
    EXPECT_EQ(outputPoint->GetNumberOfRejectedMeasures(), csvLine[3].toInt());
    EXPECT_NEAR(outputPoint->GetResidualRms(), csvLine[4].toDouble(), 0.01);
    SurfacePoint sp = outputPoint->GetAdjustedSurfacePoint();
    SurfacePoint originalSP = outputPoint->GetAprioriSurfacePoint();
    EXPECT_NEAR(sp.GetLatitude().planetocentric(Angle::Degrees), csvLine[5].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetLongitude().positiveEast(Angle::Degrees), csvLine[6].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetLocalRadius().kilometers(), csvLine[7].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetLatSigmaDistance().meters(), csvLine[8].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetLonSigmaDistance().meters(), csvLine[9].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetLocalRadiusSigma().meters(), csvLine[10].toDouble(), 0.000001);

    EXPECT_NEAR(sp.GetLocalRadius().meters() - originalSP.GetLocalRadius().meters(), csvLine[13].toDouble(), 0.000001);

    EXPECT_NEAR(sp.GetX().kilometers(), csvLine[14].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetY().kilometers(), csvLine[15].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetZ().kilometers(), csvLine[16].toDouble(), 0.000001);
  }

  // Spot check a few points for hard-coded values
  // A few "Free" points:
  compareCsvLine(line.getRow(30), "AS15_000031957,FREE,3,0,0.33,24.25013429,6.15097049,1735.93990498,270.686673,265.71814949,500.96936636,860.25757782,-1823.63225092,-677.74580607,1573.65050902,169.59077233,712.98695579");
  compareCsvLine(line.getRow(185), "AS15_000055107,FREE,2,0,2.22,24.26598395,6.7584199,1735.27498642,303.08880622,295.63583269,562.91702785,876.14340919,-1869.62256482,-708.50507503,1570.96622125,186.17020478,713.15150216");
  compareCsvLine(line.getRow(396), "AS15_Tie14,FREE,4,0,0.76,23.34007345,4.52764905,1737.15233677,245.96408206,251.30256849,443.11511364,1022.0802375,-1897.32803894,-372.27333324,1590.02287604,125.90958875,688.23852718");

  // A few "Constrained" points:
  compareCsvLine(line.getRow(352), "AS15_SocetPAN_01,CONSTRAINED,3,0,0.27,27.61487917,2.18951566,1735.78407256,160.95594035,162.33480464,285.90370753,103.62038201,223.18289907,306.44755665,1536.92627508,58.76110233,804.5813224", 2);
  compareCsvLine(line.getRow(360), "AS15_SocetPAN_10,CONSTRAINED,4,0,1.14,25.96587004,3.54262524,1735.7217212,113.85792191,113.34018724,189.03898836,-54.11384188,174.35206204,4.97119624,1557.52735028,96.42556503,759.96089173", 2);
  compareCsvLine(line.getRow(380), "AS15_SocetPAN_40,CONSTRAINED,2,0,0.42,25.77498986,1.88090885,1735.56132008,133.81390715,132.83511312,230.53183344,23.85721705,82.0639009,171.57008493,1562.04594094,51.29735453,754.68811809", 2);

  // Check for the correct line output format and csv file structure for the images.csv file
  line = CSVReader(imagesOutput,
                     false, 0, ',', false, true);

  numColumns = line.columns();
  numRows = line.rows();

  ASSERT_EQ(numColumns, 34);
  ASSERT_EQ(numRows, 9);

  // Validate the line information is correct
  csvLine = line.getRow(0);
  compareCsvLine(csvLine, "Image,rms,rms,rms,X,X,X,X,X,Y,Y,Y,Y,Y,Z,Z,Z,Z,Z,RA,RA,RA,RA,RA,DEC,DEC,DEC,DEC,DEC,TWIST,TWIST,TWIST,TWIST,TWIST");
  csvLine = line.getRow(1);
  compareCsvLine(csvLine, "Filename,sample res,line res,total res,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma");

  // Since there are few lines left, just test all remaining lines:
  csvLine = line.getRow(2);
  compareCsvLine(csvLine,
"cube1.cub,1.4775447404019,2.7418764825318,2.2023879205705,774.31325994618,0.66867452194612,774.98193446812,1000.0,0.66224060,1070.7396447319,-1.1002200905474,1069.6394246413,1000.0,0.43290316,1281.8228287147,-0.45849850672954,1281.3643302079,1000.0,0.54474183,143.75643505897,0.55133443011393,144.30776948908,2.0,0.36404432,46.120746428714,0.17614019313154,46.296886621845,2.0,0.23277628,-168.38036906625,-0.47498900185037,-168.8553580681,2.0,0.29075877", 1);

  compareCsvLine(line.getRow(3),
"cube2.cub,1.3226216663653,1.2405002035852,1.2822185514634,807.40648922512,0.69654326156559,808.10303248669,1000.0,0.57888224,1052.4498749809,-0.96327187949306,1051.4866031015,1000.0,0.38960862,1276.1624273959,-0.28924089930814,1275.8731864966,1000.0,0.45879385,142.09480144256,0.57573715081764,142.67053859338,2.0,0.36232721,46.380723240595,0.18178389767783,46.562507138273,2.0,0.23283200,-167.23409620674,-0.48023085646307,-167.7143270632,2.0,0.28905743", 1);
  compareCsvLine(line.getRow(4),
"cube3.cub,1.0551824938999,1.2494288227077,1.1563914731711,840.1468023615,0.78500512575822,840.93180748726,1000.0,0.50254430,1033.7079747498,-0.85015359390486,1032.8578211559,1000.0,0.38078186,1269.9529056955,-0.12214007887034,1269.8307656167,1000.0,0.40203785,140.43705778894,0.59084918829917,141.02790697724,2.0,0.36076038,46.66560674281,0.16383404619686,46.829440789007,2.0,0.23287739,-166.11784867517,-0.48897359391078,-166.60682226908,2.0,0.28757386", 1);
  compareCsvLine(line.getRow(5),
"cube4.cub,1.3502729604515,1.353711082842,1.3519931145416,872.53562217205,0.84854359880116,873.38416577086,1000.0,0.43826180,1014.5161278324,-0.71668415091661,1013.7994436815,1000.0,0.40745608,1263.1947139434,0.060025366522923,1263.2547393099,1000.0,0.38749885,138.82875165909,0.59675276674134,139.42550442583,2.0,0.35906774,46.979962778796,0.15383884472574,47.133801623522,2.0,0.23295681,-165.05256071326,-0.4845337845258,-165.53709449778,2.0,0.28606425", 1);
  compareCsvLine(line.getRow(6),
"cube5.cub,1.0263921013246,1.1679291845654,1.0994406136352,904.5796666618,0.93777162375064,905.51743828555,1000.0,0.39302293,994.86764790093,-0.57507847205151,994.29256942888,1000.0,0.46264597,1255.8845902492,0.22662104012307,1256.1112112893,1000.0,0.41928560,137.21862030953,0.61179683816536,137.8304171477,2.0,0.35719491,47.323691611416,0.15310733827143,47.476798949688,2.0,0.23298158,-163.98385410236,-0.48795663815321,-164.47181074051,2.0,0.28449883", 1);
  compareCsvLine(line.getRow(7), "cube6.cub,1.4808878258505,1.8005531118893,1.6484872249257,657.49557080071,0.62269372858298,658.11826452929,1000.0,0.45809629,1126.2181683192,0.078381328281994,1126.2965496475,1000.0,0.63657967,1301.3147739155,0.19617725731274,1301.5109511729,1000.0,0.64002265,118.21104209923,0.96148569390809,119.17252779314,2.0,0.32064269,53.317384096946,-0.13644143601752,53.180942660929,2.0,0.22650237,-150.28440526839,-0.61302136663558,-150.89742663503,2.0,0.28070089", 1);
  compareCsvLine(line.getRow(8), "cube7.cub,14.959149730568,8.8574178952351,12.292884373335,489.97204152251,0.39576403287539,490.36780555539,1000.0,0.51371964,1197.7389611966,-0.60564162355907,1197.133319573,1000.0,0.35200830,1313.3186725115,-0.082013858944591,1313.2366586526,1000.0,0.47543551,159.32459434907,0.64488282485685,159.96947717393,2.0,0.21847866,84.419718861554,0.2565469596083,84.676265821162,2.0,0.23543646,-178.65839562226,-0.27266123419525,-178.93105685646,2.0,0.21759697", 1);

  // Check for the correct line output format and csv file structure for the residuals.csv file
  line = CSVReader(residualsOutput,
                     false, 0, ',', false, true);

  numColumns = line.columns();
  numRows = line.rows();

  ASSERT_EQ(numColumns, 7);
  ASSERT_EQ(numRows, 972);

  // Validate the line information is correct
  csvLine = line.getRow(0);
  compareCsvLine(csvLine, "x image,y image,Measured,Measured,sample,line,Residual Vector");
  csvLine = line.getRow(1);
  compareCsvLine(csvLine, "Point,Image,Image,coordinate,coordinate,Sample,Line,residual,residual,Magnitude");
  csvLine = line.getRow(2);
  compareCsvLine(csvLine, "Label,Filename,Serial Number,(mm),(mm),(pixels),(pixels),(pixels),(pixels),(pixels),Rejected");

  // Check line/sample and residuals 
  // Check all measures
  ControlMeasure* measure;
  for (int i=3; i < numRows; i++) {
    csvLine = line.getRow(i);
    EXPECT_NO_THROW({
      outputPoint = outputNet.GetPoint(csvLine[0]); 
    }) << "Point in residuals.csv is not present in output network.";
    EXPECT_NO_THROW({
      measure = outputPoint->GetMeasure(csvLine[2]);
   }) << "Point in residuals.csv is not present in output network.";
    // Compare sample, line, residuals
    EXPECT_NEAR(csvLine[5].toDouble(), measure->GetSample(), 0.000001);
    EXPECT_NEAR(csvLine[6].toDouble(), measure->GetLine(), 0.000001);
    EXPECT_NEAR(csvLine[7].toDouble(), measure->GetSampleResidual(), 0.000001);
    EXPECT_NEAR(csvLine[8].toDouble(), measure->GetLineResidual(), 0.000001);
    EXPECT_NEAR(csvLine[9].toDouble(), measure->GetResidualMagnitude(), 0.000001);
  }

  // Spot check a few measures for hard-coded values:
  compareCsvLine(line.getRow(14), "AS15_000031448,/tmp/qt_temp-l7wTTZ/cube1.cub,APOLLO15/METRIC/1971-07-31T14:00:53.547,    -24.91466687,     -8.24555718,   4109.77150653,   2450.19288272,-0.00343036,      0.70304341,      0.70305178", 2); 

  compareCsvLine(line.getRow(142), "AS15_000032200,/tmp/qt_temp-l7wTTZ/cube2.cub,APOLLO15/METRIC/1971-07-31T14:01:16.947,    -25.59176645,    -10.57595225,   4143.71597937,   2333.56318790,     -0.00372340,      0.48459237,      0.48460667", 2);
  compareCsvLine(line.getRow(424), "AS15_000055094,/tmp/qt_temp-l7wTTZ/cube1.cub,APOLLO15/METRIC/1971-07-31T14:00:53.547,     20.35945982,     34.23830188,   1844.18431849,   4576.36730130,      0.00691810,     -0.57578795,      0.57582951", 2);
  compareCsvLine(line.getRow(970), "AS15_test01,/tmp/qt_temp-l7wTTZ/cube3.cub,APOLLO15/METRIC/1971-07-31T14:01:40.346,     -5.04180936,    -34.53366079,   3115.51026031,   1134.42313078,     -3.07166949,      1.44947401,      3.39648765", 2);

  // Test output network size
  ControlNet inputNet(controlNetPath);
  EXPECT_EQ(outputNet.GetNumPoints(), inputNet.GetNumPoints());
  EXPECT_EQ(outputNet.GetNumMeasures(), inputNet.GetNumMeasures());

  // Check that each input point is in the output net and check that the type is the same
  QList<ControlPoint*> inputPoints = inputNet.GetPoints();

  for (int i=0; i < inputPoints.length(); i++) {
    ControlPoint* inputPoint = inputPoints[i];
    ControlPoint* outputPoint;
    EXPECT_NO_THROW({
        outputPoint = outputNet.GetPoint(inputPoint->GetId());
    }
    );

    QString outputType = outputPoint->GetPointTypeString();
    QString inputType = inputPoint->GetPointTypeString();
    EXPECT_EQ(outputType.toStdString(), inputType.toStdString());
  }
}

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

