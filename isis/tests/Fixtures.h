#ifndef Fixtures_h
#define Fixtures_h

#include "gtest/gtest.h"

#include <string>

#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include <nlohmann/json.hpp>

#include "Cube.h"
#include "IException.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "ControlNet.h"
#include "FileList.h"
#include "FileName.h"

using json = nlohmann::json;

namespace Isis {

  class TempTestingFiles : public ::testing::Test {
    protected:
      QTemporaryDir tempDir;

      void SetUp() override;
  };



  class SmallCube : public TempTestingFiles {
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


  class DefaultCube : public TempTestingFiles {
    protected:
      Cube *testCube;
      Cube *projTestCube;

      Pvl label;
      Pvl projLabel;
      json isd;

      void SetUp() override;
      void TearDown() override;
  };

  class LineScannerCube : public TempTestingFiles {
    protected:
      Cube *testCube;
      Cube *projTestCube;

      Pvl label;
      Pvl projLabel;
      json isd;

      void SetUp() override;
      void TearDown() override;
  };

  class Hayabusa2OncTSmallCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      Pvl label;

      void SetUp() override;
      void TearDown() override;
      void resetCube();
  };

  class Hayabusa2OncTCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      Pvl label;

      void SetUp() override;
      void TearDown() override;
  };

  class ThreeImageNetwork : public TempTestingFiles {
    protected:

      ControlNet *network;

      Cube *cube1;
      Cube *cube2;
      Cube *cube3;
      
      FileName *isdPath1;
      FileName *isdPath2;
      FileName *isdPath3; 

      FileName *threeImageOverlapFile;
      FileName *twoImageOverlapFile;

      FileList *cubeList;
      QString cubeListFile;

      void SetUp() override;
      void TearDown() override;
  };

  class Hayabusa2OncW2Cube : public DefaultCube {
    protected:
      void setInstrument(QString ikid, QString instrumentId, QString spacecraftName); 
  };


}

#endif
