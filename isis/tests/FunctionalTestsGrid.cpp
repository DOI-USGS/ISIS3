#include "grid.h"

#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>

#include "Fixtures.h"
#include "LineManager.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/grid.xml").expanded();

// Can't create camera model?
TEST_F(SmallCube, FunctionalTestGridGrid) {
  // QVector<QString> args = {"to=" + tempDir.path()+"/output.cub"};
  QVector<QString> args = {"to=/Users/kdlee/output.cub"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  grid(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  line.SetLine(1);
  outputCube.read(line);
  EXPECT_NEAR(line[0], 5.5233364105224609, .00000000000000001);
  EXPECT_NEAR(line[1], 4.9986977577209473, .00000000000000001);
}

TEST_F(SmallCube, FunctionalTestGridImage) {
  // The default linc and sinc are 10 and our image size is 10x10, so make linc and sinc smaller
  // than 10 to see grid.
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub", "mode=image", "linc=5", "sinc=5"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  grid(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  double pixelValue = 0.0;
  for (int i = 1; i <= outputCube.lineCount(); i++) { // 1 based
    line.SetLine(i);
    outputCube.read(line);

    for (int j = 0; j < line.size(); j++) { // 0 based
      if (i % 5 == 1 || j % 5 == 0) {
        EXPECT_EQ(line[j], Isis::Hrs);
      }
      else {
        EXPECT_DOUBLE_EQ(line[j], pixelValue);
      }
      pixelValue++;
    }
  }
}

TEST_F(SmallCube, FunctionalTestGridSetBkgndAndLine) {
  QVector<QString> args = {"to=" + tempDir.path() + "/output.cub", "mode=image", "linc=5", "sinc=5", "bkgndvalue=hrs", "linevalue=lrs"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  grid(testCube, options, &appLog);

  Cube outputCube;
  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  LineManager line(outputCube);
  for (int i = 1; i <= outputCube.lineCount(); i++) { // 1 based
    line.SetLine(i);
    outputCube.read(line);

    for (int j = 0; j < line.size(); j++) { // 0 based
      if (i % 5 == 1 || j % 5 == 0 ) {
        EXPECT_EQ(line[j], Isis::Lrs);
      }
      else {
        EXPECT_EQ(line[j], Isis::Hrs);
      }
    }
  }
  outputCube.close();

  args = {"to=" + tempDir.path() + "/output.cub", "mode=image", "linc=5", "sinc=5", "bkgndvalue=lrs", "linevalue=null"};
  options = UserInterface(APP_XML, args);
  grid(testCube, options, &appLog);

  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  line = LineManager(outputCube);
  for (int i = 1; i <= outputCube.lineCount(); i++) { // 1 based
    line.SetLine(i);
    outputCube.read(line);

    for (int j = 0; j < line.size(); j++) { // 0 based
      if (i % 5 == 1 || j % 5 == 0) {
        EXPECT_EQ(line[j], Isis::Null);
      }
      else {
        EXPECT_EQ(line[j], Isis::Lrs);
      }
    }
  }
  outputCube.close();

  args = {"to=" + tempDir.path() + "/output.cub", "mode=image", "linc=5", "sinc=5", "bkgndvalue=null", "linevalue=dn", "dnvalue=0"};
  options = UserInterface(APP_XML, args);
  grid(testCube, options, &appLog);

  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  line = LineManager(outputCube);
  for (int i = 1; i <= outputCube.lineCount(); i++) { // 1 based
    line.SetLine(i);
    outputCube.read(line);

    for (int j = 0; j < line.size(); j++) { // 0 based
      if (i % 5 == 1 || j % 5 == 0) {
        EXPECT_DOUBLE_EQ(line[j], 0.0);
      }
      else {
        EXPECT_EQ(line[j], Isis::Null);
      }
    }
  }
  outputCube.close();

  args = {"to=" + tempDir.path() + "/output.cub", "mode=image", "linc=5", "sinc=5", "bkgndvalue=DN", "bkgnddnvalue=0"};
  options = UserInterface(APP_XML, args);
  grid(testCube, options, &appLog);

  try {
    outputCube.open(tempDir.path() + "/output.cub", "r");
  }
  catch (IException &e) {
    // Fail test or throw exception?
    throw IException(IException::User,
                     "Unable to open the file [" + tempDir.path() + "/output.cub" + "] as a cube.",
                     _FILEINFO_);
  }

  line = LineManager(outputCube);
  for (int i = 1; i <= outputCube.lineCount(); i++) { // 1 based
    line.SetLine(i);
    outputCube.read(line);

    for (int j = 0; j < line.size(); j++) { // 0 based
      if (i % 5 == 1 || j % 5 == 0) {
        EXPECT_EQ(line[j], Isis::Hrs);
      }
      else {
        EXPECT_DOUBLE_EQ(line[j], 0.0);
      }
    }
  }
  outputCube.close();
}