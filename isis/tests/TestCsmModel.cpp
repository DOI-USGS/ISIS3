#include "TestCsmModel.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Sensor model Name
const std::string TestCsmModel::SENSOR_MODEL_NAME = "TestCsmModel";

// Sensor model Parameter names
const std::vector<std::string> TestCsmModel::PARAM_NAMES = {
  "center_latitude",
  "center_longitude",
  "scale", // pixels per degree
};

// Sensor model Parameter units
const std::vector<std::string> TestCsmModel::PARAM_UNITS = {
//  "unitless", // Are these used/defined in an enum or anything like this?
  "rad", // TODO: or degree?
  "rad",
  "pixels per degree",
};

// Sensor model Parameter Types
const std::vector<csm::param::Type> TestCsmModel::PARAM_TYPES = {
  csm::param::REAL,
  csm::param::REAL,
  csm::param::REAL
};

// Sensor model Parameter sharing criteria
const std::vector<csm::SharingCriteria> TestCsmModel::PARAM_SHARING_CRITERIA = {
  csm::SharingCriteria(),
  csm::SharingCriteria(),
  csm::SharingCriteria()
};


/**
 * Constructor. Resizes the parameter values list based on the number of PARAM_NAMES.
 */
TestCsmModel::TestCsmModel() {
  m_param_values.resize(TestCsmModel::PARAM_NAMES.size(), 0.0);
  m_param_sigmas.resize(TestCsmModel::PARAM_NAMES.size(), 0.0);
  m_noAdjustments = std::vector<double>(TestCsmModel::PARAM_NAMES.size(), 0.0);
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
  std::string timeString;
  timeString = "20000101T12000" + std::to_string(m_referenceTime) + "Z";
  return timeString;
}


/**
 * Returns the current model state for the sensor model.
 *
 * @return std::string model state
 */
