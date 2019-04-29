#include "BundleObservation.h"

// qt lib
#include <QDebug>
#include <QList>
#include <QString>
#include <QStringList>

// boost lib
//#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

// Isis lib
#include "BundleImage.h"
#include "BundleMeasure.h"
#include "BundlePolynomialContinuityConstraint.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "CameraGroundMap.h"
//#include "LinearAlgebra.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

using namespace std;
using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Constructs a BundleObservation initialized to a default state.
   */
  BundleObservation::BundleObservation() {
    m_serialNumbers.clear();
    m_imageNames.clear();
    m_parameterNamesList.clear();
    m_observationNumber = "";
    m_instrumentId = "";
    m_instrumentRotation = NULL;
    m_instrumentPosition = NULL;
    m_index = 0;
    m_weights.clear();
    m_corrections.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();
  }


  /**
   * Constructs a BundleObservation from an BundleImage, an instrument id, an observation
   * number to assign to this BundleObservation, and a target body.
   *
   * @param image QSharedPointer to the primary image in the observation  
   * @param observationNumber Observation number of the observation
   * @param instrumentId Id of the instrument for the observation
   * @param bundleTargetBody QSharedPointer to the target body of the observation
   */
  BundleObservation::BundleObservation(BundleImageQsp image, QString observationNumber,
                                       QString instrumentId, BundleTargetBodyQsp bundleTargetBody) {
    m_serialNumbers.clear();
    m_imageNames.clear();
    m_parameterNamesList.clear();
    m_observationNumber = "";
    m_instrumentId = "";
    m_instrumentRotation = NULL;
    m_instrumentPosition = NULL;
    m_index = 0;
    m_weights.clear();
    m_corrections.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();

    m_observationNumber = observationNumber;
    m_instrumentId = instrumentId;

    m_bundleTargetBody = bundleTargetBody;

    if (image) {
      append(image);
      m_serialNumbers.append(image->serialNumber());
      m_imageNames.append(image->fileName());
      m_cubeSerialNumberToBundleImageMap.insert(image->serialNumber(), image);

      // set the observations spice position and rotation objects from the primary image in the
      // observation (this is, by design at the moment, the first image added to the observation)
// TODO: comment below is wrong I think, also, I don't like this form of if/then/else in terms of
//       readability
      // if the image, camera, or instrument position/orientation is null, then set to null
      // these will be updated during the bundle adjustment
      m_instrumentPosition = (image->camera() ? 
                               (image->camera()->instrumentPosition() ?
                                 image->camera()->instrumentPosition() : NULL)
                               : NULL);
      m_instrumentRotation = (image->camera() ? 
                               (image->camera()->instrumentRotation() ?
                                  image->camera()->instrumentRotation() : NULL)
                               : NULL);

      // set the observations target body spice rotation object from the primary image in the
      // observation (this is, by design at the moment, the first image added to the observation)
      // if the image, camera, or instrument position/orientation is null, then set to null
//      m_bodyRotation = (image->camera() ?
//                           (image->camera()->bodyRotation() ?
//                             image->camera()->bodyRotation() : NULL)
//                           : NULL);
    }
  }


  /**
   * Creates a copy of another BundleObservation.
   *
   * @param src Reference to the BundleObservation to copy
   */
  BundleObservation::BundleObservation(const BundleObservation &src) {
    m_serialNumbers = src.m_serialNumbers;
    m_cubeSerialNumberToBundleImageMap = src.m_cubeSerialNumberToBundleImageMap;

    m_observationNumber = src.m_observationNumber;
    m_instrumentId = src.m_instrumentId;

    m_instrumentPosition = src.m_instrumentPosition;
    m_instrumentRotation = src.m_instrumentRotation;

    m_solveSettings = src.m_solveSettings;

    m_continuityConstraints = src.m_continuityConstraints;

    m_index = src.m_index;
  }


  /**
   * Destructor.
   *
   * Contained BundleImages will remain until all shared pointers are deleted.
   */
  BundleObservation::~BundleObservation() {
    clear();
  }


  /**
   * Assignment operator
   *
   * Assigns the state of the source BundleObservation to this BundleObservation
   *
   * @param BundleObservation Reference to the source BundleObservation to assign from
   *
   * @return BundleObservation& Reference to this BundleObservation
   */
  BundleObservation &BundleObservation::operator=(const BundleObservation &src) {
    if (&src != this) {
      m_serialNumbers = src.m_serialNumbers;
      m_cubeSerialNumberToBundleImageMap = src.m_cubeSerialNumberToBundleImageMap;

      m_observationNumber = src.m_observationNumber;
      m_instrumentId = src.m_instrumentId;

      m_instrumentPosition = src.m_instrumentPosition;
      m_instrumentRotation = src.m_instrumentRotation;

      m_solveSettings = src.m_solveSettings;

      m_continuityConstraints = src.m_continuityConstraints;
    }
    return *this;
  }


  /**
   * Appends a BundleImage shared pointer to the BundleObservation.
   * If the pointer is valid, then the BundleImage and its serial number will be inserted into
   * the serial number to BundleImage map.
   * 
   * @param image The BundleImage to be appended.
   * 
   * @see QVector::append()
   */
  void BundleObservation::append(const BundleImageQsp &image) {
    if (!image) {
      return;
    }

    // if this is a multi-image observation, we have to apply the observations exterior orientation
    // to this new image
    if (size() > 0) {

      std::vector<double> poly1, poly2, poly3;

      // update spice position of new image to match observations primary image
      SpicePosition *spicePosition = image->camera()->instrumentPosition();

      double positionBaseTime = m_instrumentPosition->GetBaseTime();
      double positiontimeScale = m_instrumentPosition->GetTimeScale();
      m_instrumentPosition->GetPolynomial(poly1, poly2, poly3);

      // set number of position segments in images spice position
      int spkPolynomialSegments = m_instrumentPosition->numPolynomialSegments();
      spicePosition->setPolynomialSegments(spkPolynomialSegments);

      // loop over position segments
      for (int i = 0; i < spkPolynomialSegments; i++) {
        spicePosition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());
        spicePosition->SetOverrideBaseTime(positionBaseTime, positiontimeScale);
        spicePosition->SetPolynomial(poly1, poly2, poly3,
                                     m_solveSettings->positionInterpolationType(),i);
      }

      // update spice rotation of new image to match observations primary image
      SpiceRotation *spiceRotation = image->camera()->instrumentRotation();

      double rotationBaseTime = m_instrumentRotation->GetBaseTime();
      double rotationtimeScale = m_instrumentRotation->GetTimeScale();
      m_instrumentRotation->GetPolynomial(poly1, poly2, poly3);

      // set number of rotation segments in images spice position
      int ckPolynomialSegments = m_instrumentRotation->numPolynomialSegments();
      spiceRotation->setPolynomialSegments(ckPolynomialSegments);

      // loop over rotation segments
      for (int i = 0; i < ckPolynomialSegments; i++) {
        spiceRotation->SetPolynomialDegree(m_solveSettings->ckSolveDegree());
        spiceRotation->SetOverrideBaseTime(rotationBaseTime, rotationtimeScale);
        spiceRotation->SetPolynomial(poly1, poly2, poly3,
                                     m_solveSettings->pointingInterpolationType(),i);
      }
    }

    m_cubeSerialNumberToBundleImageMap.insert(image->serialNumber(), image);

    QVector<BundleImageQsp>::append(image);
  }


  /**
   * Returns the BundleImage shared pointer associated with the given serial number.
   * If no BundleImage with that serial number is contained a NULL pointer is returned.
   * 
   * @param cubeSerialNumber The serial number of the cube to be returned.
   * 
   * @return BundleImageQsp A shared pointer to the BundleImage (NULL if not found).
   */
  BundleImageQsp BundleObservation::imageByCubeSerialNumber(QString cubeSerialNumber) {
    BundleImageQsp bundleImage;

    if (m_cubeSerialNumberToBundleImageMap.contains(cubeSerialNumber)) {
      bundleImage = m_cubeSerialNumberToBundleImageMap.value(cubeSerialNumber);
    }

    return bundleImage;
  }


  /**
   * Set solve parameters
   *
   * @param solveSettings The solve settings to use
   *
   * @return bool Returns true if settings were successfully set
   *
   * @internal
   *   @todo initParameterWeights() doesn't return false, so this methods always 
   *         returns true.
   */
  bool BundleObservation::setSolveSettings(BundleObservationSolveSettings solveSettings) {
    m_solveSettings.reset( new BundleObservationSolveSettings(solveSettings) );

    // initialize solution parameters for this observation
    int nCameraAngleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();
    int nCameraPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();

    nCameraAngleCoefficients *= m_solveSettings->numberCkPolySegments();
    nCameraPositionCoefficients *= m_solveSettings->numberSpkPolySegments();

    int nParameters = 3*nCameraPositionCoefficients + 2*nCameraAngleCoefficients;
    if (nCameraAngleCoefficients >= 1 && m_solveSettings->solveTwist()) {
      nParameters += nCameraAngleCoefficients;
    }

    // set number of position and rotation piecewise polynomial segments in SPICE position and
    // rotation members
    if (m_instrumentPosition) {
      m_instrumentPosition->setPolynomialSegments(m_solveSettings->numberSpkPolySegments());
    }
    if (m_instrumentRotation) {
      m_instrumentRotation->setPolynomialSegments(m_solveSettings->numberCkPolySegments());
    }

    // size vectors and set to zero
    m_weights.resize(nParameters);
    m_weights.clear();
    m_corrections.resize(nParameters);
    m_corrections.clear();
    m_adjustedSigmas.resize(nParameters);
    m_adjustedSigmas.clear();
    m_aprioriSigmas.resize(nParameters);
    for ( int i = 0; i < nParameters; i++) // initialize apriori sigmas to Isis::Null
      m_aprioriSigmas[i] = Isis::Null;

    if (!initParameterWeights()) {
      // TODO: some message here!!!!!!!!!!!
      // TODO:  do we need this??? initParameterWeights() never returns false...
      return false;
    }

    return true;
  }


  /**
   * Sets this BundleObserations continuity constraint member variable
   *
   * @param polyConstraints piecewise polynomial continuity constraints
   *
   */
  void BundleObservation::setContinuityConstraints(BundlePolynomialContinuityConstraintQsp
                                                   polyConstraints) {
    m_continuityConstraints = polyConstraints;
  }


  /**
   * Returns position contribution of continuity constraints
   *
   * @return SparseBlockMatrix Returns position contribution of continuity constraints to the
   *                              bundle adjustment normal equations
   */
  SparseBlockMatrix &BundleObservation::continuityContraintSpkMatrix() {
    return m_continuityConstraints->normalsSpkMatrix();
  }


  /**
   * Returns pointing contribution of continuity constraints
   *
   * @return SparseBlockMatrix Returns pointing contribution of continuity constraints to the
   *                              bundle adjustment normal equations
   */
  SparseBlockMatrix &BundleObservation::continuityContraintCkMatrix() {
    return m_continuityConstraints->normalsCkMatrix();
  }


  /**
   * Returns contribution of continuity constraints to normal equations right hand side
   *
   * @return LinearAlgebra::Vector Returns contribution of continuity constraints to the normal
   *                                  equation hand side
   */
  LinearAlgebra::Vector &BundleObservation::continuityRHS() {
    return m_continuityConstraints->rightHandSideVector();
  }


  /**
   * Accesses the instrument id
   *
   * @return QString Returns the instrument id of the observation
   */
  QString BundleObservation::instrumentId() {
    return m_instrumentId;
  }


  /**
   * Accesses the instrument's spice rotation
   *
   * @return SpiceRotation* Returns the SpiceRotation for this observation
   */
  SpiceRotation *BundleObservation::spiceRotation() {
    return m_instrumentRotation;
  }


  /**
   * Accesses the instrument's spice position 
   *
   * @return SpicePosition* Returns the SpicePosition for this observation
   */
  SpicePosition *BundleObservation::spicePosition() {
    return m_instrumentPosition;
  }


  /**
   * Accesses the solve parameter weights
   * 
   * @return LinearAlgebra::Vector Returns the parameter weights for solving
   */
  LinearAlgebra::Vector &BundleObservation::parameterWeights() {
    return m_weights;
  }


  /**
   * Accesses the parameter corrections 
   *
   * @return LinearAlgebra::Vector Returns the parameter corrections
   */
  LinearAlgebra::Vector &BundleObservation::parameterCorrections() {
    return m_corrections;
  }


  /**
   * Accesses the a priori sigmas
   *
   * @return LinearAlgebra::Vector Returns the a priori sigmas
   */
  LinearAlgebra::Vector &BundleObservation::aprioriSigmas() {
    return m_aprioriSigmas;
  }


  /**
   * Accesses the adjusted sigmas 
   *
   * @return LinearAlgebra::Vector Returns the adjusted sigmas
   */
  LinearAlgebra::Vector &BundleObservation::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * Accesses the solve settings
   *
   * @return const BundleObservationSolveSettingsQsp Returns a pointer to the solve
   *                                                    settings for this BundleObservation
   */
  const BundleObservationSolveSettingsQsp BundleObservation::solveSettings() { 
    return m_solveSettings;
  }


  /**
   * Initializes the BundleObservation's exterior orientation from the primary image in the
   * observation. Currently this is by default considered to be the first image added to the
   * observation.
   *
   * @return bool Returns true upon successful initialization
   */
  bool BundleObservation::initializeExteriorOrientation() {
    if (size() != 1) {
      return false;
    }

    if (m_solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      // number of position polynomial segments we're solving for this observation
      int spkPolynomialSegments = m_solveSettings->numberSpkPolySegments();

      // set number of position segments in images spice position
      m_instrumentPosition->setPolynomialSegments(spkPolynomialSegments);

      // first, set degree of the spk polynomial to be fit for a priori values
      m_instrumentPosition->SetPolynomialDegree(m_solveSettings->spkDegree());

      // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
      // function, or polynomial function over constant hermite spline)
      // TODO: verify - think this actually performs the a priori fit
      m_instrumentPosition->SetPolynomial(m_solveSettings->positionInterpolationType());

      // finally, set the degree of the spk polynomial actually used in the bundle adjustment
      m_instrumentPosition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());
    }

    if (m_solveSettings->instrumentPointingSolveOption() !=
        BundleObservationSolveSettings::NoPointingFactors) {

      // number of pointing polynomial segments we're solving for this observation
      int ckPolynomialSegments = m_solveSettings->numberCkPolySegments();

      // set number of rotation segments in images spice position
      m_instrumentRotation->setPolynomialSegments(ckPolynomialSegments);

      // first, set the degree of the ck polynomial to be fit for a priori values
      m_instrumentRotation->SetPolynomialDegree(m_solveSettings->ckDegree());

      // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
      // function, or polynomial function over constant hermite spline)
      // TODO: verify - I think this actually performs the a priori fit
      m_instrumentRotation->SetPolynomial(m_solveSettings->pointingInterpolationType());

      // finally, set the degree of the ck polynomial actually used in the bundle adjustment
      m_instrumentRotation->SetPolynomialDegree(m_solveSettings->ckSolveDegree());
    }

    return true;
  }


  /**
   * Sets the body rotation
   *
   * @todo check to make sure m_bundleTargetBody is valid
   */
  void BundleObservation::setBodyRotation() {
    std::vector<Angle> raCoefs = m_bundleTargetBody->poleRaCoefs();
    std::vector<Angle> decCoefs = m_bundleTargetBody->poleDecCoefs();
    std::vector<Angle> pmCoefs = m_bundleTargetBody->pmCoefs();

    for (int i = 0; i < size(); i++) {
      at(i)->camera()->bodyRotation()->setPckPolynomial(raCoefs, decCoefs, pmCoefs);
    }
  }


  /**
   * Initializes the parameter weights for solving
   * 
   * @return bool Returns true upon successful initialization
   *
   * @internal  
   *   @todo Don't like this, don't like this, don't like this, don't like this, don't like this.
   *         By the way, this seems klunky to me, would like to come up with a better way.
   *         Also, apriori sigmas are in two places, the BundleObservationSolveSettings AND in the
   *         the BundleObservation class too - this is unnecessary should only be in the
   *         BundleObservationSolveSettings. But, they are split into position and pointing.
   *
   *   @todo always returns true?
   */
  bool BundleObservation::initParameterWeights() {

                                   // weights for
    double posWeight    = 0.0;     // position
    double velWeight    = 0.0;     // velocity
    double accWeight    = 0.0;     // acceleration
    double angWeight    = 0.0;     // angles
    double angVelWeight = 0.0;     // angular velocity
    double angAccWeight = 0.0;     // angular acceleration

    QList<double> aprioriPointingSigmas = m_solveSettings->aprioriPointingSigmas();
    QList<double> aprioriPositionSigmas = m_solveSettings->aprioriPositionSigmas();

    int nCamPosCoeffsSolved = 3  *m_solveSettings->numberCameraPositionCoefficientsSolved();

    int nCamAngleCoeffsSolved;
    if (m_solveSettings->solveTwist()) {
      nCamAngleCoeffsSolved = 3  *m_solveSettings->numberCameraAngleCoefficientsSolved();
    }
    else {
      nCamAngleCoeffsSolved = 2  *m_solveSettings->numberCameraAngleCoefficientsSolved();
    }

    if (aprioriPositionSigmas.size() >= 1 && aprioriPositionSigmas.at(0) > 0.0) {
      posWeight = aprioriPositionSigmas.at(0);
      posWeight = 1.0 / (posWeight  *posWeight * 1.0e-6);
    }
    if (aprioriPositionSigmas.size() >= 2 && aprioriPositionSigmas.at(1) > 0.0) {
      velWeight = aprioriPositionSigmas.at(1);
      velWeight = 1.0 / (velWeight  *velWeight * 1.0e-6);
    }
    if (aprioriPositionSigmas.size() >= 3 && aprioriPositionSigmas.at(2) > 0.0) {
      accWeight = aprioriPositionSigmas.at(2);
      accWeight = 1.0 / (accWeight  *accWeight * 1.0e-6);
    }

    if (aprioriPointingSigmas.size() >= 1 && aprioriPointingSigmas.at(0) > 0.0) {
      angWeight = aprioriPointingSigmas.at(0);
      angWeight = 1.0 / (angWeight  *angWeight * DEG2RAD * DEG2RAD);
    }
    if (aprioriPointingSigmas.size() >= 2 && aprioriPointingSigmas.at(1) > 0.0) {
      angVelWeight = aprioriPointingSigmas.at(1);
      angVelWeight = 1.0 / (angVelWeight * angVelWeight * DEG2RAD * DEG2RAD);
    }
    if (aprioriPointingSigmas.size() >= 3 && aprioriPointingSigmas.at(2) > 0.0) {
      angAccWeight = aprioriPointingSigmas.at(2);
      angAccWeight = 1.0 / (angAccWeight * angAccWeight * DEG2RAD * DEG2RAD);
    }

    int numSpkSegments = m_solveSettings->numberSpkPolySegments();
    int nSpkTerms = m_solveSettings->spkSolveDegree()+1;
    nSpkTerms = m_solveSettings->numberCameraPositionCoefficientsSolved();

    int index = 0;
    for (int i = 0; i < numSpkSegments; i++) {
      for ( int j = 0; j < nCamPosCoeffsSolved; j++) {
        if (j % nSpkTerms == 0) {
         m_aprioriSigmas[index] = aprioriPositionSigmas.at(0);
         m_weights[index] = posWeight;
        }
        if (j % nSpkTerms == 1) {
         m_aprioriSigmas[index] = aprioriPositionSigmas.at(1);
         m_weights[index] = velWeight;
        }
        if (j % nSpkTerms == 2) {
         m_aprioriSigmas[index] = aprioriPositionSigmas.at(2);
         m_weights[index] = accWeight;
        }
        index++;
      }
    }

    int numCkSegments = m_solveSettings->numberCkPolySegments();
    int nCkTerms = m_solveSettings->ckSolveDegree()+1;
    nCkTerms = m_solveSettings->numberCameraAngleCoefficientsSolved();

    for (int i = 0; i < numCkSegments; i++) {
      for ( int j = 0; j < nCamAngleCoeffsSolved; j++) {
        if (j % nCkTerms == 0) {
          m_aprioriSigmas[index] = aprioriPointingSigmas.at(0);
          m_weights[index] = angWeight;
        }
        if (j % nCkTerms == 1) {
          m_aprioriSigmas[index] = aprioriPointingSigmas.at(1);
          m_weights[index] = angVelWeight;
        }
        if (j % nCkTerms == 2) {
          m_aprioriSigmas[index] = aprioriPointingSigmas.at(2);
          m_weights[index] = angAccWeight;
        }
        index++;
      }
    }

    return true;
  }


  /**
   * Computes bundle adjustment partial derivatives for this observation.
   *
   * NOTE: this is different from previous version in that we are using two separate matrices for
   *       position and pointing partials as opposed to one matrix containing both.
   *
   * @param coeffImagePosition Matrix of partial derivatives for position parameters.
   * @param coeffImagePointing Matrix of partial derivatives for pointing parameters.
   * @param camera Camera. This will vary for each image in the observation.
   *
   */
