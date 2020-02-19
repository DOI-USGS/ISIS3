#ifndef Fixtures_h
#define Fixtures_h

#include "gtest/gtest.h"

#include <string>

#include <QString>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include <nlohmann/json.hpp>

#include "IException.h"
#include "PvlGroup.h"

#include "Cube.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "ControlNet.h"
#include "FileList.h"

using json = nlohmann::json;

namespace Isis {

  static QTemporaryDir testTempDir;


   class DefaultCube : public ::testing::Test {
      protected:
        Cube *testCube;
        QTemporaryFile tempFile;

        Pvl label;
        json isd;

        void SetUp() override;
        void TearDown() override;
   };


   class ThreeImageNetwork : public ::testing::Test {
      protected:

        ControlNet *network;

        Cube *cube1;
        Cube *cube2;
        Cube *cube3;

        FileList *cubeList;

        QTemporaryFile cubeTempPath1;
        QTemporaryFile cubeTempPath2;
        QTemporaryFile cubeTempPath3;
        QTemporaryFile cubeListTempPath;

        void SetUp() override;
        void TearDown() override;
   };
}

#endif
