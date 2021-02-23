#include "AlternativeTestCsmModel.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


// Sensor model Name
const std::string AlternativeTestCsmModel::SENSOR_MODEL_NAME = "AlternativeTestCsmModel";

// Sensor model Parameter names
const std::vector<std::string> AlternativeTestCsmModel::PARAM_NAMES = {
  "test_param_one",
  "test_param_two",
  "test_param_three",
  "test_param_four"
};

// Sensor model Parameter units
const std::vector<std::string> AlternativeTestCsmModel::PARAM_UNITS = {
  "m",
  "rad",
  "K",
  "unitless"
};

// Sensor model Parameter Types
const std::vector<csm::param::Type> AlternativeTestCsmModel::PARAM_TYPES = {
  csm::param::FICTITIOUS,
  csm::param::REAL,
  csm::param::FIXED,
  csm::param::NONE
};

// Sensor model Parameter sharing criteria
const std::vector<csm::SharingCriteria> AlternativeTestCsmModel::PARAM_SHARING_CRITERIA = {
  csm::SharingCriteria(),
  csm::SharingCriteria(),
  csm::SharingCriteria(),
  csm::SharingCriteria()
};


/**
 * Constructor. Resizes the parameter values list based on the number of PARAM_NAMES.
 */
AlternativeTestCsmModel::AlternativeTestCsmModel() {
  m_param_values.resize(AlternativeTestCsmModel::PARAM_NAMES.size(), 0.0);
};


/**
 * Default destructor
 */
AlternativeTestCsmModel::~AlternativeTestCsmModel() {
};


/**
 * Returns the sensor model family.
 * 
 * @return std::string Sensor model family
 */
std::string AlternativeTestCsmModel::getFamily() const {
  return "AlternativeTestCsmModelFamily";
}


/**
 * Returns the version of the sensor model
 * 
 * @return csm::Version sensor model version
 */
csm::Version AlternativeTestCsmModel::getVersion() const {
  return csm::Version(1,0,0);
}


/**
 * Returns the name of the sensor model.
 * 
 * @return std::string sensor model name
 */
std::string AlternativeTestCsmModel::getModelName() const {
  return AlternativeTestCsmModel::SENSOR_MODEL_NAME;
}


/**
 * Returns the pedigree of the sensor model.
 * 
 * @return std::string sensor model pedigree
 */
std::string AlternativeTestCsmModel::getPedigree() const {
  return "AlternativeTestCsmModelPedigree";
}


/**
 * Returns the image identifier.
 * 
 * @return std::string image identifier
 */
std::string AlternativeTestCsmModel::getImageIdentifier() const {
  return "AlternativeTestCsmModelImageIdentifier";
}


/**
 * Does nothing. Empty implementation for test.
 * 
 * @param imageId image identifier
 * @param warnings CSM warnings list
 */
void AlternativeTestCsmModel::setImageIdentifier(const std::string& imageId,
                                      csm::WarningList* warnings) {
  // do nothing for test
}


/**
 * Returns the sensor identifier for the sensor model. 
 * 
 * @return std::string sensor identifier
 */
std::string AlternativeTestCsmModel::getSensorIdentifier() const {
  return "AlternativeTestCsmModelSensorIdentifier";
}


/**
 * Returns the platform identifier for the sensor model.
 * 
 * @return std::string platform identifier
 */
std::string AlternativeTestCsmModel::getPlatformIdentifier() const {
  return "AlternativeTestCsmModel_PlatformIdentifier";
}


/**
 * Returns the collection identifier for the sensor model.
 * 
 * @return std::string collection identifier
 */
std::string AlternativeTestCsmModel::getCollectionIdentifier() const {
  return "AlternativeTestCsmModel_CollectionIdentifier";
}


/**
 * Returns the trajectory identifier for the sensor model. 
 * 
 * @return std::string trajectory identifier
 */
std::string AlternativeTestCsmModel::getTrajectoryIdentifier() const {
  return "AlternativeTestCsmModel_TrajectoryIdentifier";
}


