#include "Fixtures.h"

namespace Isis {

  void DefaultCube::SetUp() {
    std::ifstream isdFile("data/defaultImage/defaultCube.isd");
    std::ifstream cubeLabel("data/defaultImage/defaultCube.pvl");
    isdFile >> isd;
    cubeLabel >> label;

    testCube = new Cube();
    testCube->fromIsd(tempFile.fileName() + ".cub", label, isd, "rw");
  }


  void DefaultCube::TearDown() {
    if (testCube->isOpen()) {
      testCube->close();
    }
  }


  void ThreeImageNetwork::SetUp() {
    FileName labelPath1("data/threeImageNetwork/cube1.pvl");
    FileName labelPath2("data/threeImageNetwork/cube2.pvl");
    FileName labelPath3("data/threeImageNetwork/cube3.pvl");

    FileName isdPath1("data/threeImageNetwork/cube1.isd");
    FileName isdPath2("data/threeImageNetwork/cube2.isd");
    FileName isdPath3("data/threeImageNetwork/cube3.isd");

    threeImageOverlapFile = new FileName("data/threeImageNetwork/threeImageOverlaps.lis");
    twoImageOverlapFile = new FileName("data/threeImageNetwork/twoImageOverlaps.lis");

    cube1 = new Cube();
    cubeTempPath1.open();
    cube1->fromIsd(cubeTempPath1.fileName() + ".cub", labelPath1, isdPath1, "rw");

    cube2 = new Cube();
    cubeTempPath2.open();
    cube2->fromIsd(cubeTempPath2.fileName() + ".cub", labelPath2, isdPath2, "rw");

    cube3 = new Cube();
    cubeTempPath3.open();
    cube3->fromIsd(cubeTempPath3.fileName() + ".cub", labelPath3, isdPath3, "rw");

    cubeList = new FileList();
    cubeList->append(cube1->fileName());
    cubeList->append(cube2->fileName());
    cubeList->append(cube3->fileName());

    cubeListTempPath.open();
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
