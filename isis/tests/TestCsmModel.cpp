#include "TestCsmModel.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

TestCsmModel::TestCsmModel() {
  m_modelState = "TestCsmModel_ModelState";
};

TestCsmModel::~TestCsmModel() {
};

std::string TestCsmModel::getFamily() const {
  return "TestCsmModelFamily";
}

csm::Version TestCsmModel::getVersion() const {
  return csm::Version(1,0,0);
}

std::string TestCsmModel::getModelName() const {
  return "TestCsmModelName";
}

std::string TestCsmModel::getPedigree() const {
  return "TestCsmModelPedigree";
}

std::string TestCsmModel::getImageIdentifier() const {
  return "TestCsmModelImageIdentifier";
}

void TestCsmModel::setImageIdentifier(const std::string& imageId,
                                      csm::WarningList* warnings) {
  // do nothing for test
}

std::string TestCsmModel::getSensorIdentifier() const {
  return "TestCsmModelSensorIdentifier";
}

std::string TestCsmModel::getPlatformIdentifier() const {
  return "TestCsmModel_PlatformIdentifier";
}

std::string TestCsmModel::getCollectionIdentifier() const {
  return "TestCsmModel_CollectionIdentifier";
}

std::string TestCsmModel::getTrajectoryIdentifier() const {
  return "TestCsmModel_TrajectoryIdentifier";
}

std::string TestCsmModel::getSensorType() const {
  return "TestCsmModel_SensorType";
}

std::string TestCsmModel::getSensorMode() const {
  return "TestCsmModel_SensorMode";
}

std::string TestCsmModel::getReferenceDateAndTime() const {
  return "TestCsmModel_ReferenceDateTime";
}

std::string TestCsmModel::getModelState() const {
  return m_modelState;
}

void TestCsmModel::replaceModelState(const std::string& argState) {
  m_modelState = argState;
}

std::string TestCsmModel::constructStateFromIsd(const csm::Isd isd){
  std::string filename = isd.filename();
  std::ifstream isdFile(filename);

  if (isdFile.fail()) {
    std::cout << "Could not open file: " << filename << std::endl;
  }

  json parsedIsd;
  isdFile >> parsedIsd;
  json state;
  state["name"] = parsedIsd.at("name");
  state["test_param_one"] = parsedIsd.at("test_param_one");
  state["test_param_two"] = parsedIsd.at("test_param_two");
  return state.dump();
}
