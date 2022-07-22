#include <map>
#include <cmath>

#include <QtMath>
#include <QFile>
#include <QScopedPointer>

#include "Pvl.h"
#include "PvlGroup.h"
#include "Statistics.h"
#include "CSVReader.h"
#include "Latitude.h"
#include "Longitude.h"
#include "ControlPoint.h"
#include "CSMCamera.h"
#include "LidarData.h"
#include "SerialNumber.h"

#include "jigsaw.h"

#include "TestUtilities.h"
#include "NetworkFixtures.h"
#include "CsmFixtures.h"

#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;


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
    ControlPoint* outputPoint = nullptr;
    EXPECT_NO_THROW({
        outputPoint = outputNet.GetPoint(inputPoint->GetId());
    }
    );

    QString outputType = outputPoint->GetPointTypeString();
    QString inputType = inputPoint->GetPointTypeString();
    EXPECT_EQ(outputType.toStdString(), inputType.toStdString());
  }
}

TEST_F(ApolloNetwork, FunctionalTestJigsawBundleXYZ) {
  // Bundle Lat / Lat Bundleout
  QVector<QString> args = {"radius=yes",
                           "errorpropagation=yes",
                           "spsolve=position",
                           "spacecraft_position_sigma=1000.0",
                           "camsolve=angles",
                           "twist=yes",
                           "camera_angles_sigma=2.",
                           "update=no",
                           "control_point_coordinate_type_bundle=LAT",
                           "control_point_coordinate_type_reports=LAT",
                           "cnet="+controlNetPath,
                           "fromlist="+tempDir.path() + "/cubes.lis",
                           "onet="+tempDir.path()+"/latlat_out.net",
                           "file_prefix="+tempDir.path()+"/latlat"};

  UserInterface ui(APP_XML, args);


  jigsaw(ui);

  QString bundleoutPath = tempDir.path() + "/latlat_bundleout.txt";


  QFile bundleFile(bundleoutPath);
  QString bundleOut;
  if (bundleFile.open(QIODevice::ReadOnly)) {
     bundleOut = bundleFile.read(bundleFile.size());
  }
  else {
    FAIL() << "Failed to open latlat_bundleout.txt" << std::endl;
  }
  bundleFile.close();
  QStringList lines = bundleOut.split("\n");

  EXPECT_THAT(lines[24].toStdString(), HasSubstr("LATITUDINAL"));
  EXPECT_THAT(lines[58].toStdString(), HasSubstr("LATITUDE"));
  EXPECT_THAT(lines[59].toStdString(), HasSubstr("LONGITUDE"));
  EXPECT_THAT(lines[60].toStdString(), HasSubstr("RADIUS"));

  EXPECT_THAT(lines[245].toStdString(), HasSubstr("Latitude"));
  EXPECT_THAT(lines[249].toStdString(), HasSubstr("Longitude"));
  EXPECT_THAT(lines[253].toStdString(), HasSubstr("Radius"));

  EXPECT_THAT(lines[668].toStdString(), HasSubstr("LATITUDE"));
  EXPECT_THAT(lines[669].toStdString(), HasSubstr("LONGITUDE"));
  EXPECT_THAT(lines[670].toStdString(), HasSubstr("RADIUS"));


  // Rectangular Bundle, Latitudinal output
  QVector<QString> args3 = {"radius=yes",
                           "errorpropagation=yes",
                           "spsolve=position",
                           "spacecraft_position_sigma=1000.0",
                           "camsolve=angles",
                           "twist=yes",
                           "camera_angles_sigma=2.",
                           "update=no",
                           "bundleout=no",
                           "control_point_coordinate_type_bundle=RECT",
                           "control_point_coordinate_type_reports=LAT",
                           "cnet="+controlNetPath,
                           "fromlist="+tempDir.path() + "/cubes.lis",
                           "onet="+tempDir.path()+"/rectlat_out.net",
                           "file_prefix="+tempDir.path()+"/rectlat"};

  UserInterface ui3(APP_XML, args3);
  jigsaw(ui3);

  // Compare newtwork and images.csv against the latitude, latitude bundle

  // Compare network against the latitude/latitude network
  ControlNet latLatNet(tempDir.path()+"/latlat_out.net");
  ControlNet rectLatNet(tempDir.path()+"/rectlat_out.net");
  QString latLatImagesOutput = tempDir.path()+"/latlat_bundleout_images.csv";
  QString rectLatImagesOutput = tempDir.path()+"/rectlat_bundleout_images.csv";

  QList<ControlPoint*> latLatPoints = latLatNet.GetPoints();

  for (int i=0; i < latLatPoints.length(); i++) {
    ControlPoint* latLatPoint = latLatPoints[i];
    ControlPoint *rectLatPoint = nullptr;
    EXPECT_NO_THROW({
        rectLatPoint = rectLatNet.GetPoint(latLatPoint->GetId());
    }
    ) << "Point in latitude/latitude bundle not found in rectangular/latitude bundle.";

    EXPECT_EQ(latLatPoint->GetPointTypeString(), rectLatPoint->GetPointTypeString());
    EXPECT_EQ(latLatPoint->GetNumMeasures(), rectLatPoint->GetNumMeasures());
    EXPECT_EQ(latLatPoint->GetNumberOfRejectedMeasures(), rectLatPoint->GetNumberOfRejectedMeasures());
    EXPECT_NEAR(latLatPoint->GetResidualRms(), rectLatPoint->GetResidualRms(), 0.1);
  }

  // Check for match between lat/lat csv and rect/lat csv.
  CSVReader latLatReader = CSVReader(latLatImagesOutput, false, 0, ',', false, true);
  CSVReader rectLatReader = CSVReader(rectLatImagesOutput, false, 0, ',', false, true);

  // Skip the header (lines 1-2) as this was tested previously
  for (int i=2; i < latLatReader.rows(); i++) {
    compareCsvLine(latLatReader.getRow(i), rectLatReader.getRow(i), 0, 0.2); // Large tolerance noted.
  }


  // Rectangular bundle, rectangular report
  QVector<QString> args2 = {"radius=yes",
                           "errorpropagation=yes",
                           "spsolve=position",
                           "spacecraft_position_sigma=1000.0",
                           "camsolve=angles",
                           "twist=yes",
                           "camera_angles_sigma=2.",
                           "update=no",
                           "control_point_coordinate_type_bundle=RECT",
                           "control_point_coordinate_type_reports=RECT",
                           "cnet="+controlNetPath,
                           "fromlist="+tempDir.path() + "/cubes.lis",
                           "onet="+tempDir.path()+"/rectrect_out.net",
                           "file_prefix="+tempDir.path()+"/rectrect"};

  UserInterface ui2(APP_XML, args2);
  jigsaw(ui2);

  QString bundleoutPath2 = tempDir.path() + "/rectrect_bundleout.txt";

  QFile bundleFile2(bundleoutPath2);
  QString bundleOut2;
  if (bundleFile2.open(QIODevice::ReadOnly)) {
     bundleOut2 = bundleFile2.read(bundleFile2.size());
  }
  else {
    FAIL() << "Failed to open rectrect_bundleout.txt" << std::endl;
  }
  bundleFile2.close();
  lines = bundleOut2.split("\n");

  EXPECT_THAT(lines[24].toStdString(), HasSubstr("RECTANGULAR"));
  EXPECT_THAT(lines[58].toStdString(), HasSubstr("X"));
  EXPECT_THAT(lines[59].toStdString(), HasSubstr("Y"));
  EXPECT_THAT(lines[60].toStdString(), HasSubstr("Z"));

  EXPECT_THAT(lines[245].toStdString(), HasSubstr("POINT X"));
  EXPECT_THAT(lines[249].toStdString(), HasSubstr("POINT Y"));
  EXPECT_THAT(lines[253].toStdString(), HasSubstr("POINT Z"));

  EXPECT_THAT(lines[668].toStdString(), HasSubstr("BODY-FIXED-X"));
  EXPECT_THAT(lines[669].toStdString(), HasSubstr("BODY-FIXED-Y"));
  EXPECT_THAT(lines[670].toStdString(), HasSubstr("BODY-FIXED-Z"));


  // Compare newtwork and images.csv against the rectangular, latitude bundle

  // Compare network against the rect/lat network
  ControlNet rectRectNet(tempDir.path()+"/rectlat_out.net");
  QString rectRectImagesOutput = tempDir.path()+"/rectrect_bundleout_images.csv";

  QList<ControlPoint*> rectLatPoints = rectLatNet.GetPoints();

  for (int i=0; i < rectLatPoints.length(); i++) {
    ControlPoint* rectLatPoint = rectLatPoints[i];
    ControlPoint* rectRectPoint = nullptr;
    EXPECT_NO_THROW({
        rectRectPoint = rectRectNet.GetPoint(rectLatPoint->GetId());
    }
    ) << "Point in rectangular/latitude bundle net not found in rectangular/rectangular bundle net.";

    EXPECT_EQ(rectLatPoint->GetPointTypeString(), rectRectPoint->GetPointTypeString());
    EXPECT_EQ(rectLatPoint->GetNumMeasures(), rectRectPoint->GetNumMeasures());
    EXPECT_EQ(rectLatPoint->GetNumberOfRejectedMeasures(), rectRectPoint->GetNumberOfRejectedMeasures());
    EXPECT_NEAR(rectLatPoint->GetResidualRms(), rectRectPoint->GetResidualRms(), 0.1);
  }

  // Check for match between lat/lat csv and rect/lat csv.
  CSVReader rectRectReader = CSVReader(rectRectImagesOutput, false, 0, ',', false, true);

  // Skip the header (lines 1-2) as this was tested previously
  for (int i=2; i < rectRectReader.rows(); i++) {
    compareCsvLine(rectLatReader.getRow(i), rectRectReader.getRow(i), 0);
  }

  // Latitudinal Bundle, Rectangular output
  QVector<QString> args4 = {"radius=yes",
                           "errorpropagation=yes",
                           "spsolve=position",
                           "spacecraft_position_sigma=1000.0",
                           "camsolve=angles",
                           "twist=yes",
                           "camera_angles_sigma=2.",
                           "update=no",
                           "bundleout=no",
                           "control_point_coordinate_type_bundle=LAT",
                           "control_point_coordinate_type_reports=RECT",
                           "cnet="+controlNetPath,
                           "fromlist="+tempDir.path() + "/cubes.lis",
                           "onet="+tempDir.path()+"/apollo_out.net",
                           "file_prefix="+tempDir.path()+"/latrect"};

  UserInterface ui4(APP_XML, args4);
  jigsaw(ui4);


  QString bundleoutPath4 = tempDir.path() + "/rectrect_bundleout.txt";

  QFile bundleFile4(bundleoutPath4);
  QString bundleOut4;
  if (bundleFile4.open(QIODevice::ReadOnly)) {
     bundleOut4 = bundleFile4.read(bundleFile2.size());
  }
  else {
    FAIL() << "Failed to open rectrect_bundleout.txt" << std::endl;
  }
  bundleFile4.close();
  lines = bundleOut4.split("\n");

  EXPECT_THAT(lines[24].toStdString(), HasSubstr("RECTANGULAR"));
  EXPECT_THAT(lines[58].toStdString(), HasSubstr("X"));
  EXPECT_THAT(lines[59].toStdString(), HasSubstr("Y"));
  EXPECT_THAT(lines[60].toStdString(), HasSubstr("Z"));

  EXPECT_THAT(lines[245].toStdString(), HasSubstr("POINT X"));
  EXPECT_THAT(lines[249].toStdString(), HasSubstr("POINT Y"));
  EXPECT_THAT(lines[253].toStdString(), HasSubstr("POINT Z"));

  EXPECT_THAT(lines[668].toStdString(), HasSubstr("BODY-FIXED-X"));
  EXPECT_THAT(lines[669].toStdString(), HasSubstr("BODY-FIXED-Y"));
  EXPECT_THAT(lines[670].toStdString(), HasSubstr("BODY-FIXED-Z"));

  bundleFile4.close();

  // Compare newtwork and images.csv against the latitude, latitude bundle

  // Compare network against the lat/lat network
  ControlNet latRectNet(tempDir.path()+"/rectlat_out.net");
  QString latRectImagesOutput = tempDir.path()+"/rectrect_bundleout_images.csv";

  QList<ControlPoint*> latRectPoints = latRectNet.GetPoints();

  for (int i=0; i < latRectPoints.length(); i++) {
    ControlPoint* latRectPoint = latRectPoints[i];
    ControlPoint *latLatPoint = nullptr;
    EXPECT_NO_THROW({
        latLatPoint = latLatNet.GetPoint(latRectPoint->GetId());
    }
    ) << "Point in rectangular/latitude bundle net not found in rectangular/rectangular bundle net.";

    EXPECT_EQ(latLatPoint->GetPointTypeString(), latRectPoint->GetPointTypeString());
    EXPECT_EQ(latLatPoint->GetNumMeasures(), latRectPoint->GetNumMeasures());
    EXPECT_EQ(latLatPoint->GetNumberOfRejectedMeasures(), latRectPoint->GetNumberOfRejectedMeasures());
    EXPECT_NEAR(latLatPoint->GetResidualRms(), latRectPoint->GetResidualRms(), 0.1);
  }

  // Check for match between lat/lat csv and lat/rect csv.
  CSVReader latRectReader = CSVReader(latRectImagesOutput, false, 0, ',', false, true);

  // Skip the header (lines 1-2) as the header was tested in the apollo test
  for (int i=2; i < latRectReader.rows(); i++) {
    compareCsvLine(latRectReader.getRow(i), latLatReader.getRow(i), 0, 0.2);
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

  QFile leftFile(prefix.path() + "/bundleout_images_LUNARRECONNAISSANCEORBITER_NACL.csv");
  if (!leftFile.open(QIODevice::ReadOnly)) {
    FAIL() << leftFile.errorString().toStdString();
  }

  // skip the first two lines, we don't want to compare the header.
  leftFile.readLine();
  leftFile.readLine();

  QString line = leftFile.readLine();
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


  QFile rightFile(prefix.path() + "/bundleout_images_LUNARRECONNAISSANCEORBITER_NACR.csv");
  if (!rightFile.open(QIODevice::ReadOnly)) {
    FAIL() << rightFile.errorString().toStdString();
  }

  // skip the first two lines, we don't want to compare the header.
  rightFile.readLine();
  rightFile.readLine();

  line = rightFile.readLine();
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
  heldList.append(cubes[5]->fileName());
  heldList.write(heldlistpath);

  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+controlNetPath, "onet="+outCnetFileName, "heldlist="+heldlistpath,
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


TEST_F(ApolloNetwork, FunctionalTestJigsawOutlierRejection) {
  QTemporaryDir prefix;

  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+controlNetPath, "onet="+outCnetFileName,
                           "radius=yes", "errorpropagation=yes", "outlier_rejection=True", "spsolve=position", "Spacecraft_position_sigma=1000",
                           "Residuals_csv=on", "Camsolve=angles", "Twist=yes", "Camera_angles_sigma=2",
                           "Output_csv=off", "imagescsv=on", "file_prefix="+prefix.path() + "/"};

  UserInterface options(APP_XML, args);

  Pvl log;

  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Unable to bundle: " << e.what() << std::endl;
  }

  QString residualsCsv = prefix.path() + "/residuals.csv";
  QFile bo(residualsCsv);

  QString contents;
  if (bo.open(QIODevice::ReadOnly)) {
    contents = bo.read(bo.size());
  }
  else {
    FAIL() << "Failed to open bundleout.txt" << std::endl;
  }

  int nRejectedCsv = 0;
  QStringList lines = contents.split("\n");
  for (int i = 0; i < lines.size(); i++) {
    if (lines[i].right(1).trimmed() == "*") {
       nRejectedCsv++;
    }
  }

  ASSERT_EQ(nRejectedCsv, 51);
}



