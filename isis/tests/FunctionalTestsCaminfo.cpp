#include "Cube.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "caminfo.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/caminfo.xml").expanded();


TEST_F(DefaultCube, FunctionalTestCaminfoCsv) {
    QString outFileName = tempDir.path() + "/outTemp.csv";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outFileName,
        "FORMAT=flat", "APPEND=false", "STATISTICS=true", "CAMSTATS=true",
        "GEOMETRY=true", "spice=true"};

    UserInterface options(APP_XML, args);
    try {
       caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    CSVReader::CSVAxis csvLine;
    // Access the "Header" information first
    CSVReader header = CSVReader(outFileName,
                                 false, 0, ',', false, true);

    // Validate the header information is correct
    csvLine = header.getRow(1);
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "caminfo");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[2], "Viking1/VISB/33322515");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[3], "default.cub");
    EXPECT_EQ(csvLine[4].toInt(), 1056);
    EXPECT_EQ(csvLine[5].toInt(), 1204);
    EXPECT_EQ(csvLine[6].toInt(), 1);
    EXPECT_NEAR(csvLine[7].toDouble(), 9.928647808629, 0.001);
    EXPECT_NEAR(csvLine[8].toDouble(), 10.434709827388, 0.001);
    EXPECT_NEAR(csvLine[9].toDouble(), 255.64554860056, 0.001);
    EXPECT_NEAR(csvLine[10].toDouble(), 256.14606965798, 0.001);
    EXPECT_NEAR(csvLine[11].toDouble(), 18.840683405214, 0.001);
    EXPECT_NEAR(csvLine[12].toDouble(), 18.985953933844, 0.001);
    EXPECT_NEAR(csvLine[13].toDouble(), 79.756143324179, 0.001);
    EXPECT_NEAR(csvLine[14].toDouble(), 81.304900825912, 0.001);
    EXPECT_NEAR(csvLine[15].toDouble(), 10.798462192382, 0.001);
    EXPECT_NEAR(csvLine[16].toDouble(), 13.50263114771, 0.001);
    EXPECT_NEAR(csvLine[17].toDouble(), 69.941096000691, 0.001);
    EXPECT_NEAR(csvLine[18].toDouble(), 70.311945037864, 0.001);
    EXPECT_NEAR(csvLine[19].toDouble(), 7.7698055343487, 0.001);
    EXPECT_NEAR(csvLine[20].toDouble(), 7.803173604843, 0.001);
    EXPECT_NEAR(csvLine[21].toDouble(), 19.180671075411, 0.001);
    EXPECT_NEAR(csvLine[22].toDouble(), 19.525658781648, 0.001);
    EXPECT_NEAR(csvLine[23].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[24].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[25].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[26].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[27].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[28].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[29].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[30].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[31].toDouble(), 100, 0.001);
    ASSERT_EQ(csvLine[32].toInt(), 1271424);
    EXPECT_NEAR(csvLine[33].toDouble(), 1, 0.001);
    EXPECT_NEAR(csvLine[34].toDouble(), 1, 0.001);
    EXPECT_NEAR(csvLine[35].toDouble(), 1, 0.001);
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[36], "MARS");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[37], "1977-07-09T20:05:51.5549999");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[38], "1977-07-09T20:05:51.5549999");
    EXPECT_NEAR(csvLine[39].toDouble(), 528.0, 0.001);
    EXPECT_NEAR(csvLine[40].toDouble(), 602.0, 0.001);
    EXPECT_NEAR(csvLine[41].toDouble(), 10.181441189059, 0.001);
    EXPECT_NEAR(csvLine[42].toDouble(), 255.89292858638001, 0.001);
    EXPECT_NEAR(csvLine[43].toDouble(), 3412288.6566562001, 0.001);
    EXPECT_NEAR(csvLine[44].toDouble(), 310.20703346939001, 0.001);
    EXPECT_NEAR(csvLine[45].toDouble(), -46.327247017379, 0.001);
    EXPECT_NEAR(csvLine[46].toDouble(), 255.64554860056, 0.001);
    EXPECT_NEAR(csvLine[47].toDouble(), 10.086794148631, 0.001);
    EXPECT_NEAR(csvLine[48].toDouble(), 255.96651410281, 0.001);
    EXPECT_NEAR(csvLine[49].toDouble(), 9.928647808629, 0.001);
    EXPECT_NEAR(csvLine[50].toDouble(), 256.14606965798, 0.001);
    EXPECT_NEAR(csvLine[51].toDouble(), 10.279980555851, 0.001);
    EXPECT_NEAR(csvLine[52].toDouble(), 255.82316032959, 0.001);
    EXPECT_NEAR(csvLine[53].toDouble(), 10.434709827388, 0.001);
    EXPECT_NEAR(csvLine[54].toDouble(), 80.528382053153, 0.001);
    EXPECT_NEAR(csvLine[55].toDouble(), 12.13356433166, 0.001);
    EXPECT_NEAR(csvLine[56].toDouble(), 70.127983086993, 0.001);
    EXPECT_NEAR(csvLine[57].toDouble(), 332.65918485196, 0.001);
    EXPECT_NEAR(csvLine[58].toDouble(), 9.9273765164008, 0.001);
    EXPECT_NEAR(csvLine[59].toDouble(), 294.73518831328, 0.001);
    EXPECT_NEAR(csvLine[60].toDouble(), 7.7862975334032, 0.001);
    EXPECT_NEAR(csvLine[61].toDouble(), 4160.7294345949, 0.001);
    EXPECT_NEAR(csvLine[62].toDouble(), 762.37204489156, 0.001);
    EXPECT_NEAR(csvLine[63].toDouble(), 18.904248476287, 0.001);
    EXPECT_NEAR(csvLine[64].toDouble(), 18.904248476287, 0.001);
    EXPECT_NEAR(csvLine[65].toDouble(), 18.904248476287, 0.001);
    EXPECT_NEAR(csvLine[66].toDouble(), 18.913336801664, 0.001);
    EXPECT_NEAR(csvLine[67].toDouble(), 92.033828011827, 0.001);
    EXPECT_NEAR(csvLine[68].toDouble(), 118.87356332432, 0.001);
    EXPECT_NEAR(csvLine[69].toDouble(), -22.740326163641, 0.001);
    EXPECT_NEAR(csvLine[70].toDouble(), 319.09846558533, 0.001);
    EXPECT_NEAR(csvLine[71].toDouble(), 240.08514371127, 0.001);
    EXPECT_NEAR(csvLine[72].toDouble(), 267.53187323573, 0.001);
    EXPECT_NEAR(csvLine[73].toDouble(), 10.078847382918, 0.001);
    EXPECT_NEAR(csvLine[74].toDouble(), 253.65422317887, 0.001);
    EXPECT_NEAR(csvLine[75].toDouble(), 0.0092584293412006, 0.001);
    EXPECT_NEAR(csvLine[76].toDouble(), -0.21479478952768, 0.001);
    EXPECT_NEAR(csvLine[77].toDouble(), 1.3359751259293, 0.001);
    EXPECT_NEAR(csvLine[78].toDouble(), 2.4227562244446, 0.001);
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[79], "FALSE");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[80], "FALSE");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[81], "FALSE");
    EXPECT_NEAR(csvLine[82].toDouble(), 19.336214228383, 0.001);
    EXPECT_NEAR(csvLine[83].toDouble(), 19.336214228383, 0.001);
    EXPECT_NEAR(csvLine[84].toDouble(), 19.336214228383, 0.001);
    EXPECT_NEAR(csvLine[85].toDouble(), 19.336214228383, 0.001);
}


