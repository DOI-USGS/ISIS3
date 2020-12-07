#include "AlternativeTestCsmModel.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::string AlternativeTestCsmModel::SENSOR_MODEL_NAME = "TestCsmModelName";
const std::vector<std::string> AlternativeTestCsmModel::PARAM_NAMES = {
  "TestParam1",
  "TestParam2"
};
const std::vector<std::string> AlternativeTestCsmModel::PARAM_UNITS = {
  "m",
  "rad"
};
const std::vector<csm::param::Type> AlternativeTestCsmModel::PARAM_TYPES = {
  csm::param::FICTITIOUS,
  csm::param::REAL
};
const std::vector<csm::SharingCriteria> AlternativeTestCsmModel::PARAM_SHARING_CRITERIA = {
  csm::SharingCriteria(),
  csm::SharingCriteria()
};

AlternativeTestCsmModel::AlternativeTestCsmModel() {
  m_modelState = "AlternativeTestCsmModel_ModelState";
  m_param_values.resize(AlternativeTestCsmModel::PARAM_NAMES.size(), 0.0);
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

csm::EcefCoord AlternativeTestCsmModel::getReferencePoint() const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}

void AlternativeTestCsmModel::setReferencePoint(const csm::EcefCoord& groundPt) {
  // do nothing for test
}

int AlternativeTestCsmModel::getNumParameters() const {
  return m_param_values.size();
}

std::string AlternativeTestCsmModel::getParameterName(int index) const {
  return AlternativeTestCsmModel::PARAM_NAMES[index];
}


std::string AlternativeTestCsmModel::getParameterUnits(int index) const {
  return AlternativeTestCsmModel::PARAM_UNITS[index];
}

bool AlternativeTestCsmModel::hasShareableParameters() const {
  return false;
}

bool AlternativeTestCsmModel::isParameterShareable(int index) const {
  return false;
}

csm::SharingCriteria AlternativeTestCsmModel::getParameterSharingCriteria(int index) const {
  return AlternativeTestCsmModel::PARAM_SHARING_CRITERIA[index];
}

double AlternativeTestCsmModel::getParameterValue(int index) const {
  return m_param_values[index];
}

void AlternativeTestCsmModel::setParameterValue(int index, double value) {
  m_param_values[index] = value;
}

csm::param::Type AlternativeTestCsmModel::getParameterType(int index) const {
  return AlternativeTestCsmModel::PARAM_TYPES[index];
}

void AlternativeTestCsmModel::setParameterType(int index, csm::param::Type pType) {
  // do nothing for test
}

double AlternativeTestCsmModel::getParameterCovariance(int index1,
                                            int index2) const {
  // default to identity covariance matrix
  if (index1 == index2) {
    return 1.0;
  }
  return 0.0;
                              }
void AlternativeTestCsmModel::setParameterCovariance(int index1,
                                          int index2,
                                          double covariance) {
  // do nothing for test
}

int AlternativeTestCsmModel::getNumGeometricCorrectionSwitches() const {
  return 0;
}

std::string AlternativeTestCsmModel::getGeometricCorrectionName(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "AlternativeTestCsmModel::getGeometricCorrectionName");
}

void AlternativeTestCsmModel::setGeometricCorrectionSwitch(int index,
                                  bool value,
                                  csm::param::Type pType) {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                  "AlternativeTestCsmModel::setGeometricCorrectionSwitch");
}

bool AlternativeTestCsmModel::getGeometricCorrectionSwitch(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "AlternativeTestCsmModel::getGeometricCorrectionSwitch");
}

std::vector<double> AlternativeTestCsmModel::getCrossCovarianceMatrix(
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
