#include "TestCsmModel.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Sensor model Name
const std::string TestCsmModel::SENSOR_MODEL_NAME = "TestCsmModel";

// Sensor model Parameter names
const std::vector<std::string> TestCsmModel::PARAM_NAMES = {
  "test_param_one",
  "test_param_two"
};

// Sensor model Parameter units
const std::vector<std::string> TestCsmModel::PARAM_UNITS = {
  "m",
  "rad"
};

// Sensor model Parameter Types
const std::vector<csm::param::Type> TestCsmModel::PARAM_TYPES = {
  csm::param::FICTITIOUS,
  csm::param::REAL
};

// Sensor model Parameter sharing criteria
const std::vector<csm::SharingCriteria> TestCsmModel::PARAM_SHARING_CRITERIA = {
  csm::SharingCriteria(),
  csm::SharingCriteria()
};


/**
 * Constructor. Resizes the parameter values list based on the number of PARAM_NAMES.
 */
TestCsmModel::TestCsmModel() {
  m_param_values.resize(TestCsmModel::PARAM_NAMES.size(), 0.0);
};


/**
 * Default destructor
 */
TestCsmModel::~TestCsmModel() {
};


/**
 * Returns the sensor model family.
 * 
 * @return std::string Sensor model family
 */
std::string TestCsmModel::getFamily() const {
  return "TestCsmModelFamily";
}


/**
 * Returns the version of the sensor model
 * 
 * @return csm::Version sensor model version
 */
csm::Version TestCsmModel::getVersion() const {
  return csm::Version(1,0,0);
}


/**
 * Returns the name of the sensor model.
 * 
 * @return std::string sensor model name
 */
std::string TestCsmModel::getModelName() const {
  return TestCsmModel::SENSOR_MODEL_NAME;
}


/**
 * Returns the pedigree of the sensor model.
 * 
 * @return std::string sensor model pedigree
 */
std::string TestCsmModel::getPedigree() const {
  return "TestCsmModelPedigree";
}


/**
 * Returns the image identifier.
 * 
 * @return std::string image identifier
 */
std::string TestCsmModel::getImageIdentifier() const {
  return "TestCsmModelImageIdentifier";
}


/**
 * Does nothing. Empty implementation for test.
 * 
 * @param imageId image identifier
 * @param warnings CSM warnings list
 */
void TestCsmModel::setImageIdentifier(const std::string& imageId,
                                      csm::WarningList* warnings) {
  // do nothing for test
}


/**
 * Returns the sensor identifier for the sensor model. 
 * 
 * @return std::string sensor identifier
 */
std::string TestCsmModel::getSensorIdentifier() const {
  return "TestCsmModelSensorIdentifier";
}


/**
 * Returns the platform identifier for the sensor model.
 * 
 * @return std::string platform identifier
 */
std::string TestCsmModel::getPlatformIdentifier() const {
  return "TestCsmModel_PlatformIdentifier";
}


/**
 * Returns the collection identifier for the sensor model.
 * 
 * @return std::string collection identifier
 */
std::string TestCsmModel::getCollectionIdentifier() const {
  return "TestCsmModel_CollectionIdentifier";
}


/**
 * Returns the trajectory identifier for the sensor model. 
 * 
 * @return std::string trajectory identifier
 */
std::string TestCsmModel::getTrajectoryIdentifier() const {
  return "TestCsmModel_TrajectoryIdentifier";
}


/**
 * Reeturns the sensor type for the sensor model.
 * 
 * @return std::string sensor type
 */
std::string TestCsmModel::getSensorType() const {
  return "TestCsmModel_SensorType";
}


/**
 * Returns the sensor mode for the sensor model
 * 
 * @return std::string sensor mode
 */
std::string TestCsmModel::getSensorMode() const {
  return "TestCsmModel_SensorMode";
}


/**
 * Returns the reference date and time for the sensor model
 * 
 * @return std::string reference date and time
 */
std::string TestCsmModel::getReferenceDateAndTime() const {
  return "20000101T115959Z";
}


/**
 * Returns the current model state for the sensor model. 
 * 
 * @return std::string model state
 */
std::string TestCsmModel::getModelState() const {
  json state;
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    state[TestCsmModel::PARAM_NAMES[param_index]] = m_param_values[param_index];
  }
  return TestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump();
}


/**
 * Uses the supplied sensor model state to set the steat of the current sensor model. 
 * 
 * @param argState the model state
 */
