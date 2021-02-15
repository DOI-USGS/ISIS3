#include <iostream>

#include "TestCsmPlugin.h"
#include "TestCsmModel.h"
#include "AlternativeTestCsmModel.h"

// Static Instance of itself
const TestCsmPlugin TestCsmPlugin::m_registeredPlugin;

// Declaration of static variables
const std::string TestCsmPlugin::PLUGIN_NAME = "TestCsmPlugin";
const std::string TestCsmPlugin::MANUFACTURER_NAME = "TestCsmPluginCreator";
const std::string TestCsmPlugin::RELEASE_DATE = "20201208";
const int TestCsmPlugin::N_SENSOR_MODELS = 2;

/**
 * Default constructor
 */
TestCsmPlugin::TestCsmPlugin() {
  // deliberately blank for testing
}


/**
 * Default destructor
 */
TestCsmPlugin::~TestCsmPlugin() {}


/**
 * Gets the name of the plugin.
 *
 * @return std::string name of the plugin
 */
std::string TestCsmPlugin::getPluginName() const {
  return TestCsmPlugin::PLUGIN_NAME;
}


/**
 * Gets the name of the manufacturer of the plugin.
 *
 * @return std::string the name of the manufacturer of the plugin
 */
std::string TestCsmPlugin::getManufacturer() const {
  return TestCsmPlugin::MANUFACTURER_NAME;
}


/**
 * Gets the release date of the plugin.
 *
 * @return std::string release date
 */
std::string TestCsmPlugin::getReleaseDate() const {
  return TestCsmPlugin::RELEASE_DATE;
}


/**
 * Returns the version of CSM the Plugin uses.
 *
 * @return csm::Version CSM version plugin uses
 */
csm::Version TestCsmPlugin::getCsmVersion() const {
  return csm::Version(3,0,3);
}


/**
 * Returns the number of sensor models in the plugin
 *
 * @return size_t Number of sensor models in the plugin
 */
size_t TestCsmPlugin::getNumModels() const {
  return TestCsmPlugin::N_SENSOR_MODELS;
}


/**
 * Returns the model name at the given index.
 *
 * @param modelIndex The index number for the sensor model
 *
 * @return std::string model name
 */
std::string TestCsmPlugin::getModelName(size_t modelIndex) const {
  std::vector<std::string> supportedModelNames = {
    TestCsmModel::SENSOR_MODEL_NAME,
    AlternativeTestCsmModel::SENSOR_MODEL_NAME};
  return supportedModelNames[modelIndex];
}


/**
 * Returns the sensor model family at the given index.
 *
 * @param modelIndex the index number for the sensor model family
 *
 * @return std::string sensor model family
 */
std::string TestCsmPlugin::getModelFamily(size_t modelIndex) const {
  return "TestModelFamily";
}


/**
 * Returns the CSM sensor model version for a given model.
 *
 * @param modelName the model name
 *
 * @return csm::Version the version of the csm sensor model
 */
csm::Version TestCsmPlugin::getModelVersion(const std::string& modelName) const {
  return csm::Version(1,0,0);
}


/**
 * Tests if the sensor model can be created from a given state.
 *
 * @param modelName the model name
 * @param modelState the model state
 * @param warnings the warning list
 *
 * @return bool if the model can be constructed from the state
 */
bool TestCsmPlugin::canModelBeConstructedFromState(const std::string& modelName,
                                    const std::string& modelState,
                                    csm::WarningList* warnings) const {
  try {
    csm::Model *model = constructModelFromState(modelState, warnings);
    return static_cast<bool>(model);
  }
  catch (std::exception &e) {
    // No op
  }
  return false;
}


/**
 * Checks to see if the CSM sensor model can be constructed from
 * a given ISD.
 *
 * @param imageSupportData isd for the image
 * @param modelName name of the sensor model
 * @param warnings warnings list
 *
 * @return bool true if the model can be constructed from the isd
 */