/**
 * Reeturns the sensor type for the sensor model.
 * 
 * @return std::string sensor type
 */
std::string AlternativeTestCsmModel::getSensorType() const {
  return "AlternativeTestCsmModel_SensorType";
}


/**
 * Returns the sensor mode for the sensor model
 * 
 * @return std::string sensor mode
 */
std::string AlternativeTestCsmModel::getSensorMode() const {
  return "AlternativeTestCsmModel_SensorMode";
}


/**
 * Returns the reference date and time for the sensor model
 * 
 * @return std::string reference date and time
 */
std::string AlternativeTestCsmModel::getReferenceDateAndTime() const {
  return "20000101T115959Z";
}


/**
 * Returns the current model state for the sensor model. 
 * 
 * @return std::string model state
 */
std::string AlternativeTestCsmModel::getModelState() const {
  json state;
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    state[AlternativeTestCsmModel::PARAM_NAMES[param_index]] = m_param_values[param_index];
  }
  return AlternativeTestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump();
}


/**
 * Uses the supplied sensor model state to set the steat of the current sensor model. 
 * 
 * @param argState the model state
 */
void AlternativeTestCsmModel::replaceModelState(const std::string& argState) {
  // Get the JSON substring
  json state = json::parse(argState.substr(argState.find("\n") + 1));
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    m_param_values[param_index] = state.at(AlternativeTestCsmModel::PARAM_NAMES[param_index]);
  }
}


/**
 * Constructs and returns a sensor model state from an ISD.
 * 
 * @param isd instrument support data
 * 
 * @return std::string sensor model state
 */
std::string AlternativeTestCsmModel::constructStateFromIsd(const csm::Isd isd){
  std::string filename = isd.filename();
  std::ifstream isdFile(filename);

  if (isdFile.fail()) {
    std::cout << "Could not open file: " << filename << std::endl;
  }

  json parsedIsd;
  isdFile >> parsedIsd;

  json state;
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    state[AlternativeTestCsmModel::PARAM_NAMES[param_index]] = parsedIsd.at(AlternativeTestCsmModel::PARAM_NAMES[param_index]);
  }
  return AlternativeTestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump();
}


/**
 * Returns a default reference point. 
 * 
 * @return csm::EcefCoord reference point
 */
csm::EcefCoord AlternativeTestCsmModel::getReferencePoint() const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}


/**
 * Does nothing. Minimal implementation for test.
 * 
 * @param groundPt the ground point
 */
void AlternativeTestCsmModel::setReferencePoint(const csm::EcefCoord& groundPt) {
  // do nothing for test
}


/**
 * Returns the number of sensor model parameters
 * 
 * @return int number of parameters
 */
int AlternativeTestCsmModel::getNumParameters() const {
  return m_param_values.size();
}


/**
 * Returns the semsor model parameter name at the provided index
 * 
 * @param index parameter index
 * 
 * @return std::string parameter name
 */
std::string AlternativeTestCsmModel::getParameterName(int index) const {
  return AlternativeTestCsmModel::PARAM_NAMES[index];
}


/**
 * Returns the sensor model parameter units at the provided index
 * 
 * @param index parameter unit index
 * 
 * @return std::string parameter units
 */
std::string AlternativeTestCsmModel::getParameterUnits(int index) const {
  return AlternativeTestCsmModel::PARAM_UNITS[index];
}


/**
 * True if the sensor model has sharable parameters. 
 * 
 * @return bool Always returns false
 */
bool AlternativeTestCsmModel::hasShareableParameters() const {
  return false;
}


/**
 * True if the sensor model parameter at the provided index is sharable.
 * 
 * @param index Parameter index
 * 
 * @return bool Always returns false
 */
bool AlternativeTestCsmModel::isParameterShareable(int index) const {
  return false;
}


/**
 * Returns the sharing criteria for the sensor model parameter at the provided index
 * 
 * @param index Parameter index
 * 
 * @return csm::SharingCriteria CSM sharing criteria for the parameter
 */
