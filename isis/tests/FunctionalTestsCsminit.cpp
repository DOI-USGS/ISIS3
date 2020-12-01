#include "csminit.h"
#include "spiceinit.h"

#include <QString>
#include <iostream>

#include "TestCsmPlugin.h"
#include "Fixtures.h"
#include "TestUtilities.h"
#include "StringBlob.h"
#include "FileName.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/csminit.xml").expanded();

class CSMPluginFixture : public TempTestingFiles {
  protected:
      TestCsmPlugin *plugin;
      Cube *testCube;
      Pvl label;
      QString isdPath;

      void SetUp() override {
        TempTestingFiles::SetUp();

        // Create and populate default test ISD
        json isd;
        isd["name"] = "test_isd";
        isd["test_param_one"] = "value_one";
        isd["test_param_two"] = "value_two";

        isdPath = tempDir.path() + "/default.json";
        std::ofstream file(isdPath.toStdString());
        file << isd;
//        std::ifstream cubeLabel("/scratch/csm2020-3/ISIS3/build/CTXTestLabel.pvl");
//        cubeLabel >> label;
        testCube = new Cube("/scratch/csm2020-3/jesse_test_data/test_data/F02_036648_2021_XN_22N022W.cub");
//        testCube->fromLabel(tempDir.path() + "/default.cub", label, "rw");
        plugin = new TestCsmPlugin();
        csm::PluginList pluginList = plugin->getList();
        bool alreadyFound = false;
        for (auto const& loadedPlugin : pluginList) {
          if (loadedPlugin) {
            if ( (loadedPlugin->getPluginName() != "TestCsmPlugin") || alreadyFound) {
              // Note: documentation says explicitly not to do this, but I'm doing it
              // anyway for testing. See pg. 39 API documentation version 3.0.3
              plugin->removePlugin(loadedPlugin->getPluginName());
            }
            else {
              alreadyFound = true;
            }
          }
        }
      }

      void TearDown() override {
        if (plugin) {
          plugin->removePlugin(plugin->getPluginName());
          delete plugin;
        }
      }
};

TEST_F(CSMPluginFixture, CSMInitDefault) {
  QVector<QString> args = {
    "from="+testCube->fileName(),
    "isd="+isdPath
  };

  UserInterface options(APP_XML, args);
  csminit(options);

  // Get a model and a state string
  StringBlob stateString("","CSMState");
  testCube->read(stateString);

  PvlObject blobPvl = stateString.Label(); 
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String"); 

  // Better test for state string
  EXPECT_GT(stateString.string().length(), 20);

  // check blob label ModelName and Plugin Name 
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), "TestCsmPlugin");
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), "TestCsmModelName");
}

TEST_F(CSMPluginFixture, CSMinitRunTwice) {
  // Run csminit twice in a row.
  QVector<QString> args = {
    "from="+testCube->fileName(),
    "isd="+isdPath};

  UserInterface options(APP_XML, args);

  csminit(options);
  csminit(options);

  StringBlob stateString("", "CSMState");
  Cube cub("/scratch/csm2020-3/jesse_test_data/test_data/F02_036648_2021_XN_22N022W.cub");
  cub.read(stateString);
  PvlObject blobPvl = stateString.Label(); 
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String"); 

  // Better test for state string
  EXPECT_GT(stateString.string().length(), 20);

  // check blob label ModelName and Plugin Name 
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), "TestCsmPlugin");
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), "TestCsmModelName");
}

TEST_F(CSMPluginFixture, CSMinitMultiplePossibleModels) {
  // Create a test ISD that works for 2 models
  json isd;
  isd["name"] = "test_isd";
  isd["test_param_one"] = "value_one";
  isd["test_param_two"] = "value_two";
  isd["test_param_three"] = "value_three";

  isdPath = tempDir.path() + "/multimodel.json";
  std::ofstream file(isdPath.toStdString());
  file << isd;

  QVector<QString> args = {
    "from="+testCube->fileName(),
    "isd="+isdPath};

  UserInterface options(APP_XML, args);

  EXPECT_ANY_THROW(csminit(options));

  args = {
    "from="+testCube->fileName(),
    "isd="+isdPath,
    "modelName=AlternativeTestCsmModelName"};
  
  UserInterface betterOptions(APP_XML, args);
  csminit(betterOptions);

  StringBlob stateString("", "CSMState");
  testCube->read(stateString);
  PvlObject blobPvl = stateString.Label(); 
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String"); 

  // Better test for state string
  EXPECT_GT(stateString.string().length(), 20);

  // check blob label ModelName and Plugin Name 
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), "TestCsmPlugin");
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), "TestCsmModelName");
}


TEST_F(CSMPluginFixture, CSMinitFails) {
  // Create an ISD that will result in no successful models
  json isd;
  isd["name"] = "failing_isd";
  isd["test_param_one"] = "value_one";
  isd["test_param_does_not_exist"] = "failing_value";

  isdPath = tempDir.path() + "/failing.json";
  std::ofstream file(isdPath.toStdString());
  file << isd;

  QVector<QString> args = {
    "from="+testCube->fileName(),
    "isd="+isdPath};

  UserInterface options(APP_XML, args);

  EXPECT_ANY_THROW(csminit(options));
}
