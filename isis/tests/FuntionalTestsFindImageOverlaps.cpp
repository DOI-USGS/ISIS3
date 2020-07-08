#include "findimageoverlaps.h"
#include "Fixtures.h"
#include "TestUtilities.h"
#include "IException.h"
#include "FileList.h"
#include "ImagePolygon.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/findimageoverlaps.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestFindImageOverlapsNoOverlap) {
  ImagePolygon fp1;
  fp1.Create(*cube1);
  cube1->write(fp1);
  
  Cube newCube2;
  json newIsd2; 
  std::ifstream i(isdPath2->expanded().toStdString());
  i >> newIsd2; 

  newIsd2["instrument_position"]["positions"] = {{1,1,1}, {2,2,2}, {3,3,3}};
  newCube2.fromIsd(tempDir.path()+"/new2.cub", *cube2->label(), newIsd2, "rw"); 

  ImagePolygon fp2;
  fp2.Create(newCube2);
  newCube2.write(fp2); 

  FileList cubes; 
  cubes.append(cube1->fileName());
  cubes.append(newCube2.fileName());
  cube1->close();
  cube2->close();
  newCube2.close();
  
  QString cubeListPath = tempDir.path() + "/cubes.lis"; 
  cubes.write(cubeListPath);
  QVector<QString> args = {"from="+cubeListPath, "overlapList="+tempDir.path()+"/overlaps.txt"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  
  try {
    findimageoverlaps(options, &appLog);
    FAIL() << "Expected an IException with message: \"No overlaps were found\".";
  }
  catch(IException &e) {
    EXPECT_TRUE(e.toString().toLatin1().contains("No overlaps were found"))
      << e.toString().toStdString();
  }

} 
