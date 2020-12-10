#include "csminit.h"

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
      isd["test_param_one"] = 1.0;
      isd["test_param_two"] = 2.0;

      isdPath = tempDir.path() + "/default.json";
      std::ofstream file(isdPath.toStdString());
      file << isd;
      file.flush();

      json altIsd;
      altIsd["test_param_one"] = 1.0;
      altIsd["test_param_two"] = 2.0;
      altIsd["test_param_three"] = 3.0;
      altIsd["test_param_four"] = 4.0;

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

      plugin = csm::Plugin::findPlugin(TestCsmPlugin::PLUGIN_NAME);
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
  EXPECT_TRUE(plugin->canModelBeConstructedFromState(modelName, stateString.string()));

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
  ASSERT_EQ(infoGroup["ModelParameterNames"].size(), 2);
  EXPECT_EQ(infoGroup["ModelParameterNames"][0].toStdString(), TestCsmModel::PARAM_NAMES[0]);
  EXPECT_EQ(infoGroup["ModelParameterNames"][1].toStdString(), TestCsmModel::PARAM_NAMES[1]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterUnits"));
  ASSERT_EQ(infoGroup["ModelParameterUnits"].size(), 2);
  EXPECT_EQ(infoGroup["ModelParameterUnits"][0].toStdString(), TestCsmModel::PARAM_UNITS[0]);
  EXPECT_EQ(infoGroup["ModelParameterUnits"][1].toStdString(), TestCsmModel::PARAM_UNITS[1]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterTypes"));
  ASSERT_EQ(infoGroup["ModelParameterTypes"].size(), 2);
  EXPECT_EQ(infoGroup["ModelParameterTypes"][0].toStdString(), "FICTITIOUS");
  EXPECT_EQ(infoGroup["ModelParameterTypes"][1].toStdString(), "REAL");

  // Check the Kernels group
  ASSERT_TRUE(testCube->hasGroup("Kernels"));
  PvlGroup &kernGroup = testCube->group("Kernels");
  EXPECT_TRUE(kernGroup.hasKeyword("ShapeModel"));
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
    "modelName="+QString::fromStdString(AlternativeTestCsmModel::SENSOR_MODEL_NAME)};

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
  EXPECT_TRUE(plugin->canModelBeConstructedFromState(modelName, stateString.string()));

  // Make sure there is only one CSMState Blob on the label
  PvlObject *label = testCube->label();
  label->deleteObject("String");
  EXPECT_FALSE(label->hasObject("String"));

  // Check that there is only one CsmInfo group
  ASSERT_TRUE(testCube->hasGroup("CsmInfo"));
  testCube->deleteGroup("CsmInfo");
  ASSERT_FALSE(testCube->hasGroup("CsmInfo"));

  // Check that there is only one ShapeModel
  ASSERT_TRUE(testCube->hasGroup("Kernels"));
  PvlGroup &kernGroup = testCube->group("Kernels");
  EXPECT_TRUE(kernGroup.hasKeyword("ShapeModel"));
  kernGroup.deleteKeyword("ShapeModel");
  EXPECT_FALSE(kernGroup.hasKeyword("ShapeModel"));
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
    "modelName="+QString::fromStdString(AlternativeTestCsmModel::SENSOR_MODEL_NAME)};

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
  EXPECT_TRUE(plugin->canModelBeConstructedFromState(modelName, stateString.string()));

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
  ASSERT_EQ(infoGroup["ModelParameterNames"].size(), 3);
  EXPECT_EQ(infoGroup["ModelParameterNames"][0].toStdString(), AlternativeTestCsmModel::PARAM_NAMES[0]);
  EXPECT_EQ(infoGroup["ModelParameterNames"][1].toStdString(), AlternativeTestCsmModel::PARAM_NAMES[1]);
  EXPECT_EQ(infoGroup["ModelParameterNames"][2].toStdString(), AlternativeTestCsmModel::PARAM_NAMES[2]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterUnits"));
  ASSERT_EQ(infoGroup["ModelParameterUnits"].size(), 3);
  EXPECT_EQ(infoGroup["ModelParameterUnits"][0].toStdString(), AlternativeTestCsmModel::PARAM_UNITS[0]);
  EXPECT_EQ(infoGroup["ModelParameterUnits"][1].toStdString(), AlternativeTestCsmModel::PARAM_UNITS[1]);
  EXPECT_EQ(infoGroup["ModelParameterUnits"][2].toStdString(), AlternativeTestCsmModel::PARAM_UNITS[2]);
  ASSERT_TRUE(infoGroup.hasKeyword("ModelParameterTypes"));
  ASSERT_EQ(infoGroup["ModelParameterTypes"].size(), 3);
  EXPECT_EQ(infoGroup["ModelParameterTypes"][0].toStdString(), "FICTITIOUS");
  EXPECT_EQ(infoGroup["ModelParameterTypes"][1].toStdString(), "REAL");
  EXPECT_EQ(infoGroup["ModelParameterTypes"][2].toStdString(), "FIXED");
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


// This test uses the DefaultCube fixture because it has already has attached
// spice data that csminit should remove.
TEST_F(DefaultCube, CSMinitSpiceCleanup) {
  // Create an ISD
  json isd;
  isd["test_param_one"] = 1.0;
  isd["test_param_two"] = 2.0;

  QString isdPath = tempDir.path() + "/default.json";
  std::ofstream file(isdPath.toStdString());
  file << isd;
  file.flush();

  QVector<QString> args = {
    "from="+testCube->fileName(),
    "isd="+isdPath};
  QString cubeFile = testCube->fileName();

  UserInterface options(APP_XML, args);

  testCube->close();
  csminit(options);
  Cube outputCube(cubeFile);

  EXPECT_FALSE(outputCube.hasTable("InstrumentPointing"));
  EXPECT_FALSE(outputCube.hasTable("InstrumentPosition"));
  EXPECT_FALSE(outputCube.hasTable("BodyRotation"));
  EXPECT_FALSE(outputCube.hasTable("SunPosition"));
  EXPECT_FALSE(outputCube.hasTable("CameraStatistics"));
  ASSERT_TRUE(outputCube.hasGroup("Kernels"));
  EXPECT_EQ(outputCube.group("Kernels").keywords(), 2);
}


// This test uses the DefaultCube fixture because it has already has attached
// spice data that csminit should not remove on a failure.
TEST_F(DefaultCube, CSMinitSpiceNoCleanup) {
  // Create an ISD that will result in no successful models
  json isd;
  isd["test_param_one"] = "value_one";
  isd["test_param_does_not_exist"] = "failing_value";

  QString isdPath = tempDir.path() + "/default.json";
  std::ofstream file(isdPath.toStdString());
  file << isd;
  file.flush();

  QVector<QString> args = {
    "from="+testCube->fileName(),
    "isd="+isdPath};
  QString cubeFile = testCube->fileName();

  UserInterface options(APP_XML, args);

  testCube->close();
  // Expect a failure due to being unable to construct any model from the isd
  EXPECT_ANY_THROW(csminit(options));
  Cube outputCube(cubeFile);

  // The cube should still be intact and we should still be able to get a camera
  EXPECT_NO_THROW(outputCube.camera());
}