csm::SharingCriteria AlternativeTestCsmModel::getParameterSharingCriteria(int index) const {
  return AlternativeTestCsmModel::PARAM_SHARING_CRITERIA[index];
}


/**
 * Returns the sensor model parameter value at the provided index.
 * 
 * @param index Parameter index
 * 
 * @return double Value at provided index
 */
double AlternativeTestCsmModel::getParameterValue(int index) const {
  return m_param_values[index];
}


/**
 * Set the sensor model parameter at the provided index to the provided 
 * value. 
 * 
 * @param index Parameter index
 * @param value Value to set the parameter to
 */
void AlternativeTestCsmModel::setParameterValue(int index, double value) {
  m_param_values[index] = value;
}


/**
 * Returns the type of the sensor model parameter at the provided index.
 * 
 * @param index Parameter index
 * 
 * @return csm::param::Type Type of parameter
 */
csm::param::Type AlternativeTestCsmModel::getParameterType(int index) const {
  return AlternativeTestCsmModel::PARAM_TYPES[index];
}


/**
 * Does nothing. Minimal implementation for testing
 * 
 * @param index Parameter index
 * @param pType Parameter type
 */
void AlternativeTestCsmModel::setParameterType(int index, csm::param::Type pType) {
  // do nothing for test
}


/**
 * Returns the covariance between the two sensor model parameters at the provided indicies. 
 * Defaults to identity covariance matrix for testing. 
 * 
 * @param index1 First parameter index
 * @param index2 Second parameter index
 * 
 * @return double Parameter covariance
 */
double AlternativeTestCsmModel::getParameterCovariance(int index1,
                                            int index2) const {
  // default to identity covariance matrix
  if (index1 == index2) {
    return 1.0;
  }
  return 0.0;
}

/**
 * Does nothing. Minimal implementation for testing.
 * 
 * @param index1 First parameter index
 * @param index2 Second parameter index
 * @param covariance Covariance between the two parameters
 */
void AlternativeTestCsmModel::setParameterCovariance(int index1,
                                          int index2,
                                          double covariance) {
  // do nothing for test
}


/**
 * Returns the number of geometric correction switches.
 * 
 * @return int Number of geometric correction switches.
 */
int AlternativeTestCsmModel::getNumGeometricCorrectionSwitches() const {
  return 0;
}


/**
 * Always throws an error, as no geometric correction switches exist for this class. 
 *  
 * @param index Geometric correction index
 * 
 * @return std::string Geometric correction
 */
std::string AlternativeTestCsmModel::getGeometricCorrectionName(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "AlternativeTestCsmModel::getGeometricCorrectionName");
}


/** 
 * Always throws an error, as no geometric correction switches exist for this class.
 * 
 * @param index Geometric correction index
 * @param value Value to set
 * @param pType Parameter type
 */
void AlternativeTestCsmModel::setGeometricCorrectionSwitch(int index,
                                  bool value,
                                  csm::param::Type pType) {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                  "AlternativeTestCsmModel::setGeometricCorrectionSwitch");
}


/**
 * Always throws an error, as no geometric correction switches exist for this class.
 * 
 * @param index Geometric correction index
 * 
 * @return bool If the geometric correction switch can be accessed.
 */
bool AlternativeTestCsmModel::getGeometricCorrectionSwitch(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "AlternativeTestCsmModel::getGeometricCorrectionSwitch");
}


/**
 * Returns the cross covariance matrix.
 * 
 * @param comparisonModel The geometric model to compare with.
 * @param pSet Set of parameters to use
 * @param otherModels Not used.
 * 
 * @return std::vector<double> covariance matrix
 */
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

// csm::RasterGM methods
csm::ImageCoord AlternativeTestCsmModel::groundToImage(const csm::EcefCoord& groundPt,
                         double desiredPrecision,
                         double* achievedPrecision,
                         csm::WarningList* warnings) const {
  return csm::ImageCoord(0.0,0.0);
}


