#ifndef CubeFixtures_h
#define CubeFixtures_h

#include "gtest/gtest.h"

#include <memory>
#include <utility>
#include <vector>

#include "Cube.h"
#include "FileList.h"
#include "TempFixtures.h"

namespace Isis {

  class SmallCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };


  class LargeCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };


  class SpecialSmallCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };

  class SmallGapCube : public TempTestingFiles {
    protected:
      Cube *horzCube;
      Cube *vertCube;
      Cube *bandCube;

      void SetUp() override;
      void TearDown() override;
  };


  class NullPixelCube : public TempTestingFiles {
    protected:
      Cube *testCube;
      void SetUp() override;
      void TearDown() override;
  };


  class ApolloCube : public LargeCube {
    protected:
      std::vector<std::pair<int, int>> reseaus;
      int reseauSize;

      void SetUp() override;
  };

  class RingsCube : public TempTestingFiles {
    protected:

      // pixtures of Saturn's rings
      Cube *ring1;
      Cube *ring2;
      FileList cubeFileList;
      QString cubeListPath;

      void SetUp() override;
  };

}

#endif