TEST_F(ApolloNetwork, FunctionalTestJigsawMEstimator) {
  QTemporaryDir prefix;
  QString newNetworkPath = prefix.path()+"/badMeasures.net";

  QVector<QString> pid = {"AS15_000031985",
                          "AS15_000033079",
                          "AS15_SocetPAN_03",
                          "AS15_Tie03"};

  QVector<QString> mid = {"APOLLO15/METRIC/1971-07-31T14:01:40.346",
                          "APOLLO15/METRIC/1971-07-31T14:02:27.179",
                          "APOLLO15/METRIC/1971-07-31T14:02:03.751",
                          "APOLLO15/METRIC/1971-07-31T14:00:53.547"};

  for (int i = 0; i < pid.size(); i++) {
    // grab random points and add error to a single measure
    ControlPoint *point = network->GetPoint(pid[i]);
    ControlMeasure *measure = point->GetMeasure(mid[i]);
    measure->SetCoordinate(measure->GetLine()+50, measure->GetLine()+50);
  }

  network->Write(newNetworkPath);

  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+newNetworkPath, "onet="+outCnetFileName,
                           "Radius=yes", "Errorpropagation=yes", "Spsolve=position","Spacecraft_position_sigma=1000.0",
                           "Camsolve=angles", "twist=yes", "Camera_angles_sigma=2",
                           "Model1=huber", "Max_model1_c_quantile=0.6", "Model2=chen", "Max_model2_c_quantile=0.98", "Sigma0=1e-3",
                           "bundleout_txt=yes", "Output_csv=on", "imagescsv=on", "file_prefix="+prefix.path()+"/"};

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

  ControlNet onet;
  onet.ReadControl(outCnetFileName);

  QVector<double> presiduals = {};
  QVector<QVector<double>> mresiduals = {{1.27975, 1.54281, 1.8778, 1.30159},
                                         {2.25115, 2.33559, 0.547574, 3.16777},
                                         {1.15396, 0.69243, 1.03005, 0.848934},
                                         {2.24641, 4.39168, 0.560941, 2.844}};

  for (int i = 0; i < pid.size(); i++) {
    ControlPoint *point = network->GetPoint(pid[i]);
    QList<ControlMeasure*> measures = point->getMeasures();
    for (int j = 0; j < measures.size(); j++ ) {
      EXPECT_NEAR(measures.at(j)->GetResidualMagnitude(), mresiduals[i][j], 0.0001);
    }
  }

  QFile bo(prefix.path()+"/bundleout.txt");
  QString contents;
  if (bo.open(QIODevice::ReadOnly)) {
    contents = bo.read(bo.size());
  }
  else {
    FAIL() << "Failed to open bundleout.txt" << std::endl;
  }

  QStringList lines = contents.split("\n");

  EXPECT_THAT(lines[32].toStdString(), HasSubstr("Tier 0 Enabled: TRUE"));
  EXPECT_THAT(lines[33].toStdString(), HasSubstr("Maximum Likelihood Model: Huber"));
  EXPECT_THAT(lines[34].toStdString(), HasSubstr("Quantile used for tweaking constant: 0.6"));
  EXPECT_THAT(lines[35].toStdString(), HasSubstr("Quantile weighted R^2 Residual value: 0.207"));
  EXPECT_THAT(lines[36].toStdString(), HasSubstr("Approx. weighted Residual cutoff: N/A"));

  EXPECT_THAT(lines[38].toStdString(), HasSubstr("Tier 1 Enabled: TRUE"));
  EXPECT_THAT(lines[39].toStdString(), HasSubstr("Maximum Likelihood Model: Chen"));
  EXPECT_THAT(lines[40].toStdString(), HasSubstr("Quantile used for tweaking constant: 0.98"));
  EXPECT_THAT(lines[41].toStdString(), HasSubstr("Quantile weighted R^2 Residual value: 1.0"));
  EXPECT_THAT(lines[42].toStdString(), HasSubstr("Approx. weighted Residual cutoff: 1.0"));

  EXPECT_THAT(lines[44].toStdString(), HasSubstr(" Tier 2 Enabled: FALSE"));
}


 TEST_F(ObservationPair, FunctionalTestJigsawErrorNoSolve) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName,
                           "camsolve=None", "spsolve=None"};

  UserInterface options(APP_XML, args);
  Pvl log;

  try {
    jigsaw(options, &log);
    FAIL() << "Should throw" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Must either solve for camera pointing or spacecraft position"));
  }
}