TEST_F(DefaultCube, FunctionalTestCaminfoDefault) {
    QString outFileName = tempDir.path() + "/outTemp.csv";
    QVector<QString> args = {"to="+outFileName,
        "ISISLABEL=true", "ORIGINAL=true", "STATISTICS=true", "CAMSTATS=true",
        "POLYGON=true", "polysinc=100", "polylinc=100"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(testCube, options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    Pvl pvlobject = Pvl(outFileName);

    ASSERT_TRUE(pvlobject.hasObject("Caminfo"));
    PvlObject camobj = pvlobject.findObject("Caminfo");
    ASSERT_TRUE(camobj.hasObject("Camstats"));
    PvlObject camstats = camobj.findObject("Camstats");

    EXPECT_NEAR(camstats.findKeyword("MinimumLatitude"), 9.9286479874788, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumLatitude"), 10.434709753119, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumLongitude"), 255.64554871862, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumLongitude"), 256.14606952525, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumResolution"), 18.985953877821999, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumPhase"), 79.756143590222, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumPhase"), 81.304900313013, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumEmission"), 10.798462835458, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumEmission"), 13.502630463571, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumIncidence"), 69.941096124192, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumIncidence"), 70.311944975377, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalTimeMinimum"), 7.7698055422189, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalTimeMaximum"), 7.8031735959943, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMaximum"), 19.525658668048, 0.001 );

    EXPECT_TRUE(camobj.hasObject("IsisLabel"));
    EXPECT_TRUE(camobj.hasObject("Parameters"));
    EXPECT_FALSE(camobj.hasObject("OriginalLabel"));

    ASSERT_TRUE(camobj.hasObject("Statistics"));
    PvlObject statistics = camobj.findObject("Statistics");

    EXPECT_NEAR(statistics.findKeyword("MeanValue"), 127.49950846428, 0.001);
    EXPECT_NEAR(statistics.findKeyword("StandardDeviation"), 73.322672255332, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("MinimumValue"), 1.0, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("MaximumValue"), 254, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("PercentHIS"), 0, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("PercentHRS"), 0, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("PercentLIS"), 0, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("PercentLRS"), 0, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("PercentNull"), 0.39208006141146, 0.001 );
    EXPECT_NEAR(statistics.findKeyword("TotalPixels"), 1271424, 0.001 );

    ASSERT_TRUE(camobj.hasObject("Geometry"));
    PvlObject geometry = camobj.findObject("Geometry");

    EXPECT_DOUBLE_EQ(geometry.findKeyword("BandsUsed"), 1);
    EXPECT_DOUBLE_EQ(geometry.findKeyword("ReferenceBand"), 1);
    EXPECT_DOUBLE_EQ(geometry.findKeyword("OriginalBand"), 1);
    EXPECT_EQ(geometry.findKeyword("Target")[0].toStdString(), "MARS");
    EXPECT_EQ(geometry.findKeyword("StartTime")[0].toStdString(), "1977-07-09T20:05:51.5549999");
    EXPECT_EQ(geometry.findKeyword("EndTime")[0].toStdString(), "1977-07-09T20:05:51.5549999");
    EXPECT_DOUBLE_EQ(geometry.findKeyword("CenterLine"), 528.0);
    EXPECT_DOUBLE_EQ(geometry.findKeyword("CenterSample"), 602.0);
    EXPECT_NEAR(geometry.findKeyword("CenterLatitude"), 10.181441241544, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("CenterLongitude"), 255.89292858176, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("CenterRadius"), 3412288.6569794999, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("RightAscension"), 310.20703346939001, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("Declination"), -46.327247017379, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("UpperLeftLongitude"), 255.64554860056, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("UpperLeftLatitude"), 10.086794148631, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("LowerLeftLongitude"), 255.96651410281, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("LowerLeftLatitude"), 9.928647808629, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("LowerRightLongitude"), 256.14606965798, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("LowerRightLatitude"), 10.279980555851, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("UpperRightLongitude"), 255.82316032959, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("UpperRightLatitude"), 10.434709827388, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("PhaseAngle"), 80.528382053153, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("EmissionAngle"), 12.13356433166, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("IncidenceAngle"), 70.127983086993, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("NorthAzimuth"), 332.65918485196, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("OffNadir"), 9.9273765164008, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SolarLongitude"), -1.7976931348623099e+308, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("LocalTime"), 7.7862975334032, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("TargetCenterDistance"), 4160.7294345949, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SlantDistance"), 762.37204489156, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SampleResolution"), 18.904248476287, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("LineResolution"), 18.904248476287, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("PixelResolution"), 18.904248476287, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("MeanGroundResolution"), 18.913336801664, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSolarAzimuth"), 92.033828011827, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSolarGroundAzimuth"), 118.87356332432, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSolarLatitude"), -22.740326163641, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSolarLongitude"), 319.09846558533, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSpacecraftAzimuth"), 240.08514371127, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSpacecraftGroundAzimuth"), 267.53187323573, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSpacecraftLatitude"), 10.078847382918, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("SubSpacecraftLongitude"), 253.65422317887, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ParallaxX"), 0.0092584293412006, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ParallaxY"), -0.21479478952768, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ShadowX"), 1.3359751259293, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ShadowY"), 2.4227562244446, 0.0001);
    EXPECT_EQ(geometry.findKeyword("HasLongitudeBoundary")[0].toStdString(), "FALSE");
    EXPECT_EQ(geometry.findKeyword("HasNorthPole")[0].toStdString(), "FALSE");
    EXPECT_EQ(geometry.findKeyword("HasSouthPole")[0].toStdString(), "FALSE");
    EXPECT_NEAR(geometry.findKeyword("ObliqueSampleResolution"), 19.336214228383, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ObliqueLineResolution"), 19.336214228383, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ObliquePixelResolution"), 19.336214228383, 0.0001);
    EXPECT_NEAR(geometry.findKeyword("ObliqueDetectorResolution"), 19.336214228383, 0.0001);
}


