#include "csminit.h"
#include "spiceinit.h"

#include <QString>
#include <iostream>

#include "AlternativeTestCsmModel.h"
#include "TestCsmPlugin.h"
#include "Fixtures.h"
#include "TestUtilities.h"
#include "TestCsmModel.h"
#include "StringBlob.h"
#include "FileName.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/csminit.xml").expanded();

class CSMPluginFixture : public TempTestingFiles {
  protected:
    const csm::Plugin *plugin;
    Cube *testCube;
    Pvl label;
    QString isdPath;
    QString altIsdPath;
    QString filename;
    TestCsmModel model;
    AlternativeTestCsmModel altModel;

    void SetUp() override {
      TempTestingFiles::SetUp();

      // Create and populate test ISDs
      json isd;
      isd["name"] = "test_isd";
      isd["test_param_one"] = "value_one";
      isd["test_param_two"] = "value_two";

      isdPath = tempDir.path() + "/default.json";
      std::ofstream file(isdPath.toStdString());
      file << isd;
      file.flush();

      json altIsd;
      altIsd["name"] = "test_isd";
      altIsd["test_param_one"] = "value_one";
      altIsd["test_param_two"] = "value_two";
      altIsd["test_param_three"] = "value_three";

      altIsdPath = tempDir.path() + "/alternate.json";
      std::ofstream altFile(altIsdPath.toStdString());
      altFile << altIsd;
      altFile.flush();

      std::ifstream cubeLabel("data/threeImageNetwork/cube1.pvl");
      cubeLabel >> label;
      testCube = new Cube();
      filename = tempDir.path() + "/csminitCube.cub";
      testCube->fromLabel(filename, label, "rw");
      testCube->close();

      plugin = csm::Plugin::findPlugin("TestCsmPlugin");
    }

    void TearDown() override {
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

  // Check that the plugin can create a model from the state string
  std::string modelName = QString(blobPvl.findKeyword("ModelName")).toStdString();
  EXPECT_TRUE(plugin->canModelBeConstructedFromState(stateString.string(), modelName));

  // Check blob label ModelName and Plugin Name
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), plugin->getPluginName());
  EXPECT_EQ(modelName, TestCsmModel::SENSOR_MODEL_NAME);

  // Check the CsmInfo group
  ASSERT_TRUE(testCube->hasGroup("CsmInfo"));
  PvlGroup &infoGroup = testCube->group("CsmInfo");
  ASSERT_TRUE(infoGroup.hasKeyword("CSMPlatformID"));
  EXPECT_EQ(infoGroup["CSMPlatformID"][0].toStdString(), model.getPlatformIdentifier());
  ASSERT_TRUE(infoGroup.hasKeyword("CSMInstrumentId"));
  EXPECT_EQ(infoGroup["CSMInstrumentId"][0].toStdString(), model.getSensorIdentifier());
  ASSERT_TRUE(infoGroup.hasKeyword("ReferenceTime"));
  EXPECT_EQ(infoGroup["ReferenceTime"][0].toStdString(), model.getReferenceDateAndTime());
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterNames"));
  ASSERT_EQ(infoGroup["ModelParameterNames"].size(), 3);
  ASSERT_EQ(infoGroup["ModelParameterNames"][0].toStdString(), TestCsmModel::PARAM_NAMES[0]);
  ASSERT_EQ(infoGroup["ModelParameterNames"][1].toStdString(), TestCsmModel::PARAM_NAMES[1]);
  ASSERT_EQ(infoGroup["ModelParameterNames"][2].toStdString(), TestCsmModel::PARAM_NAMES[2]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterUnits"));
  ASSERT_EQ(infoGroup["ModelParameterUnits"].size(), 3);
  ASSERT_EQ(infoGroup["ModelParameterUnits"][0].toStdString(), TestCsmModel::PARAM_UNITS[0]);
  ASSERT_EQ(infoGroup["ModelParameterUnits"][1].toStdString(), TestCsmModel::PARAM_UNITS[1]);
  ASSERT_EQ(infoGroup["ModelParameterUnits"][2].toStdString(), TestCsmModel::PARAM_UNITS[2]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterTypes"));
  ASSERT_EQ(infoGroup["ModelParameterTypes"].size(), 3);
  ASSERT_EQ(infoGroup["ModelParameterTypes"][0].toStdString(), "FICTITIOUS");
  ASSERT_EQ(infoGroup["ModelParameterTypes"][1].toStdString(), "REAL");
  ASSERT_EQ(infoGroup["ModelParameterTypes"][2].toStdString(), "FIXED");
}

TEST_F(CSMPluginFixture, CSMinitRunTwice) {
  // Run csminit twice in a row. Verify that end result is as if csminit were
  // only run once with the second arguments.
  QVector<QString> args = {
    "from="+filename,
    "isd="+isdPath};

  UserInterface options(APP_XML, args);

  csminit(options);

  QVector<QString> altArgs = {
    "from="+filename,
    "isd="+altIsdPath,
    "modelName=AlternativeTestCsmModelName"};

  UserInterface altOptions(APP_XML, altArgs);

  csminit(altOptions);

  testCube->open(filename);

  StringBlob stateString("", "CSMState");
  testCube->read(stateString);
  PvlObject blobPvl = stateString.Label();

  // Verify contents of the StringBlob's PVL label
  EXPECT_EQ(stateString.Name().toStdString(), "CSMState");
  EXPECT_EQ(stateString.Type().toStdString(), "String");

  // Check blob label ModelName and Plugin Name
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), plugin->getPluginName());
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), AlternativeTestCsmModel::SENSOR_MODEL_NAME);

  // Check that the plugin can create a model from the state string
  std::string modelName = QString(blobPvl.findKeyword("ModelName")).toStdString();
  EXPECT_TRUE(plugin->canModelBeConstructedFromState(stateString.string(), modelName));

  // Make sure there is only one CSMState Blob on the label
  PvlObject *label = testCube->label();
  label->deleteObject("String");
  EXPECT_FALSE(label->hasObject("String"));

  // Check that there is only one CsmInfo group
  ASSERT_TRUE(testCube->hasGroup("CsmInfo"));
  testCube->deleteGroup("CsmInfo");
  ASSERT_FALSE(testCube->hasGroup("CsmInfo"));
}