TEST_F(ObservationPair, FunctionalTestJigsawErrorTBParamsNoTarget) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";

  // just use isdPath for a valid PVL file without the wanted groups
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName, "SOLVETARGETBODY=TRUE", "tbparameters="+cubeRPath};

  UserInterface options(APP_XML, args);

  Pvl log;

  try {
    jigsaw(options, &log);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Input Target parameters file missing main Target object"));
  }
}


TEST_F(ObservationPair, FunctionalTestJigsawErrorTBParamsNoSolve) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";

  std::istringstream iss(R"(
    Object = Target
    Group = "NAME"
       Name=Enceladus
    EndGroup
    END_OBJECT
  )");

  QString tbsolvepath = prefix.path() + "/tbsolve.pvl";
  Pvl tbsolve;
  iss >> tbsolve;
  tbsolve.write(tbsolvepath);

  // just use isdPath for a valid PVL file without the wanted groups
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+cnetPath, "onet="+outCnetFileName, "SOLVETARGETBODY=TRUE", "tbparameters="+tbsolvepath};

  UserInterface options(APP_XML, args);

  Pvl log;

  try {
    jigsaw(options, &log);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Must solve for at least one target body option"));
  }
}


TEST_F(ApolloNetwork, FunctionalTestJigsawPoleRaDecW0WdotMeanRadius) {
  QTemporaryDir prefix;
  QString tbParamsPath = prefix.path() + "/tbparams.pvl";

  std::istringstream tbPvlStr(R"(Object = Target
  Group = "NAME"
    Name=Moon
  EndGroup
  Group = "POLERIGHTASCENSION"
    Ra=position
    RaValue=269.9949
    RaSigma=0.0
    RaVelocityValue=0.0031
    RaVelocitySigma=0.0
    RaAccelerationValue=0.0
    RaAccelerationSigma=1.0
  EndGroup
  Group = "POLEDECLINATION"
    Dec=position
    DecValue=66.5392
    DecSigma=0.0
    DecVelocityValue=0.0130
    DecVelocitySigma=0.0
    DecAccelerationValue=0.0
    DecAccelerationSigma=1.0
  EndGroup
  Group = "PRIME MERIDIAN"
    Pm=velocity
    PmValue=38.32132
    PmSigma=0.0
    PmVelocityValue=13.17635815
    PmVelocitySigma=0.0
    PmAccelerationValue=0.0
    PmAccelerationSigma=1.0
  EndGroup
  Group = "RADII"
    RadiiSolveOption=mean
    RadiusAValue=1737400
    RadiusASigma=0.0
    RadiusBValue=1737400
    RadiusBSigma=0.0
    RadiusCValue=1737400
    RadiusCSigma=0.0
    MeanRadiusValue=1737400
    MeanRadiusSigma=0.0
  EndGroup
EndObject
End)");

  Pvl tbParams;
  tbPvlStr >> tbParams;
  tbParams.write(tbParamsPath);

  QString outCnetFileName = prefix.path() + "/outTemp.net";

  for(int i = 0; i < cubes.size(); i++) {
      Pvl *label = cubes[i]->label();
      // get body rotation
      PvlObject &br = label->object(4);
      PvlKeyword ra("PoleRa");
      ra+= "269.9949";
      ra+= "0.036";
      ra+= "0.0";

      PvlKeyword dec("PoleDec");
      dec += "66.5392";
      dec += "0.0130";
      dec += "0.0";

      PvlKeyword pm("PrimeMeridian");
      pm += "38.3213";
      pm += "13.17635815";
      pm += "1.4E-12";

      PvlKeyword &ft = br.findKeyword("FrameTypeCode");
      ft.setValue("2");

      br.addKeyword(ra);
      br.addKeyword(dec);
      br.addKeyword(pm);
      cubes[i]->close();
      delete cubes[i];
      cubes[i] = nullptr;
  }

  // just use isdPath for a valid PVL file without the wanted groups
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+controlNetPath, "onet="+outCnetFileName,
                          "Solvetargetbody=yes", "Errorpropagation=yes",  "Camsolve=angles", "twist=off", "camera_angles_sigma=2.0", "bundleout_txt=yes",
                          "imagescsv=no", "output_csv=no", "residuals_csv=no", "file_prefix="+prefix.path()+"/", "tbparameters="+tbParamsPath};

  UserInterface options(APP_XML, args);

  Pvl log;

  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Failed to bundle: " << e.what() << std::endl;
  }

  QFile bo(prefix.path() + "/bundleout.txt");
  QString contents;
  if (bo.open(QIODevice::ReadOnly)) {
    contents = bo.read(bo.size());
  }
  else {
    FAIL() << "Failed to open bundleout.txt" << std::endl;
  }

  QStringList lines = contents.split("\n");

  EXPECT_THAT(lines[76].toStdString(), HasSubstr("RADII: MEAN"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, lines[77].trimmed(), "");

  QStringList columns = lines[160].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "RA");
  EXPECT_NEAR(columns[2].toDouble(), 269.9949, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 2.65243903, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 272.64733903, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00167495, 0.0001);

  columns = lines[161].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "DEC");
  EXPECT_NEAR(columns[2].toDouble(), 66.5392, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1.17580491, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 67.71500491, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00068524, 0.0001);

  columns = lines[162].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PM");
  EXPECT_NEAR(columns[1].toDouble(), 38.32132, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -383.36347956, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), -345.04215956, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 1.55731615, 0.0001);

  columns = lines[163].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PMv");
  EXPECT_NEAR(columns[1].toDouble(), 13.17635815, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -0.03669501, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 13.13966314, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 0.00015007, 0.0001);

  columns = lines[164].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "MeanRadius");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -1.67807036, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1735.72192964, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 0.07865419, 0.0001);

}