void BundleObservation::computePartials(LinearAlgebra::Matrix &coeffImagePosition,
                                        LinearAlgebra::Matrix &coeffImagePointing,
                                        Camera &camera) {

  // get numbers of spk and ck coefficients being solved
  int numCamPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();
  int numCamAngleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    // resize coeffImagePosition matrix if necessary
    if ((int) coeffImagePosition.size2() != positionSegmentSize()) {
      coeffImagePosition.resize(2, positionSegmentSize(), false);
    }

    // resize coeffImagePointing matrix if necessary
    if ((int) coeffImagePointing.size2() != pointingSegmentSize()) {
      coeffImagePointing.resize(2, pointingSegmentSize(), false);
    }

    if (m_solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      int index = 0;

      // Add the partial for the x coordinate of the position (differentiating
      // point(x,y,z) - spacecraftPosition(x,y,z) in J2000
      for (int cameraCoef = 0; cameraCoef < numCamPositionCoefficients; cameraCoef++) {
        camera.GroundMap()->GetdXYdPosition(SpicePosition::WRT_X, cameraCoef,
                                            &coeffImagePosition(0, index),
                                            &coeffImagePosition(1, index));
        index++;
      }

      // Add the partial for the y coordinate of the position
      for (int cameraCoef = 0; cameraCoef < numCamPositionCoefficients; cameraCoef++) {
        camera.GroundMap()->GetdXYdPosition(SpicePosition::WRT_Y, cameraCoef,
                                            &coeffImagePosition(0, index),
                                            &coeffImagePosition(1, index));
        index++;
      }

      // Add the partial for the z coordinate of the position
      for (int cameraCoef = 0; cameraCoef < numCamPositionCoefficients; cameraCoef++) {
        camera.GroundMap()->GetdXYdPosition(SpicePosition::WRT_Z, cameraCoef,
                                            &coeffImagePosition(0, index),
                                            &coeffImagePosition(1, index));
        index++;
      }
    }

    if (m_solveSettings->instrumentPointingSolveOption() !=
        BundleObservationSolveSettings::NoPointingFactors) {

      int index = 0;

      // Add the partials for ra
      for (int cameraCoef = 0; cameraCoef < numCamAngleCoefficients; cameraCoef++) {
        camera.GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_RightAscension, cameraCoef,
                                               &coeffImagePointing(0, index),
                                               &coeffImagePointing(1, index));
        index++;
      }

      // Add the partials for dec
      for (int cameraCoef = 0; cameraCoef < numCamAngleCoefficients; cameraCoef++) {
        camera.GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Declination, cameraCoef,
                                               &coeffImagePointing(0, index),
                                               &coeffImagePointing(1, index));
        index++;
      }

      // Add the partial for twist if necessary
      if (m_solveSettings->solveTwist()) {
        for (int cameraCoef = 0; cameraCoef < numCamAngleCoefficients; cameraCoef++) {
          camera.GroundMap()->GetdXYdOrientation(SpiceRotation::WRT_Twist, cameraCoef,
                                                 &coeffImagePointing(0, index),
                                                 &coeffImagePointing(1, index));
          index++;
        }
      }
    }
  }


