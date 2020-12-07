#include "TestCsmModel.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::string TestCsmModel::SENSOR_MODEL_NAME = "TestCsmModelName";
const std::vector<std::string> TestCsmModel::PARAM_NAMES = {
  "TestParam1",
  "TestParam2"
};
const std::vector<std::string> TestCsmModel::PARAM_UNITS = {
  "m",
  "rad"
};
const std::vector<csm::param::Type> TestCsmModel::PARAM_TYPES = {
  csm::param::FICTITIOUS,
  csm::param::REAL
};
const std::vector<csm::SharingCriteria> TestCsmModel::PARAM_SHARING_CRITERIA = {
  csm::SharingCriteria(),
  csm::SharingCriteria()
};

TestCsmModel::TestCsmModel() {
  m_param_values.resize(TestCsmModel::PARAM_NAMES.size(), 0.0);
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
  return TestCsmModel::SENSOR_MODEL_NAME;
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
  json state;
  state["test_param_one"] = m_param_values[0];
  state["test_param_two"] = m_param_values[1];
  return TestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump();
}

void TestCsmModel::replaceModelState(const std::string& argState) {
  // Get the JSON substring
  json state = json::parse(argState.substr(argState.find("\n") + 1));
  m_param_values[0] = state.at("test_param_one");
  m_param_values[1] = state.at("test_param_two");
}

std::string TestCsmModel::constructStateFromIsd(const csm::Isd isd){
  std::string filename = isd.filename();
  std::ifstream isdFile(filename);

  if (isdFile.fail()) {
    std::cout << "Could not open file: " << filename << std::endl;
  }

  json parsedIsd;
  isdFile >> parsedIsd;
  // Only extract the first 2 parameters from the file
  json state;
  state["test_param_one"] = parsedIsd.at("test_param_one");
  state["test_param_two"] = parsedIsd.at("test_param_two");
  return TestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump();
}


csm::EcefCoord TestCsmModel::getReferencePoint() const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}

void TestCsmModel::setReferencePoint(const csm::EcefCoord& groundPt) {
  // do nothing for test
}

int TestCsmModel::getNumParameters() const {
  return m_param_values.size();
}

std::string TestCsmModel::getParameterName(int index) const {
  return TestCsmModel::PARAM_NAMES[index];
}


std::string TestCsmModel::getParameterUnits(int index) const {
  return TestCsmModel::PARAM_UNITS[index];
}

bool TestCsmModel::hasShareableParameters() const {
  return false;
}

bool TestCsmModel::isParameterShareable(int index) const {
  return false;
}

csm::SharingCriteria TestCsmModel::getParameterSharingCriteria(int index) const {
  return TestCsmModel::PARAM_SHARING_CRITERIA[index];
}

double TestCsmModel::getParameterValue(int index) const {
  return m_param_values[index];
}

void TestCsmModel::setParameterValue(int index, double value) {
  m_param_values[index] = value;
}

csm::param::Type TestCsmModel::getParameterType(int index) const {
  return TestCsmModel::PARAM_TYPES[index];
}

void TestCsmModel::setParameterType(int index, csm::param::Type pType) {
  // do nothing for test
}

double TestCsmModel::getParameterCovariance(int index1,
                                            int index2) const {
  // default to identity covariance matrix
  if (index1 == index2) {
    return 1.0;
  }
  return 0.0;
                              }
void TestCsmModel::setParameterCovariance(int index1,
                                          int index2,
                                          double covariance) {
  // do nothing for test
}

int TestCsmModel::getNumGeometricCorrectionSwitches() const {
  return 0;
}

std::string TestCsmModel::getGeometricCorrectionName(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "TestCsmModel::getGeometricCorrectionName");
}

void TestCsmModel::setGeometricCorrectionSwitch(int index,
                                  bool value,
                                  csm::param::Type pType) {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                  "TestCsmModel::setGeometricCorrectionSwitch");
}

bool TestCsmModel::getGeometricCorrectionSwitch(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "TestCsmModel::getGeometricCorrectionSwitch");
}

std::vector<double> TestCsmModel::getCrossCovarianceMatrix(
      const csm::GeometricModel& comparisonModel,
      csm::param::Set pSet,
      const csm::GeometricModel::GeometricModelList& otherModels) const {
  const std::vector<int>& rowIndices = getParameterSetIndices(pSet);
  size_t numRows = rowIndices.size();
  const std::vector<int>& colIndices = comparisonModel.getParameterSetIndices(pSet);
  size_t numCols = colIndices.size();
  std::vector<double> covariance(numRows * numCols, 0.0);

  if (&comparisonModel == this) {
    for (size_t rowIndex = 0; rowIndex <  numRows; numRows++) {
      for (size_t colIndex = 0; colIndex < numCols; colIndex++) {
        covariance[rowIndex * numCols + colIndex] = getParameterCovariance(rowIndices[rowIndex],
                                                                           colIndices[colIndex]);
      }
    }
  }

  return covariance;
}


