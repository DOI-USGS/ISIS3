/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BundleObservation.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>

#include "BundleImage.h"
#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "LinearAlgebra.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

using namespace std;

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
//    m_solution.clear();
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
//    m_solution.clear();
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
      // if the image, camera, or instrument position/orientation is null, then set to null
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
   * @return @b BundleObservation& Reference to this BundleObservation
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
    }
    return *this;
  }


  /**
   * Appends a BundleImage shared pointer to the BundleObservation.
   * If the pointer is valid, then the BundleImage and its serial number will be inserted into
   * the serial number to BundleImage map.
   *
   * @param value The BundleImage to be appended.
   *
   * @see QVector::append()
   */
  void BundleObservation::append(const BundleImageQsp &value) {
    if (value) {
      m_cubeSerialNumberToBundleImageMap.insert(value->serialNumber(), value);
    }
    QVector<BundleImageQsp>::append(value);
  }


  /**
   * Returns the BundleImage shared pointer associated with the given serial number.
   * If no BundleImage with that serial number is contained a NULL pointer is returned.
   *
   * @param cubeSerialNumber The serial number of the cube to be returned.
   *
   * @return @b BundleImageQsp A shared pointer to the BundleImage (NULL if not found).
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
   * @return @b bool Returns true if settings were successfully set
   *
   * @internal
   *   @todo initParameterWeights() doesn't return false, so this methods always
   *         returns true.
   */
  bool BundleObservation::setSolveSettings(BundleObservationSolveSettings solveSettings) {
    m_solveSettings = BundleObservationSolveSettingsQsp(
                        new BundleObservationSolveSettings(solveSettings));

    // initialize solution parameters for this observation
    int nCameraAngleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();
    int nCameraPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();

    int nParameters = 3*nCameraPositionCoefficients + 2*nCameraAngleCoefficients;
    if (nCameraAngleCoefficients >= 1 && m_solveSettings->solveTwist()) {
      nParameters += nCameraAngleCoefficients;
    }
    // size vectors and set to zero
    m_weights.resize(nParameters);
    m_weights.clear();
    m_corrections.resize(nParameters);
    m_corrections.clear();
//    m_solution.resize(nParameters);
//    m_solution.clear();
    m_adjustedSigmas.resize(nParameters);
    m_adjustedSigmas.clear();
    m_aprioriSigmas.resize(nParameters);
    for ( int i = 0; i < nParameters; i++) // initialize apriori sigmas to -1.0
      m_aprioriSigmas[i] = Isis::Null;

    if (!initParameterWeights()) {
      // TODO: some message here!!!!!!!!!!!
      // TODO:  do we need this??? initParameterWeights() never returns false...
      return false;
    }

    return true;
  }


  /**
   * Accesses the instrument id
   *
   * @return @b QString Returns the instrument id of the observation
   */
  QString BundleObservation::instrumentId() {
    return m_instrumentId;
  }


  /**
   * Accesses the instrument's spice rotation
   *
   * @return @b SpiceRotation* Returns the SpiceRotation for this observation
   */
  SpiceRotation *BundleObservation::spiceRotation() {
    return m_instrumentRotation;
  }


  /**
   * Accesses the instrument's spice position
   *
   * @return @b SpicePosition* Returns the SpicePosition for this observation
   */
  SpicePosition *BundleObservation::spicePosition() {
    return m_instrumentPosition;
  }


  /**
   * Accesses the solve parameter weights
   *
   * @return @b LinearAlgebra::Vector Returns the parameter weights for solving
   */
  LinearAlgebra::Vector &BundleObservation::parameterWeights() {
    return m_weights;
  }


  /**
   * Accesses the parameter corrections
   *
   * @return @b LinearAlgebra::Vector Returns the parameter corrections
   */
  LinearAlgebra::Vector &BundleObservation::parameterCorrections() {
    return m_corrections;
  }


  /**
   * @internal
   *   @todo
   */
