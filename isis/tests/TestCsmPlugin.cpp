#include <iostream>

#include "TestCsmPlugin.h"
#include "TestCsmModel.h"
#include "AlternativeTestCsmModel.h"

// Static Instance of itself
const TestCsmPlugin TestCsmPlugin::m_registeredPlugin;

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
  return "TestCsmPlugin";
}


/**
 * Gets the name of the manufacturer of the plugin.
 * 
 * @return std::string the name of the manufacturer of the plugin
 */
std::string TestCsmPlugin::getManufacturer() const {
  return "TestCsmPluginCreator";
}


/**
 * Gets the release date of the plugin.
 * 
 * @return std::string release date
 */
std::string TestCsmPlugin::getReleaseDate() const {
  return "TestDate";
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
  return 2;
}


/**
 * Returns the model name at the given index.
 * 
 * @param modelIndex The index number for the sensor model
 * 
 * @return std::string model name
 */
std::string TestCsmPlugin::getModelName(size_t modelIndex) const {
  if (modelIndex == 0) {
    return "TestModelName"; 
  }
  else {
    return "AlternativeTestCsmModelName";
  }
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
  return csm::Version(3,0,3);
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
    std::string msg = "Could not create model [";
    msg += modelName;
    msg += "] with error [";
    msg += e.what();
    msg += "]";
    std::cout << msg << std::endl;
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
    std::string msg = "Could not create model [";
    msg += modelName;
    msg += "] with error [";
    msg += e.what();
    msg += "]";
    std::cout << msg << std::endl;
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
  return "ISDConvertedtoModelState";
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
  return "TestModelNameFromModelState";
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
  TestCsmModel *model = new TestCsmModel();
  model->replaceModelState(modelState);
  // Left simple because this isn't needed by current tests.
  return model;
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
  if (modelName == "TestModelName") {
    TestCsmModel *model = new TestCsmModel();
    model->replaceModelState(model->constructStateFromIsd(imageSupportData));
    return model;
  }
  else if (modelName == "AlternativeTestCsmModelName") {
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