TEST_F(ApolloNetwork, FunctionalTestJigsawPoleRaDecW0WdotTriaxial) {
  QTemporaryDir prefix;
  QString tbParamsPath = prefix.path() + "/tbparams.pvl";

  std::istringstream tbPvlStr(R"(Object = Target
  Group = "NAME"
    Name=Moon
  EndGroup
  Group = "POLERIGHTASCENSION"
    Ra=position
    RaValue=269.9949
    RaSigma=0.0
    RaVelocityValue=0.0031
    RaVelocitySigma=0.0
    RaAccelerationValue=0.0
    RaAccelerationSigma=1.0
  EndGroup
  Group = "POLEDECLINATION"
    Dec=position
    DecValue=66.5392
    DecSigma=0.0
    DecVelocityValue=0.0130
    DecVelocitySigma=0.0
    DecAccelerationValue=0.0
    DecAccelerationSigma=1.0
  EndGroup
  Group = "PRIME MERIDIAN"
    Pm=velocity
    PmValue=38.32132
    PmSigma=0.0
    PmVelocityValue=13.17635815
    PmVelocitySigma=0.0
    PmAccelerationValue=0.0
    PmAccelerationSigma=1.0
  EndGroup
  Group = "RADII"
    RadiiSolveOption=triaxial
    RadiusAValue=1737400
    RadiusASigma=0.0
    RadiusBValue=1737400
    RadiusBSigma=0.0
    RadiusCValue=1737400
    RadiusCSigma=0.0
    MeanRadiusValue=1737400
    MeanRadiusSigma=0.0
  EndGroup
EndObject
End)");

  Pvl tbParams;
  tbPvlStr >> tbParams;
  tbParams.write(tbParamsPath);

  QString outCnetFileName = prefix.path() + "/outTemp.net";

  for(int i = 0; i < cubes.size(); i++) {
      Pvl *label = cubes[i]->label();
      // get body rotation
      PvlObject &br = label->object(4);
      PvlKeyword ra("PoleRa");
      ra+= "269.9949";
      ra+= "0.036";
      ra+= "0.0";

      PvlKeyword dec("PoleDec");
      dec += "66.5392";
      dec += "0.0130";
      dec += "0.0";

      PvlKeyword pm("PrimeMeridian");
      pm += "38.3213";
      pm += "13.17635815";
      pm += "1.4E-12";

      PvlKeyword &ft = br.findKeyword("FrameTypeCode");
      ft.setValue("2");

      br.addKeyword(ra);
      br.addKeyword(dec);
      br.addKeyword(pm);
      cubes[i]->close();
      delete cubes[i];
      cubes[i] = nullptr;
  }

  // just use isdPath for a valid PVL file without the wanted groups
  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+controlNetPath, "onet="+outCnetFileName,
                          "Solvetargetbody=yes", "Errorpropagation=yes",  "Camsolve=angles", "twist=off", "camera_angles_sigma=2.0", "bundleout_txt=yes",
                          "imagescsv=no", "output_csv=no", "residuals_csv=no", "file_prefix="+prefix.path()+"/", "tbparameters="+tbParamsPath};

  UserInterface options(APP_XML, args);

  Pvl log;

  try {
    jigsaw(options, &log);
  }
  catch (IException &e) {
    FAIL() << "Failed to bundle: " << e.what() << std::endl;
  }

  QFile bo(prefix.path() + "/bundleout.txt");
  QString contents;
  if (bo.open(QIODevice::ReadOnly)) {
    contents = bo.read(bo.size());
  }
  else {
    FAIL() << "Failed to open bundleout.txt" << std::endl;
  }

  QStringList lines = contents.split("\n");

  EXPECT_THAT(lines[76].toStdString(), HasSubstr("RADII: TRIAXIAL"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, lines[77].trimmed(), "");

  QStringList columns = lines[160].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "RA");
  EXPECT_NEAR(columns[2].toDouble(), 269.9949, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 2.95997958, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 272.95487958, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00199725, 0.0001);

  columns = lines[161].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "DEC");
  EXPECT_NEAR(columns[2].toDouble(), 66.5392, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1.16195781, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 67.70115781, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00149539, 0.0001);

  columns = lines[162].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PM");
  EXPECT_NEAR(columns[1].toDouble(), 38.32132, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -291.78617547, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), -253.4648554, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 2.00568417, 0.0001);

  columns = lines[163].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PMv");
  EXPECT_NEAR(columns[1].toDouble(), 13.17635815, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -0.02785056, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 13.14850759, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 0.00019333, 0.0001);

  columns = lines[164].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "RadiusA");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), 6.87282091, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1744.27282091, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 1.23289971, 0.0001);

  columns = lines[165].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "RadiusB");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), 2.34406319, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1739.74406319, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 12.52974045, 0.0001);

  columns = lines[166].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "RadiusC");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -37.55670044, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1699.84329956, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 5.34723296, 0.0001);
}


