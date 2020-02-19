#include "Fixtures.h"

namespace Isis {


  void DefaultCube::SetUp() {
    tempFile.setFileTemplate(testTempDir.path() + "/XXXXXX.cub");

    // Open the file to generate its filename
    tempFile.open();

    std::ifstream isdFile("data/defaultImage/defaultCube.isd");
    std::ifstream cubeLabel("data/defaultImage/defaultCube.pvl");
    isdFile >> isd;
    cubeLabel >> label;

    testCube = new Cube();
    testCube->fromIsd(tempFile.fileName(), label, isd, "rw");
  }


  void DefaultCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }
  }


  void ThreeImageNetwork::SetUp() {
    cubeTempPath1.setFileTemplate(testTempDir.path() + "/XXXXXX.cub");
    cubeTempPath2.setFileTemplate(testTempDir.path() + "/XXXXXX.cub");
    cubeTempPath3.setFileTemplate(testTempDir.path() + "/XXXXXX.cub");
    cubeListTempPath.setFileTemplate(testTempDir.path() + "/XXXXXX.lis");

    // Open the files to generate their filenames
    cubeTempPath1.open();
    cubeTempPath2.open();
    cubeTempPath3.open();
    cubeListTempPath.open();

    FileName labelPath1("data/threeImageNetwork/cube1.pvl");
    FileName labelPath2("data/threeImageNetwork/cube2.pvl");
    FileName labelPath3("data/threeImageNetwork/cube3.pvl");

    FileName isdPath1("data/threeImageNetwork/cube1.isd");
    FileName isdPath2("data/threeImageNetwork/cube2.isd");
    FileName isdPath3("data/threeImageNetwork/cube3.isd");

    cube1 = new Cube();
    cube1->fromIsd(cubeTempPath1.fileName(), labelPath1, isdPath1, "rw");

    cube2 = new Cube();
    cube2->fromIsd(cubeTempPath2.fileName(), labelPath2, isdPath2, "rw");

    cube3 = new Cube();
    cube3->fromIsd(cubeTempPath3.fileName(), labelPath3, isdPath3, "rw");

    cubeList = new FileList();
    cubeList->append(cube1->fileName());
    cubeList->append(cube2->fileName());
    cubeList->append(cube3->fileName());

    cubeList->write(cubeListTempPath.fileName());

    network = new ControlNet();
    network->ReadControl("data/threeImageNetwork/controlnetwork.net");
  }


  void ThreeImageNetwork::TearDown() {
    delete cube1;
    delete cube2;
    delete cube3;

    delete cubeList;
    delete network;

  }
}