//  LinearAlgebra::Vector &BundleObservation::parameterSolution() {
//    return m_solution;
//  }


  /**
   * Accesses the a priori sigmas
   *
   * @return @b LinearAlgebra::Vector Returns the a priori sigmas
   */
  LinearAlgebra::Vector &BundleObservation::aprioriSigmas() {
    return m_aprioriSigmas;
  }


  /**
   * Accesses the adjusted sigmas
   *
   * @return @b LinearAlgebra::Vector Returns the adjusted sigmas
   */
  LinearAlgebra::Vector &BundleObservation::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * Accesses the solve settings
   *
   * @return @b const BundleObservationSolveSettingsQsp Returns a pointer to the solve
   *                                                    settings for this BundleObservation
   */
  const BundleObservationSolveSettingsQsp BundleObservation::solveSettings() {
    return m_solveSettings;
  }


  /**
   * Initializes the exterior orientation
   *
   * @return @b bool Returns true upon successful intialization
   *
   * @internal
   *   @todo Should this always return true?
   */
  bool BundleObservation::initializeExteriorOrientation() {

    if (m_solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      double positionBaseTime = 0.0;
      double positiontimeScale = 0.0;
      std::vector<double> posPoly1, posPoly2, posPoly3;

      for (int i = 0; i < size(); i++) {
        BundleImageQsp image = at(i);
        SpicePosition *spicePosition = image->camera()->instrumentPosition();

        if (i > 0) {
          spicePosition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());
          spicePosition->SetOverrideBaseTime(positionBaseTime, positiontimeScale);
          spicePosition->SetPolynomial(posPoly1, posPoly2, posPoly3,
                                       m_solveSettings->positionInterpolationType());
        }
        else {
          // first, set the degree of the spk polynomial to be fit for a priori values
          spicePosition->SetPolynomialDegree(m_solveSettings->spkDegree());

          // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
          // function, or polynomial function over constant hermite spline)
          // TODO: verify - I think this actually performs the a priori fit
          spicePosition->SetPolynomial(m_solveSettings->positionInterpolationType());

          // finally, set the degree of the spk polynomial actually used in the bundle adjustment
          spicePosition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());

          if (m_instrumentPosition) { // ??? TODO: why is this different from rotation code below???
            positionBaseTime = m_instrumentPosition->GetBaseTime();
            positiontimeScale = m_instrumentPosition->GetTimeScale();
            m_instrumentPosition->GetPolynomial(posPoly1, posPoly2, posPoly3);
          }
        }
      }
    }

    if (m_solveSettings->instrumentPointingSolveOption() !=
        BundleObservationSolveSettings::NoPointingFactors) {

      double rotationBaseTime = 0.0;
      double rotationtimeScale = 0.0;
      std::vector<double> anglePoly1, anglePoly2, anglePoly3;

      for (int i = 0; i < size(); i++) {
        BundleImageQsp image = at(i);
        SpiceRotation *spicerotation = image->camera()->instrumentRotation();

        if (i > 0) {
          spicerotation->SetPolynomialDegree(m_solveSettings->ckSolveDegree());
          spicerotation->SetOverrideBaseTime(rotationBaseTime, rotationtimeScale);
          spicerotation->SetPolynomial(anglePoly1, anglePoly2, anglePoly3,
                                       m_solveSettings->pointingInterpolationType());
        }
        else {
          // first, set the degree of the spk polynomial to be fit for a priori values
          spicerotation->SetPolynomialDegree(m_solveSettings->ckDegree());

          // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
          // function, or polynomial function over constant hermite spline)
          // TODO: verify - I think this actually performs the a priori fit
          spicerotation->SetPolynomial(m_solveSettings->pointingInterpolationType());

          // finally, set the degree of the spk polynomial actually used in the bundle adjustment
          spicerotation->SetPolynomialDegree(m_solveSettings->ckSolveDegree());

          rotationBaseTime = spicerotation->GetBaseTime();
          rotationtimeScale = spicerotation->GetTimeScale();
          spicerotation->GetPolynomial(anglePoly1, anglePoly2, anglePoly3);
        }
      }
    }

    return true;
  }


  /**
   * Intializes the body rotation
   *
   * @todo check to make sure m_bundleTargetBody is valid
   */
  void BundleObservation::initializeBodyRotation() {
    std::vector<Angle> raCoefs = m_bundleTargetBody->poleRaCoefs();
    std::vector<Angle> decCoefs = m_bundleTargetBody->poleDecCoefs();
    std::vector<Angle> pmCoefs = m_bundleTargetBody->pmCoefs();

    for (int i = 0; i < size(); i++) {
      BundleImageQsp image = at(i);
      image->camera()->bodyRotation()->setPckPolynomial(raCoefs, decCoefs, pmCoefs);
    }
  }


  /**
   * Updates the body rotation
   *
   * @internal
   *   @todo Is this a duplicate of initializeBodyRotation?
   */
  void BundleObservation::updateBodyRotation() {
    std::vector<Angle> raCoefs = m_bundleTargetBody->poleRaCoefs();
    std::vector<Angle> decCoefs = m_bundleTargetBody->poleDecCoefs();
    std::vector<Angle> pmCoefs = m_bundleTargetBody->pmCoefs();

    for (int i = 0; i < size(); i++) {
      BundleImageQsp image = at(i);
      image->camera()->bodyRotation()->setPckPolynomial(raCoefs, decCoefs, pmCoefs);
    }
  }


