#include <QtMath>

#include <QFile>

#include "Pvl.h"
#include "PvlGroup.h"
#include "Statistics.h"
#include "CSVReader.h"
#include "Latitude.h"
#include "Longitude.h"
#include "ControlPoint.h"

#include "jigsaw.h"

#include "TestUtilities.h"
#include "Fixtures.h"
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
  EXPECT_THAT(lines[57].toStdString(), HasSubstr("LATITUDE"));
  EXPECT_THAT(lines[58].toStdString(), HasSubstr("LONGITUDE"));
  EXPECT_THAT(lines[59].toStdString(), HasSubstr("RADIUS"));

  EXPECT_THAT(lines[244].toStdString(), HasSubstr("Latitude"));
  EXPECT_THAT(lines[248].toStdString(), HasSubstr("Longitude"));
  EXPECT_THAT(lines[252].toStdString(), HasSubstr("Radius"));

  EXPECT_THAT(lines[667].toStdString(), HasSubstr("LATITUDE"));
  EXPECT_THAT(lines[668].toStdString(), HasSubstr("LONGITUDE"));


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
  EXPECT_THAT(lines[57].toStdString(), HasSubstr("X"));
  EXPECT_THAT(lines[58].toStdString(), HasSubstr("Y"));
  EXPECT_THAT(lines[59].toStdString(), HasSubstr("Z"));

  EXPECT_THAT(lines[244].toStdString(), HasSubstr("POINT X"));
  EXPECT_THAT(lines[248].toStdString(), HasSubstr("POINT Y"));
  EXPECT_THAT(lines[252].toStdString(), HasSubstr("POINT Z"));

  EXPECT_THAT(lines[667].toStdString(), HasSubstr("BODY-FIXED-X"));
  EXPECT_THAT(lines[668].toStdString(), HasSubstr("BODY-FIXED-Y"));
  EXPECT_THAT(lines[669].toStdString(), HasSubstr("BODY-FIXED-Z"));


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
  EXPECT_THAT(lines[57].toStdString(), HasSubstr("X"));
  EXPECT_THAT(lines[58].toStdString(), HasSubstr("Y"));
  EXPECT_THAT(lines[59].toStdString(), HasSubstr("Z"));

  EXPECT_THAT(lines[244].toStdString(), HasSubstr("POINT X"));
  EXPECT_THAT(lines[248].toStdString(), HasSubstr("POINT Y"));
  EXPECT_THAT(lines[252].toStdString(), HasSubstr("POINT Z"));

  EXPECT_THAT(lines[667].toStdString(), HasSubstr("BODY-FIXED-X"));
  EXPECT_THAT(lines[668].toStdString(), HasSubstr("BODY-FIXED-Y"));
  EXPECT_THAT(lines[669].toStdString(), HasSubstr("BODY-FIXED-Z"));

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

  EXPECT_THAT(lines[31].toStdString(), HasSubstr("Tier 0 Enabled: TRUE"));
  EXPECT_THAT(lines[32].toStdString(), HasSubstr("Maximum Likelihood Model: Huber"));
  EXPECT_THAT(lines[33].toStdString(), HasSubstr("Quantile used for tweaking constant: 0.6"));
  EXPECT_THAT(lines[34].toStdString(), HasSubstr("Quantile weighted R^2 Residual value: 0.207"));
  EXPECT_THAT(lines[35].toStdString(), HasSubstr("Approx. weighted Residual cutoff: N/A"));

  EXPECT_THAT(lines[37].toStdString(), HasSubstr("Tier 1 Enabled: TRUE"));
  EXPECT_THAT(lines[38].toStdString(), HasSubstr("Maximum Likelihood Model: Chen"));
  EXPECT_THAT(lines[39].toStdString(), HasSubstr("Quantile used for tweaking constant: 0.98"));
  EXPECT_THAT(lines[40].toStdString(), HasSubstr("Quantile weighted R^2 Residual value: 1.0"));
  EXPECT_THAT(lines[41].toStdString(), HasSubstr("Approx. weighted Residual cutoff: 1.0"));

  EXPECT_THAT(lines[43].toStdString(), HasSubstr(" Tier 2 Enabled: FALSE"));
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

  EXPECT_THAT(lines[75].toStdString(), HasSubstr("RADII: MEAN"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, lines[76].trimmed(), "");

  QStringList columns = lines[159].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "RA");
  EXPECT_NEAR(columns[2].toDouble(), 269.9949, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 2.65243903, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 272.64733903, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00167495, 0.0001);

  columns = lines[160].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "DEC");
  EXPECT_NEAR(columns[2].toDouble(), 66.5392, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1.17580491, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 67.71500491, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00068524, 0.0001);

  columns = lines[161].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PM");
  EXPECT_NEAR(columns[1].toDouble(), 38.32132, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -383.36347956, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), -345.04215956, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 1.55731615, 0.0001);

  columns = lines[162].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PMv");
  EXPECT_NEAR(columns[1].toDouble(), 13.17635815, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -0.03669501, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 13.13966314, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 0.00015007, 0.0001);

  columns = lines[163].split(QRegExp("\\s+"), QString::SkipEmptyParts);
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

  EXPECT_THAT(lines[75].toStdString(), HasSubstr("RADII: TRIAXIAL"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, lines[76].trimmed(), "");

  QStringList columns = lines[159].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "RA");
  EXPECT_NEAR(columns[2].toDouble(), 269.9949, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 2.95997958, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 272.95487958, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00199725, 0.0001);

  columns = lines[160].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "POLE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[1], "DEC");
  EXPECT_NEAR(columns[2].toDouble(), 66.5392, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1.16195781, 0.0001);
  EXPECT_NEAR(columns[4].toDouble(), 67.70115781, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[5], "FREE");
  EXPECT_NEAR(columns[6].toDouble(), 0.00149539, 0.0001);

  columns = lines[161].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PM");
  EXPECT_NEAR(columns[1].toDouble(), 38.32132, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -291.78617547, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), -253.4648554, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 2.00568417, 0.0001);

  columns = lines[162].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "PMv");
  EXPECT_NEAR(columns[1].toDouble(), 13.17635815, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -0.02785056, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 13.14850759, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 0.00019333, 0.0001);

  columns = lines[163].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "RadiusA");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), 6.87282091, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1744.27282091, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 1.23289971, 0.0001);

  columns = lines[164].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "RadiusB");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), 2.34406319, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1739.74406319, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 12.52974045, 0.0001);

  columns = lines[165].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[0], "RadiusC");
  EXPECT_NEAR(columns[1].toDouble(), 1737.4, 0.0001);
  EXPECT_NEAR(columns[2].toDouble(), -37.55670044, 0.0001);
  EXPECT_NEAR(columns[3].toDouble(), 1699.84329956, 0.0001);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, columns[4], "FREE");
  EXPECT_NEAR(columns[5].toDouble(), 5.34723296, 0.0001);

}

