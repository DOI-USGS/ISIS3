#include <map>
#include <cmath>

#include <QtMath>
#include <QFile>
#include <QScopedPointer>

// #include "Pvl.h"
// #include "PvlGroup.h"
// #include "Statistics.h"
#include "CSVReader.h"
// #include "Latitude.h"
// #include "Longitude.h"
// #include "ControlPoint.h"
// #include "CSMCamera.h"
// #include "LidarData.h"
// #include "SerialNumber.h"

#include "TestUtilities.h"
#include "NetworkFixtures.h"
#include "CsmFixtures.h"

#include "gmock/gmock.h"
#include "gtest/gtest-spi.h"

using namespace Isis;
using namespace testing;

TEST(TestUtilities, IsNumeric) {
    EXPECT_TRUE(isNumeric("0")           );
    EXPECT_TRUE(isNumeric("1")           );
    EXPECT_TRUE(isNumeric("3.14")        );
    EXPECT_TRUE(isNumeric("347")         );
    EXPECT_TRUE(isNumeric("194602754")   );
    EXPECT_TRUE(isNumeric("-3")          );
    EXPECT_TRUE(isNumeric("-4867")       );
    EXPECT_TRUE(isNumeric("5e7")         ); // sci notation
    EXPECT_TRUE(isNumeric("3.875e18")    ); // sci notation
    EXPECT_TRUE(isNumeric("-4.55")       ); 
    EXPECT_TRUE(isNumeric("34564.488564"));
    EXPECT_TRUE(isNumeric(".99431")      );

    EXPECT_FALSE(isNumeric("abcdef")            ); // alphabet (hex)
    EXPECT_FALSE(isNumeric("kittenrainbowmagic")); // alphabet
    EXPECT_FALSE(isNumeric("13466-234")         ); // hyphen
    EXPECT_FALSE(isNumeric("e34e")              ); // Wrong e's
    EXPECT_FALSE(isNumeric("e3")                );
    EXPECT_FALSE(isNumeric("5.4e")              );
    EXPECT_FALSE(isNumeric("Hello World")       ); // Words
    EXPECT_FALSE(isNumeric("123 4 56")          ); // Spaces
    EXPECT_FALSE(isNumeric("45..54")            ); // Double Decimal
    EXPECT_FALSE(isNumeric("321.")              ); // Decimal Point with no digits
    EXPECT_FALSE(isNumeric("22/7")              ); // Fractional Pi
    EXPECT_FALSE(isNumeric("2024.07.31")        ); // Decimal Date
    EXPECT_FALSE(isNumeric("2/3/2007")          ); // Date
    EXPECT_FALSE(isNumeric("6A1F")              ); // Hexadecimal
}

TEST(TestUtilities, CompareCsvLine) {
    CSVReader csv = CSVReader("data/testUtilities/testcsv1.csv",
                                false, 0, ',', false, true);
    CSVReader::CSVAxis csvLine;
    CSVReader::CSVAxis csvLine2;
    

    // Standard line found in another test
    csvLine = csv.getRow(0);
    compareCsvLine(csvLine, "3-d,3-d,3-d,Sigma,Sigma,Sigma,Correction,Correction,Correction,Coordinate,Coordinate,Coordinate");

    EXPECT_NONFATAL_FAILURE({
        EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "3-c,3-e,3-f,Sigma,Alpha,Sigma,Correction,Correction,Correction,Coordinate,Coordinate,Coordinate"), "");
    }, "Actual: 4"); // need double-layer expect for more than one expected failure with "Actual: 3" for 3 failures.


    // Test for Near doubles, csv file has 3.141592653589793.  Default tolerance 0.000001
    csvLine = csv.getRow(1);
    compareCsvLine(csvLine, "Near Doubles, 3.1415926535898");
    compareCsvLine(csvLine, "Near Doubles, 3.141593");

    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Near Doubles, 3.14159"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Near Doubles, 3.141591"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Near Doubles, Pi"), "");


    // Scientific Notation (csv has sci notation)
    csvLine = csv.getRow(2);
    compareCsvLine(csvLine, "Sci Notation, 4.78e3");
    compareCsvLine(csvLine, "Sci Notation, 4780");

    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Sci Notation, 478"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Sci Notation, 4783"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Sci Notation, Text"), "");


    // Scientific Notation (csv has standard notation)
    csvLine = csv.getRow(3);
    compareCsvLine(csvLine, "Sci Notation, 4.78e3");
    compareCsvLine(csvLine, "Sci Notation, 4780");

    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Sci Notation, 478"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Sci Notation, 4783"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Sci Notation, Text"), "");

    // Compare only second cell and onward
    compareCsvLine(csvLine, "Pie Notation, 4780", 1);    
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Pie Notation, 4780"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Pie Notation, 4783", 1), "");

    
    // Text vs Numbers
    csvLine = csv.getRow(4);
    compareCsvLine(csvLine, "Zeroes and Strings, 0");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "0, 0"), "");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Zeroes and Strings, Zeroes and Strings"), "");

    // Compare Multiple Lines
    csvLine = csv.getRow(0);
    csvLine2 = csv.getRow(5);
    compareCsvLine(csvLine, csvLine2);
    csvLine2 = csv.getRow(6);

    EXPECT_NONFATAL_FAILURE({
        EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, csvLine2), "");
    }, "Actual: 6");

    
}
