#include "AlternativeTestCsmModel.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

AlternativeTestCsmModel::AlternativeTestCsmModel() {
  m_modelState = "AlternativeTestCsmModel_ModelState";
};

AlternativeTestCsmModel::~AlternativeTestCsmModel() {
};

std::string AlternativeTestCsmModel::getFamily() const {
  return "AlternativeTestCsmModelFamily";
}

csm::Version AlternativeTestCsmModel::getVersion() const {
  return csm::Version(1,0,0);
}

std::string AlternativeTestCsmModel::getModelName() const {
  return "AlternativeTestCsmModelName";
}

std::string AlternativeTestCsmModel::getPedigree() const {
  return "AlternativeTestCsmModelPedigree";
}

std::string AlternativeTestCsmModel::getImageIdentifier() const {
  return "AlternativeTestCsmModelImageIdentifier";
}

void AlternativeTestCsmModel::setImageIdentifier(const std::string& imageId,
                                      csm::WarningList* warnings) {
  // do nothing for test
}

std::string AlternativeTestCsmModel::getSensorIdentifier() const {
  return "AlternativeTestCsmModelSensorIdentifier";
}

std::string AlternativeTestCsmModel::getPlatformIdentifier() const {
  return "AlternativeTestCsmModel_PlatformIdentifier";
}

std::string AlternativeTestCsmModel::getCollectionIdentifier() const {
  return "AlternativeTestCsmModel_CollectionIdentifier";
}

std::string AlternativeTestCsmModel::getTrajectoryIdentifier() const {
  return "AlternativeTestCsmModel_TrajectoryIdentifier";
}

std::string AlternativeTestCsmModel::getSensorType() const {
  return "AlternativeTestCsmModel_SensorType";
}

std::string AlternativeTestCsmModel::getSensorMode() const {
  return "AlternativeTestCsmModel_SensorMode";
}

std::string AlternativeTestCsmModel::getReferenceDateAndTime() const {
  return "AlternativeTestCsmModel_ReferenceDateTime";
}

std::string AlternativeTestCsmModel::getModelState() const {
  return m_modelState;
}

void AlternativeTestCsmModel::replaceModelState(const std::string& argState) {
  m_modelState = argState;
}

std::string AlternativeTestCsmModel::constructStateFromIsd(const csm::Isd isd){
  std::string filename = isd.filename();
  std::ifstream isdFile(filename);
  json parsedIsd;
  isdFile >> parsedIsd;

  json state;
  state["name"] = parsedIsd.at("name");
  state["test_param_one"] = parsedIsd.at("test_param_one");
  state["test_param_two"] = parsedIsd.at("test_param_two");
  state["test_param_three"] = parsedIsd.at("test_param_three");
  return state.dump();
}