/**
   * Applies the parameter corrections 
   *
   * @param corrections Vector of corrections to apply
   *
   * @throws IException::Unknown "Instrument position is NULL, but position solve option is 
   *                              [not NoPositionFactors]"
   * @throws IException::Unknown "Instrument position is NULL, but pointing solve option is
   *                              [not NoPointingFactors]"
   * @throws IException::Unknown "Unable to apply parameter corrections to BundleObservation."
   *
   * @return bool Returns true upon successful application of corrections
   *
   * @internal
   *   @todo always returns true?
   */  
  bool BundleObservation::applyParameterCorrections(LinearAlgebra::Vector corrections,
                                                    bool updateBodyRotation) {

    int index = 0;

    try {
      int nCameraAngleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();
      int nCameraPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();

      BundleObservationSolveSettings::InstrumentPositionSolveOption positionOption
          = m_solveSettings->instrumentPositionSolveOption();
      if (positionOption != BundleObservationSolveSettings::NoPositionFactors) {

        if (!m_instrumentPosition) {
          QString msg = "Instrument position is NULL, but position solve option is ";
          msg.append(BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                     positionOption));
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        int numSpkSegments = m_solveSettings->numberSpkPolySegments();

        std::vector<double> coefX(nCameraPositionCoefficients);
        std::vector<double> coefY(nCameraPositionCoefficients);
        std::vector<double> coefZ(nCameraPositionCoefficients);

        // loop over segments
        for (int i = 0; i < numSpkSegments; i++) {

          // get spk polynomial for segment i
          m_instrumentPosition->GetPolynomial(coefX, coefY, coefZ, i);

          // update X coordinate coefficient(s) and sum parameter correction
          for (int j = 0; j < nCameraPositionCoefficients; j++) {
            coefX[j] += corrections(index);
            index++;
          }

          // update Y coordinate coefficient(s) and sum parameter correction
          for (int j = 0; j < nCameraPositionCoefficients; j++) {
            coefY[j] += corrections(index);
            index++;
          }

          // update Z coordinate coefficient(s) and sum parameter correction
          for (int j = 0; j < nCameraPositionCoefficients; j++) {
            coefZ[j] += corrections(index);
            index++;
          }

          // apply updates for segment i to all images in observation
          for (int k = 0; k < size(); k++) {
            BundleImageQsp image = at(k);
            SpicePosition *spiceposition = image->camera()->instrumentPosition();
            spiceposition->SetPolynomial(coefX, coefY, coefZ,
                                         m_solveSettings->positionInterpolationType(), i);
          }
        }
      }

      BundleObservationSolveSettings::InstrumentPointingSolveOption pointingOption
          = m_solveSettings->instrumentPointingSolveOption();
      if (pointingOption != BundleObservationSolveSettings::NoPointingFactors) {

        if (!m_instrumentRotation) {
          QString msg = "Instrument rotation is NULL, but pointing solve option is ";
          msg.append(BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                     pointingOption));
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        int numCkSegments = m_solveSettings->numberCkPolySegments();

        std::vector<double> coefRA(nCameraAngleCoefficients);
        std::vector<double> coefDEC(nCameraAngleCoefficients);
        std::vector<double> coefTWI(nCameraAngleCoefficients);

        // loop over segments
        for (int i = 0; i < numCkSegments; i++) {

          // get spk polynomial for segment i
          m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI, i);

          // update RA coordinate coefficient(s) and sum parameter correction
          for (int j = 0; j < nCameraAngleCoefficients; j++) {
            coefRA[j] += corrections(index);
            index++;
          }

          // update DEC coefficient(s)
          for (int j = 0; j < nCameraAngleCoefficients; j++) {
            coefDEC[j] += corrections(index);
            index++;
          }

          if (m_solveSettings->solveTwist()) {
            // update TWIST coefficient(s)
            for (int j = 0; j < nCameraAngleCoefficients; j++) {
              coefTWI[j] += corrections(index);
              index++;
            }
          }

          // apply updates to all images in observation
          for (int k = 0; k < size(); k++) {
            BundleImageQsp image = at(k);
            SpiceRotation *spiceRotation = image->camera()->instrumentRotation();
            spiceRotation->SetPolynomial(coefRA, coefDEC, coefTWI,
                                         m_solveSettings->pointingInterpolationType(), i);
          }
        }
      }

      // update corrections
      m_corrections += corrections;
    } 
    catch (IException &e) {
      QString msg = "Unable to apply parameter corrections to BundleObservation.";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }

    try {
      // TODO: temporary location for this?
      if (m_continuityConstraints) {
//        clock_t updateContinuityRHSClock1 = clock();

        m_continuityConstraints->updateRightHandSide();

//        clock_t updateContinuityRHSClock2 = clock();

//        double updateContinuityRHS = (updateContinuityRHSClock2 - updateContinuityRHSClock1)
//            / (double)CLOCKS_PER_SEC;

//        qDebug() << "update Continuity RHS: " << updateContinuityRHS;
      }
    }
    catch (IException &e) {
      QString msg = "Unable to update continuity constraint right hand side for BundleObservation.";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }

    if (updateBodyRotation) {
      setBodyRotation();
    }


    return true;
  }


  /**
   * Applies piecewise polynomial continuity constraints between segments
   *
   * @param diagonalBlock diagonal block of normal equations matrix corresponding to this
   *                      observation
   * @param rhs diagonal portion of right hand side vector of normal equations corresponding to this
   *                     observation
   */