TEST_F(VikThmNetwork, FunctionalTestJigsawScconfig) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";
  QString scconfigPath = "data/vikingThemisNetwork/themis_vo.pvl";

  QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+controlNetPath,
                           "onet="+outCnetFileName, "scconfig="+scconfigPath,
                           "radius=true", "point_radius_sigma=50",
                           "bundleout_txt=no", "output_csv=no",
                           "residuals_csv=no", "file_prefix="+prefix.path()+"/"};

   UserInterface options(APP_XML, args);

   try {
     jigsaw(options);
   }
   catch (IException &e) {
     FAIL() << "Failed to bundle: " << e.what() << std::endl;
   }

   CSVReader header = CSVReader(prefix.path()+"/bundleout_images_MARS_ODYSSEY_THEMIS_IR.csv",
                      false, 0, ',', false, true);

   // Cube 1
   CSVReader::CSVAxis csvLine = header.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/I28234014RDR_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.098907844, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.215695753, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.167790672, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 2830.732839, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), 1273.737178, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 2222.226081, 0.0001);
   // Final RA(t0)
   EXPECT_NEAR(csvLine[21].toDouble(), -0.293061477, 0.0001);
   // Final RA(t1)
   EXPECT_NEAR(csvLine[26].toDouble(), -0.006286268, 0.0001);
   // Final RA(t2)
   EXPECT_NEAR(csvLine[31].toDouble(), -0.001770652, 0.0001);
   // Final DEC(t0)
   EXPECT_NEAR(csvLine[36].toDouble(), 0.280832383, 0.0001);
   // Final DEC(t1)
   EXPECT_NEAR(csvLine[41].toDouble(), 0.03537654, 0.0001);
   // Final DEC(t2)
   EXPECT_NEAR(csvLine[46].toDouble(), -0.008331205, 0.0001);
   // Final TWIST(t0)
   EXPECT_NEAR(csvLine[51].toDouble(), 0.140615905, 0.0001);
   // Final TWIST(t1)
   EXPECT_NEAR(csvLine[56].toDouble(), -0.01323743, 0.0001);
   // Final TWIST(t2)
   EXPECT_NEAR(csvLine[61].toDouble(), 0.014712461, 0.0001);

   // Cube 2
   csvLine = header.getRow(3);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/I52634011RDR_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.127427856, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.339851251, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.256648331, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 3638.299799, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), 265.2465868, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 1122.429048, 0.0001);
   // Final RA(t0)
   EXPECT_NEAR(csvLine[21].toDouble(), -0.043427688, 0.0001);
   // Final RA(t1)
   EXPECT_NEAR(csvLine[26].toDouble(), -0.00150114, 0.0001);
   // Final RA(t2)
   EXPECT_NEAR(csvLine[31].toDouble(), -0.001761841, 0.0001);
   // Final DEC(t0)
   EXPECT_NEAR(csvLine[36].toDouble(), 0.217420457, 0.0001);
   // Final DEC(t1)
   EXPECT_NEAR(csvLine[41].toDouble(), -0.012838311, 0.0001);
   // Final DEC(t2)
   EXPECT_NEAR(csvLine[46].toDouble(), -0.00276288, 0.0001);
   // Final TWIST(t0)
   EXPECT_NEAR(csvLine[51].toDouble(), -0.029666148, 0.0001);
   // Final TWIST(t1)
   EXPECT_NEAR(csvLine[56].toDouble(), -0.006404881, 0.0001);
   // Final TWIST(t2)
   EXPECT_NEAR(csvLine[61].toDouble(), 0.009706339, 0.0001);


   header = CSVReader(prefix.path()+"/bundleout_images_VIKING_ORBITER_2_VISUAL_IMAGING_SUBSYSTEM_CAMERA_A.csv",
                      false, 0, ',', false, true);

   csvLine = header.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/F704b51.lev1_slo_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.462520744, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.270902588, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.379020877, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 3194.402972, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), 1260.005005, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 11151.90611, 0.0001);
   // Final RA
   EXPECT_NEAR(csvLine[21].toDouble(), -93.38593055, 0.0001);
   // Final DEC
   EXPECT_NEAR(csvLine[26].toDouble(), 163.6079355, 0.0001);
   // Final TWIST
   EXPECT_NEAR(csvLine[31].toDouble(), 63.04898685, 0.0001);


   header = CSVReader(prefix.path()+"/bundleout_images_VIKING_ORBITER_1_VISUAL_IMAGING_SUBSYSTEM_CAMERA_B.csv",
                   false, 0, ',', false, true);

   csvLine = header.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/F857a32.lev1_slo_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.326314933, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.24252818, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.287490307, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 13478.98055, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), -813.5504098, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 1067.407005, 0.0001);
   // Final RA
   EXPECT_NEAR(csvLine[21].toDouble(), -94.26870465, 0.0001);
   // Final DEC
   EXPECT_NEAR(csvLine[26].toDouble(), 91.72112715, 0.0001);
   // Final TWIST
   EXPECT_NEAR(csvLine[31].toDouble(), -22.90143017, 0.0001);
 }


TEST_F(VikThmNetwork, FunctionalTestJigsawScconfigHeld) {
   QTemporaryDir prefix;
   QString heldListPath = prefix.path() + "/heldlist.lis";
   FileList heldList;
   heldList.append("data/vikingThemisNetwork/I28234014RDR_crop.cub");
   heldList.write(heldListPath);

   QString outCnetFileName = prefix.path() + "/outTemp.net";
   QString scconfigPath = "data/vikingThemisNetwork/themis_vo.pvl";

   QVector<QString> args = {"fromlist="+cubeListFile, "cnet="+controlNetPath,
                           "onet="+outCnetFileName, "heldlist="+heldListPath, "scconfig="+scconfigPath,
                           "radius=true", "point_radius_sigma=50",
                           "bundleout_txt=no", "output_csv=no",
                           "residuals_csv=no", "file_prefix="+prefix.path()+"/"};

   UserInterface options(APP_XML, args);

   try {
    jigsaw(options);
   }
   catch (IException &e) {
    FAIL() << "Failed to bundle: " << e.what() << std::endl;
   }

   CSVReader heldHeader = CSVReader(prefix.path() + "/bundleout_images_held.csv",
                                    false, 0, ',', false, true);

   CSVReader::CSVAxis csvLine = heldHeader.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/I28234014RDR_crop.cub");
   // sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 1.30E-11, 0.0001);
   // line res
   EXPECT_NEAR(csvLine[2].toDouble(), 1.41E-11, 0.0001);
   // total res
   EXPECT_NEAR(csvLine[3].toDouble(), 1.35E-11, 0.0001);
   // final X
   EXPECT_NEAR(csvLine[6].toDouble(), 2830.732839, 0.0001);
   // final Y
   EXPECT_NEAR(csvLine[11].toDouble(), 1273.737178, 0.0001);
   // final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 2222.226081, 0.0001);
   // final RA
   EXPECT_NEAR(csvLine[21].toDouble(), -126.5131868, 0.0001);
   // final DEC
   EXPECT_NEAR(csvLine[26].toDouble(), 55.60096761, 0.0001);
   // final TWIST
   EXPECT_NEAR(csvLine[31].toDouble(), 151.8463271, 0.0001);

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

   CSVReader header = CSVReader(prefix.path()+"/bundleout_images_MARS_ODYSSEY_THEMIS_IR.csv",
                                false, 0, ',', false, true);

   // Cube 1
   csvLine = header.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/I52634011RDR_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.402714712, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.990233446, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.755890672, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 3638.299799, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), 265.2465868, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 1122.429048, 0.0001);
   // Final RA(t0)
   EXPECT_NEAR(csvLine[21].toDouble(), -0.181038886, 0.0001);
   // Final RA(t1)
   EXPECT_NEAR(csvLine[26].toDouble(), 0.004969535, 0.0001);
   // Final RA(t2)
   EXPECT_NEAR(csvLine[31].toDouble(), -4.95E-04, 0.0001);
   // Final DEC(t0)
   EXPECT_NEAR(csvLine[36].toDouble(), 0.545011107, 0.0001);
   // Final DEC(t1)
   EXPECT_NEAR(csvLine[41].toDouble(), 0.060712178, 0.0001);
   // Final DEC(t2)
   EXPECT_NEAR(csvLine[46].toDouble(), -0.01481148, 0.0001);
   // Final TWIST(t0)
   EXPECT_NEAR(csvLine[51].toDouble(), 0.059124183, 0.0001);
   // Final TWIST(t1)
   EXPECT_NEAR(csvLine[56].toDouble(), -0.004663389, 0.0001);
   // Final TWIST(t2)
   EXPECT_NEAR(csvLine[61].toDouble(), 0.018341326, 0.0001);

   header = CSVReader(prefix.path()+"/bundleout_images_VIKING_ORBITER_2_VISUAL_IMAGING_SUBSYSTEM_CAMERA_A.csv",
                      false, 0, ',', false, true);

   csvLine = header.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/F704b51.lev1_slo_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.573332032, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.376017989, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.484819114, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 3194.402972, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), 1260.005005, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 11151.90611, 0.0001);
   // Final RA
   EXPECT_NEAR(csvLine[21].toDouble(), -93.385133891536, 0.0001);
   // Final DEC
   EXPECT_NEAR(csvLine[26].toDouble(), 163.6079355, 0.0001);
   // Final TWIST
   EXPECT_NEAR(csvLine[31].toDouble(), 63.100333749725003, 0.0001);

   header = CSVReader(prefix.path()+"/bundleout_images_VIKING_ORBITER_1_VISUAL_IMAGING_SUBSYSTEM_CAMERA_B.csv",
                      false, 0, ',', false, true);

   csvLine = header.getRow(2);
   EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "data/vikingThemisNetwork/F857a32.lev1_slo_crop.cub");
   // Sample res
   EXPECT_NEAR(csvLine[1].toDouble(), 0.561652424, 0.0001);
   // Line res
   EXPECT_NEAR(csvLine[2].toDouble(), 0.326697864, 0.0001);
   // Total res
   EXPECT_NEAR(csvLine[3].toDouble(), 0.459448005, 0.0001);
   // Final X
   EXPECT_NEAR(csvLine[6].toDouble(), 13478.98055, 0.0001);
   // Final Y
   EXPECT_NEAR(csvLine[11].toDouble(), -813.5504098, 0.0001);
   // Final Z
   EXPECT_NEAR(csvLine[16].toDouble(), 1067.407005, 0.0001);
   // Final RA
   EXPECT_NEAR(csvLine[21].toDouble(), -94.268452130639005, 0.0001);
   // Final DEC
   EXPECT_NEAR(csvLine[26].toDouble(), 91.720551340206001, 0.0001);
   // Final TWIST
   EXPECT_NEAR(csvLine[31].toDouble(), -22.86701551000799, 0.0001);
}

