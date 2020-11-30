#include <iostream>

#include "TestCsmPlugin.h"
#include "TestCsmModel.h"

// Static Instance of itself
const TestCsmPlugin TestCsmPlugin::m_registeredPlugin;

TestCsmPlugin::TestCsmPlugin() {
  // deliberately blank
}

TestCsmPlugin::~TestCsmPlugin() {}

std::string TestCsmPlugin::getPluginName() const {
  return "TestCsmPlugin";
}

std::string TestCsmPlugin::getManufacturer() const {
  return "TestCsmPluginCreator";
}

std::string TestCsmPlugin::getReleaseDate() const {
  return "TestDate";
}

csm::Version TestCsmPlugin::getCsmVersion() const {
  return csm::Version(3,0,3);
}

size_t TestCsmPlugin::getNumModels() const {
  return 2;
}

std::string TestCsmPlugin::getModelName(size_t modelIndex) const {
  return "TestModelName";
}

std::string TestCsmPlugin::getModelFamily(size_t modelIndex) const {
  return "TestModelFamily";
}

csm::Version TestCsmPlugin::getModelVersion(const std::string& modelName) const { 
  return csm::Version(3,0,3);
}

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

bool TestCsmPlugin::canISDBeConvertedToModelState(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const{
  return true;
}

std::string TestCsmPlugin::convertISDToModelState(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* warnings) const {
  return "ISDConvertedtoModelState";
}

std::string TestCsmPlugin::getModelNameFromModelState(
    const std::string& modelState,
    csm::WarningList* warnings) const {
  return "TestModelNameFromModelState";
}

csm::Model* TestCsmPlugin::constructModelFromState(
      const std::string& modelState,
      csm::WarningList* warnings) const {
  TestCsmModel *model = new TestCsmModel();
  return model;
}

csm::Model* TestCsmPlugin::constructModelFromISD(
    const csm::Isd& imageSupportData,
    const std::string& modelName,
    csm::WarningList* ) const {
  if (modelName == "TestModelName") {
    TestCsmModel *model = new TestCsmModel();
    return model;
  }
  else {
    csm::Error::ErrorType errorType = csm::Error::SENSOR_MODEL_NOT_SUPPORTED;
    std::string msg = "TstCsmPlugin failed to construct model from ISD";
    std::string func = "TestCsmPlugin::constructModelFromIsd";
    throw csm::Error(errorType, msg, func);
  }
}