std::string TestCsmModel::getModelState() const {
  json state;
  state["reference_time"] =  m_referenceTime;

  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    state[TestCsmModel::PARAM_NAMES[param_index]] = m_param_values[param_index];
  }

  state["center_latitude_sigma"] = m_param_sigmas[0];
  state["center_longitude_sigma"] = m_param_sigmas[1];
  state["scale_sigma"] = m_param_sigmas[2];

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

  // set reference time
  m_referenceTime = state.at("reference_time");

  // set parameter values
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    // No error-checking, but that's probably fine for now.
    m_param_values[param_index] = double(state.at(TestCsmModel::PARAM_NAMES[param_index]));
  }

  // set parameter sigmas
  for (size_t sigma_index = 0; sigma_index < m_param_sigmas.size(); sigma_index++) {
    // No error-checking, but that's probably fine for now.
    m_param_sigmas[sigma_index] = double(state.at(TestCsmModel::PARAM_NAMES[sigma_index]+"_sigma"));
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
  // TODO: modified this so no longer true. Breaks existing tests? *Only extract the first 2 parameters from the file*
  json state;
  state["reference_time"] =  parsedIsd.at("reference_time");
  for (size_t param_index = 0; param_index < m_param_values.size(); param_index++) {
    state[TestCsmModel::PARAM_NAMES[param_index]] = parsedIsd.at(TestCsmModel::PARAM_NAMES[param_index]);
  }
  state["center_latitude_sigma"] = parsedIsd.at("center_latitude_sigma");
  state["center_longitude_sigma"] = parsedIsd.at("center_longitude_sigma");
  state["scale_sigma"] = parsedIsd.at("scale_sigma");

  std::cout << "state output: " <<   TestCsmModel::SENSOR_MODEL_NAME + "\n" + state.dump() << std::endl;;
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
  // Just return variances along the diagonal
  if (index1 == index2) {
    return m_param_sigmas[index1]*m_param_sigmas[index1];
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

csm::ImageCoord TestCsmModel::groundToImage(const csm::EcefCoord& groundPt,
                         double desiredPrecision,
                         double* achievedPrecision,
                         csm::WarningList* warnings) const {

  return groundToImage(groundPt, m_noAdjustments, desiredPrecision, achievedPrecision, warnings);
}


csm::ImageCoord TestCsmModel::groundToImage(const csm::EcefCoord& groundPt, const std::vector<double> &adjustments,
                         double desiredPrecision,
                         double* achievedPrecision,
                         csm::WarningList* warnings) const {
  csm::ImageCoord imageCoord;

  double center_lat = getValue(0, adjustments);
  double center_longitude = getValue(1, adjustments);
  double scale = getValue(2, adjustments);

  double R = 1000000;
  double lat = asin(groundPt.z/R)*(180/M_PI);
  double lon = atan2(groundPt.y, groundPt.x)*(180/M_PI);

  imageCoord.samp = (lon - center_longitude)*scale + getImageSize().samp/2.0;
  imageCoord.line = (lat - center_lat)*scale + getImageSize().line/2.0;

  return imageCoord;
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
  csm::EcefCoord groundPt;

  double center_lat = m_param_values[0];
  double center_longitude = m_param_values[1];
  double scale = m_param_values[2];

  double lon = center_longitude + (imagePt.samp - getImageSize().samp/2.0)/scale;
  double lat = center_lat + (imagePt.line - getImageSize().line/2.0)/scale;
  lon *= M_PI/180;
  lat *= M_PI/180;

  double R = 1000000.0; // TODO: getfromElliposid?
  groundPt.x = R * cos(lat) * cos(lon);
  groundPt.y = R * cos(lat) * sin(lon);
  groundPt.z = R * sin(lat);

  return groundPt;
}

csm::EcefCoordCovar TestCsmModel::imageToGround(const csm::ImageCoordCovar& imagePt,
                                    double height,
                                    double heightVariance,
                                    double desiredPrecision,
                                    double* achievedPrecision,
                                    csm::WarningList* warnings) const {
  csm::EcefCoordCovar groundPt;
  return groundPt;
}


csm::EcefLocus TestCsmModel::imageToProximateImagingLocus(
  const csm::ImageCoord& imagePt,
  const csm::EcefCoord& groundPt,
  double desiredPrecision,
  double* achievedPrecision,
  csm::WarningList* warnings) const {

  // TODO: not required to implement for testing, but return the ground point as the point for the locus
  return csm::EcefLocus(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}


csm::EcefLocus TestCsmModel::imageToRemoteImagingLocus(
  const csm::ImageCoord& imagePt,
  double desiredPrecision,
  double* achievedPrecision,
  csm::WarningList* warnings) const {

  // Convert: center lat, lon, radius+altitude to x,y,z and use that for s/c position
  csm::EcefCoord sensorPosition = getSensorPosition(imagePt);

  // Look vector = scale_to_unit_vector(groundPt - sensorPosition)
  csm::EcefCoord groundPt = imageToGround(imagePt);
  std::vector<double> look(3);
  look[0] = groundPt.x - sensorPosition.x;
  look[1] = groundPt.y - sensorPosition.y;
  look[2] = groundPt.z - sensorPosition.z;
  double length = sqrt(look[0]*look[0] + look[1]*look[1] + look[2]*look[2]);

  return csm::EcefLocus(sensorPosition.x, sensorPosition.y, sensorPosition.z, look[0]/length, look[1]/length, look[2]/length);
}


csm::ImageCoord TestCsmModel::getImageStart() const {
  return csm::ImageCoord(0.0, 0.0);
}


csm::ImageVector TestCsmModel::getImageSize() const {
  // todo: should probably come from input ISD
  return csm::ImageVector(1024, 1024);
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
  // Convert: center lat, lon, radius+altitude to x,y,z and use that for s/c position
  csm::EcefCoord sensorPosition;

  double lon = m_param_values[1] * M_PI/180;
  double lat = m_param_values[0] * M_PI/180;

  double altitude = 10000; // TODO: more realistic value?
  double R = 1000000.0 + altitude; // TODO: getfromElliposid?  // only line different from imageToGround.
  sensorPosition.x = R * cos(lat) * cos(lon);
  sensorPosition.y = R * cos(lat) * sin(lon);
  sensorPosition.z = R * sin(lat);

  return sensorPosition;
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
  csm::ImageCoord imagePt = groundToImage(groundPt, desiredPrecision, achievedPrecision);
  return computeSensorPartials(index, imagePt, groundPt, desiredPrecision, achievedPrecision);
}


csm::RasterGM::SensorPartials TestCsmModel::computeSensorPartials(
            int index,
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const {

  double delta = 1.0;

  // latitude/longitude
  if (index == 2) {
    delta = 0.5;
  }
  else {
    // scale
    delta = 0.0035;
  }

  std::vector<double> parameterAdjustments(getNumParameters(), 0.0);
  parameterAdjustments[index] = delta;

  csm::ImageCoord imagePt1 = groundToImage(groundPt, parameterAdjustments, desiredPrecision, achievedPrecision);

  csm::RasterGM::SensorPartials partials;

  partials.first = (imagePt1.line - imagePt.line) / delta;
  partials.second = (imagePt1.samp - imagePt.samp) / delta;

  return partials;
}


std::vector<double> TestCsmModel::computeGroundPartials(const csm::EcefCoord& groundPt) const {
 // Partial of line, sample wrt X, Y, Z
  double x = groundPt.x;
  double y = groundPt.y;
  double z = groundPt.z;

  csm::ImageCoord ipB = groundToImage(groundPt);
  csm::EcefCoord nextPoint = imageToGround(csm::ImageCoord(ipB.line + 1, ipB.samp + 1));

  double dx = nextPoint.x - x;
  double dy = nextPoint.y - y;
  double dz = nextPoint.z - z;

  double pixelGroundSize = sqrt((dx * dx + dy * dy + dz * dz) / 2.0);
  // If the ground size is too small, try the opposite direction
  if (pixelGroundSize < 1e-10) {
    nextPoint = imageToGround(csm::ImageCoord(ipB.line - 1, ipB.samp - 1));
    dx = nextPoint.x - x;
    dy = nextPoint.y - y;
    dz = nextPoint.z - z;
    pixelGroundSize = sqrt((dx * dx + dy * dy + dz * dz) / 2.0);
  }

  csm::ImageCoord ipX = groundToImage(csm::EcefCoord(x + pixelGroundSize, y, z));
  csm::ImageCoord ipY = groundToImage(csm::EcefCoord(x, y + pixelGroundSize, z));
  csm::ImageCoord ipZ = groundToImage(csm::EcefCoord(x, y, z + pixelGroundSize));

  std::vector<double> partials(6, 0.0);
  partials[0] = (ipX.line - ipB.line) / pixelGroundSize;
  partials[3] = (ipX.samp - ipB.samp) / pixelGroundSize;
  partials[1] = (ipY.line - ipB.line) / pixelGroundSize;
  partials[4] = (ipY.samp - ipB.samp) / pixelGroundSize;
  partials[2] = (ipZ.line - ipB.line) / pixelGroundSize;
  partials[5] = (ipZ.samp - ipB.samp) / pixelGroundSize;

  return partials;
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


csm::Ellipsoid TestCsmModel::getEllipsoid() const {
  return csm::Ellipsoid(1000000, 1000000);
}


double TestCsmModel::getValue(int index, const std::vector<double> &adjustments) const {
  return m_param_values[index] + adjustments[index];
}