// These tests exercise the bundle adjustment of images from the MiniRF radar
// instrument onboard LRO.
TEST_F(MiniRFNetwork, FunctionalTestJigsawRadar) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";

  // solving for position only, with error propagation
  QVector<QString> args1 = {"fromlist="+cubeListFile, "cnet="+controlNetPath,
                           "onet="+outCnetFileName, "maxits=10", "errorprop=yes",
                           "bundleout_txt=no", "spsolve=accelerations",
                           "camsolve=no", "file_prefix="+prefix.path()+"/radar_sparse_poh"};

 UserInterface options1(APP_XML, args1);

 try {
   jigsaw(options1);
 }
 catch (IException &e) {
   FAIL() << "Failed to bundle: " << e.what() << std::endl;
 }

 CSVReader line = CSVReader(prefix.path() + "/radar_sparse_poh_bundleout_images.csv",
                    false, 0, ',', false, true);

 compareCsvLine(line.getRow(2),
  "crop.cub,9.8139190988466,4.8931845871077,7.7542331497775,42.739839720201,"
   "1.7158245398686,44.45566426007,FREE,0.03708163,1.665220622852,0.0019502310020231,"
   "1.6671708538541,FREE,0.00054302,-1.86715747234976e-05,-1.28312272946101e-05,"
   "-3.15028020181077e-05,FREE,0.00000677,692.90383188675,-0.1098533109018,"
   "692.79397857585,FREE,0.30619406,0.24237983300688,-0.0072287435477999,"
   "0.23515108945908,FREE,0.00485910,-3.00885428043456e-04,4.44355023463745e-05,"
   "-2.56449925697081e-04,FREE,0.00004580,-1638.4392469801,0.28515208655059,"
   "-1638.1540948936,FREE,0.13905870,0.14114990073144,0.0042010473963839,"
   "0.14535094812783,FREE,0.00195111,7.11893629983806e-04,-1.94289168875697e-05,"
   "6.92464713096237e-04,FREE,0.00001960,-3.5174543904688,0.0,-3.5174543904688,"
   "N/A,N/A,22.963021964112,0.0,22.963021964112,N/A,N/A,-167.27682369046,0.0,"
   "-167.27682369046,N/A,N/A", 1);
  compareCsvLine(line.getRow(3),
   "crop.cub,9.4726337699208,4.4502293176623,7.4005179385914,43.334861031865,"
   "1.6979463834046,45.03280741527,FREE,0.03324825,1.665259796398,0.0018842183991388,"
   "1.6671440147971,FREE,0.00055998,-1.89435136303748e-05,-9.30674560525859e-06,"
   "-2.82502592356334e-05,FREE,0.00000531,691.77793318689,0.14530608015382,"
   "691.92323926704,FREE,0.24468876,0.24222242028266,-9.61881250584634e-04,"
   "0.24126053903208,FREE,0.00327354,-3.00398354003216e-04,7.06472600669352e-05,"
   "-2.29751093936281e-04,FREE,0.00003782,-1638.8768398909,0.1787933435063,"
   "-1638.6980465474,FREE,0.11974833,0.14111215838631,0.0016775200513291,"
   "0.14278967843764,FREE,0.00148666,7.12115647093142e-04,-2.79125310626517e-05,"
   "6.84203116030491e-04,FREE,0.00001795,-3.5715716179672,0.0,-3.5715716179672,"
   "N/A,N/A,22.925340565245,0.0,22.925340565245,N/A,N/A,-167.232707452,0.0,"
   "-167.232707452,N/A,N/A", 1);
  compareCsvLine(line.getRow(4),
   "crop.cub,7.7843773766903,3.7525948190357,6.1105850382177,18.252351060798,"
   "1.6293808986805,19.881731959478,FREE,0.05126731,1.6664421546381,0.0011330852282056,"
   "1.6675752398663,FREE,0.00084377,-8.03729008106401e-06,-1.00999933057467e-05,"
   "-1.81372833868107e-05,FREE,0.00000802,687.11638998215,0.84171392566316,"
   "687.95810390782,FREE,0.35116896,0.25146497467017,0.0062984499626118,"
   "0.25776342463279,FREE,0.00531669,-2.98460449098946e-04,7.8687578071165e-05,"
   "-2.19772871027781e-04,FREE,0.00006324,-1641.3198305082,-0.16249776415743,"
   "-1641.4823282724,FREE,0.17712610,0.1192736170679,-0.0019411382629964,"
   "0.1173324788049,FREE,0.00243224,7.13385968670405e-04,-3.00026391718403e-05,"
   "6.83383329498565e-04,FREE,0.00002969,-1.5077677229935,0.0,-1.5077677229935,"
   "N/A,N/A,22.723572340278,0.0,22.723572340278,N/A,N/A,-169.13683247633,0.0,"
   "-169.13683247633,N/A,N/A", 1);

  // solving for position, velocity, acceleration, using polynomial over a constant hermite
  // spline, with error propagation
  QVector<QString> args2 = {"fromlist="+cubeListFile, "cnet="+controlNetPath,
                           "onet="+outCnetFileName,"maxits=10", "errorprop=yes",
                           "spsolve=position", "overhermite=yes", "bundleout_txt=no",
                           "camsolve=no", "file_prefix="+prefix.path()+"/radar_sparse"};

 UserInterface options2(APP_XML, args2);

 try {
   jigsaw(options2);
 }
 catch (IException &e) {
   FAIL() << "Failed to bundle: " << e.what() << std::endl;
 }

 line = CSVReader(prefix.path() + "/radar_sparse_bundleout_images.csv",
                    false, 0, ',', false, true);

 compareCsvLine(line.getRow(2),
  "crop.cub,10.395150034381,4.1153054682532,7.9055323455898,0.0,1.6654573414268,"
  "1.6654573414268,FREE,0.02693579,0.0,0.1230110379916,0.1230110379916,FREE,"
  "0.18157895,0.0,0.18002883109547,0.18002883109547,FREE,0.08185824,-3.5174543904688,"
  "0.0,-3.5174543904688,N/A,N/A,22.963021964112,0.0,22.963021964112,N/A,N/A,-167.27682369046,"
  "0.0,-167.27682369046,N/A,N/A", 1);
  compareCsvLine(line.getRow(3),
   "crop.cub,9.9765857703763,3.7759501192875,7.5428795210126,0.0,1.6377191835121,"
   "1.6377191835121,FREE,0.02317939,0.0,0.34307587759526,0.34307587759526,FREE,"
   "0.15780215,0.0,0.084476381186715,0.084476381186715,FREE,0.07459370,-3.5715716179672,"
   "0.0,-3.5715716179672,N/A,N/A,22.925340565245,0.0,22.925340565245,N/A,N/A,-167.232707452,"
   "0.0,-167.232707452,N/A,N/A", 1);
  compareCsvLine(line.getRow(4),
   "crop.cub,8.3956138816403,3.3226339162902,6.3845997756819,0.0,1.5871768717531,"
   "1.5871768717531,FREE,0.03193788,0.0,0.71840477660901,0.71840477660901,FREE,"
   "0.21557003,0.0,-0.095359125080397,-0.095359125080397,FREE,0.10479701,-1.5077677229935,"
   "0.0,-1.5077677229935,N/A,N/A,22.723572340278,0.0,22.723572340278,N/A,N/A,-169.13683247633,"
   "0.0,-169.13683247633,N/A,N/A", 1);
}


