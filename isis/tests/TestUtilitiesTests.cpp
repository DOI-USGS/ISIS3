#include "TestUtilities.h"
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
    EXPECT_FALSE(isNumeric("89-e3")             );
}

TEST(TestUtilities, CompareCsvLine) {
    CSVReader csv = CSVReader("data/testUtilities/testcsv1.csv",
                                false, 0, ',', false, true);
    CSVReader::CSVAxis csvLine;
    CSVReader::CSVAxis csvLine2;
    

    // Sample line with many words
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


    // Sample line with lots of numbers
    csvLine = csv.getRow(7);
    compareCsvLine(csvLine, "AS15_000031957,FREE,3,0,0.33,24.25013429,6.15097050,1735.93990543,270.68671676,265.71819251,500.96944842,860.25781493,-1823.63228489,-677.74533463,1573.65050943,169.59077243,712.98695596");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "AS15_000031957,FREE,3,0,0.33,24.25013429,6.15097050,1742.85730233,270.68671676,265.71819251,500.96944842,860.25781493,-1823.63228489,-677.74533463,1573.65050943,169.59077243,712.98695596"), "");


    // long numbers
    csvLine = csv.getRow(8);
    compareCsvLine(csvLine, "Long Numbers, 3.14159265358979323846264338327950288419716939937510");
    compareCsvLine(csvLine, "Long Numbers, 3.14159265358979323846264338327950288419716939937510e0");
    compareCsvLine(csvLine, "Long Numbers, 3.1415926535898");
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Long Numbers, 3.1417"), "");


    // plus and minus
    csvLine = csv.getRow(9);
    compareCsvLine(csvLine, "Plus and Minus, 0, -1, +302, 5.46e-3, -4.7e4, 3+4, 56-62, 89-e3");
    compareCsvLine(csvLine, "Plus and Minus, 0, -1, 302, .00546, -4.7e4, 3+4, 56-62, 89-e3");
    EXPECT_NONFATAL_FAILURE({
        EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, "Plus and Minus, 0, -1, +302, 5.46e3, 4.7e4, 3+A, 56-62, 89-e3"), "");
    }, "Actual: 3");


    // very small
    csvLine = csv.getRow(10);
    csvLine2 = csv.getRow(11);
    compareCsvLine(csvLine, "Very Small, 3.685e-38");
    compareCsvLine(csvLine, "Very Small, 4.152e-36");
    compareCsvLine(csvLine, csvLine, 1, 1e-42);
    EXPECT_NONFATAL_FAILURE(compareCsvLine(csvLine, csvLine2, 1, 1e-39), "");
}