TEST_F(CSMPluginFixture, CSMinitMultiplePossibleModels) {
  // Test csminit when multiple possible models can be created. First, verify that this will fail
  // without specifying the MODELNAME, then specify the MODELNAME and check the results.

  QVector<QString> args = {
    "from="+filename,
    "isd="+altIsdPath};

  UserInterface options(APP_XML, args);

  // If there are two possible models, csminit will fail and prompt the user to specify the model
  // and/or plugin name.
  EXPECT_ANY_THROW(csminit(options));

  // Re-run with the model name specified
  args = {
    "from="+filename,
    "isd="+altIsdPath,
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

  // Check that the plugin can create a model from the state string
  std::string modelName = QString(blobPvl.findKeyword("ModelName")).toStdString();
  EXPECT_TRUE(plugin->canModelBeConstructedFromState(stateString.string(), modelName));

  // check blob label ModelName and Plugin Name
  EXPECT_EQ(QString(blobPvl.findKeyword("PluginName")).toStdString(), plugin->getPluginName());
  EXPECT_EQ(QString(blobPvl.findKeyword("ModelName")).toStdString(), AlternativeTestCsmModel::SENSOR_MODEL_NAME);

  // Check the CsmInfo group
  ASSERT_TRUE(testCube->hasGroup("CsmInfo"));
  PvlGroup &infoGroup = testCube->group("CsmInfo");
  ASSERT_TRUE(infoGroup.hasKeyword("CSMPlatformID"));
  EXPECT_EQ(infoGroup["CSMPlatformID"][0].toStdString(), altModel.getPlatformIdentifier());
  ASSERT_TRUE(infoGroup.hasKeyword("CSMInstrumentId"));
  EXPECT_EQ(infoGroup["CSMInstrumentId"][0].toStdString(), altModel.getSensorIdentifier());
  ASSERT_TRUE(infoGroup.hasKeyword("ReferenceTime"));
  EXPECT_EQ(infoGroup["ReferenceTime"][0].toStdString(), altModel.getReferenceDateAndTime());
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterNames"));
  ASSERT_EQ(infoGroup["ModelParameterNames"].size(), 2);
  ASSERT_EQ(infoGroup["ModelParameterNames"][0].toStdString(), AlternativeTestCsmModel::PARAM_NAMES[0]);
  ASSERT_EQ(infoGroup["ModelParameterNames"][1].toStdString(), AlternativeTestCsmModel::PARAM_NAMES[1]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterUnits"));
  ASSERT_EQ(infoGroup["ModelParameterUnits"].size(), 2);
  ASSERT_EQ(infoGroup["ModelParameterUnits"][0].toStdString(), AlternativeTestCsmModel::PARAM_UNITS[0]);
  ASSERT_EQ(infoGroup["ModelParameterUnits"][1].toStdString(), AlternativeTestCsmModel::PARAM_UNITS[1]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterTypes"));
  ASSERT_EQ(infoGroup["ModelParameterTypes"].size(), 2);
  ASSERT_EQ(infoGroup["ModelParameterTypes"][0].toStdString(), "FICTITIOUS");
  ASSERT_EQ(infoGroup["ModelParameterTypes"][1].toStdString(), "REAL");
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