TEST_F(CSMNetwork, FunctionalTestJigsawCSM) {
  QTemporaryDir prefix;
  QString outCnetFileName = prefix.path() + "/outTemp.net";

  // solving for position only, with error propagation
  QVector<QString> args1 = {"fromlist="+cubeListFile,
                            "cnet=data/CSMNetwork/test.net",
                            "onet="+outCnetFileName,
                            "maxits=10",
                            "errorprop=yes",
                            "bundleout_txt=yes",
                            "update=yes",
                            "csmsolveset=adjustable",
                            "POINT_LATITUDE_SIGMA=1125",
                            "POINT_LONGITUDE_SIGMA=1125",
                            "file_prefix="+tempDir.path()+"/"
                         };

  UserInterface options1(APP_XML, args1);
  try {
   jigsaw(options1);

  }
  catch (IException &e) {
   FAIL() << "Failed to bundle: " << e.what() << std::endl;
  }

  CSVReader line = CSVReader(tempDir.path() + "/bundleout_points.csv", false, 0,
                              ',', false, true);

  compareCsvLine(line.getRow(25),
                 "csm_test_019,	FREE,	5,	0, 0,	1.5,	358.5,	1000,	0,	0,	0,"
                 "	950.4447199,	-1195.518876,	0,	999.3147674,	-26.16797812,	26.17694831", 1);
  compareCsvLine(line.getRow(28),
                 "csm_test_022,	FREE,	5,	0,	0,	1.5,	1.5,	1000, 0, 0, 0,"
                 "	288.1013812,	-1391.568893,	0,	999.3147674,"
                 " 26.16797812,	26.17694831", 1);
  compareCsvLine(line.getRow(49),
                 "csm_test_043,	FREE,	5,	0,	0,	-1.5,	358.5,	1000, 0, 0, 0,"
                 "	1392.108941,	-833.2591342,	0,	999.3147674,	-26.16797812,"
                 "	-26.17694831", 1);
  compareCsvLine(line.getRow(52),
                 "csm_test_046,	FREE,	5,	0,	0,	-1.5,	1.5,	1000, 0, 0, 0,	51.85037177,"
                 "	-597.070682,	0,	999.3147674,	26.16797812,	-26.17694831", 1);
  compareCsvLine(line.getRow(11),
                 "csm_test_005,	FREE,	2,	0,	0,	3.5,	0.5,	1000, 0, 0, 0,	684.8038438,"
                 "	233.517266,	0,	998.0967925,	8.71025875,	61.04853953", 1);


  line = CSVReader(tempDir.path() + "/bundleout_images.csv", false, 0, ',', false, true);
  compareCsvLine(line.getRow(3),
                 "Test_B.cub,	6.65E-12,	1.41E-13,	4.70E-12,	2.875,	0.125,	3,	0.004162598,	0,"
                 "	-0.0078125,	0.0078125,	6.29E-15,	0.004162598,	0,	258,	-2,	256,	68.2,	0",
                 1);
  compareCsvLine(line.getRow(5),
                 "Test_D.cub, 3.96E-12,	1.31E-13,	2.80E-12,	-0.125,	0.125,	2.23E-17,"
                 "	0.004162598,	0,	-2.875,	-0.125,	-3,	0.004162598,	0,	254,"
                 "	2,	256,	68.2,	0", 1);
  compareCsvLine(line.getRow(7),
                 "Test_F.cub,	1.55E-13,	8.46E-14,	1.25E-13,	0,	1.39E-17,	1.39E-17,"
                 "	0.004162598,	0,	3.03125,	-0.03125,	3,	0.004162598,	0,"
                 "	272,	-16,	256,	68.2,	0", 1);
  compareCsvLine(line.getRow(9),
                 "Test_H.cub, 6.65E-12,	1.10E-13,	4.70E-12,	-3.03125,	0.03125,"
                 "	-3,	0.004162598,	0,	0,	6.17E-15,"
                 "	6.17E-15,	0.004162598",1);
  compareCsvLine(line.getRow(11),
                 "Test_J.cub, 2.76E-12,	9.95E-14,	1.96E-12,	-0.0625,	0.0625,"
                 "-2.63E-17,	0.016650391, 0,	-0.03125,	0.03125, 6.23E-15,	0.016650391", 1);

  Cube testB(tempDir.path() + "/Test_B.cub");
  CSMCamera *camB = dynamic_cast<CSMCamera*>(testB.camera());
  EXPECT_NEAR(camB->getParameterValue(0), 3.0, 0.00000001);
  EXPECT_NEAR(camB->getParameterValue(1), 0.0, 0.00000001);
  EXPECT_NEAR(camB->getParameterValue(2), 256.0, 0.00000001);

  Cube testD(tempDir.path() + "/Test_D.cub");
  CSMCamera *camD = dynamic_cast<CSMCamera*>(testD.camera());
  EXPECT_NEAR(camD->getParameterValue(0), 0.0, 0.00000001);
  EXPECT_NEAR(camD->getParameterValue(1), -3.0, 0.00000001);
  EXPECT_NEAR(camD->getParameterValue(2), 256.0, 0.00000001);

  Cube testF(tempDir.path() + "/Test_F.cub");
  CSMCamera *camF = dynamic_cast<CSMCamera*>(testF.camera());
  EXPECT_NEAR(camF->getParameterValue(0), 0.0, 0.00000001);
  EXPECT_NEAR(camF->getParameterValue(1), 3.0, 0.00000001);
  EXPECT_NEAR(camF->getParameterValue(2), 256.0, 0.00000001);

  Cube testH(tempDir.path() + "/Test_H.cub");
  CSMCamera *camH = dynamic_cast<CSMCamera*>(testH.camera());
  EXPECT_NEAR(camH->getParameterValue(0), -3.0, 0.00000001);
  EXPECT_NEAR(camH->getParameterValue(1), 0.0, 0.00000001);
  EXPECT_NEAR(camH->getParameterValue(2), 256.0, 0.00000001);

  Cube testJ(tempDir.path() + "/Test_J.cub");
  CSMCamera *camJ = dynamic_cast<CSMCamera*>(testJ.camera());
  EXPECT_NEAR(camJ->getParameterValue(0), 0.0, 0.00000001);
  EXPECT_NEAR(camJ->getParameterValue(1), 0.0, 0.00000001);
  EXPECT_NEAR(camJ->getParameterValue(2), 128.0, 0.00000001);
}