/*
  bool BundleObservation::initializeExteriorOrientation() {

    if (m_solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      for (int i = 0; i < size(); i++) {
        BundleImageQsp image = at(i);
        SpicePosition *spiceposition = image->camera()->instrumentPosition();

        // first, set the degree of the spk polynomial to be fit for a priori values
        spiceposition->SetPolynomialDegree(m_solveSettings->spkDegree());

        // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
        // function, or polynomial function over constant hermite spline)
        // TODO: verify - I think this actually performs the a priori fit
        spiceposition->SetPolynomial(m_solveSettings->positionInterpolationType());

        // finally, set the degree of the spk polynomial actually used in the bundle adjustment
        spiceposition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());
      }
    }

    if (m_solveSettings->instrumentPointingSolveOption() !=
        BundleObservationSolveSettings::NoPointingFactors) {

      for (int i = 0; i < size(); i++) {
        BundleImageQsp image = at(i);
        SpiceRotation *spicerotation = image->camera()->instrumentRotation();

        // first, set the degree of the spk polynomial to be fit for a priori values
        spicerotation->SetPolynomialDegree(m_solveSettings->ckDegree());

        // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
        // function, or polynomial function over constant hermite spline)
        // TODO: verify - I think this actually performs the a priori fit
        spicerotation->SetPolynomial(m_solveSettings->pointingInterpolationType());

        // finally, set the degree of the spk polynomial actually used in the bundle adjustment
        spicerotation->SetPolynomialDegree(m_solveSettings->ckSolveDegree());
      }
    }

    return true;
  }
*/


  /**
   * Initializes the paramater weights for solving
   *
   * @return @b bool Returns true upon successful intialization
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

    int nSpkTerms = m_solveSettings->spkSolveDegree()+1;
    nSpkTerms = m_solveSettings->numberCameraPositionCoefficientsSolved();
    for ( int i = 0; i < nCamPosCoeffsSolved; i++) {
      if (i % nSpkTerms == 0) {
       m_aprioriSigmas[i] = aprioriPositionSigmas.at(0);
       m_weights[i] = posWeight;
      }
      if (i % nSpkTerms == 1) {
       m_aprioriSigmas[i] = aprioriPositionSigmas.at(1);
       m_weights[i] = velWeight;
      }
      if (i % nSpkTerms == 2) {
       m_aprioriSigmas[i] = aprioriPositionSigmas.at(2);
       m_weights[i] = accWeight;
      }
    }

    int nCkTerms = m_solveSettings->ckSolveDegree()+1;
    nCkTerms = m_solveSettings->numberCameraAngleCoefficientsSolved();
    for ( int i = 0; i < nCamAngleCoeffsSolved; i++) {
      if (i % nCkTerms == 0) {
        m_aprioriSigmas[nCamPosCoeffsSolved + i] = aprioriPointingSigmas.at(0);
        m_weights[nCamPosCoeffsSolved + i] = angWeight;
      }
      if (i % nCkTerms == 1) {
        m_aprioriSigmas[nCamPosCoeffsSolved + i] = aprioriPointingSigmas.at(1);
        m_weights[nCamPosCoeffsSolved + i] = angVelWeight;
      }
      if (i % nCkTerms == 2) {
        m_aprioriSigmas[nCamPosCoeffsSolved + i] = aprioriPointingSigmas.at(2);
        m_weights[nCamPosCoeffsSolved + i] = angAccWeight;
      }
    }

//    for ( int i = 0; i < (int)m_weights.size(); i++ )
//      std::cout << m_weights[i] << std::endl;

    return true;
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
   * @return @b bool Returns true upon successful application of corrections
   *
   * @internal
   *   @todo always returns true?
   */
  bool BundleObservation::applyParameterCorrections(LinearAlgebra::Vector corrections) {

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

        std::vector<double> coefX(nCameraPositionCoefficients);
        std::vector<double> coefY(nCameraPositionCoefficients);
        std::vector<double> coefZ(nCameraPositionCoefficients);

        m_instrumentPosition->GetPolynomial(coefX, coefY, coefZ);

        // update X coordinate coefficient(s) and sum parameter correction
        for (int i = 0; i < nCameraPositionCoefficients; i++) {
          coefX[i] += corrections(index);
          index++;
        }

        // update Y coordinate coefficient(s) and sum parameter correction
        for (int i = 0; i < nCameraPositionCoefficients; i++) {
          coefY[i] += corrections(index);
          index++;
        }

        // update Z coordinate coefficient(s) and sum parameter correction
        for (int i = 0; i < nCameraPositionCoefficients; i++) {
          coefZ[i] += corrections(index);
          index++;
        }

        // apply updates to all images in observation
        for (int i = 0; i < size(); i++) {
          BundleImageQsp image = at(i);
          SpicePosition *spiceposition = image->camera()->instrumentPosition();
          spiceposition->SetPolynomial(coefX, coefY, coefZ,
                                       m_solveSettings->positionInterpolationType());
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

        std::vector<double> coefRA(nCameraPositionCoefficients);
        std::vector<double> coefDEC(nCameraPositionCoefficients);
        std::vector<double> coefTWI(nCameraPositionCoefficients);

        m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI);

        // update RA coefficient(s)
        for (int i = 0; i < nCameraAngleCoefficients; i++) {
          coefRA[i] += corrections(index);
          index++;
        }

        // update DEC coefficient(s)
        for (int i = 0; i < nCameraAngleCoefficients; i++) {
          coefDEC[i] += corrections(index);
          index++;
        }

        if (m_solveSettings->solveTwist()) {
          // update TWIST coefficient(s)
          for (int i = 0; i < nCameraAngleCoefficients; i++) {
            coefTWI[i] += corrections(index);
            index++;
          }
        }

        // apply updates to all images in observation
        for (int i = 0; i < size(); i++) {
          BundleImageQsp image = at(i);
          SpiceRotation *spiceRotation = image->camera()->instrumentRotation();
          spiceRotation->SetPolynomial(coefRA, coefDEC, coefTWI,
                                       m_solveSettings->pointingInterpolationType());
        }
      }

      // update corrections
      m_corrections += corrections;

    }
    catch (IException &e) {
      QString msg = "Unable to apply parameter corrections to BundleObservation.";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
    return true;
  }


  /**
   * Returns the number of position parameters there are
   *
   * @return @b int Returns the number of position parameters
   */
  int BundleObservation::numberPositionParameters() {
    return 3.0 * m_solveSettings->numberCameraPositionCoefficientsSolved();
  }


  /**
   * Returns the number of pointing parameters being solved for
   *
   * @return @b int Returns the number of pointing parameters
   */
  int BundleObservation::numberPointingParameters() {
    int angleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    if (m_solveSettings->solveTwist()) {
      return 3.0 * angleCoefficients;
    }
    return 2.0 * angleCoefficients;
  }


  /**
   * Returns the number of total parameters there are for solving
   *
   * The total number of parameters is equal to the number of position parameters and number of
   * pointing parameters
   *
   * @return @b int Returns the number of parameters there are
   */
  int BundleObservation::numberParameters() {
    return numberPositionParameters() + numberPointingParameters();
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
   * Accesses the observation's index
   *
   * @return @b int Returns the observation's index
   */
  int BundleObservation::index() {
    return m_index;
  }

  /**
 * @brief Creates and returns a formatted QString representing the bundle coefficients and
 * parameters
 *
 * @depricated The function formatBundleOutputString is depricated as of ISIS 3.9
 * and will be removed in ISIS 4.0
 *
 * @param errorPropagation Boolean indicating whether or not to attach more information
 *     (corrections, sigmas, adjusted sigmas...) to the output QString
 * @param imageCSV Boolean which is set to true if the function is being
 *     called from BundleSolutionInfo::outputImagesCSV().  It is set to false by default
 *     for backwards compatibility.
 *
 * @return @b QString Returns a formatted QString representing the BundleObservation
 *
 * @internal
 *   @history 2016-10-26 Ian Humphrey - Default values are now provided for parameters that are
 *                           not being solved. Fixes #4464.
 */
QString BundleObservation::formatBundleOutputString(bool errorPropagation, bool imageCSV) {

  std::cerr << "The function formatBundleOutputString is depricated as of ISIS 3.9"
               "and will be removed in ISIS 4.0" << std::endl;

  std::vector<double> coefX;
  std::vector<double> coefY;
  std::vector<double> coefZ;
  std::vector<double> coefRA;
  std::vector<double> coefDEC;
  std::vector<double> coefTWI;

  int nPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();
  int nPointingCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

  // Indicate if we need to obtain default position or pointing values
  bool useDefaultPosition = false;
  bool useDefaultPointing = false;
  // Indicate if we need to use default values when not solving twist
  bool useDefaultTwist = !(m_solveSettings->solveTwist());

  // If we aren't solving for position, set the number of coefficients to 1 so we can output the
  // instrumentPosition's center coordinate values for X, Y, and Z
  if (nPositionCoefficients == 0) {
    nPositionCoefficients = 1;
    useDefaultPosition = true;
  }
  // If we arent' solving for pointing, set the number of coefficients to 1 so we can output the
  // instrumentPointing's center angles for RA, DEC, and TWI
  if (nPointingCoefficients == 0) {
    nPointingCoefficients = 1;
    useDefaultPointing = true;
  }

  // Force number of position and pointing parameters to each be 3 (X,Y,Z; RA,DEC,TWI)
  // so we can always output a value for them
  int nPositionParameters = 3 * nPositionCoefficients;
  int nPointingParameters = 3 * nPointingCoefficients;
  int nParameters = nPositionParameters + nPointingParameters;

  coefX.resize(nPositionCoefficients);
  coefY.resize(nPositionCoefficients);
  coefZ.resize(nPositionCoefficients);
  coefRA.resize(nPointingCoefficients);
  coefDEC.resize(nPointingCoefficients);
  coefTWI.resize(nPointingCoefficients);

  if (m_instrumentPosition) {
    if (!useDefaultPosition) {
      m_instrumentPosition->GetPolynomial(coefX, coefY, coefZ);
    }
    // Use the position's center coordinate if not solving for spacecraft position
    else {
      const std::vector<double> centerCoord = m_instrumentPosition->GetCenterCoordinate();
      coefX[0] = centerCoord[0];
      coefY[0] = centerCoord[1];
      coefZ[0] = centerCoord[2];
    }
  }

  if (m_instrumentRotation) {
    if (!useDefaultPointing) {
      m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI);
    }
    // Use the pointing's center angles if not solving for pointing (rotation)
    else {
      const std::vector<double> centerAngles = m_instrumentRotation->GetCenterAngles();
      coefRA[0] = centerAngles[0];
      coefDEC[0] = centerAngles[1];
      coefTWI[0] = centerAngles[2];
    }
  }

  // for convenience, create vectors of parameters names and values in the correct sequence
  std::vector<double> finalParameterValues;
  QStringList parameterNamesList;

  if (!imageCSV) {

    QString str("%1(t%2)");

    if (nPositionCoefficients > 0) {
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefX[i]);
        if (i == 0)
          parameterNamesList.append( str.arg("  X  ").arg("0") );
        else
          parameterNamesList.append( str.arg("     ").arg(i) );
      }
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefY[i]);
        if (i == 0)
          parameterNamesList.append( str.arg("  Y  ").arg("0") );
        else
          parameterNamesList.append( str.arg("     ").arg(i) );
      }
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefZ[i]);
        if (i == 0)
          parameterNamesList.append( str.arg("  Z  ").arg("0") );
        else
          parameterNamesList.append( str.arg("     ").arg(i) );
      }
    }
    if (nPointingCoefficients > 0) {
      for (int i = 0; i < nPointingCoefficients; i++) {
        finalParameterValues.push_back(coefRA[i] * RAD2DEG);
        if (i == 0)
          parameterNamesList.append( str.arg(" RA  ").arg("0") );
        else
          parameterNamesList.append( str.arg("     ").arg(i) );
      }
      for (int i = 0; i < nPointingCoefficients; i++) {
        finalParameterValues.push_back(coefDEC[i] * RAD2DEG);
        if (i == 0)
          parameterNamesList.append( str.arg("DEC  ").arg("0") );
        else
          parameterNamesList.append( str.arg("     ").arg(i) );
      }
      for (int i = 0; i < nPointingCoefficients; i++) {
        finalParameterValues.push_back(coefTWI[i] * RAD2DEG);
        if (i == 0)
          parameterNamesList.append( str.arg("TWI  ").arg("0") );
        else
          parameterNamesList.append( str.arg("     ").arg(i) );
      }
    }

  }// end if(!imageCSV)

  else {
    if (nPositionCoefficients > 0) {
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefX[i]);
      }
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefY[i]);
      }
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefZ[i]);
      }
    }
    if (nPointingCoefficients > 0) {
      for (int i = 0; i < nPointingCoefficients; i++) {
        finalParameterValues.push_back(coefRA[i] * RAD2DEG);
      }
      for (int i = 0; i < nPointingCoefficients; i++) {
        finalParameterValues.push_back(coefDEC[i] * RAD2DEG);
      }
      for (int i = 0; i < nPointingCoefficients; i++) {
        finalParameterValues.push_back(coefTWI[i] * RAD2DEG);
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
      // If not using the default position, we can correctly access sigmas and corrections
      // members
      if (!useDefaultPosition) {
        correction = m_corrections(i);
        adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
        sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
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

    // We need to use an offset of -3 (1 coef; X,Y,Z) if we used the default center coordinate
    // (i.e. we did not solve for position), as m_corrections and m_*sigmas are populated
    // according to which parameters are solved
    int offset = 0;
    if (useDefaultPosition) {
      offset = 3;
    }
    // pointing parameters
    for (int i = nPositionParameters; i < nParameters; i++) {
      if (!useDefaultPointing) {
        // If solving camera and not solving for twist, provide default values for twist to
        // prevent bad indexing into m_corrections and m_*sigmas
        // TWIST is last parameter, which corresponds to nParameters - nPointingCoefficients
        if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        else {
          correction = m_corrections(i - offset);
          adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
          sigma = ( IsSpecial(m_aprioriSigmas[i - offset]) ? "FREE" :
                  toString(m_aprioriSigmas[i-offset], 8) );
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
    // position parameters
    for (int i = 0; i < nPositionParameters; i++) {
      if (!useDefaultPosition) {
        correction = m_corrections(i);
        adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
        sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
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

    // If not solving position, we need to offset access to correction and sigma members by -3
    // (X,Y,Z) since m_corrections and m_*sigmas are populated according to which parameters are
    // solved
    int offset = 0;
    if (useDefaultPosition) {
      offset = 3;
    }
    // pointing parameters
    for (int i = nPositionParameters; i < nParameters; i++) {
      if (!useDefaultPointing) {
        // Use default values if solving camera but not solving for TWIST to prevent bad indexing
        // into m_corrections and m_*sigmas
        if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        else {
          correction = m_corrections(i - offset);
          adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
          sigma = ( IsSpecial(m_aprioriSigmas[i-offset]) ? "FREE" :
              toString(m_aprioriSigmas[i-offset], 8) );
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
        qStr += toString(finalParameterValues[i] - correction * RAD2DEG) + ",";
        qStr += toString(correction * RAD2DEG) + ",";
        qStr += toString(finalParameterValues[i]) + ",";
        qStr += sigma + ",";
        qStr += adjustedSigma + ",";
      }
      else {
        qStr += toString(finalParameterValues[i] - correction * RAD2DEG) + ",";
        qStr += toString(correction * RAD2DEG) + ",";
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
  * @brief Fetches data for the log file output methods.
  *
  * @param finalParameterValues Reference to QVector<double> of calculated
  * position and pointing
  * @param nPositionCoefficients Reference to int of the number of position coefficients
  * @param nPointingCoefficients Reference to int of the number of pointing coefficients
  * @param useDefaultPosition Reference to boolean of whether to use default position
  * @param useDefaultPointing Reference to boolean of whether to use default pointing
  * @param useDefaultTwist Reference to bollean of whether to use defualt twist
  */
  void BundleObservation::bundleOutputFetchData(QVector<double> &finalParameterValues,
                          int &nPositionCoefficients, int &nPointingCoefficients,
                          bool &useDefaultPosition,
                          bool &useDefaultPointing, bool &useDefaultTwist) {

    std::vector<double> coefX,coefY,coefZ,coefRA,coefDEC,coefTWI;
    nPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();
    nPointingCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    // Indicate if we need to obtain default position or pointing values
    useDefaultPosition = false;
    useDefaultPointing = false;
    // Indicate if we need to use default values when not solving twist
    useDefaultTwist = !(m_solveSettings->solveTwist());

    // If we aren't solving for position, set the number of coefficients to 1 so we
    // can output the instrumentPosition's center coordinate values for X, Y, and Z
    if (nPositionCoefficients == 0) {
      nPositionCoefficients = 1;
      useDefaultPosition = true;
    }

    // If we arent' solving for pointing, set the number of coefficients to 1 so we
    // can output the instrumentPointing's center angles for RA, DEC, and TWI
    if (nPointingCoefficients == 0) {
      nPointingCoefficients = 1;
      useDefaultPointing = true;
    }

    coefX.resize(nPositionCoefficients);
    coefY.resize(nPositionCoefficients);
    coefZ.resize(nPositionCoefficients);
    coefRA.resize(nPointingCoefficients);
    coefDEC.resize(nPointingCoefficients);
    coefTWI.resize(nPointingCoefficients);

    if (m_instrumentPosition) {
      if (!useDefaultPosition) {
        m_instrumentPosition->GetPolynomial(coefX,coefY,coefZ);
      }
      // Use the position's center coordinate if not solving for spacecraft position
      else {
        const std::vector<double> centerCoord = m_instrumentPosition->GetCenterCoordinate();
        coefX[0] = centerCoord[0];
        coefY[0] = centerCoord[1];
        coefZ[0] = centerCoord[2];
      }
    }

    if (m_instrumentRotation) {
      if (!useDefaultPointing) {
        m_instrumentRotation->GetPolynomial(coefRA,coefDEC,coefTWI);
      }
      // Use the pointing's center angles if not solving for pointing (rotation)
      else {
        const std::vector<double> centerAngles = m_instrumentRotation->GetCenterAngles();
        coefRA[0] = centerAngles[0];
        coefDEC[0] = centerAngles[1];
        coefTWI[0] = centerAngles[2];
      }
    }

    // Combine all vectors into one
    if (nPositionCoefficients > 0) {
      for (int i=0; i < nPositionCoefficients; i++) {
        finalParameterValues.append(coefX[i]);
      }
      for (int i=0; i < nPositionCoefficients; i++) {
        finalParameterValues.append(coefY[i]);
      }
      for (int i=0; i < nPositionCoefficients; i++) {
        finalParameterValues.append(coefZ[i]);
      }
    }
    if (nPointingCoefficients > 0) {
      for (int i=0; i < nPointingCoefficients; i++) {
        finalParameterValues.append(coefRA[i]);
      }
      for (int i=0; i < nPointingCoefficients; i++) {
        finalParameterValues.append(coefDEC[i]);
      }
      for (int i=0; i < nPointingCoefficients; i++) {
        finalParameterValues.append(coefTWI[i]);
      }
    }

  }


  /**
   * @brief Takes in an open std::ofstream and writes out information which goes into the
   * bundleout.txt file.
   *
   * @param fpOut The open std::ofstream object which is passed in from
   * BundleSolutionInfo::outputText()
   * @param errorPropagation Boolean indicating whether or not to attach more information
   *     (corrections, sigmas, adjusted sigmas...) to the output.
   */
  void BundleObservation::bundleOutputString(std::ostream &fpOut, bool errorPropagation) {

    char buf[4096];

    QVector<double> finalParameterValues;
    int nPositionCoefficients, nPointingCoefficients;
    bool useDefaultPosition, useDefaultPointing,useDefaultTwist;

    bundleOutputFetchData(finalParameterValues,
                          nPositionCoefficients,nPointingCoefficients,
                          useDefaultPosition,useDefaultPointing,useDefaultTwist);

    int nPositionParameters = 3 * nPositionCoefficients;
    int nPointingParameters = 3 * nPointingCoefficients;
    int nParameters = nPositionParameters + nPointingParameters;

    // for convenience, create vectors of parameters names and values in the correct sequence
    QStringList parameterNamesListX,parameterNamesListY,parameterNamesListZ,
        parameterNamesListRA,parameterNamesListDEC,parameterNamesListTWI,
        parameterNamesList;
    QStringList correctionUnitListX,correctionUnitListY,correctionUnitListZ,
        correctionUnitListRA,correctionUnitListDEC,correctionUnitListTWI,
        correctionUnitList;

    QString str("%1(%2)  ");
    QString str2("%1(%2) ");
    QString strN("%1(%2)");


    if (nPositionCoefficients > 0) {
      for (int j = 0; j < nPositionCoefficients;j++) {
        if (j == 0) {
          parameterNamesListX.append(str.arg("  X  ").arg("km"));
          parameterNamesListY.append(str.arg("  Y  ").arg("km"));
          parameterNamesListZ.append(str.arg("  Z  ").arg("km"));
          correctionUnitListX.append("m");
          correctionUnitListY.append("m");
          correctionUnitListZ.append("m");
        } //end inner-if

        else if (j==1) {
          parameterNamesListX.append( str2.arg("    ").arg("km/s") );
          parameterNamesListY.append( str2.arg("    ").arg("km/s") );
          parameterNamesListZ.append( str2.arg("    ").arg("km/s") );
          correctionUnitListX.append("m/s");
          correctionUnitListY.append("m/s");
          correctionUnitListZ.append("m/s");
        }
        else {
          QString str("%1(%2)");
          parameterNamesListX.append(strN.arg("   ").arg("km/s^"+toString(j) ) );
          parameterNamesListY.append(strN.arg("   ").arg("km/s^"+toString(j) ) );
          parameterNamesListZ.append(strN.arg("   ").arg("km/s^"+toString(j) ) );
          correctionUnitListX.append("m/s^"+toString(j));
          correctionUnitListY.append("m/s^"+toString(j));
          correctionUnitListZ.append("m/s^"+toString(j));
        }
      }//end for
    }//end outer-if

    if (nPointingCoefficients > 0) {
      for (int j = 0; j < nPointingCoefficients;j++) {
        if (j == 0) {
          parameterNamesListRA.append(str.arg(" RA  ").arg("dd"));
          parameterNamesListDEC.append(str.arg("DEC  ").arg("dd"));
          parameterNamesListTWI.append(str.arg("TWI  ").arg("dd"));
          correctionUnitListRA.append("dd");
          correctionUnitListDEC.append("dd");
          correctionUnitListTWI.append("dd");
        } //end inner-if

        else if (j==1) {
          parameterNamesListRA.append( str2.arg("    ").arg("dd/s") );
          parameterNamesListDEC.append( str2.arg("    ").arg("dd/s") );
          parameterNamesListTWI.append( str2.arg("    ").arg("dd/s") );
          correctionUnitListRA.append("dd/s");
          correctionUnitListDEC.append("dd/s");
          correctionUnitListTWI.append("dd/s");
        }
        else {
          parameterNamesListRA.append(strN.arg("   ").arg("dd/s^"+toString(j) ) );
          parameterNamesListDEC.append(strN.arg("   ").arg("dd/s^"+toString(j) ) );
          parameterNamesListTWI.append(strN.arg("   ").arg("dd/s^"+toString(j) ) );
          correctionUnitListRA.append("dd/s^"+toString(j));
          correctionUnitListDEC.append("dd/s^"+toString(j));
          correctionUnitListTWI.append("dd/s^"+toString(j));
        }
      }//end for
    }// end outer-if

     //Put all of the parameter names together into one QStringList
    parameterNamesList.append(parameterNamesListX);
    parameterNamesList.append(parameterNamesListY);
    parameterNamesList.append(parameterNamesListZ);
    parameterNamesList.append(parameterNamesListRA);
    parameterNamesList.append(parameterNamesListDEC);
    parameterNamesList.append(parameterNamesListTWI);

    //Put all of the correction unit names together into one QStringList
    correctionUnitList.append(correctionUnitListX);
    correctionUnitList.append(correctionUnitListY);
    correctionUnitList.append(correctionUnitListZ);
    correctionUnitList.append(correctionUnitListDEC);
    correctionUnitList.append(correctionUnitListRA);
    correctionUnitList.append(correctionUnitListTWI);

    // Save the list of parameter names we've accumulated above
    m_parameterNamesList = parameterNamesList;

    // Set up default values when we are using default position
    QString sigma = "N/A";
    QString adjustedSigma = "N/A";
    double correction = 0.0;

    // position parameters
    for (int i = 0; i < nPositionParameters; i++) {
      // If not using the default position, we can correctly access sigmas and corrections
      // members
      if (!useDefaultPosition) {
        correction = m_corrections(i);
        adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
        sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
      }

      sprintf(buf,"%s",parameterNamesList.at(i).toStdString().c_str() );
      fpOut << buf;
      sprintf(buf,"%18.8lf  ",finalParameterValues[i] - correction);
      fpOut << buf;
      sprintf(buf,"%20.8lf  ",correction);
      fpOut << buf;
      sprintf(buf,"%23.8lf  ",finalParameterValues[i]);
      fpOut << buf;
      sprintf(buf,"            ");
      fpOut << buf;
      sprintf(buf,"%6s",sigma.toStdString().c_str());
      fpOut << buf;
      sprintf(buf,"            ");
      fpOut << buf;
      if (errorPropagation) {
        sprintf(buf,"%s",adjustedSigma.toStdString().c_str());
      }
      else {
        sprintf(buf,"%s","N/A");
      }
      fpOut<<buf;
      sprintf(buf,"        ");
      fpOut<<buf;
      sprintf(buf,"%s\n",correctionUnitList.at(i).toStdString().c_str() );
      fpOut<<buf;

    }

    // We need to use an offset of -3 (1 coef; X,Y,Z) if we used the default center coordinate
    // (i.e. we did not solve for position), as m_corrections and m_*sigmas are populated
    // according to which parameters are solved
    int offset = 0;
    if (useDefaultPosition) {
      offset = 3;
    }

    // pointing parameters
    for (int i = nPositionParameters; i < nParameters; i++) {
      if (!useDefaultPointing) {
        // If solving camera and not solving for twist, provide default values for twist to
        // prevent bad indexing into m_corrections and m_*sigmas
        // TWIST is last parameter, which corresponds to nParameters - nPointingCoefficients
        if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        else {
          correction = m_corrections(i - offset);
          adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
          sigma = ( IsSpecial(m_aprioriSigmas[i - offset]) ? "FREE" :
                  toString(m_aprioriSigmas[i-offset], 8) );
        }
      }
      // We are using default pointing, so provide default correction and sigma values to output
      else {
        correction = 0.0;
        adjustedSigma = "N/A";
        sigma = "N/A";
      }

      sprintf(buf,"%s",parameterNamesList.at(i).toStdString().c_str() );
      fpOut << buf;
      sprintf(buf,"%18.8lf  ",(finalParameterValues[i]*RAD2DEG - correction*RAD2DEG));
      fpOut << buf;
      sprintf(buf,"%20.8lf  ",(correction*RAD2DEG));
      fpOut << buf;
      sprintf(buf,"%23.8lf  ",(finalParameterValues[i]*RAD2DEG));
      fpOut << buf;
      sprintf(buf,"            ");
      fpOut << buf;
      sprintf(buf,"%6s",sigma.toStdString().c_str());
      fpOut << buf;
      sprintf(buf,"            ");
      fpOut << buf;
      if (errorPropagation) {
        sprintf(buf,"%s",adjustedSigma.toStdString().c_str());
      }
      else {
        sprintf(buf,"%s","N/A");
      }
      fpOut<<buf;
      sprintf(buf,"        ");
      fpOut<<buf;
      sprintf(buf,"%s\n",correctionUnitList.at(i).toStdString().c_str() );
      fpOut<<buf;
    }

  }

  /**
   * @brief Creates and returns a formatted QString representing the bundle coefficients and
   * parameters in csv format.
   *
   * @param errorPropagation Boolean indicating whether or not to attach more information
   *     (corrections, sigmas, adjusted sigmas...) to the output QString
   *
   * @return @b QString Returns a formatted QString representing the BundleObservation in
   * csv format
   */
  QString BundleObservation::bundleOutputCSV(bool errorPropagation) {

    QVector<double> finalParameterValues;
    int nPositionCoefficients, nPointingCoefficients;
    bool useDefaultPosition, useDefaultPointing,useDefaultTwist;

    bundleOutputFetchData(finalParameterValues,
                          nPositionCoefficients,nPointingCoefficients,
                          useDefaultPosition,useDefaultPointing,useDefaultTwist);

    int nPositionParameters = 3 * nPositionCoefficients;
    int nPointingParameters = 3 * nPointingCoefficients;
    int nParameters = nPositionParameters + nPointingParameters;

    QString finalqStr = "";

    // Set up default values when we are using default position
    QString sigma = "N/A";
    QString adjustedSigma = "N/A";
    double correction = 0.0;

    // Position parameters
    for (int i = 0; i < nPositionParameters; i++) {
      if (!useDefaultPosition) {
        correction = m_corrections(i);
        adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
        sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
      }
      // Provide default values for position if not solving position
      else {
        correction = 0.0;
        adjustedSigma = "N/A";
        sigma = "N/A";
      }

      finalqStr += toString(finalParameterValues[i] - correction) + ",";
      finalqStr += toString(correction) + ",";
      finalqStr += toString(finalParameterValues[i]) + ",";
      finalqStr += sigma + ",";
      if (errorPropagation) {
        finalqStr += adjustedSigma + ",";
      }
      else {
        finalqStr += "N/A,";
      }

    }

    // If not solving position, we need to offset access to correction and sigma members by -3
    // (X,Y,Z) since m_corrections and m_*sigmas are populated according to which parameters are
    // solved
    int offset = 0;
    if (useDefaultPosition) {
      offset = 3;
    }
    // pointing parameters
    for (int i = nPositionParameters; i < nParameters; i++) {
      if (!useDefaultPointing) {
        // Use default values if solving camera but not solving for TWIST to prevent bad indexing
        // into m_corrections and m_*sigmas
        if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
          correction = 0.0;
          adjustedSigma = "N/A";
          sigma = "N/A";
        }
        else {
          correction = m_corrections(i - offset);
          adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
          sigma = ( IsSpecial(m_aprioriSigmas[i-offset]) ? "FREE" :
              toString(m_aprioriSigmas[i-offset], 8) );
        }
      }
      // Provide default values for pointing if not solving pointing
      else {
        correction = 0.0;
        adjustedSigma = "N/A";
        sigma = "N/A";
      }

      finalqStr += toString(finalParameterValues[i]*RAD2DEG - correction * RAD2DEG) + ",";
      finalqStr += toString(correction * RAD2DEG) + ",";
      finalqStr += toString(finalParameterValues[i]*RAD2DEG) + ",";
      finalqStr += sigma + ",";
      if (errorPropagation) {
        finalqStr += adjustedSigma + ",";
      }
      else {
        finalqStr += "N/A,";
      }

    }

    return finalqStr;
  }


  /**
   * Access to parameters for CorrelationMatrix to use.
   *
   * @return @b QStringList Returns a QStringList of the names of the parameters
   */
  QStringList BundleObservation::parameterList() {
    return m_parameterNamesList;
  }


  /**
   * Access to image names for CorrelationMatrix to use.
   *
   * @return @b QStringList Returns a QStringList of the image names
   */
  QStringList BundleObservation::imageNames() {
    return m_imageNames;
  }
}