//  void BundleObservation::applyContinuityConstraints(LinearAlgebra::Matrix *diagonalBlock,
//                                                     LinearAlgebra::Vector &rhs) {

//    *diagonalBlock += m_continuityConstraints->normalsMatrix();
//    rhs += m_continuityConstraints->rightHandSideVector();
//  }


  /**
   * Returns the number of position parameters there are
   *
   * @return int Returns the number of position parameters
   */
  int BundleObservation::numberPositionParameters() {
    int numSegments;
    if(m_instrumentPosition) {
      numSegments = m_instrumentPosition->numPolynomialSegments();
    }
    else {
      numSegments = m_solveSettings->numberSpkPolySegments();
    }
    int numCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();

    return 3.0 * numSegments * numCoefficients;
  }


  /**
   * Returns the number of position parameters there are per segment
   *
   * @return int Returns the number of position parameters
   */
  int BundleObservation::numberPositionParametersPerSegment() {
    return 3.0 * m_solveSettings->numberCameraPositionCoefficientsSolved();
  }


  /**
   * Returns the number of pointing parameters being solved for
   *
   * @return int Returns the number of pointing parameters
   */
  int BundleObservation::numberPointingParameters() {
    int numSegments;
    if (m_instrumentRotation) {
      numSegments = m_instrumentRotation->numPolynomialSegments();
    }
    else {
      numSegments = m_solveSettings->numberCkPolySegments();
    }
    int numCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    if (!m_solveSettings->solveTwist()) {
      return 2.0 * numSegments * numCoefficients;
    }

    return 3.0 * numSegments * numCoefficients;
  }


  /**
   * Returns the number of pointing parameters per segment
   *
   * @return int Returns number of pointing parameters per segment
   */
  int BundleObservation::numberPointingParametersPerSegment() {
    int numCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    if (!m_solveSettings->solveTwist()) {
      return 2.0 * numCoefficients;
    }

    return 3.0 * numCoefficients;
  }



  /**
   * Returns the number of total parameters solved for this observation
   *
   * The total number of parameters is equal to the number of position parameters and number of
   * pointing parameters
   *
   * @return int Returns number of parameters solved for this observation
   */
  int BundleObservation::numberParameters() {
    return numberPositionParameters() + numberPointingParameters();
  }

  /**
   * Returns the number of piecewise polynomial position segments
   *
   * @return int Returns number of piecewise polynomial position segments
   */
  int BundleObservation::numberPolynomialPositionSegments() {
    if (m_instrumentPosition) {
      return m_instrumentPosition->numPolynomialSegments();
    }
    return m_solveSettings->numberSpkPolySegments();
  }


  /**
   * Returns the number of piecewise polynomial pointing segments
   *
   * @return int Returns number of piecewise polynomial pointing segments
   */
  int BundleObservation::numberPolynomialPointingSegments() {
    if (m_instrumentRotation) {
      return m_instrumentRotation->numPolynomialSegments();
    }
    return m_solveSettings->numberCkPolySegments();
  }


  /**
   * Returns total number of piecewise polynomial segments
   *
   * Total number of segments is the sum of position and pointing segments
   *
   * @return int Returns total number of piecewise polynomial segments
   */
  int BundleObservation::numberPolynomialSegments() {
    return (numberPolynomialPositionSegments() +
            numberPolynomialPointingSegments());
  }


  /**
   * Returns size of polynomial position segment
   *
   * @return int Returns size of polynomial position segment
   */
  int BundleObservation::positionSegmentSize() {
    return 3.0 * m_solveSettings->numberCameraPositionCoefficientsSolved();
  }


  /**
   * Returns size of polynomial pointing segment
   *
   * @return int Returns size of polynomial pointing segment
   */
  int BundleObservation::pointingSegmentSize() {

    if (m_solveSettings->solveTwist()) {
      return 3.0 * m_solveSettings->numberCameraAngleCoefficientsSolved();
    }

    return 2.0 * m_solveSettings->numberCameraAngleCoefficientsSolved();
  }


  /**
   * Returns current position segment index by ephemeris time
   *
   * @return int Returns current position segment index by ephemeris time
   */
  int BundleObservation::polyPositionSegmentIndex() {
    return m_instrumentPosition->polySegmentIndex(m_instrumentPosition->EphemerisTime());
  }


  /**
   * Returns current rotation segment index by ephemeris time
   *
   * @return int Returns current rotation segment index by ephemeris time
   */
  int BundleObservation::polyRotationSegmentIndex() {
    return m_instrumentRotation->polySegmentIndex(m_instrumentRotation->EphemerisTime());
  }


  /**
   * Returns total number of piecewise polynomial continuity constraint equations
   * used in the adjustment
   *
   * @return int Returns total number of piecewise polynomial continuity constraint equations
   */
  int BundleObservation::numberContinuityConstraints() const {
    if (m_continuityConstraints) {
      return m_continuityConstraints->numberConstraintEquations();
    }
    return 0;
  }


  /**
   * Sets the index for the observation
   *
   * @param n Value to set the index of the observation to
   */
  void BundleObservation::setIndex(int n) {
    m_index = n;
  }


  /**
   * Sets the starting matrix block in the normal equations matrix for this observation
   *
   * @param startBlock Value to set the index of the observation to
   */

  void BundleObservation::setNormalsMatrixStartBlock(int startBlock) {
    m_normalsEquationsStartBlock = startBlock;
  }


  /**
   * Accesses the starting matrix block in the normal equations matrix for this observation
   *
   * @return int Returns starting matrix block index in the normal equations matrix for this
   *                observation
   */

  int BundleObservation::normalsMatrixStartBlock() {
    return m_normalsEquationsStartBlock;
  }


  /**
   * Accesses the observation's index
   *
   * @return int Returns the observation's index
   */
  int BundleObservation::index() {
    return m_index;
  }

  /**
   * @brief Creates and returns a formatted QString representing the bundle coefficients and
   * parameters
   *
   * @param errorPropagation Boolean indicating whether or not to attach more information
   *     (corrections, sigmas, adjusted sigmas...) to the output QString
   * @param imageCSV Boolean which is set to true if the function is being
   *     called from BundleSolutionInfo::outputImagesCSV().  It is set to false by default
   *     for backwards compatibility.
   *
   * @return QString Returns a formatted QString representing the BundleObservation
   *
   * @internal
   *   @history 2016-10-26 Ian Humphrey - Default values are now provided for parameters that are
   *                           not being solved. Fixes #4464.
   */
  QString BundleObservation::formatBundleContinuityConstraintString() {
    return m_continuityConstraints->formatBundleOutputString();
  }


  /**
   * Format the position segment header for bundle output files
   * 
   * @param segmentIndex The index of the position segment to format the
   *                     header for
   * 
   * @return QString The formatted segment header with segment number,
   *                    start time, and stop time.
   */
  QString BundleObservation::formatPositionSegmentHeader(int segmentIndex) {
    QString templateString = "\n    Position Segment Number: %1\n"
                             "         Segment Start Time: %2\n"
                             "           Segment End Time: %3\n";
    if (m_instrumentPosition) {
      std::vector<double> polyKnots = m_instrumentPosition->polynomialKnots();
      if (polyKnots.front() != -DBL_MAX &&
          polyKnots.back() != DBL_MAX) {
        return templateString.arg(segmentIndex + 1)
                             .arg(polyKnots[segmentIndex], -20, 'f', 5)
                             .arg(polyKnots[segmentIndex + 1], -20, 'f', 5);
      }
    }
    return templateString.arg(segmentIndex + 1)
                         .arg("N/A")
                         .arg("N/A");
  }


  /**
   * Format the pointing segment header for bundle output files
   * 
   * @param segmentIndex The index of the pointing segment to format the
   *                     header for
   * 
   * @return QString The formatted segment header with segment number,
   *                    start time, and stop time.
   */
  QString BundleObservation::formatPointingSegmentHeader(int segmentIndex) {
    QString templateString = "\n    Pointing Segment Number: %1\n"
                             "         Segment Start Time: %2\n"
                             "           Segment End Time: %3\n";
    if (m_instrumentRotation) {
      std::vector<double> polyKnots = m_instrumentRotation->polynomialKnots();
      if (polyKnots.front() != -DBL_MAX &&
          polyKnots.back() != DBL_MAX) {
        return templateString.arg(segmentIndex + 1)
                             .arg(polyKnots[segmentIndex], -20, 'f', 5)
                             .arg(polyKnots[segmentIndex + 1], -20, 'f', 5);
      }
    }
    return templateString.arg(segmentIndex + 1)
                         .arg("N/A")
                         .arg("N/A");
  }


  /**
   * @brief Creates and returns a formatted QString representing the position coefficients and
   * parameters
   *
   * @param segmentIndex The index of the segment to display parameters for.
   * @param errorPropagation Boolean indicating whether or not to attach more information
   *     (corrections, sigmas, adjusted sigmas...) to the output QString
   * @param imageCSV Boolean which is set to true if the function is being
   *     called from BundleSolutionInfo::outputImagesCSV().  It is set to false by default
   *     for backwards compatibility.
   *
   * @return QString Returns a formatted QString representing the position segment
   *
   * @internal
   *   @history 2017-10-06 Jesse Mapel - Original version, created from formatBundleOutputString
   */
  QString BundleObservation::formatPositionOutputString(int segmentIndex,
                                                        bool errorPropagation, bool imageCSV) {
    std::vector<double> coefX;
    std::vector<double> coefY;
    std::vector<double> coefZ;

    int nPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();

    // Indicate if we need to obtain default position or pointing values
    bool useDefaultPosition = false;

    // If we aren't solving for position, set the number of coefficients to 1 so we can output the
    // instrumentPosition's center coordinate values for X, Y, and Z
    if (nPositionCoefficients == 0) {
      nPositionCoefficients = 1;
      useDefaultPosition = true;
    }

    // Force number of position and pointing parameters to each be 3 (X,Y,Z; RA,DEC,TWI)
    // so we can always output a value for them
    int nPositionParameters = 3 * nPositionCoefficients;

    coefX.resize(nPositionCoefficients);
    coefY.resize(nPositionCoefficients);
    coefZ.resize(nPositionCoefficients);

    if (m_instrumentPosition) {
      if (!useDefaultPosition) {
        m_instrumentPosition->GetPolynomial(coefX, coefY, coefZ, segmentIndex);
      }
      // Use the position's center coordinate if not solving for spacecraft position
      else {
        const std::vector<double> centerCoord = m_instrumentPosition->GetCenterCoordinate();
        coefX[0] = centerCoord[0];
        coefY[0] = centerCoord[1];
        coefZ[0] = centerCoord[2];
      }
    }

    // for convenience, create lists of parameters names and values in the correct sequence
    QList<double> finalParameterValues;
    QStringList parameterNamesList;

    if (!imageCSV) {

      if (useDefaultPosition) {
        QString str("%1    ");

        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefX[i]);
          if (i == 0)
            parameterNamesList.append( str.arg("  X  ") );
          else
            parameterNamesList.append( str.arg("     ") );
        }
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefY[i]);
          if (i == 0)
            parameterNamesList.append( str.arg("  Y  ") );
          else
            parameterNamesList.append( str.arg("     ") );
        }
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefZ[i]);
          if (i == 0)
            parameterNamesList.append( str.arg("  Z  ") );
          else
            parameterNamesList.append( str.arg("     ") );
        }
      }
      else {
        QString str("%1(t%2)");

        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefX[i]);
          if (i == 0)
            parameterNamesList.append( str.arg("  X  ").arg("0") );
          else
            parameterNamesList.append( str.arg("     ").arg(i) );
        }
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefY[i]);
          if (i == 0)
            parameterNamesList.append( str.arg("  Y  ").arg("0") );
          else
            parameterNamesList.append( str.arg("     ").arg(i) );
        }
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefZ[i]);
          if (i == 0)
            parameterNamesList.append( str.arg("  Z  ").arg("0") );
          else
            parameterNamesList.append( str.arg("     ").arg(i) );
        }
      }
    }// end if(!imageCSV)

    else {
      if (nPositionCoefficients > 0) {
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefX[i]);
        }
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefY[i]);
        }
        for (int i = 0; i < nPositionCoefficients; i++) {
          finalParameterValues.append(coefZ[i]);
        }
      }
    }//end else

    // Save the list of parameter names we've accumulated above
    m_parameterNamesList = parameterNamesList;

    QString finalqStr = "";
    QString qStr = "";

    // Set up default values when we are using default position
    QString sigma = "N/A";
    QString adjustedSigma = "N/A";
    double correction = 0.0;

    // this implies we're writing to bundleout.txt
    if (!imageCSV) {
      // position parameters
      for (int i = 0; i < nPositionParameters; i++) {
        int index = nPositionParameters*segmentIndex+i;
        // If not using the default position, we can correctly access sigmas and corrections
        // members
        if (!useDefaultPosition) {
          correction = m_corrections(index);
          adjustedSigma = QString::number(m_adjustedSigmas[index], 'f', 8);
          sigma = ( IsSpecial(m_aprioriSigmas[index]) ? "FREE"
                                                      : toString(m_aprioriSigmas[index], 8) );
        }
        if (errorPropagation) {
          qStr = QString("%1%2%3%4%5%6\n").
          arg( parameterNamesList.at(i) ).
          arg(finalParameterValues[i] - correction, 17, 'f', 8).
          arg(correction, 21, 'f', 8).
          arg(finalParameterValues[i], 20, 'f', 8).
          arg(sigma, 18).
          arg(adjustedSigma, 18);
        }
        else {
          qStr = QString("%1%2%3%4%5%6\n").
          arg( parameterNamesList.at(i) ).
          arg(finalParameterValues[i] - correction, 17, 'f', 8).
          arg(correction, 21, 'f', 8).
          arg(finalParameterValues[i], 20, 'f', 8).
          arg(sigma, 18).
          arg("N/A", 18);
        }
        finalqStr += qStr;
      }
    }
    // this implies we're writing to images.csv
    else {
      // position parameters
      for (int i = 0; i < nPositionParameters; i++) {
        int index = nPositionParameters*segmentIndex+i;
        if (!useDefaultPosition) {
          correction = m_corrections(index);
          adjustedSigma = QString::number(m_adjustedSigmas[index], 'f', 8);
          sigma = ( IsSpecial(m_aprioriSigmas[index]) ? "FREE" : toString(m_aprioriSigmas[index], 8) );
        }
        // Provide default values for position if not solving position
        else {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        qStr = "";
        if (errorPropagation) {
          qStr += toString(finalParameterValues[i] - correction) + ",";
          qStr += toString(correction) + ",";
          qStr += toString(finalParameterValues[i]) + ",";
          qStr += sigma + ",";
          qStr += adjustedSigma + ",";
        }
        else {
          qStr += toString(finalParameterValues[i] - correction) + ",";
          qStr += toString(correction) + ",";
          qStr += toString(finalParameterValues[i]) + ",";
          qStr += sigma + ",";
          qStr += "N/A,";
        }
        finalqStr += qStr;
      }
    }

    return finalqStr;
  }


  /**
   * @brief Creates and returns a formatted QString representing the bundle coefficients and
   * parameters
   *
   * @param errorPropagation Boolean indicating whether or not to attach more information
   *     (corrections, sigmas, adjusted sigmas...) to the output QString
   * @param imageCSV Boolean which is set to true if the function is being
   *     called from BundleSolutionInfo::outputImagesCSV().  It is set to false by default
   *     for backwards compatibility.
   *
   * @return QString Returns a formatted QString representing the BundleObservation
   *
   * @internal
   *   @history 2016-10-26 Ian Humphrey - Default values are now provided for parameters that are
   *                           not being solved. Fixes #4464.
   */
  QString BundleObservation::formatPointingOutputString(int segmentIndex,
                                                        bool errorPropagation, bool imageCSV) {
    std::vector<double> coefRA;
    std::vector<double> coefDEC;
    std::vector<double> coefTWI;

    int nPointingCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    // Indicate if we need to obtain default position or pointing values
    bool useDefaultPointing = false;
    // Indicate if we need to use default values when not solving twist
    bool useDefaultTwist = !(m_solveSettings->solveTwist());

    // If we aren't solving for pointing, set the number of coefficients to 1 so we can output the
    // instrumentPointing's center angles for RA, DEC, and TWI
    if (nPointingCoefficients == 0) {
      nPointingCoefficients = 1;
      useDefaultPointing = true;
    }

    // Force number of pointing parameters to be 3 (RA,DEC,TWI)
    // so we can always output a value for them
    int nPointingParameters = 3 * nPointingCoefficients;

    // Calculate which the indices of the pointing segment's parameters.
    //
    // The parameters are ordered with all of the position parameters first
    // and all of the pointing parameters after them.
    //
    // With n position segments and m pointing segments they are ordered as follows:
    // [position 1 params]...[position n params][pointing 1 params]...[pointing m params]
    //
    // See BundleObservation::applyParameterCorrections for more information
    int segmentStartIndex = 3 * m_solveSettings->numberCameraPositionCoefficientsSolved()
                            * m_solveSettings->numberSpkPolySegments()
                            + nPointingParameters * segmentIndex;
    // If using default twist, then there are no twist parameters.
    // So, the starting index would be
    // 3 * m_solveSettings->numberCameraPositionCoefficientsSolved()
    // * m_solveSettings->numberSpkPolySegments()
    // + 2 * nPointingCoefficients * segmentIndex;
    if (useDefaultTwist) {
      segmentStartIndex -= nPointingCoefficients * segmentIndex;
    }
    // NOTE: This is actually one past the last index so it can be used in for loops
    int segmentEndIndex = segmentStartIndex + nPointingParameters;

    coefRA.resize(nPointingCoefficients);
    coefDEC.resize(nPointingCoefficients);
    coefTWI.resize(nPointingCoefficients);

    if (m_instrumentRotation) {
      if (!useDefaultPointing) {
        m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI, segmentIndex);
      }
      // Use the pointing's center angles if not solving for pointing (rotation)
      else {
        const std::vector<double> centerAngles = m_instrumentRotation->GetCenterAngles();
        coefRA[0] = centerAngles[0];
        coefDEC[0] = centerAngles[1];
        coefTWI[0] = centerAngles[2];
      }
    }

    // for convenience, create lists of parameters names and values in the correct sequence
    QList<double> finalParameterValues;
    QStringList parameterNamesList;

    if (!imageCSV) {
      if (useDefaultPointing) {

        QString str("%1    ");

        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefRA[i] * RAD2DEG);
          if (i == 0)
            parameterNamesList.append( str.arg(" RA  ") );
          else
            parameterNamesList.append( str.arg("     ") );
        }
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefDEC[i] * RAD2DEG);
          if (i == 0)
            parameterNamesList.append( str.arg("DEC  ") );
          else
            parameterNamesList.append( str.arg("     ") );
        }
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefTWI[i] * RAD2DEG);
          if (i == 0)
            parameterNamesList.append( str.arg("TWI  ") );
          else
            parameterNamesList.append( str.arg("     ") );
        }
      }
      else {

        QString str("%1(t%2)");

        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefRA[i] * RAD2DEG);
          if (i == 0)
            parameterNamesList.append( str.arg(" RA  ").arg("0") );
          else
            parameterNamesList.append( str.arg("     ").arg(i) );
        }
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefDEC[i] * RAD2DEG);
          if (i == 0)
            parameterNamesList.append( str.arg("DEC  ").arg("0") );
          else
            parameterNamesList.append( str.arg("     ").arg(i) );
        }
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefTWI[i] * RAD2DEG);
          if (i == 0)
            parameterNamesList.append( str.arg("TWI  ").arg("0") );
          else
            parameterNamesList.append( str.arg("     ").arg(i) );
        }
      }

    }// end if(!imageCSV)

    else {
      if (nPointingCoefficients > 0) {
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefRA[i] * RAD2DEG);
        }
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefDEC[i] * RAD2DEG);
        }
        for (int i = 0; i < nPointingCoefficients; i++) {
          finalParameterValues.append(coefTWI[i] * RAD2DEG);
        }
      }
    }//end else

    // Save the list of parameter names we've accumulated above
    m_parameterNamesList = parameterNamesList;

    QString finalqStr = "";
    QString qStr = "";

    // Set up default values when we are using default position
    QString sigma = "N/A";
    QString adjustedSigma = "N/A";
    double correction = 0.0;

    // this implies we're writing to bundleout.txt
    if (!imageCSV) {
      // pointing parameters
      // TODO more documentation about indices used
      int partialIndex = segmentStartIndex;
      for (int i = 0; i < nPointingParameters; i++) {
        if (!useDefaultPointing) {
          // If solving camera and not solving for twist, provide default values for twist to
          // prevent bad indexing into m_corrections and m_*sigmas
          // TWIST is last parameter in a segment, corresponding to
          // nParameters - nPointingCoefficients
          if ( (i >= (2 * nPointingCoefficients)) && useDefaultTwist) {
            correction = 0.0;
            adjustedSigma = "N/A";
            sigma = "N/A";
          }
          else {
            correction = m_corrections(partialIndex);
            adjustedSigma = QString::number(m_adjustedSigmas(partialIndex)
                                            * RAD2DEG, 'f', 8);
            sigma = ( IsSpecial(m_aprioriSigmas[partialIndex]) ? "FREE" :
                    toString(m_aprioriSigmas[partialIndex], 8) );
            partialIndex++;
          }
        }
        // We are using default pointing, so provide default correction and sigma values to output
        else {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        if (errorPropagation) {
          qStr = QString("%1%2%3%4%5%6\n").
          arg( parameterNamesList.at(i) ).
          arg( (finalParameterValues[i] - correction * RAD2DEG), 17, 'f', 8).
          arg(correction * RAD2DEG, 21, 'f', 8).
          arg(finalParameterValues[i], 20, 'f', 8).
          arg(sigma, 18).
          arg(adjustedSigma, 18);
        }
        else {
          qStr = QString("%1%2%3%4%5%6\n").
          arg( parameterNamesList.at(i) ).
          arg( (finalParameterValues[i] - correction * RAD2DEG), 17, 'f', 8).
          arg(correction * RAD2DEG, 21, 'f', 8).
          arg(finalParameterValues[i], 20, 'f', 8).
          arg(sigma, 18).
          arg("N/A", 18);
        }
        finalqStr += qStr;
      }
    }
    // this implies we're writing to images.csv
    else {
      for (int i = segmentStartIndex; i < segmentEndIndex; i++) {
        if (!useDefaultPointing) {
          // Use default values if solving camera but not solving for TWIST to prevent bad indexing
          // into m_corrections and m_*sigmas
          if ( (i >= segmentStartIndex + 2 * nPointingCoefficients) && useDefaultTwist) {
            correction = 0.0;
            adjustedSigma = "N/A";
            sigma = "N/A";
          }
          else {
            correction = m_corrections(i);
            adjustedSigma = QString::number(m_adjustedSigmas(i) * RAD2DEG, 'f', 8);
            sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" :
                                                      toString(m_aprioriSigmas[i], 8) );
          }
        }
        // Provide default values for pointing if not solving pointing
        else {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        qStr = "";
        if (errorPropagation) {
          qStr += toString(finalParameterValues[i - segmentStartIndex]
                           - correction * RAD2DEG) + ",";
          qStr += toString(correction * RAD2DEG) + ",";
          qStr += toString(finalParameterValues[i - segmentStartIndex]) + ",";
          qStr += sigma + ",";
          qStr += adjustedSigma + ",";
        }
        else {
          qStr += toString(finalParameterValues[i - segmentStartIndex]
                           - correction * RAD2DEG) + ",";
          qStr += toString(correction * RAD2DEG) + ",";
          qStr += toString(finalParameterValues[i - segmentStartIndex]) + ",";
          qStr += sigma + ",";
          qStr += "N/A,";
        }
        finalqStr += qStr;
      }
    }

    return finalqStr;
  }


  /**
   * Access to parameters for CorrelationMatrix to use.
   *
   * @return QStringList Returns a QStringList of the names of the parameters
   */
  QStringList BundleObservation::parameterList() {
    return m_parameterNamesList;
  }


  /**
   * Access to image names for CorrelationMatrix to use.
   *
   * @return QStringList Returns a QStringList of the image names
   */
  QStringList BundleObservation::imageNames() {
    return m_imageNames;
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained image parameter residuals.
   *
   * @return double Weighted sum of squares of constrained image parameter residuals.
   */
  double BundleObservation::vtpv() {
    double vtpv = 0.0;

    for (int i = 0; i < (int)m_corrections.size(); i++) {
      if (m_weights[i] > 0.0) {
        vtpv += m_corrections[i]* m_corrections[i] * m_weights[i];
      }

    }

    return vtpv;
  }
}