bool TestCsmPlugin::canModelBeConstructedFromISD(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const {
  try {
    csm::Model *model =
        constructModelFromISD(imageSupportData, modelName, warnings);
    return static_cast<bool>(model);
  } catch (std::exception &e) {
    // no op
  }
  return false;
}


/**
 * True if the ISD can be converted to a state.
 *
 * @param imageSupportData the image support data
 * @param modelName the name of the model
 * @param warnings the warnings list
 *
 * @return bool true if the ISD can be converted to a state.
 */
bool TestCsmPlugin::canISDBeConvertedToModelState(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const{
  try {
    convertISDToModelState(imageSupportData, modelName, warnings);
  }
  catch (std::exception &e) {
    // no op
    return false;
  }
  return true;
}


/**
 * Conver an ISD (Image Support Data) to a model state.
 *
 * @param imageSupportData The ISD
 * @param modelName The name of the model.
 * @param warnings The warnings list.
 *
 * @return std::string the model state converted from the ISD
 */
std::string TestCsmPlugin::convertISDToModelState(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const {
  csm::Model *model =
    constructModelFromISD(imageSupportData, modelName, warnings);
  return model->getModelState();
}


/**
 * Extracts and returns the model name from the model state.
 *
 * @param modelState State of the sensor model
 * @param warnings The warnings list
 *
 * @return std::string The sensor model name
 */
std::string TestCsmPlugin::getModelNameFromModelState(
    const std::string& modelState,
    csm::WarningList* warnings) const {

  std::string name = modelState.substr(0, modelState.find("\n"));

  if (name == "") {
    csm::Error::ErrorType aErrorType = csm::Error::INVALID_SENSOR_MODEL_STATE;
    std::string aMessage = "No model_name key in the model state object.";
    std::string aFunction = "TestCsmPlugin::getModelNameFromModelState";
    csm::Error csmErr(aErrorType, aMessage, aFunction);
    throw(csmErr);
  }
  return name;
}


/**
 * Creates and returns a sensor model from a state string.
 *
 * @param modelState State of the sensor model
 * @param warnings The csm warnings list
 *
 * @return csm::Model* The constructed sensor model
 */
csm::Model* TestCsmPlugin::constructModelFromState(
      const std::string& modelState,
      csm::WarningList* warnings) const {

  std::string modelName = getModelNameFromModelState(modelState, warnings);
  if (modelName == TestCsmModel::SENSOR_MODEL_NAME) {
    TestCsmModel *model = new TestCsmModel();
    model->replaceModelState(modelState);
    return model;
  }
  else if (modelName == AlternativeTestCsmModel::SENSOR_MODEL_NAME) {
    AlternativeTestCsmModel *model = new AlternativeTestCsmModel();
    model->replaceModelState(modelState);
    return model;
  }
  else {
    csm::Error::ErrorType errorType = csm::Error::SENSOR_MODEL_NOT_SUPPORTED;
    std::string msg = "TstCsmPlugin failed to construct model from State";
    std::string func = "TestCsmPlugin::constructModelFromState";
    throw csm::Error(errorType, msg, func);
  }
}


/**
 * Constructs and returns a sensor model from an ISD.
 *
 * @param imageSupportData The image support data for an image
 * @param modelName The sensor model name
 *
 * @return csm::Model* the model constructed from the ISD
 */
csm::Model* TestCsmPlugin::constructModelFromISD(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* ) const {
  if (modelName == TestCsmModel::SENSOR_MODEL_NAME) {
    TestCsmModel *model = new TestCsmModel();
    model->replaceModelState(model->constructStateFromIsd(imageSupportData));
    return model;
  }
  else if (modelName == AlternativeTestCsmModel::SENSOR_MODEL_NAME) {
    AlternativeTestCsmModel *model = new AlternativeTestCsmModel();
    model->replaceModelState(model->constructStateFromIsd(imageSupportData));
    return model;
  }
  else {
    csm::Error::ErrorType errorType = csm::Error::SENSOR_MODEL_NOT_SUPPORTED;
    std::string msg = "TstCsmPlugin failed to construct model from ISD";
    std::string func = "TestCsmPlugin::constructModelFromIsd";
    throw csm::Error(errorType, msg, func);
  }
}

