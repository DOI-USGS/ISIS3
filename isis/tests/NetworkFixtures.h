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

      ControlNet *network = nullptr;
      QString networkFile;

      Cube *cube1 = nullptr;
      Cube *cube2 = nullptr;
      Cube *cube3 = nullptr;

      Cube *cube1map = nullptr;
      Cube *cube2map = nullptr;
      Cube *cube3map = nullptr;

      FileName *isdPath1 = nullptr;
      FileName *isdPath2 = nullptr;
      FileName *isdPath3 = nullptr;

      FileName *threeImageOverlapFile = nullptr;
      FileName *twoImageOverlapFile = nullptr;

      FileList *cubeList = nullptr;
      QString cubeListFile;
      QString twoCubeListFile;

      // Optional use of FastGeom algorithms
      QString grid_fastgeom_config;
      QString radial_fastgeom_config;

      std::vector<std::vector<double>> coords;

      void SetUp() override;
      void AddFeatures();
      void TearDown() override;
  };

  class ObservationPair : public TempTestingFiles {
    protected:

      Cube *cubeL = nullptr;
      Cube *cubeR = nullptr;

      QString cubeLPath;
      QString cubeRPath;

      FileName *isdPathL = nullptr;
      FileName *isdPathR = nullptr;

      FileList *cubeList = nullptr;
      QString cubeListFile;

      ControlNet *network = nullptr;
      QString cnetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class ApolloNetwork : public TempTestingFiles {
    protected:
      QVector<FileName> isdFiles;
      QVector<FileName> labelFiles;
      QVector<Cube*> cubes;

      FileList *cubeList = nullptr;
      QString cubeListFile;

      ControlNet *network = nullptr;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

    class LidarObservationPair : public TempTestingFiles {
    protected:

      Cube *cube1 = nullptr;
      Cube *cube2 = nullptr;

      QString cube1Path;
      QString cube2Path;

      FileName *isdPath1 = nullptr;
      FileName *isdPath2 = nullptr;

      FileList *cubeList = nullptr;
      QString cubeListFile;

      QString csvPath;

      void SetUp() override;
      void TearDown() override;
  };

  class LidarNetwork : public LidarObservationPair {
    protected:

      LidarData rangeData;
      QString lidarDataPath;

      ControlNet *network = nullptr;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class MiniRFNetwork : public TempTestingFiles {
    protected:
      Cube *testCube1 = nullptr;
      Cube *testCube2 = nullptr;
      Cube *testCube3 = nullptr;

      FileList *cubeList = nullptr;
      QString cubeListFile;

      ControlNet *network = nullptr;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class VikThmNetwork : public TempTestingFiles {
    protected:
      Cube *testCube1 = nullptr;
      Cube *testCube2 = nullptr;
      Cube *testCube3 = nullptr;
      Cube *testCube4 = nullptr;

      FileList *cubeList = nullptr;
      QString cubeListFile;

      ControlNet *network = nullptr;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

}

#endif