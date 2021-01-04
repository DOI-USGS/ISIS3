#include <iostream>
#include <time.h>

#include <QRegExp>
#include <QString>

#include "Fixtures.h"
#include "md5wrapper.h"

#include "topds4.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/topds4.xml").expanded();

TEST_F(SmallCube, FunctionalTestTopds4CurrentTime) {
  QString templateFile = tempDir.path()+"/current_time.tpl";
  QString renderedFile = tempDir.path()+"/current_time.txt";
  std::ofstream of;
  of.open(templateFile.toStdString());
  of << "{{currentTime()}}";
  of.close();
  QVector<QString> args = {"template=" + templateFile, "to=" + renderedFile};
  UserInterface options(APP_XML, args);

  topds4(testCube, options);

  std::ifstream renderedStream;
  renderedStream.open(renderedFile.toStdString());
  std::string line;
  std::getline(renderedStream, line);
  time_t startTime = time(NULL);
  struct tm *tmbuf = gmtime(&startTime);
  char yearstr[80];
  // Just check that the year is correct
  strftime(yearstr, 80, "%Y", tmbuf);
  EXPECT_EQ(yearstr, line.substr(0, 4));
  // Check that the rest is the correct format
  // YYYY-MM-DDTHH:MM:SS
  QRegExp timeRegex("\\d{4}-\\d{2}-\\d{2}T\\d{2}:\\d{2}:\\d{2}");
  EXPECT_TRUE(QString::fromStdString(line).contains(timeRegex)) << "String [" << line << "] "
                                                                << "does not match the time format "
                                                                << "[YYYY-MM-DDTHH:MM:SS].";
}

TEST_F(SmallCube, FunctionalTestTopds4ImageFileName) {
  QString templateFile = tempDir.path()+"/current_time.tpl";
  QString renderedFile = tempDir.path()+"/current_time.txt";
  std::ofstream of;
  of.open(templateFile.toStdString());
  of << "{{imageFileName()}}";
  of.close();
  QVector<QString> args = {"template=" + templateFile, "to=" + renderedFile};
  UserInterface options(APP_XML, args);

  topds4(testCube, options);

  std::ifstream renderedStream;
  renderedStream.open(renderedFile.toStdString());
  std::string line;
  std::getline(renderedStream, line);
  EXPECT_EQ("small.img", line);
}

TEST_F(SmallCube, FunctionalTestTopds4MD5Hash) {
  QString templateFile = tempDir.path()+"/current_time.tpl";
  QString renderedFile = tempDir.path()+"/current_time.txt";
  std::ofstream of;
  of.open(templateFile.toStdString());
  of << "{{md5Hash()}}";
  of.close();
  QVector<QString> args = {"template=" + templateFile, "to=" + renderedFile};
  UserInterface options(APP_XML, args);

  topds4(testCube, options);

  std::ifstream renderedStream;
  renderedStream.open(renderedFile.toStdString());
  std::string line;
  std::getline(renderedStream, line);
  md5wrapper md5;
  EXPECT_EQ(md5.getHashFromFile(testCube->fileName()).toStdString(), line);
}
