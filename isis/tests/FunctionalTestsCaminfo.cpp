#include "Cube.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "CameraStatistics.h"

#include "caminfo.h"

#include "gtest/gtest.h"

using namespace Isis;
using ::testing::HasSubstr;

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
    EXPECT_NEAR(csvLine[9].toDouble(), 10.181983206084, 0.001);
    EXPECT_NEAR(csvLine[10].toDouble(), 0.11084102743244, 0.001);
    EXPECT_NEAR(csvLine[11].toDouble(), 255.64554860056, 0.001);
    EXPECT_NEAR(csvLine[12].toDouble(), 256.14606965798, 0.001);
    EXPECT_NEAR(csvLine[13].toDouble(), 255.89390491018, 0.001);
    EXPECT_NEAR(csvLine[14].toDouble(), 0.10658330462779, 0.001);
    EXPECT_NEAR(csvLine[15].toDouble(), 18.840683405214, 0.001);
    EXPECT_NEAR(csvLine[16].toDouble(), 18.985953933844, 0.001);
    EXPECT_NEAR(csvLine[17].toDouble(), 18.90816559308, 0.001);
    EXPECT_NEAR(csvLine[18].toDouble(), 0.038060007185836, 0.001);
    EXPECT_NEAR(csvLine[19].toDouble(), 18.840683425668, 0.001);
    EXPECT_NEAR(csvLine[20].toDouble(), 18.985953877822, 0.001);
    EXPECT_NEAR(csvLine[21].toDouble(), 18.90816559308, 0.001);
    EXPECT_NEAR(csvLine[22].toDouble(), 0.038060007185836, 0.001);
    EXPECT_NEAR(csvLine[23].toDouble(), 18.840683425668, 0.001);
    EXPECT_NEAR(csvLine[24].toDouble(), 18.985953877822, 0.001);
    EXPECT_NEAR(csvLine[25].toDouble(), 18.90816559308, 0.001);
    EXPECT_NEAR(csvLine[26].toDouble(), 0.038060007185836, 0.001);
    EXPECT_NEAR(csvLine[27].toDouble(), 19.180671135452, 0.001);
    EXPECT_NEAR(csvLine[28].toDouble(), 19.525658668048, 0.001);
    EXPECT_NEAR(csvLine[29].toDouble(), 19.342626220123, 0.001);
    EXPECT_NEAR(csvLine[30].toDouble(), 0.078013435023742, 0.001);
    EXPECT_NEAR(csvLine[31].toDouble(), 19.180671135452, 0.001);
    EXPECT_NEAR(csvLine[32].toDouble(), 19.525658668048, 0.001);
    EXPECT_NEAR(csvLine[33].toDouble(), 19.342626220123, 0.001);
    EXPECT_NEAR(csvLine[34].toDouble(), 0.078013435023742, 0.001);
    EXPECT_NEAR(csvLine[35].toDouble(), 19.180671135452, 0.001);
    EXPECT_NEAR(csvLine[36].toDouble(), 19.525658668048, 0.001);
    EXPECT_NEAR(csvLine[37].toDouble(), 19.342626220123, 0.001);
    EXPECT_NEAR(csvLine[38].toDouble(), 0.078013435023742, 0.001);
    EXPECT_NEAR(csvLine[39].toDouble(), 1.0, 0.001);
    EXPECT_NEAR(csvLine[40].toDouble(), 1.0, 0.001);
    EXPECT_NEAR(csvLine[41].toDouble(), 1.0, 0.001);
    EXPECT_NEAR(csvLine[42].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[43].toDouble(), 79.756143590222, 0.001);
    EXPECT_NEAR(csvLine[44].toDouble(), 81.304900313013, 0.001);
    EXPECT_NEAR(csvLine[45].toDouble(), 80.529097153288, 0.001);
    EXPECT_NEAR(csvLine[46].toDouble(), 0.44420861263609, 0.001);
    EXPECT_NEAR(csvLine[47].toDouble(), 10.798462835458, 0.001);
    EXPECT_NEAR(csvLine[48].toDouble(), 13.502630463571, 0.001);
    EXPECT_NEAR(csvLine[49].toDouble(), 12.15148695101, 0.001);
    EXPECT_NEAR(csvLine[50].toDouble(), 0.56543791358696, 0.001);
    EXPECT_NEAR(csvLine[51].toDouble(), 69.941096124192, 0.001);
    EXPECT_NEAR(csvLine[52].toDouble(), 70.311944975377, 0.001);
    EXPECT_NEAR(csvLine[53].toDouble(), 70.127459134075, 0.001);
    EXPECT_NEAR(csvLine[54].toDouble(), 0.1024903912555, 0.001);
    EXPECT_NEAR(csvLine[55].toDouble(), 7.7698055422189, 0.001);
    EXPECT_NEAR(csvLine[56].toDouble(), 7.8031735959943, 0.001);
    EXPECT_NEAR(csvLine[57].toDouble(), 7.7863626216564, 0.001);
    EXPECT_NEAR(csvLine[58].toDouble(), 0.0071055546164837, 0.001);
    EXPECT_NEAR(csvLine[59].toDouble(), 3410663.3374636, 0.001);
    EXPECT_NEAR(csvLine[60].toDouble(), 3413492.0662692, 0.001);
    EXPECT_NEAR(csvLine[61].toDouble(), 3412205.8144925, 0.001);
    EXPECT_NEAR(csvLine[62].toDouble(), 648.57630811947, 0.001);
    EXPECT_NEAR(csvLine[63].toDouble(), 312.29940658175, 0.001);
    EXPECT_NEAR(csvLine[64].toDouble(), 350.59781250198, 0.001);
    EXPECT_NEAR(csvLine[65].toDouble(), 332.96766151038, 0.001);
    EXPECT_NEAR(csvLine[66].toDouble(), 0.67383190647698, 0.001);
    EXPECT_NEAR(csvLine[67].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[68].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[69].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[70].toDouble(), -1.79769313486231e+308, 0.001);
    EXPECT_NEAR(csvLine[71].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[72].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[73].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[74].toDouble(), 0.0, 0.001);
    EXPECT_NEAR(csvLine[75].toDouble(), 100, 0.001);
    ASSERT_EQ(csvLine[76].toInt(), 1271424);
    EXPECT_NEAR(csvLine[77].toDouble(), 1, 0.001);
    EXPECT_NEAR(csvLine[78].toDouble(), 1, 0.001);
    EXPECT_NEAR(csvLine[79].toDouble(), 1, 0.001);
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[80], "MARS");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[81], "1977-07-09T20:05:51.5549999");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[82], "1977-07-09T20:05:51.5549999");
    EXPECT_NEAR(csvLine[83].toDouble(), 528.0, 0.001);
    EXPECT_NEAR(csvLine[84].toDouble(), 602.0, 0.001);
    EXPECT_NEAR(csvLine[85].toDouble(), 10.181441189059, 0.001);
    EXPECT_NEAR(csvLine[86].toDouble(), 255.89292858638001, 0.001);
    EXPECT_NEAR(csvLine[87].toDouble(), 3412288.6566562001, 0.001);
    EXPECT_NEAR(csvLine[88].toDouble(), 310.20703346939001, 0.001);
    EXPECT_NEAR(csvLine[89].toDouble(), -46.327247017379, 0.001);
    EXPECT_NEAR(csvLine[90].toDouble(), 255.64554860056, 0.001);
    EXPECT_NEAR(csvLine[91].toDouble(), 10.086794148631, 0.001);
    EXPECT_NEAR(csvLine[92].toDouble(), 255.96651410281, 0.001);
    EXPECT_NEAR(csvLine[93].toDouble(), 9.928647808629, 0.001);
    EXPECT_NEAR(csvLine[94].toDouble(), 256.14606965798, 0.001);
    EXPECT_NEAR(csvLine[95].toDouble(), 10.279980555851, 0.001);
    EXPECT_NEAR(csvLine[96].toDouble(), 255.82316032959, 0.001);
    EXPECT_NEAR(csvLine[97].toDouble(), 10.434709827388, 0.001);
    EXPECT_NEAR(csvLine[98].toDouble(), 80.528382053153, 0.001);
    EXPECT_NEAR(csvLine[99].toDouble(), 12.13356433166, 0.001);
    EXPECT_NEAR(csvLine[100].toDouble(), 70.127983086993, 0.001);
    EXPECT_NEAR(csvLine[101].toDouble(), 332.65918485196, 0.001);
    EXPECT_NEAR(csvLine[102].toDouble(), 9.9273765164008, 0.001);
    EXPECT_NEAR(csvLine[103].toDouble(), 294.73518831328, 0.001);
    EXPECT_NEAR(csvLine[104].toDouble(), 7.7862975334032, 0.001);
    EXPECT_NEAR(csvLine[105].toDouble(), 4160.7294345949, 0.001);
    EXPECT_NEAR(csvLine[106].toDouble(), 762.37204489156, 0.001);
    EXPECT_NEAR(csvLine[107].toDouble(), 18.904248476287, 0.001);
    EXPECT_NEAR(csvLine[108].toDouble(), 18.904248476287, 0.001);
    EXPECT_NEAR(csvLine[109].toDouble(), 18.904248476287, 0.001);
    EXPECT_NEAR(csvLine[110].toDouble(), 18.913336801664, 0.001);
    EXPECT_NEAR(csvLine[111].toDouble(), 92.033828011827, 0.001);
    EXPECT_NEAR(csvLine[112].toDouble(), 118.87356332432, 0.001);
    EXPECT_NEAR(csvLine[113].toDouble(), -22.740326163641, 0.001);
    EXPECT_NEAR(csvLine[114].toDouble(), 319.09846558533, 0.001);
    EXPECT_NEAR(csvLine[115].toDouble(), 240.08514371127, 0.001);
    EXPECT_NEAR(csvLine[116].toDouble(), 267.53187323573, 0.001);
    EXPECT_NEAR(csvLine[117].toDouble(), 10.078847382918, 0.001);
    EXPECT_NEAR(csvLine[118].toDouble(), 253.65422317887, 0.001);
    EXPECT_NEAR(csvLine[119].toDouble(), 0.0092584293412006, 0.001);
    EXPECT_NEAR(csvLine[120].toDouble(), -0.21479478952768, 0.001);
    EXPECT_NEAR(csvLine[121].toDouble(), 1.3359751259293, 0.001);
    EXPECT_NEAR(csvLine[122].toDouble(), 2.4227562244446, 0.001);
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[123], "FALSE");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[124], "FALSE");
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[125], "FALSE");
    EXPECT_NEAR(csvLine[126].toDouble(), 19.336214228383, 0.001);
    EXPECT_NEAR(csvLine[127].toDouble(), 19.336214228383, 0.001);
    EXPECT_NEAR(csvLine[128].toDouble(), 19.336214228383, 0.001);
    EXPECT_NEAR(csvLine[129].toDouble(), 19.336214228383, 0.001);
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

    EXPECT_NEAR(camstats.findKeyword("LatitudeMinimum"), 9.9286479874788, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeMaximum"), 10.434709753119, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeAverage"), 10.181983206084, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeStandardDeviation"), 0.11084102743244, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeMinimum"), 255.64554871862, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeMaximum"), 256.14606952525, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeAverage"), 255.89390491018, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeStandardDeviation"), 0.10658330458136, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionMinimum"), 18.840683425668, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionMaximum"), 18.985953877822, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionAverage"), 18.90816559308, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionStandardDeviation"), 0.038060007171614, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionMinimum"), 18.840683425668, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionMaximum"), 18.985953877822, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionAverage"), 18.90816559308, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionStandardDeviation"), 0.038060007171614, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionMinimum"), 18.840683425668, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionMaximum"), 18.985953877822, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionAverage"), 18.90816559308, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionStandardDeviation"), 0.038060007171614, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionMaximum"), 19.525658668048, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionAverage"), 19.342626220123, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionStandardDeviation"), 0.078013435023742, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionMaximum"), 19.525658668048, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionAverage"), 19.342626220123, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionStandardDeviation"), 0.078013435023742, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMaximum"), 19.525658668048, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionAverage"), 19.342626220123, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionStandardDeviation"), 0.078013435023742, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioMinimum"), 1.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioMaximun"), 1.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioAverage"), 1.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioStandardDeviation"), 0.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseMinimum"), 79.756143590222, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseMaximum"), 81.304900313013, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseAverage"), 80.529097153288, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseStandardDeviation"), 0.44420861263609, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionMinimum"), 10.798462835458, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionMaximum"), 13.502630463571, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionAverage"), 12.15148695101, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionStandardDeviation"), 0.56543791358689, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceMinimum"), 69.941096124192, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceMaximum"), 70.311944975377, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceAverage"), 70.127459134075, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceStandardDeviation"), 0.10249039125851, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeMinimum"), 7.7698055422189, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeMaximum"), 7.8031735959943, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeAverage"), 7.7863626216564, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeStandardDeviation"), 0.0071055546198845, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusMinimum"), 3410663.3374636, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusMaximum"), 3413492.0662692, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusAverage"), 3412205.8144925, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusStandardDeviation"), 648.57630914361, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthMinimum"), 312.29940658572, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthMaximum"), 350.59781250682, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthAverage"), 332.96766151042, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthStandardDeviation"), 0.67383189468183, 0.001 );

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
    EXPECT_NEAR(geometry.findKeyword("SolarLongitude"), 294.73518830594998, 0.0001);
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

    EXPECT_NEAR(camstats.findKeyword("LatitudeMinimum"), 9.9286479874788, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeMaximum"), 10.434709753119, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeMinimum"), 255.64554871862, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeMaximum"), 256.14606952525, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionMaximum"), 18.985953877821999, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseMinimum"), 79.756145388578005, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseMaximum"), 81.304900313013, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionMinimum"), 10.798462835458, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionMaximum"), 13.502630463571, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceMinimum"), 69.941096124192, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceMaximum"), 70.311944975377, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeMinimum"), 7.7698055422189, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeMaximum"), 7.8031735959943, 0.001 );
}