csm::ImageCoordCovar AlternativeTestCsmModel::groundToImage(const csm::EcefCoordCovar& groundPt,
                              double desiredPrecision,
                              double* achievedPrecision,
                              csm::WarningList* warnings) const {
  return csm::ImageCoordCovar(0.0, 0.0, 0.0, 0.0, 0.0);
}
  

csm::EcefCoord AlternativeTestCsmModel::imageToGround(const csm::ImageCoord& imagePt,
                               double height,
                               double desiredPrecision,
                               double* achievedPrecision,
                               csm::WarningList* warnings) const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}

csm::EcefCoordCovar AlternativeTestCsmModel::imageToGround(const csm::ImageCoordCovar& imagePt,
                                    double height,
                                    double heightVariance,
                                    double desiredPrecision,
                                    double* achievedPrecision,
                                    csm::WarningList* warnings) const {
  return csm::EcefCoordCovar(0.0, 0.0, 0.0);
}


csm::EcefLocus AlternativeTestCsmModel::imageToProximateImagingLocus(
  const csm::ImageCoord& imagePt,
  const csm::EcefCoord& groundPt,
  double desiredPrecision,
  double* achievedPrecision,
  csm::WarningList* warnings) const {
  return csm::EcefLocus(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}


csm::EcefLocus AlternativeTestCsmModel::imageToRemoteImagingLocus(
  const csm::ImageCoord& imagePt,
  double desiredPrecision,
  double* achievedPrecision,
  csm::WarningList* warnings) const {
  return csm::EcefLocus(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}


csm::ImageCoord AlternativeTestCsmModel::getImageStart() const {
  return csm::ImageCoord(0.0, 0.0);
}


csm::ImageVector AlternativeTestCsmModel::getImageSize() const {
  return csm::ImageVector(0.0, 0.0);
}


std::pair<csm::ImageCoord,csm::ImageCoord> AlternativeTestCsmModel::getValidImageRange() const {
  std::pair<csm::ImageCoord,csm::ImageCoord> csmPair(csm::ImageCoord(0.0, 0.0), csm::ImageCoord(0.0, 0.0));
  return csmPair;
}


std::pair<double,double> AlternativeTestCsmModel::getValidHeightRange() const {
  std::pair<double, double> csmPair(0.0, 0.0);
  return csmPair;
}


csm::EcefVector AlternativeTestCsmModel::getIlluminationDirection(const csm::EcefCoord& groundPt) const {
  return csm::EcefVector(0.0, 0.0, 0.0);
}


double AlternativeTestCsmModel::getImageTime(const csm::ImageCoord& imagePt) const {
  return 0.0;
}


csm::EcefCoord AlternativeTestCsmModel::getSensorPosition(const csm::ImageCoord& imagePt) const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}


csm::EcefCoord AlternativeTestCsmModel::getSensorPosition(double time) const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}


csm::EcefVector AlternativeTestCsmModel::getSensorVelocity(const csm::ImageCoord& imagePt) const {
  return csm::EcefVector(0.0, 0.0, 0.0);
}


csm::EcefVector AlternativeTestCsmModel::getSensorVelocity(double time) const {
  return csm::EcefVector(0.0, 0.0, 0.0);
}


csm::RasterGM::SensorPartials AlternativeTestCsmModel::computeSensorPartials(
            int index,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const {
  return csm::RasterGM::SensorPartials(0.0, 0.0);
}


csm::RasterGM::SensorPartials AlternativeTestCsmModel::computeSensorPartials(
            int index,
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const {
  
  return csm::RasterGM::SensorPartials(0.0, 0.0);
}


std::vector<double> AlternativeTestCsmModel::computeGroundPartials(const csm::EcefCoord& groundPt) const {
  std::vector<double> vec = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  return vec;
}

const csm::CorrelationModel& AlternativeTestCsmModel::getCorrelationModel() const {
  return m_correlationModel;
}

std::vector<double> AlternativeTestCsmModel::getUnmodeledCrossCovariance(
            const csm::ImageCoord& pt1,
            const csm::ImageCoord& pt2) const {
  std::vector<double> vec = {0.0, 0.0, 0.0, 0.0};
  return vec;
}


