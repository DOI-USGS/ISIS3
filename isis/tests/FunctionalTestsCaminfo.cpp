#include "Cube.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "caminfo.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/app.xml").expanded();

// removed boundary test because default tests check for polygon

TEST_F(DefaultCube, FunctionalTestCaminfoCsv) {
    QTemporaryDir prefix;
    QString outCubeFileName = prefix.path() + "/outTemp.cub";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
        "FORMAT=flat", "APPEND=false", "STATISTICS=true", "CAMSTATS=true",
        "GEOMETRY=true", "spice=true"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    Cube oCube(outCubeFileName, "r");

    // csv test
        // run with csv, and without
        // make sure values in csv match those in pvl

    // to run without csv, set FORMAT=PVL
    // any other value gives csv
}

TEST_F(DefaultCube, FunctionalTestCaminfoDefault1) {
    QTemporaryDir prefix;
    QString outCubeFileName = prefix.path() + "/outTemp.cub";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
        "ISISLABEL=true", "ORIGINAL=true", "STATISTICS=true", "CAMSTATS=true",
        "POLYGON=true", "polysinc=100", "polylinc=100"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    Cube outCube(outCubeFileName);
    //Cube outCube(outCubeFileName, "r");
    Pvl *pvlobject = outCube.label();
    //Pvl pvlobject = Pvl(outCubeFileName);

    EXPECT_TRUE(pvlobject->hasKeyword("Camstats"));
    PvlGroup group = pvlobject->findGroup("Camstats");

    EXPECT_NEAR(group.findKeyword("MinimumLatitude"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumLatitude"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MinimumLongitude"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumLongitude"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumResolution"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumResolution"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MinimumPhase"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumPhase"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MinimumEmission"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumEmission"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MinimumIncidence"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("MaximumIncidence"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("LocalTimeMinimum"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("LocalTimeMaximum"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("ObliqueResolutionMinimum"), 10000, 0.000001 );
    EXPECT_NEAR(group.findKeyword("ObliqueResolutionMaximum"), 10000, 0.000001 );

    EXPECT_TRUE(pvlobject->hasKeyword("IsisLabel"));
    EXPECT_TRUE(pvlobject->hasKeyword("Parameters"));
    EXPECT_TRUE(pvlobject->hasKeyword("OriginalLabel"));

    EXPECT_TRUE(pvlobject->hasKeyword("Statistics"));
    group = pvlobject->findGroup("Statistics");
    // check stats
        // use functions in cube class to get comparison values,
        // compare those to the file





    // open output file in pvl object
    // find this camstats object, check those values are near the expected
    //      values with a tolerance +/- 10^-6
    // EXPECT_NEAR(value, expected_value, tolerance)
    // check with all camstats values
    // test one object to take the values
    // put in something random for expected_value (something not close)
    // when running test, it'll say what the actual value is
    // plug it into expected value
    // check isislabel object is there
    //  check that parameter object is there exactly like IsisLabel
    // check original label exists

    // check stats
        // for this object, don't hard code values, cube class has a function
        // that gives stats for cube, use that to get comparison values for cube

    // geometry
        // check values like caminfo

    // POLYGON
        // check everything except gis Footprint
        // make sure gisFootprint is there, don't try to match it

    // Mapping  see if it exists

    // for the next tests, see what is different from this one, and check those
    // diff in arg and inputs

    // csv test
        // run with, and without
        // make sure values in csv match those in pvl

}

TEST_F(DefaultCube, FunctionalTestCaminfoDefault2) {
    QTemporaryDir prefix;
    QString outCubeFileName = prefix.path() + "/outTemp.cub";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
        "ISISLABEL=true", "ORIGINAL=true", "STATISTICS=true", "CAMSTATS=true",
        "POLYGON=true"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    // Assert some stuff
}

TEST_F(DefaultCube, FunctionalTestCaminfoDefault3) {
    QTemporaryDir prefix;
    QString outCubeFileName = prefix.path() + "/outTemp.cub";
    QVector<QString> args = {"from="+ testCube->fileName(),  "to="+outCubeFileName,
        "ISISLABEL=true", "ORIGINAL=true", "STATISTICS=true", "CAMSTATS=true",
        "POLYGON=true", "INCTYPE=VERTICES"};

    UserInterface options(APP_XML, args);
    try {
        caminfo(options);
    }
    catch (IException &e) {
        FAIL() << "Unable to open image: " << e.what() << std::endl;
    }

    // Assert some stuff
}


// minirf tests use radar 1 and radar 2
// radar pvl   radar isd
// create a radar fixture

// grab cubes already there, and use the crop app
// crop a small area of the cube to crop to like a 10x10 cube
// use path to data


// under tests there is a data directory
// put the cropped cube in a minirf folder
// pass this cube in as a template
// remember all paths are relative to the test directory
// see line 126 in fixtures.cpp
