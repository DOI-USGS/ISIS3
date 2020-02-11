#include <QTemporaryFile>
#include "campt.h"

#include "Fixtures.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

QString CAMPT_XML = FileName("$ISISROOT/bin/xml/campt.xml").expanded();


TEST_F(TestCube, BadColumnError) {

  // set up bad coordinates file
  std::ofstream of;
  QTemporaryFile badList;
  ASSERT_TRUE(badList.open());
  of.open(badList.fileName().toStdString());
  of << "1, 10,\n10,100,500\n100";
  of.close();

  // configure UserInterface arguments
  QVector<QString> args = {"to=output.pvl", "coordlist=" + badList.fileName(),
                           "coordtype=image"};
  UserInterface options1(CAMPT_XML, args);
  Pvl appLog;

  try {
    campt(testCube, options1, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("Coordinate file formatted incorrectly."))
      << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected an IException with message: \"Coordinate file formatted incorrectly.\n"
              "Each row must have two columns: a sample,line or a latitude,longitude pair.\"";
  }
}


TEST_F(TestCube, FlatFileError) {
  // configure UserInterface arguments for flat file error
  QVector<QString> args = {"format=flat"};
  UserInterface options3(CAMPT_XML, args);
  Pvl appLog;

  try {
    campt(testCube, options3, &appLog);
    FAIL() << "Expected an exception to be thrown";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("Flat file must have a name."))
      << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected an IException with message: \"Flat file must have a name.\"";
  }
}