void TestCsmModel::replaceModelState(const std::string& argState) {
  // Get the JSON substring
  json state = json::parse(argState.substr(argState.find("\n") + 1));
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    m_param_values[param_index] = state.at(TestCsmModel::PARAM_NAMES[param_index]);
  }
}


/**
 * Constructs and returns a sensor model state from an ISD.
 * 
 * @param isd instrument support data
 * 
 * @return std::string sensor model state
 */
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
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    state[TestCsmModel::PARAM_NAMES[param_index]] = parsedIsd.at(TestCsmModel::PARAM_NAMES[param_index]);
  }
  return TestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump();
}


/**
 * Returns a default reference point. 
 * 
 * @return csm::EcefCoord reference point
 */
csm::EcefCoord TestCsmModel::getReferencePoint() const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}


/**
 * Does nothing. Minimal implementation for test.
 * 
 * @param groundPt the ground point
 */
void TestCsmModel::setReferencePoint(const csm::EcefCoord& groundPt) {
  // do nothing for test
}


/**
 * Returns the number of sensor model parameters
 * 
 * @return int number of parameters
 */
int TestCsmModel::getNumParameters() const {
  return m_param_values.size();
}


/**
 * Returns the semsor model parameter name at the provided index
 * 
 * @param index parameter index
 * 
 * @return std::string parameter name
 */
std::string TestCsmModel::getParameterName(int index) const {
  return TestCsmModel::PARAM_NAMES[index];
}


/**
 * Returns the sensor model parameter units at the provided index
 * 
 * @param index parameter unit index
 * 
 * @return std::string parameter units
 */
std::string TestCsmModel::getParameterUnits(int index) const {
  return TestCsmModel::PARAM_UNITS[index];
}


/**
 * True if the sensor model has sharable parameters. 
 * 
 * @return bool Always returns false
 */
bool TestCsmModel::hasShareableParameters() const {
  return false;
}


/**
 * True if the sensor model parameter at the provided index is sharable.
 * 
 * @param index Parameter index
 * 
 * @return bool Always returns false
 */
bool TestCsmModel::isParameterShareable(int index) const {
  return false;
}



/**
 * Returns the sharing criteria for the sensor model parameter at the provided index
 * 
 * @param index Parameter index
 * 
 * @return csm::SharingCriteria CSM sharing criteria for the parameter
 */
csm::SharingCriteria TestCsmModel::getParameterSharingCriteria(int index) const {
  return TestCsmModel::PARAM_SHARING_CRITERIA[index];
}


/**
 * Returns the sensor model parameter value at the provided index.
 * 
 * @param index Parameter index
 * 
 * @return double Value at provided index
 */
double TestCsmModel::getParameterValue(int index) const {
  return m_param_values[index];
}


/**
 * Set the sensor model parameter at the provided index to the provided 
 * value. 
 * 
 * @param index Parameter index
 * @param value Value to set the parameter to
 */
void TestCsmModel::setParameterValue(int index, double value) {
  m_param_values[index] = value;
}


/**
 * Returns the type of the sensor model parameter at the provided index.
 * 
 * @param index Parameter index
 * 
 * @return csm::param::Type Type of parameter
 */
csm::param::Type TestCsmModel::getParameterType(int index) const {
  return TestCsmModel::PARAM_TYPES[index];
}


/**
 * Does nothing. Minimal implementation for testing
 * 
 * @param index Parameter index
 * @param pType Parameter type
 */
void TestCsmModel::setParameterType(int index, csm::param::Type pType) {
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
double TestCsmModel::getParameterCovariance(int index1,
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
void TestCsmModel::setParameterCovariance(int index1,
                                          int index2,
                                          double covariance) {
  // do nothing for test
}


/**
 * Returns the number of geometric correction switches.
 * 
 * @return int Number of geometric correction switches.
 */
int TestCsmModel::getNumGeometricCorrectionSwitches() const {
  return 0;
}


/**
 * Always throws an error, as no geometric correction switches exist for this class. 
 *  
 * @param index Geometric correction index
 * 
 * @return std::string Geometric correction
 */
std::string TestCsmModel::getGeometricCorrectionName(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "TestCsmModel::getGeometricCorrectionName");
}


/** 
 * Always throws an error, as no geometric correction switches exist for this class.
 * 
 * @param index Geometric correction index
 * @param value Value to set
 * @param pType Parameter type
 */
void TestCsmModel::setGeometricCorrectionSwitch(int index,
                                  bool value,
                                  csm::param::Type pType) {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                  "TestCsmModel::setGeometricCorrectionSwitch");
}


/**
 * Always throws an error, as no geometric correction switches exist for this class.
 * 
 * @param index Geometric correction index
 * 
 * @return bool If the geometric correction switch can be accessed.
 */
