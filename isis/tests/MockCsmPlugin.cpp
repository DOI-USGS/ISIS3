#include <iostream>

#include "MockCsmPlugin.h"

// Static Instance of itself
const MockCsmPlugin MockCsmPlugin::m_registeredPlugin;

// Declaration of static variables
const std::string MockCsmPlugin::PLUGIN_NAME = "MockCsmPlugin";
const std::string MockCsmPlugin::MANUFACTURER_NAME = "MockCsmPluginCreator";
const std::string MockCsmPlugin::RELEASE_DATE = "20210201";
QMap<std::string, csm::Model*> MockCsmPlugin::m_registeredModels = QMap<std::string, csm::Model*>();

/**
 * Default constructor
 */
MockCsmPlugin::MockCsmPlugin() {
  // deliberately blank for testing
}


/**
 * Default destructor
 */
MockCsmPlugin::~MockCsmPlugin() {}


/**
 * Gets the name of the plugin.
 *
 * @return std::string name of the plugin
 */
std::string MockCsmPlugin::getPluginName() const {
  return MockCsmPlugin::PLUGIN_NAME;
}


/**
 * Gets the name of the manufacturer of the plugin.
 *
 * @return std::string the name of the manufacturer of the plugin
 */
std::string MockCsmPlugin::getManufacturer() const {
  return MockCsmPlugin::MANUFACTURER_NAME;
}


/**
 * Gets the release date of the plugin.
 *
 * @return std::string release date
 */
std::string MockCsmPlugin::getReleaseDate() const {
  return MockCsmPlugin::RELEASE_DATE;
}


/**
 * Returns the version of CSM the Plugin uses.
 *
 * @return csm::Version CSM version plugin uses
 */
csm::Version MockCsmPlugin::getCsmVersion() const {
  return csm::Version(3,0,3);
}


/**
 * Returns the number of sensor models in the plugin
 *
 * @return size_t Number of sensor models in the plugin
 */
size_t MockCsmPlugin::getNumModels() const {
  // This will change as models are loaded into the plugin,
  // and we cannot access the model registry by index so just say it's empty
  return 0;
}


/**
 * Returns the model name at the given index.
 *
 * @param modelIndex The index number for the sensor model
 *
 * @return std::string model name
 */
std::string MockCsmPlugin::getModelName(size_t modelIndex) const {
  // We cannot access the model registry by index so just return a dummy value
  return "Dummy Model Name";
}


/**
 * Returns the sensor model family at the given index.
 *
 * @param modelIndex the index number for the sensor model family
 *
 * @return std::string sensor model family
 */
std::string MockCsmPlugin::getModelFamily(size_t modelIndex) const {
  // We cannot access the model registry by index so just return a dummy value
  return "TestModelFamily";
}


/**
 * Returns the CSM sensor model version for a given model.
 *
 * @param modelName the model name
 *
 * @return csm::Version the version of the csm sensor model
 */
csm::Version MockCsmPlugin::getModelVersion(const std::string& modelName) const {
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
bool MockCsmPlugin::canModelBeConstructedFromState(const std::string& modelName,
                                    const std::string& modelState,
                                    csm::WarningList* warnings) const {
  return m_registeredModels.contains(modelState);
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
bool MockCsmPlugin::canModelBeConstructedFromISD(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const {
  // Do not do anything with ISDs so that we don't interfere with csminit testing
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
bool MockCsmPlugin::canISDBeConvertedToModelState(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const{
  // Do not do anything with ISDs so that we don't interfere with csminit testing
  return false;
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
std::string MockCsmPlugin::convertISDToModelState(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const {
  // Do not do anything with ISDs so return a dummy value
  return "Dummy model state";
}


/**
 * Extracts and returns the model name from the model state.
 *
 * @param modelState State of the sensor model
 * @param warnings The warnings list
 *
 * @return std::string The sensor model name
 */
std::string MockCsmPlugin::getModelNameFromModelState(
    const std::string& modelState,
    csm::WarningList* warnings) const {
  // The state strings are just the model name so return that
  return modelState;
}


/**
 * Register a new model with the plugin.
 * The model can be accessed by calling constructModelFromState with the modelName
 * as the state string.
 *
 * @param modelName The name of the model to be used to access it
 * @param model The model to register
 */
void MockCsmPlugin::registerModel(std::string modelName, csm::Model* model) {
  m_registeredModels.insert(modelName, model);
}


/**
 * Creates and returns a sensor model from a state string.
 * This will access the internal model registry and return a pointer to the model
 * whose name is the state string. This will also remove the model from the model registry.
 *
 * @param modelState State of the sensor model
 * @param warnings The csm warnings list
 *
 * @return csm::Model* The constructed sensor model
 */
csm::Model* MockCsmPlugin::constructModelFromState(
      const std::string& modelState,
      csm::WarningList* warnings) const {
  // QMap::take will return a default value if the key is not in the map
  // so check that it's there before calling take.
  if (m_registeredModels.contains(modelState)) {
    return m_registeredModels.take(modelState);
  }
  csm::Error::ErrorType errorType = csm::Error::SENSOR_MODEL_NOT_SUPPORTED;
  std::string msg = "MockCsmPlugin failed to construct model from State";
  std::string func = "MockCsmPlugin::constructModelFromState";
  throw csm::Error(errorType, msg, func);
}


/**
 * Constructs and returns a sensor model from an ISD.
 *
 * @param imageSupportData The image support data for an image
 * @param modelName The sensor model name
 *
 * @return csm::Model* the model constructed from the ISD
 */
csm::Model* MockCsmPlugin::constructModelFromISD(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* ) const {
  // Do not do anything with ISDs so that we don't interfere with csminit testing
  csm::Error::ErrorType errorType = csm::Error::SENSOR_MODEL_NOT_SUPPORTED;
  std::string msg = "TstCsmPlugin does not support constructing modeles from ISD";
  std::string func = "MockCsmPlugin::constructModelFromIsd";
  throw csm::Error(errorType, msg, func);
}

