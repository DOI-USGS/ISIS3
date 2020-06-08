#include "isis2ascii.h"

#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>

#include "Fixtures.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "CSVReader.h"
#include "LineManager.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/isis2ascii.xml").expanded();

TEST_F(SmallCube, FunctionalTestIsis2asciiDefaultParameters) {
  QString outputFile = tempDir.path()+"/output.txt";
  QVector<QString> args = {"to=" + outputFile};
  UserInterface ui(APP_XML, args);

  isis2ascii(testCube, ui);

  CSVReader::CSVAxis csvLine;
  // Access the "Header" information first
  CSVReader header = CSVReader(outputFile,
                               false, 0, ' ', false, true);

  // Validate the header information is correct
  csvLine = header.getRow(0);
  ASSERT_EQ(csvLine[0], "Input_Cube");
  ASSERT_EQ(csvLine[1], testCube->fileName());

  csvLine = header.getRow(1);
  ASSERT_EQ(csvLine[0], "Samples");
  ASSERT_EQ(csvLine[1].toInt(), 10);

  csvLine = header.getRow(2);
  ASSERT_EQ(csvLine[0], "Lines");
  ASSERT_EQ(csvLine[1].toInt(), 10);

  csvLine = header.getRow(3);
  ASSERT_EQ(csvLine[0], "Bands");
  ASSERT_EQ(csvLine[1].toInt(), 10);

  // Check that the data after the "Header" is correct
  CSVReader reader = CSVReader(outputFile,
                               false, 4, ' ', false, true);

  int pixelValue = 0.0;
  for (int i = 0; i < reader.rows(); i++) {
    csvLine = reader.getRow(i);
    for (int j = 0; j < csvLine.dim(); j++) {
      ASSERT_EQ(csvLine[j].toInt(), pixelValue++);
    }
  }
}

TEST_F(SmallCube, FunctionalTestIsis2asciiNoHeader) {
  QString outputFile = tempDir.path()+"/output.txt";
  QVector<QString> args = {"to=" + outputFile, "header=no"};
  UserInterface ui(APP_XML, args);

  isis2ascii(testCube, ui);

  CSVReader::CSVAxis csvLine;
  // Check that no "Header" was output at the beginning of the file
  CSVReader reader = CSVReader(outputFile,
                               false, 0, ' ', false, true);

  int pixelValue = 0.0;
  for (int i = 0; i < reader.rows(); i++) {
    csvLine = reader.getRow(i);
    for (int j = 0; j < csvLine.dim(); j++) {
      ASSERT_EQ(csvLine[j].toInt(), pixelValue++);
    }
  }

}

TEST_F(SmallCube, FunctionalTestIsis2asciiSetPixelValues) {
  QString outputFile = tempDir.path()+"/output.txt";
  QVector<QString> args = {"to=" + outputFile, "setpixelvalues=yes",
                           "nullvalue=0", "lrsvalue=0", "lisvalue=0",
                           "hisvalue=255", "hrsvalue=255"};
  UserInterface ui(APP_XML, args);

  // Use a line manager to update select values with ISIS special pixel values
  LineManager line(*testCube);
  double pixelValue = 0.0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if (pixelValue == 10) {
        line[i] = Isis::NULL8;
      }
      else if (pixelValue == 11) {
        line[i] = Isis::LOW_REPR_SAT8;
      }
      else if (pixelValue == 12) {
        line[i] = Isis::HIGH_REPR_SAT8;
      }
      else if (pixelValue == 13) {
        line[i] = Isis::LOW_INSTR_SAT8;
      }
      else if (pixelValue == 14) {
        line[i] = Isis::HIGH_INSTR_SAT8;
      }
      else {
        line[i] = (double) pixelValue;
      }
      pixelValue++;
    }
    testCube->write(line);
  }

  isis2ascii(testCube, ui);

  CSVReader::CSVAxis csvLine;
  CSVReader reader = CSVReader(outputFile,
                               false, 4, ' ', false, true);
  // Check the special pixel values were output correctly
  csvLine = reader.getRow(1);
  ASSERT_EQ(csvLine[0].toInt(), 0);
  ASSERT_EQ(csvLine[1].toInt(), 0);
  ASSERT_EQ(csvLine[2].toInt(), 255);
  ASSERT_EQ(csvLine[3].toInt(), 0);
  ASSERT_EQ(csvLine[4].toInt(), 255);
}