bool TestCsmModel::getGeometricCorrectionSwitch(int index) const {
  throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE, "Index out of range.",
                   "TestCsmModel::getGeometricCorrectionSwitch");
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

// csm::RasterGM methods
csm::ImageCoord TestCsmModel::groundToImage(const csm::EcefCoord& groundPt,
                         double desiredPrecision,
                         double* achievedPrecision,
                         csm::WarningList* warnings) const {
  return csm::ImageCoord(0.0,0.0);
}


csm::ImageCoordCovar TestCsmModel::groundToImage(const csm::EcefCoordCovar& groundPt,
                              double desiredPrecision,
                              double* achievedPrecision,
                              csm::WarningList* warnings) const {
  return csm::ImageCoordCovar(0.0, 0.0, 0.0, 0.0, 0.0);
}
  

csm::EcefCoord TestCsmModel::imageToGround(const csm::ImageCoord& imagePt,
                               double height,
                               double desiredPrecision,
                               double* achievedPrecision,
                               csm::WarningList* warnings) const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}

csm::EcefCoordCovar TestCsmModel::imageToGround(const csm::ImageCoordCovar& imagePt,
                                    double height,
                                    double heightVariance,
                                    double desiredPrecision,
                                    double* achievedPrecision,
                                    csm::WarningList* warnings) const {
  return csm::EcefCoordCovar(0.0, 0.0, 0.0);
}


csm::EcefLocus TestCsmModel::imageToProximateImagingLocus(
  const csm::ImageCoord& imagePt,
  const csm::EcefCoord& groundPt,
  double desiredPrecision,
  double* achievedPrecision,
  csm::WarningList* warnings) const {
  return csm::EcefLocus(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}


csm::EcefLocus TestCsmModel::imageToRemoteImagingLocus(
  const csm::ImageCoord& imagePt,
  double desiredPrecision,
  double* achievedPrecision,
  csm::WarningList* warnings) const {
  return csm::EcefLocus(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}


csm::ImageCoord TestCsmModel::getImageStart() const {
  return csm::ImageCoord(0.0, 0.0);
}


csm::ImageVector TestCsmModel::getImageSize() const {
  return csm::ImageVector(0.0, 0.0);
}


std::pair<csm::ImageCoord,csm::ImageCoord> TestCsmModel::getValidImageRange() const {
  std::pair<csm::ImageCoord,csm::ImageCoord> csmPair(csm::ImageCoord(0.0, 0.0), csm::ImageCoord(0.0, 0.0));
  return csmPair;
}


std::pair<double,double> TestCsmModel::getValidHeightRange() const {
  std::pair<double, double> csmPair(0.0, 0.0);
  return csmPair;
}


csm::EcefVector TestCsmModel::getIlluminationDirection(const csm::EcefCoord& groundPt) const {
  return csm::EcefVector(0.0, 0.0, 0.0);
}


double TestCsmModel::getImageTime(const csm::ImageCoord& imagePt) const {
  return 0.0;
}


csm::EcefCoord TestCsmModel::getSensorPosition(const csm::ImageCoord& imagePt) const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}


csm::EcefCoord TestCsmModel::getSensorPosition(double time) const {
  return csm::EcefCoord(0.0, 0.0, 0.0);
}


csm::EcefVector TestCsmModel::getSensorVelocity(const csm::ImageCoord& imagePt) const {
  return csm::EcefVector(0.0, 0.0, 0.0);
}


csm::EcefVector TestCsmModel::getSensorVelocity(double time) const {
  return csm::EcefVector(0.0, 0.0, 0.0);
}


csm::RasterGM::SensorPartials TestCsmModel::computeSensorPartials(
            int index,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const {
  return csm::RasterGM::SensorPartials(0.0, 0.0);
}


csm::RasterGM::SensorPartials TestCsmModel::computeSensorPartials(
            int index,
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const {
  
  return csm::RasterGM::SensorPartials(0.0, 0.0);
}


std::vector<double> TestCsmModel::computeGroundPartials(const csm::EcefCoord& groundPt) const {
  std::vector<double> vec = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  return vec;
}

const csm::CorrelationModel& TestCsmModel::getCorrelationModel() const {
  return m_correlationModel;
}

std::vector<double> TestCsmModel::getUnmodeledCrossCovariance(
            const csm::ImageCoord& pt1,
            const csm::ImageCoord& pt2) const {
  std::vector<double> vec = {0.0, 0.0, 0.0, 0.0};
  return vec;
}

