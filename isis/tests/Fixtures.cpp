#include "Fixtures.h"

namespace Isis {
  
  void TestCube::SetUp() {
    tempFile.open();
    testCube.setPixelType(UnsignedByte);
  }

  void TestCube::TearDown() {
    if (testCube.isOpen()) {
      testCube.close();
    }
  }

  void TestCube::createCube(Pvl &label) {
    PvlObject cubeLabel = label.findObject("IsisCube");
    PvlGroup dimensions = cubeLabel.findObject("Core").findGroup("Dimensions");
    testCube.setDimensions(dimensions["Samples"],
                          dimensions["Lines"],
                          dimensions["Bands"]);
    testCube.create(tempFile.fileName());

    for (auto grpIt = cubeLabel.beginGroup(); grpIt!= cubeLabel.endGroup(); grpIt++) {
      testCube.putGroup(*grpIt);
    }
  }

  void TestCube::createCube(Pvl &label, nlohmann::json &isd) {
    createCube(label);
    testCube.attachSpiceFromIsd(isd);
    
    // close cube to flush and write tables
    testCube.close();
    testCube.open(tempFile.fileName()+".cub");
  }

}
