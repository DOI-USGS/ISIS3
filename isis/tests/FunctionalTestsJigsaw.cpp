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

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/jigsaw.xml").expanded();

// Check to see if a QString contains only numeric values.
bool isNumeric(QString str){
  QRegExp re("-*\\d*.*\\d*");  // a digit (\d), zero or more times (*)
  return re.exactMatch(str);
}

// Testing help function which will compare csv lines
void compareCsvLine(CSVReader::CSVAxis csvLine, QString headerStr, int initialIndex=0) { 
  QStringList compareMe = headerStr.split(",");
  for (int i=initialIndex; i<compareMe.size(); i++) {
    if (isNumeric(compareMe[i].trimmed())) {
      EXPECT_NEAR(csvLine[i].toDouble(), compareMe[i].toDouble(), 0.000001);
    }
    else{
      EXPECT_EQ(QString(csvLine[i]).toStdString(), compareMe[i].toStdString()); 
    }
  }
};


TEST_F(ApolloNetwork, FunctionalTestJigsawFirst) {
  QString controlNetFile = tempDir.path()+"/apollo.net";
  QString inputNetFile =  tempDir.path()+"/apolloNet.net";
  QVector<QString> args1 = {"radius=yes",
                            "errorpropagation=yes",
                            "spsolve=position",
                            "spacecraft_position_sigma=1000.0",
                            "camsolve=angles",
                            "twist=yes",
                            "camera_angles_sigma=2.",
                            "update=no",
                            "bundleout_txt=no",
                            "cnet="+tempDir.path()+"/apolloNet.net",
                            "fromlist="+tempDir.path() + "/cubes.lis",
                            "onet="+tempDir.path()+"/apollo_out.net",
  "file_prefix="+tempDir.path()+"/"};

  UserInterface ui1(APP_XML, args1);

  jigsaw(ui1);

  // Test points.csv, images.csv, residuals.csv 

  QString pointsOutput = tempDir.path() + "/bundleout_points.csv";
  QString imagesOutput = tempDir.path() + "/bundleout_images.csv";
  QString residualsOutput = tempDir.path() + "/residuals.csv";
  ControlNet outputNet(tempDir.path()+"/apollo_out.net");

  // Check for the correct header output format and csv file structure for the points.csv file
  CSVReader::CSVAxis csvLine;
  CSVReader header = CSVReader(pointsOutput,
                               false, 0, ',', false, true);

  int numColumns = header.columns();
  int numRows = header.rows();

  ASSERT_EQ(numColumns, 12);
  ASSERT_EQ(numRows, 398);

  // Validate the header information is correct
  csvLine = header.getRow(0);
  compareCsvLine(csvLine, "3-d,3-d,3-d,Sigma,Sigma,Sigma,Correction,Correction,Correction,Coordinate,Coordinate,Coordinate");

  csvLine = header.getRow(1);
  compareCsvLine(csvLine, "Point,Point,Accepted,Rejected,Residual,Latitude,Longitude,Radius,Latitude,Longitude,Radius,Latitude,Longitude,Radius,X,Y,Z");

  csvLine = header.getRow(2);
  compareCsvLine(csvLine, "Label,Status,Measures,Measures,RMS,(dd),(dd),(km),(m),(m),(m),(m),(m),(m),(km),(km),(km)");

  // Compare all of the values from the network against the values in the CSV 
  QList<ControlPoint*> points = outputNet.GetPoints();

  EXPECT_EQ(numRows-3, points.length());

  ControlPoint* outputPoint;
  for (int i=3; i < numRows; i++) {
    csvLine = header.getRow(i);
    EXPECT_NO_THROW({
      outputPoint = outputNet.GetPoint(csvLine[0]);
    });
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

    EXPECT_NEAR(sp.LatitudeToMeters(sp.GetLatitude().planetocentric(Angle::Radians)) - 
                originalSP.LatitudeToMeters(originalSP.GetLatitude().planetocentric(Angle::Radians)),
                csvLine[11].toDouble(), 1000.0); // FIXME
    EXPECT_NEAR(sp.LongitudeToMeters(sp.GetLongitude().positiveEast(Angle::Radians)) - 
                originalSP.LongitudeToMeters(originalSP.GetLongitude().positiveEast(Angle::Radians)),
                csvLine[12].toDouble(), 1000.0); // FIXME

    EXPECT_NEAR(sp.GetLocalRadius().meters() - originalSP.GetLocalRadius().meters(), csvLine[13].toDouble(), 0.000001);

    EXPECT_NEAR(sp.GetX().kilometers(), csvLine[14].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetY().kilometers(), csvLine[15].toDouble(), 0.000001);
    EXPECT_NEAR(sp.GetZ().kilometers(), csvLine[16].toDouble(), 0.000001);
  }

  // Spot check a few points for hard-coded values
  // A few "Free" points:

  // A few "Constrained" points:

  // Check for the correct header output format and csv file structure for the images.csv file
  header = CSVReader(imagesOutput,
                     false, 0, ',', false, true);

  numColumns = header.columns();
  numRows = header.rows();

  ASSERT_EQ(numColumns, 34);
  ASSERT_EQ(numRows, 9);

  // Validate the header information is correct
  csvLine = header.getRow(0);
  compareCsvLine(csvLine, "Image,rms,rms,rms,X,X,X,X,X,Y,Y,Y,Y,Y,Z,Z,Z,Z,Z,RA,RA,RA,RA,RA,DEC,DEC,DEC,DEC,DEC,TWIST,TWIST,TWIST,TWIST,TWIST");
  csvLine = header.getRow(1);
  compareCsvLine(csvLine, "Filename,sample res,line res,total res,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma,Initial,Correction,Final,Apriori Sigma,Adj Sigma");

  // Since there are few lines left, just test all remaining lines:
  csvLine = header.getRow(2);
  compareCsvLine(csvLine,
"cube1.cub,1.4775447404019,2.7418764825318,2.2023879205705,774.31325994618,0.66867452194612,774.98193446812,1000.0,0.66224060,1070.7396447319,-1.1002200905474,1069.6394246413,1000.0,0.43290316,1281.8228287147,-0.45849850672954,1281.3643302079,1000.0,0.54474183,143.75643505897,0.55133443011393,144.30776948908,2.0,0.36404432,46.120746428714,0.17614019313154,46.296886621845,2.0,0.23277628,-168.38036906625,-0.47498900185037,-168.8553580681,2.0,0.29075877", 1);
  compareCsvLine(header.getRow(3),
"cube2.cub,1.3226216663653,1.2405002035852,1.2822185514634,807.40648922512,0.69654326156559,808.10303248669,1000.0,0.57888224,1052.4498749809,-0.96327187949306,1051.4866031015,1000.0,0.38960862,1276.1624273959,-0.28924089930814,1275.8731864966,1000.0,0.45879385,142.09480144256,0.57573715081764,142.67053859338,2.0,0.36232721,46.380723240595,0.18178389767783,46.562507138273,2.0,0.23283200,-167.23409620674,-0.48023085646307,-167.7143270632,2.0,0.28905743", 1);
  compareCsvLine(header.getRow(4),
"cube3.cub,1.0551824938999,1.2494288227077,1.1563914731711,840.1468023615,0.78500512575822,840.93180748726,1000.0,0.50254430,1033.7079747498,-0.85015359390486,1032.8578211559,1000.0,0.38078186,1269.9529056955,-0.12214007887034,1269.8307656167,1000.0,0.40203785,140.43705778894,0.59084918829917,141.02790697724,2.0,0.36076038,46.66560674281,0.16383404619686,46.829440789007,2.0,0.23287739,-166.11784867517,-0.48897359391078,-166.60682226908,2.0,0.28757386", 1);
  compareCsvLine(header.getRow(5),
"cube4.cub,1.3502729604515,1.353711082842,1.3519931145416,872.53562217205,0.84854359880116,873.38416577086,1000.0,0.43826180,1014.5161278324,-0.71668415091661,1013.7994436815,1000.0,0.40745608,1263.1947139434,0.060025366522923,1263.2547393099,1000.0,0.38749885,138.82875165909,0.59675276674134,139.42550442583,2.0,0.35906774,46.979962778796,0.15383884472574,47.133801623522,2.0,0.23295681,-165.05256071326,-0.4845337845258,-165.53709449778,2.0,0.28606425", 1);
  compareCsvLine(header.getRow(6),
"cube5.cub,1.0263921013246,1.1679291845654,1.0994406136352,904.5796666618,0.93777162375064,905.51743828555,1000.0,0.39302293,994.86764790093,-0.57507847205151,994.29256942888,1000.0,0.46264597,1255.8845902492,0.22662104012307,1256.1112112893,1000.0,0.41928560,137.21862030953,0.61179683816536,137.8304171477,2.0,0.35719491,47.323691611416,0.15310733827143,47.476798949688,2.0,0.23298158,-163.98385410236,-0.48795663815321,-164.47181074051,2.0,0.28449883", 1);
  compareCsvLine(header.getRow(7), "cube6.cub,1.4808878258505,1.8005531118893,1.6484872249257,657.49557080071,0.62269372858298,658.11826452929,1000.0,0.45809629,1126.2181683192,0.078381328281994,1126.2965496475,1000.0,0.63657967,1301.3147739155,0.19617725731274,1301.5109511729,1000.0,0.64002265,118.21104209923,0.96148569390809,119.17252779314,2.0,0.32064269,53.317384096946,-0.13644143601752,53.180942660929,2.0,0.22650237,-150.28440526839,-0.61302136663558,-150.89742663503,2.0,0.28070089", 1);
  compareCsvLine(header.getRow(8), "cube7.cub,14.959149730568,8.8574178952351,12.292884373335,489.97204152251,0.39576403287539,490.36780555539,1000.0,0.51371964,1197.7389611966,-0.60564162355907,1197.133319573,1000.0,0.35200830,1313.3186725115,-0.082013858944591,1313.2366586526,1000.0,0.47543551,159.32459434907,0.64488282485685,159.96947717393,2.0,0.21847866,84.419718861554,0.2565469596083,84.676265821162,2.0,0.23543646,-178.65839562226,-0.27266123419525,-178.93105685646,2.0,0.21759697", 1);

  // Check for the correct header output format and csv file structure for the residuals.csv file
  header = CSVReader(residualsOutput,
                     false, 0, ',', false, true);

  numColumns = header.columns();
  numRows = header.rows();

  ASSERT_EQ(numColumns, 7);
  ASSERT_EQ(numRows, 972);

  // Validate the header information is correct
  csvLine = header.getRow(0);
  compareCsvLine(csvLine, "x image,y image,Measured,Measured,sample,line,Residual Vector");
  csvLine = header.getRow(1);
  compareCsvLine(csvLine, "Point,Image,Image,coordinate,coordinate,Sample,Line,residual,residual,Magnitude");
  csvLine = header.getRow(2);
  compareCsvLine(csvLine, "Label,Filename,Serial Number,(mm),(mm),(pixels),(pixels),(pixels),(pixels),(pixels),Rejected");

  // Check line/sample and residuals 
  // Check all measures
  ControlMeasure* measure;
  for (int i=3; i < numRows; i++) {
    csvLine = header.getRow(i);
    EXPECT_NO_THROW({
      outputPoint = outputNet.GetPoint(csvLine[0]);
    });
    EXPECT_NO_THROW({
      measure = outputPoint->GetMeasure(csvLine[2]);
   });
    // Compare sample, line, residuals
    EXPECT_NEAR(csvLine[5].toDouble(), measure->GetSample(), 0.000001);
    EXPECT_NEAR(csvLine[6].toDouble(), measure->GetLine(), 0.000001);
    EXPECT_NEAR(csvLine[7].toDouble(), measure->GetSampleResidual(), 0.000001);
    EXPECT_NEAR(csvLine[8].toDouble(), measure->GetLineResidual(), 0.000001);
    EXPECT_NEAR(csvLine[9].toDouble(), measure->GetResidualMagnitude(), 0.000001);
  }

  // Spot check a few measures for hard-coded values:
  compareCsvLine(header.getRow(14), "AS15_000031448,/tmp/qt_temp-l7wTTZ/cube1.cub,APOLLO15/METRIC/1971-07-31T14:00:53.547,    -24.91466687,     -8.24555718,   4109.77150653,   2450.19288272,-0.00343036,      0.70304341,      0.70305178", 2); 

  // Test output network size
  ControlNet inputNet("$ISISROOT/../isis/tests/data/apolloNetwork/apollo.net");

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

