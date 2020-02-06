#ifndef Fixtures_h
#define Fixtures_h

#include "gtest/gtest.h"

#include <string>

#include <QString>
#include <QTemporaryFile>

#include <nlohmann/json.hpp>

#include "IException.h"
#include "PvlGroup.h"

#include "Cube.h"
#include "Pvl.h"
#include "PvlObject.h"

namespace Isis {

   class TestCube : public ::testing::Test {
      protected:
        Cube testCube;
        QTemporaryFile tempFile;
        
        void SetUp() override; 
        void TearDown() override;
        void createCube(Pvl &label);
        void createCube(Pvl &label, nlohmann::json &isd); 
   };

}

#endif
