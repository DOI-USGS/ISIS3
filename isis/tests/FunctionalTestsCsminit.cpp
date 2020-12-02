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
      QString filename;

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
        file.flush();

        std::ifstream cubeLabel("data/threeImageNetwork/cube1.pvl");
        cubeLabel >> label;
        testCube = new Cube();
        filename = tempDir.path() + "/csminitCube.cub";
        testCube->fromLabel(filename, label, "rw");
        testCube->close();

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
              // Sometimes end up with mulitple copies of the same plugin. If so, remove it so 
              // it doesn't get considered "2 plugins" by csminit, which causes it to fail.
              alreadyFound = true;
            }
          }
        }
      }

      void TearDown() override {
        if (plugin) {
          plugin->removePlugin(plugin->getPluginName());
          delete plugin;
          plugin = NULL;
        }
        if (testCube) {
          if (testCube->isOpen()) {
            testCube->close();
          }
          delete testCube;
          testCube = NULL;
        }
      }
};

TEST_F(CSMPluginFixture, CSMInitDefault) {
  // Run csminit with defaults for everything besides FROM and ISD
  QVector<QString> args = {
    "from="+filename,
    "isd="+isdPath
  };

  UserInterface options(APP_XML, args);
  csminit(options);

  testCube->open(filename);

  // Get a model and a state string
  StringBlob stateString("","CSMState");
  testCube->read(stateString);

  // Verify contents of the StringBlob's PVL label
  PvlObject blobPvl = stateString.Label(); 
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String"); 

  // Intent is just to make sure the state string exists and is a "reasonable" length
  EXPECT_GT(stateString.string().length(), 20);

  // Check blob label ModelName and Plugin Name 
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), "TestCsmPlugin");
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), "TestCsmModelName");
}

TEST_F(CSMPluginFixture, CSMinitRunTwice) {
  // Run csminit twice in a row with the same options each time. Verify that end result is 
  // as if csminit were only run once. 
  QVector<QString> args = {
    "from="+filename,
    "isd="+isdPath};

  UserInterface options(APP_XML, args);

  csminit(options);
  csminit(options);

  testCube->open(filename);

  StringBlob stateString("", "CSMState");
  testCube->read(stateString);
  PvlObject blobPvl = stateString.Label();

  // Verify contents of the StringBlob's PVL label
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String"); 

  // Intent is just to make sure the state string exists and is a "reasonable" length
  EXPECT_GT(stateString.string().length(), 20);

  // Check blob label ModelName and Plugin Name 
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), "TestCsmPlugin");
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), "TestCsmModelName");
}

TEST_F(CSMPluginFixture, CSMinitMultiplePossibleModels) {
  // Test csminit when multiple possible models can be created. First, verify that this will fail
  // without specifying the MODELNAME, then specify the MODELNAME and check the results. 

  // Create a test ISD could work for 2 models
  json isd;
  isd["name"] = "test_isd";
  isd["test_param_one"] = "value_one";
  isd["test_param_two"] = "value_two";
  isd["test_param_three"] = "value_three";

  QString isdPath = tempDir.path() + "/multimodel.json";
  std::ofstream file(isdPath.toStdString());
  file << isd;
  file.flush();

  QVector<QString> args = {
    "from="+filename,
    "isd="+isdPath};

  UserInterface options(APP_XML, args);

  // If there are two possible models, csminit will fail and prompt the user to specify the model
  // and/or plugin name. 
  EXPECT_ANY_THROW(csminit(options));

  // Re-run with the model name specified
  args = {
    "from="+filename,
    "isd="+isdPath,
    "modelName=AlternativeTestCsmModelName"};
  
  UserInterface betterOptions(APP_XML, args);
  csminit(betterOptions);

  testCube->open(filename);
  StringBlob stateString("", "CSMState");
  testCube->read(stateString);
  PvlObject blobPvl = stateString.Label(); 

  // Check blobPvl contents
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String"); 

  // Intent is to check that the state string exists and is a "reasonable" length
  EXPECT_GT(stateString.string().length(), 20);

  // check blob label ModelName and Plugin Name 
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), "TestCsmPlugin");
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), "AlternativeTestCsmModelName");
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
  file.flush();

  QVector<QString> args = {
    "from="+filename,
    "isd="+isdPath};

  UserInterface options(APP_XML, args);
  
  // Expect a failure due to being unable to construct any model from the isd
  EXPECT_ANY_THROW(csminit(options));
}