TEST_F(DefaultCube, FunctionalTestCaminfoCamStatsTable) {

    CameraStatistics camStats(testCube->camera(), 100, 100, testCube->fileName());

    Pvl statsPvl = camStats.toPvl();

    TableField fname("Name", Isis::TableField::Text, 45);
    TableField fmin("Minimum", Isis::TableField::Double);
    TableField fmax("Maximum", Isis::TableField::Double);
    TableField favg("Average", Isis::TableField::Double);
    TableField fstd("StandardDeviation", Isis::TableField::Double);

    TableRecord record;
    record += fname;
    record += fmin;
    record += fmax;
    record += favg;
    record += fstd;

    Table table("CameraStatistics", record);

    for (int i = 1; i < statsPvl.groups(); i++) {
      PvlGroup &group = statsPvl.group(i);

      int entry = 0;
      record[entry] = group.name();
      entry++;
      for (int j = 0; j < group.keywords(); j++) {
        record[entry] = toDouble(group[j][0]);
        entry++;
      }
      table += record;
    }
    testCube->write(table);

    QString outFileName = tempDir.path() + "/outTemp.csv";
    QVector<QString> args = {"to="+outFileName,
                             "USECAMSTATSTBL=true",
                             "CAMSTATS=true"};

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

    EXPECT_NEAR(camstats.findKeyword("LatitudeMinimum"), 9.9286479874788, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeMaximum"), 10.434709753119, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeAverage"), 10.191400154932, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LatitudeStandardDeviation"), 0.12530865607936, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("LongitudeMinimum"), 255.64554871862, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeMaximum"), 256.14606952525, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeAverage"), 255.90602354651, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LongitudeStandardDeviation"), 0.11949570208348, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("SampleResolutionMinimum"), 18.840699663038, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionMaximum"), 18.985953877822, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionAverage"), 18.911994840355, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("SampleResolutionStandardDeviation"), 0.042469580108781, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("LineResolutionMinimum"), 18.840699663038, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionMaximum"), 18.985953877822, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionAverage"), 18.911994840355, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LineResolutionStandardDeviation"), 0.042469580108781, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("ResolutionMinimum"), 18.840699663038, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionMaximum"), 18.985953877822, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionAverage"), 18.911994840355, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ResolutionStandardDeviation"), 0.042469580108781, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionMaximum"), 19.525658668048, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionAverage"), 19.351980481534, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueSampleResolutionStandardDeviation"), 0.087529228348495, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionMaximum"), 19.525658668048, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionAverage"), 19.351980481534, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueLineResolutionStandardDeviation"), 0.087529228348495, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMinimum"), 19.180671135452, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionMaximum"), 19.525658668048, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionAverage"), 19.351980481534, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("ObliqueResolutionStandardDeviation"), 0.087529228348495, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("AspectRatioMinimum"), 1.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioMaximun"), 1.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioAverage"), 1.0, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("AspectRatioStandardDeviation"), 0.0, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("PhaseAngleMinimum"), 79.756386363556, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseAngleMaximum"), 81.304900313013, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseAngleAverage"), 80.556249549336, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("PhaseAngleStandardDeviation"), 0.496128069014, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("EmissionAngleMinimum"), 10.798462835458, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionAngleMaximum"), 13.502630463571, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionAngleAverage"), 12.222241624466, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("EmissionAngleStandardDeviation"), 0.63393199155819, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("IncidenceAngleMinimum"), 69.941096124192, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceAngleMaximum"), 70.311944975377, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceAngleAverage"), 70.121640864056, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("IncidenceAngleStandardDeviation"), 0.1144744474437, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeMinimum"), 7.7698055422189, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeMaximum"), 7.8031735959943, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeAverage"), 7.7871705307454, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalSolarTimeStandardDeviation"), 0.0079663801248517, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("LocalRadiusMinimum"), 3410663.3374636, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusMaximum"), 3413492.0662692, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusAverage"), 3412223.5305052, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("LocalRadiusStandardDeviation"), 719.07504507167, 0.001 );

    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthMinimum"), 331.7404023018, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthMaximum"), 334.64077228603, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthAverage"), 332.98078370368, 0.001 );
    EXPECT_NEAR(camstats.findKeyword("NorthAzimuthStandardDeviation"), 0.4900870906713, 0.001 );
}
