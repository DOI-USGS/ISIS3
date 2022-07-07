#include "gmock/gmock.h"

#include <QString>
#include <QVector>

#include "isis2std.h"

#include "Cube.h"
#include "std2isis.h"
#include "Fixtures.h"
#include "LineManager.h"
#include "Portal.h"
#include "SpecialPixel.h"
#include "Statistics.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/isis2std.xml").expanded();
static QString STD2ISIS_XML = FileName("$ISISROOT/bin/xml/std2isis.xml").expanded();


TEST_F(TempTestingFiles, FunctionalTestsIsis2StdBmp) {
  QString inputCubeFilename = tempDir.path() + "/test_input.cub";
  Cube inputCube;
  inputCube.setDimensions(128, 128, 1);
  inputCube.create(inputCubeFilename);

  int chunkSize = inputCube.lineCount() / 8.0;

  int pixVal = 0;
  LineManager lineWriter(inputCube);
  lineWriter.begin();
  // Write an integer gradient
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = pixVal++;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }

  // Write VERY big negative and positive numbers
  double floatScale = ((double)VALID_MAX4 - (double)VALID_MIN4) / (chunkSize - 1);
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = VALID_MIN4 + line * floatScale;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }

  // Write VERY small negative and positive numbers
  double tinyStart = -1e-20;
  double tinyStop = 1e-20;
  double tinyScale = (tinyStop - tinyStart) / (chunkSize - 1);
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = tinyStart + line * tinyScale;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }

  // Write all of the special pixel values
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = Lrs;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = Lis;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = His;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }
  for (int line = 0; line < chunkSize; line++) {
    for (int sample = 0; sample < lineWriter.size(); sample++) {
      lineWriter[sample] = Hrs;
    }
    inputCube.write(lineWriter);
    lineWriter++;
  }

  inputCube.close();

  QString outputBmpFilename = tempDir.path() + "/test_output.bmp";
  QVector<QString> args = {"from=" + inputCubeFilename,
                           "to=" + outputBmpFilename,
                           "mode=grayscale",
                           "format=bmp",
                           "stretch=linear"};

  UserInterface options(APP_XML, args);
  try {
    isis2std(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to translate image: " << e.what() << std::endl;
  }

  QString reingestCubeFilename = tempDir.path() + "/test_output.cub";
  QVector<QString> reingestArgs = {"from=" + outputBmpFilename,
                                   "to=" + reingestCubeFilename};

  UserInterface reingestOptions(STD2ISIS_XML, reingestArgs);
  try {
    std2isis(reingestOptions);
  }
  catch (IException &e) {
    FAIL() << "Unable to reingest image: " << e.what() << std::endl;
  }

  Cube reingestCube(reingestCubeFilename);

  Portal checkReader(reingestCube.sampleCount(), chunkSize, reingestCube.pixelType());

  // Check gradient
  checkReader.SetPosition(1, 1, 1);
  reingestCube.read(checkReader);

  Statistics gradientStats;
  gradientStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(gradientStats.Minimum(), 127);
  EXPECT_EQ(gradientStats.Maximum(), 127);

  // Check very large
  double newLargeSlope = (double)(255 - 1) / (double)(chunkSize - 1);
  Portal largeReader(reingestCube.sampleCount(), 1, reingestCube.pixelType());
  for (int line = 0; line < chunkSize; line++) {
    largeReader.SetPosition(1, chunkSize+line+1, 1);
    reingestCube.read(largeReader);

    Statistics largeStats;
    largeStats.AddData(largeReader.DoubleBuffer(), largeReader.size());
    EXPECT_EQ(largeStats.Minimum(), 1 + (int)(line * newLargeSlope));
    EXPECT_EQ(largeStats.Maximum(), 1 + (int)(line * newLargeSlope));
  }

  // Check very small
  checkReader.SetPosition(1, chunkSize*2+1, 1);
  reingestCube.read(checkReader);

  Statistics smallStats;
  smallStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(smallStats.Minimum(), 127);
  EXPECT_EQ(smallStats.Maximum(), 127);

  // Check special pixels
  checkReader.SetPosition(1, chunkSize*3+1, 1);
  reingestCube.read(checkReader);

  Statistics lrsStats;
  lrsStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(lrsStats.Minimum(), 1);
  EXPECT_EQ(lrsStats.Maximum(), 1);

  checkReader.SetPosition(1, chunkSize*4+1, 1);
  reingestCube.read(checkReader);

  Statistics lisStats;
  lisStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(lisStats.Minimum(), 1);
  EXPECT_EQ(lisStats.Maximum(), 1);

  checkReader.SetPosition(1, chunkSize*5+1, 1);
  reingestCube.read(checkReader);

  Statistics hisStats;
  hisStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(hisStats.Minimum(), HIGH_INSTR_SAT1);
  EXPECT_EQ(hisStats.Maximum(), HIGH_INSTR_SAT1);

  checkReader.SetPosition(1, chunkSize*6+1, 1);
  reingestCube.read(checkReader);

  Statistics hrsStats;
  hrsStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(hrsStats.Minimum(), HIGH_REPR_SAT1);
  EXPECT_EQ(hrsStats.Maximum(), HIGH_REPR_SAT1);

  checkReader.SetPosition(1, chunkSize*7+1, 1);
  reingestCube.read(checkReader);

  Statistics nullStats;
  nullStats.AddData(checkReader.DoubleBuffer(), checkReader.size());
  EXPECT_EQ(nullStats.Minimum(), NULL1);
  EXPECT_EQ(nullStats.Maximum(), NULL1);
}
