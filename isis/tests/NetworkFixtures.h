#ifndef NetworkFixtures_h
#define NetworkFixtures_h

#include <QString>

#include "ControlNet.h"
#include "Cube.h"
#include "FileList.h"
#include "FileName.h"
#include "LidarData.h"
#include "TempFixtures.h"

namespace Isis {

  class ThreeImageNetwork : public TempTestingFiles {
    protected:

      ControlNet *network;
      QString networkFile;

      Cube *cube1;
      Cube *cube2;
      Cube *cube3;

      Cube *cube1map;
      Cube *cube2map;
      Cube *cube3map;

      FileName *isdPath1;
      FileName *isdPath2;
      FileName *isdPath3;

      FileName *threeImageOverlapFile;
      FileName *twoImageOverlapFile;

      FileList *cubeList;
      QString cubeListFile;
      QString twoCubeListFile;

      std::vector<std::vector<double>> coords;

      void SetUp() override;
      void AddFeatures();
      void TearDown() override;
  };

  class ObservationPair : public TempTestingFiles {
    protected:

      Cube *cubeL;
      Cube *cubeR;

      QString cubeLPath;
      QString cubeRPath;

      FileName *isdPathL;
      FileName *isdPathR;

      FileList *cubeList;
      QString cubeListFile;

      ControlNet *network;
      QString cnetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class ApolloNetwork : public TempTestingFiles {
    protected:
      QVector<FileName> isdFiles;
      QVector<FileName> labelFiles;
      QVector<Cube*> cubes;

      FileList *cubeList;
      QString cubeListFile;

      ControlNet *network;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

    class LidarObservationPair : public TempTestingFiles {
    protected:

      Cube *cube1;
      Cube *cube2;

      QString cube1Path;
      QString cube2Path;

      FileName *isdPath1;
      FileName *isdPath2;

      FileList *cubeList;
      QString cubeListFile;

      QString csvPath;

      void SetUp() override;
      void TearDown() override;
  };

  class LidarNetwork : public LidarObservationPair {
    protected:

      LidarData rangeData;
      QString lidarDataPath;

      ControlNet *network;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class MiniRFNetwork : public TempTestingFiles {
    protected:
      Cube *testCube1;
      Cube *testCube2;
      Cube *testCube3;

      FileList *cubeList;
      QString cubeListFile;

      ControlNet *network;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class VikThmNetwork : public TempTestingFiles {
    protected:
      Cube *testCube1;
      Cube *testCube2;
      Cube *testCube3;
      Cube *testCube4;

      FileList *cubeList;
      QString cubeListFile;

      ControlNet *network;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

}

#endif