TEST_F(LidarNetwork, FunctionalTestJigsawLidar) {
  // copy images
  QString cube1fname = tempDir.path() + "/lidarObservationPair1Copy.cub";
  QString cube2fname = tempDir.path() + "/lidarObservationPair2Copy.cub";
  cube1->reopen("rw");
  cube2->reopen("rw");
  QScopedPointer<Cube> cube1Copy( cube1->copy(cube1fname, CubeAttributeOutput()) );
  QScopedPointer<Cube> cube2Copy( cube2->copy(cube2fname, CubeAttributeOutput()) );


  FileList cubeListCopy;
  cubeListCopy.append(cube1Copy->fileName());
  cubeListCopy.append(cube2Copy->fileName());

  cube1->close();
  cube2->close();
  cube1Copy->close();
  cube2Copy->close();

  QString cubeListFileCopy = tempDir.path() + "/cubesCopy.lis";
  cubeListCopy.write(cubeListFileCopy);

  // call jigsaw w/o lidar options & apply=true on copy of images
  QVector<QString> args1 = {"radius=yes",
                            "errorpropagation=yes",
                            "spsolve=position",
                            "spacecraft_position_sigma=1000.0",
                            "camsolve=angles",
                            "twist=yes",
                            "camera_angles_sigma=2.",
                            "update=yes",
                            "bundleout_txt=yes",
                            "cnet="+controlNetPath,
                            "fromlist="+cubeListFile,
                            "onet="+tempDir.path()+"/no_lidar.net",
                            "file_prefix="+tempDir.path()+"/no_lidar"};

  UserInterface ui1(APP_XML, args1);
  jigsaw(ui1);

  // call jigsaw w/ lidar options & apply=true
  QVector<QString> args2 = {"radius=yes",
                            "errorpropagation=yes",
                            "spsolve=position",
                            "spacecraft_position_sigma=1000.0",
                            "camsolve=angles",
                            "twist=yes",
                            "camera_angles_sigma=2.",
                            "update=yes",
                            "SIGMA0=0.00001",
                            "bundleout_txt=yes",
                            "cnet="+controlNetPath,
                            "fromlist="+cubeListFileCopy,
                            "onet="+tempDir.path()+"/lidar.net",
                            "file_prefix="+tempDir.path()+"/lidar",
                            "lidardata="+lidarDataPath,
                            "olidardata="+tempDir.path()+"/lidar_out.json",
                            "olidarformat=json",
                            "lidar_csv=yes"};

  UserInterface ui2(APP_XML, args2);
  jigsaw(ui2);

  // re-open all cubes
  // Make a new cube object to get the updated camera models after bundle adjust
  Cube bundledCube1(cube1Path);
  Cube bundledCube2(cube2Path);
  Cube bundledCube1Copy(cube1fname);
  Cube bundledCube2Copy(cube2fname);

  std::map<QString, Camera*> noLidarCameras;
  std::map<QString, Camera*> lidarCameras;
  noLidarCameras[SerialNumber::Compose(bundledCube1)] = bundledCube1.camera();
  noLidarCameras[SerialNumber::Compose(bundledCube2)] = bundledCube2.camera();
  lidarCameras[SerialNumber::Compose(bundledCube1Copy)] = bundledCube1Copy.camera();
  lidarCameras[SerialNumber::Compose(bundledCube2Copy)] = bundledCube2Copy.camera();

  // for each point in lidar data
  for (const auto &point : rangeData.points()) {
    for (const auto &sn : point->snSimultaneous()) {
      //point get measure
      ControlMeasure *m = (*point)[sn];
      // in no-lidar images do ground to image to get spacecraft position at observing time
      Camera *noLidarCamera = noLidarCameras[sn];
      bool success = noLidarCamera->SetImage(m->GetSample(), m->GetLine());

      EXPECT_TRUE(success) << "Failed to set image in no-lidar cube " << sn.toStdString()
            << " at point " << point->GetId().toStdString();
      if (!success) {
        continue;
      }

      // in lidar images do ground to image to get spacecraft position at observing time
      Camera *lidarCamera = lidarCameras[sn];
      success = lidarCamera->SetImage(m->GetSample(), m->GetLine());

      EXPECT_TRUE(success) << "Failed to set image in lidar cube " << sn.toStdString()
            << " at point " << point->GetId().toStdString();
      if (!success) {
        continue;
      }
      // check that distance from ground to spacecraft position is closer to lidar range in lidar image than in no-lidar images
      EXPECT_LT(abs(lidarCamera->SlantDistance() - point->range()), abs(noLidarCamera->SlantDistance() - point->range()))
            << "Failed for point " <<  point->GetId().toStdString();
    }
  }

  LidarData lidarDataIn;
  lidarDataIn.read(lidarDataPath);
  LidarData lidarDataOut;
  lidarDataOut.read(tempDir.path() + "/lidar_out.json");

  QFile bo(tempDir.path() + "/lidar_bundleout.txt");
  QString contents;
  if (bo.open(QIODevice::ReadOnly)) {
    contents = bo.read(bo.size());
  }
  else {
    FAIL() << "Failed to open bundleout.txt" << std::endl;
  }

  QStringList lines = contents.split("\n");

  EXPECT_THAT(lines[10].toStdString(), HasSubstr(("Lidar Data Filename: " + lidarDataPath).toStdString()));

  QStringList lidarPoints = lines[73].split(":");
  EXPECT_THAT(lidarPoints[0].trimmed().toStdString(), HasSubstr("Lidar Points"));
  EXPECT_EQ(lidarPoints[1].trimmed().toInt(), lidarDataIn.numberLidarPoints());

  int nMeasuresCube1 = lidarDataIn.GetMeasuresInCube(SerialNumber::Compose(bundledCube1Copy)).count();
  int nMeasuresCube2 = lidarDataIn.GetMeasuresInCube(SerialNumber::Compose(bundledCube2Copy)).count();
  int nValidMeasuresCube1 = lidarDataIn.GetNumberOfValidMeasuresInImage( SerialNumber::Compose(bundledCube1Copy));
  int nValidMeasuresCube2 = lidarDataIn.GetNumberOfValidMeasuresInImage( SerialNumber::Compose(bundledCube2Copy));

  QStringList lidarRangeConstraints = lines[79].split(":");
  EXPECT_THAT(lidarRangeConstraints[0].trimmed().toStdString(), HasSubstr("Lidar Range Constraints"));
  EXPECT_EQ(lidarRangeConstraints[1].trimmed().toInt(), lidarDataIn.numberSimultaneousMeasures());

  QStringList columns = lines[136].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  ASSERT_GE(columns.size(), 10);
  EXPECT_EQ(columns[6].toInt(), nValidMeasuresCube1);
  EXPECT_EQ(columns[7].toInt(), nMeasuresCube1);
  columns = lines[137].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  ASSERT_GE(columns.size(), 10);
  EXPECT_EQ(columns[6].toInt(), nValidMeasuresCube2);
  EXPECT_EQ(columns[7].toInt(), nMeasuresCube2);

  CSVReader::CSVAxis csvLine;
  CSVReader header = CSVReader(tempDir.path()+"/lidar_bundleout_lidar.csv",
                               false, 0, ',', true, true);

  for (int i = 3; i < header.rows(); i++){
    csvLine = header.getRow(i);
    QString pointId = csvLine[0].trimmed();
    EXPECT_NEAR(csvLine[2].toDouble(), lidarDataIn.point(pointId)->range(), 0.0001);
    EXPECT_NEAR(csvLine[3].toDouble(), lidarDataIn.point(pointId)->sigmaRange() * 0.001, 0.0001);
    EXPECT_NEAR(csvLine[4].toDouble(), lidarDataOut.point(pointId)->range(), 0.0001);
    // The bundle doesn't write out updated sigma ranges
    // EXPECT_NEAR(csvLine[5].toDouble(), lidarDataOut.point(pointId)->sigmaRange() * 0.001, 0.0001);
  }

}