TEST_F(DefaultCube, FunctionalTestCaminfoPoly) {
    QString outFileName = tempDir.path() + "/outTemp.pvl";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outFileName,
        "ISISLABEL=false", "ORIGINAL=false", "STATISTICS=false", "CAMSTATS=false",
        "POLYGON=true", "inctype=vertices", "numvertices=3"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    Pvl pvlobject = Pvl(outFileName);
    PvlObject camobj = pvlobject.findObject("Caminfo");
    PvlObject poly = camobj.findObject("Polygon");

    EXPECT_NEAR(poly.findKeyword("CentroidLine"), 533.58306993138, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidSample"), 608.16401376754, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidLatitude"), 10.182403056571, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidLongitude"), 255.8955754569, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidRadius"), 3412286.6660398, 0.001 );
    EXPECT_NEAR(poly.findKeyword("SurfaceArea"), 486.66203306014, 0.001 );
    EXPECT_NEAR(poly.findKeyword("GlobalCoverage"), 3.33e-04, 0.001 );
    EXPECT_NEAR(poly.findKeyword("SampleIncrement"), 1506, 0.001 );
    EXPECT_NEAR(poly.findKeyword("LineIncrement"),1506, 0.001 );
    EXPECT_TRUE(poly.hasKeyword("GisFootprint"));
}


TEST_F(DefaultCube, FunctionalTestCaminfoBoundary) {
    QString outFileName = tempDir.path() + "/outTemp.cub";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outFileName,
        "ISISLABEL=false", "ORIGINAL=false", "STATISTICS=true", "CAMSTATS=true",
        "POLYGON=true", "LINC=25", "SINC=25", "POLYSINC=100", "POLYLINC=100"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    Pvl pvlobject = Pvl(outFileName);
    PvlObject camobj = pvlobject.findObject("Caminfo");
    PvlObject poly = camobj.findObject("Polygon");

    EXPECT_NEAR(poly.findKeyword("CentroidLine"), 532.66229285950999, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidSample"), 607.53672501072003, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidLatitude"), 10.182356969859001, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidLongitude"), 255.89519621875999, 0.001 );
    EXPECT_NEAR(poly.findKeyword("CentroidRadius"), 3412287.9074047999, 0.001 );
    EXPECT_NEAR(poly.findKeyword("SurfaceArea"), 488.62348528983, 0.001 );
    EXPECT_NEAR(poly.findKeyword("GlobalCoverage"), 3.33e-04, 0.001 );
    EXPECT_NEAR(poly.findKeyword("SampleIncrement"), 100, 0.001 );
    EXPECT_NEAR(poly.findKeyword("LineIncrement"), 100, 0.001 );
    EXPECT_TRUE(poly.hasKeyword("GisFootprint"));

    PvlObject camstats = camobj.findObject("Camstats");

    EXPECT_NEAR(camstats.findKeyword("MinimumLatitude"), 9.9286479874788, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumLatitude"), 10.434709753119, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumLongitude"), 255.64554871862, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumLongitude"), 256.14606952525, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumResolution"), 18.985953877821999, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumPhase"), 79.756145388578005, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumPhase"), 81.304900313013, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumEmission"), 10.798462835458, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumEmission"), 13.502630463571, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MinimumIncidence"), 69.941096124192, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("MaximumIncidence"), 70.311944975377, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalTimeMinimum"), 7.7698055422189, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalTimeMaximum"), 7.8031735959943, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMaximum"), 19.525658668048, 0.001 );
}
