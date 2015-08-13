#include "BundleObservation.h"

#include <QDebug>

#include "Camera.h"
#include "BundleImage.h"
#include "BundleObservationSolveSettings.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * TODO
   */
  BundleObservation::BundleObservation() {
    m_serialNumbers.clear();
    m_imageNames.clear();
    m_parameterNamesList.clear();
    m_observationNumber = "";
    m_instrumentId = "";
    m_solveSettings = NULL;
    m_instrumentRotation = NULL;
    m_instrumentPosition = NULL;
    m_Index = 0;
    m_weights.clear();
    m_corrections.clear();
    m_solution.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();

  }


  /**
   * constructor
   */
  BundleObservation::BundleObservation(BundleImage *image, QString observationNumber,
                                       QString instrumentId) {
    m_serialNumbers.clear();
    m_imageNames.clear();
    m_parameterNamesList.clear();
    m_observationNumber = "";
    m_instrumentId = "";
    m_solveSettings = NULL;
    m_instrumentRotation = NULL;
    m_instrumentPosition = NULL;
    m_Index = 0;
    m_weights.clear();
    m_corrections.clear();
    m_solution.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();

    m_observationNumber = observationNumber;
    m_instrumentId = instrumentId;

    if (image) {
      append(image);
      m_serialNumbers.append(image->serialNumber());
      m_imageNames.append(image->fileName());

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

    }
  }


  /**
   *  constructor
   */
  BundleObservation::BundleObservation(const BundleObservation &src) {

    m_serialNumbers = src.m_serialNumbers;

    m_observationNumber = src.m_observationNumber;
    m_instrumentId = src.m_instrumentId;

    m_instrumentPosition = src.m_instrumentPosition;
    m_instrumentRotation = src.m_instrumentRotation;

    m_solveSettings = src.m_solveSettings;
  }


  /**
   * destructor
   */
  BundleObservation::~BundleObservation() {
    qDeleteAll(*this);
    clear();

    delete m_solveSettings;
    m_solveSettings = NULL;
  }


  /**
   * TODO
   */
  BundleObservation& BundleObservation::operator = (const BundleObservation &src) {
    if (&src != this) {
      m_serialNumbers = src.m_serialNumbers;

      m_observationNumber = src.m_observationNumber;
      m_instrumentId = src.m_instrumentId;

      m_instrumentPosition = src.m_instrumentPosition;
      m_instrumentRotation = src.m_instrumentRotation;

      m_solveSettings = src.m_solveSettings;
    }
    return *this;
  }


  /**
   * set solve parameters
   */
  bool BundleObservation::setSolveSettings(BundleObservationSolveSettings solveSettings) {
    m_solveSettings = new BundleObservationSolveSettings(solveSettings);

    // initialize solution parameters for this observation
    int nCameraAngleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();
    int nCameraPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();

    int nParameters = 3*nCameraPositionCoefficients + 2*nCameraAngleCoefficients;
    if (nCameraAngleCoefficients >= 1 && m_solveSettings->solveTwist()) {
      nParameters += nCameraAngleCoefficients;
    }

    // resize boost::numeric::ublas::vectors to the number of parameters and clear them
    // (i.e. set all elements to zero)
    m_weights.resize(nParameters);
    m_weights.clear();

    m_corrections.resize(nParameters);// ??? += operator used when set by applyParameterCorrections
    m_corrections.clear();

    m_solution.resize(nParameters);// where set???
    m_solution.clear();

    m_adjustedSigmas.resize(nParameters);// set in bundle adjust
    m_adjustedSigmas.clear();
    
    // resize boost::numeric::ublas::vector to the number of parameters and set all elements to Null
    m_aprioriSigmas.resize(nParameters);
    for ( int i = 0; i < nParameters; i++) {
      // initialize apriori sigmas to Null
      m_aprioriSigmas[i] = Isis::Null;
    }

    if (!initParameterWeights()) {
      // TODO: some message here!!!!!!!!!!!
      // TODO:  do we need this??? initParameterWeights() never returns false...
      return false;
    }

    return true;
  }


  /**
   * return instrumentId
   */
  QString BundleObservation::instrumentId() {
    return m_instrumentId;
  }


  /**
   * TODO
   */
  SpiceRotation *BundleObservation::spiceRotation() {
    return m_instrumentRotation;
  }


  /**
   * TODO
   */
  SpicePosition *BundleObservation::spicePosition() {
    return m_instrumentPosition;
  }


  /**
   * TODO
   */
  const boost::numeric::ublas::vector< double >& BundleObservation::parameterWeights() {
    return m_weights;
  }


  /**
   * TODO
   */
  const boost::numeric::ublas::vector< double > &BundleObservation::parameterCorrections() {
    return m_corrections;

  }


  /**
   * TODO
   */
  const boost::numeric::ublas::vector< double > &BundleObservation::parameterSolution() {
    return m_solution;

  }


  /**
   * TODO
   */
  const boost::numeric::ublas::vector< double > &BundleObservation::aprioriSigmas() {
    return m_aprioriSigmas;
  }


  /**
   * TODO
   */
  const boost::numeric::ublas::vector< double > &BundleObservation::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * TODO
   */
  void BundleObservation::setParameterWeights(boost::numeric::ublas::vector< double > weights) {
    m_weights = weights;
  }


  /**
   * TODO
   */
  void BundleObservation::setParameterCorrections(
      boost::numeric::ublas::vector< double > corrections) {
    m_corrections = corrections;
  }


  /**
   * TODO
   */
  void BundleObservation::setParameterSolution(boost::numeric::ublas::vector< double > solution) {
    m_solution = solution;
  }


  /**
   * TODO
   */
  void BundleObservation::setAprioriSigmas(boost::numeric::ublas::vector< double > sigmas) {
    m_aprioriSigmas = sigmas;
  }


  /**
   * TODO
   */
  void BundleObservation::setAdjustedSigmas(boost::numeric::ublas::vector< double > sigmas) {
    m_adjustedSigmas = sigmas;
  }


  /**
   * TODO
   */
  const BundleObservationSolveSettings* BundleObservation::solveSettings() {
    return m_solveSettings;
  }


  /**
   * TODO
   */
  bool BundleObservation::initializeExteriorOrientation() {

    if (m_solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      double positionBaseTime = 0.0;
      double positiontimeScale = 0.0;
      std::vector<double> posPoly1, posPoly2, posPoly3;

      for (int i = 0; i < size(); i++) {
        BundleImage *image = at(i);
        SpicePosition *spiceposition = image->camera()->instrumentPosition();

        if (i > 0) {
          spiceposition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());
          spiceposition->SetOverrideBaseTime(positionBaseTime, positiontimeScale);
          spiceposition->SetPolynomial(posPoly1, posPoly2, posPoly3,
                                       m_solveSettings->positionInterpolationType());
        }
        else {
          // first, set the degree of the spk polynomial to be fit for a priori values
          spiceposition->SetPolynomialDegree(m_solveSettings->spkDegree());

          // now, set what kind of interpolation to use (SPICE, memcache, hermitecache, polynomial
          // function, or polynomial function over constant hermite spline)
          // TODO: verify - I think this actually performs the a priori fit
          spiceposition->SetPolynomial(m_solveSettings->positionInterpolationType());

          // finally, set the degree of the spk polynomial actually used in the bundle adjustment
          spiceposition->SetPolynomialDegree(m_solveSettings->spkSolveDegree());

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
        BundleImage *image = at(i);
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


/*
  bool BundleObservation::initializeExteriorOrientation() {

    if (m_solveSettings->instrumentPositionSolveOption() !=
        BundleObservationSolveSettings::NoPositionFactors) {

      for (int i = 0; i < size(); i++) {
        BundleImage *image = at(i);
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
        BundleImage *image = at(i);
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
   * TODO
   * Don't like this, don't like this, don't like this, don't like this, don't like this.
   * By the way, this seems klunky to me, would like to come up with a better way.
   * Also, apriori sigmas are in two place, the BundleObservationSolveSettings AND in the
   * the BundleObservation class too - this is unnecessary should only be in the
   * BundleObservationSolveSettings. But, they are split into position and pointing.
   */
  bool BundleObservation::initParameterWeights() {

                                   // weights for
    double posWeight    = 0.0;     // position
    double velWeight    = 0.0;     // velocity
    double accWeight    = 0.0;     // acceleration
    double angWeight    = 0.0;     // angles
    double angvelWeight = 0.0;     // angular velocity
    double angaccWeight = 0.0;     // angular acceleration

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

    if (aprioriPositionSigmas.size() >= 1 && aprioriPositionSigmas[0] > 0.0) {
      posWeight = aprioriPositionSigmas[0];
      posWeight = 1.0 / (posWeight  *posWeight * 1.0e-6);
    }
    if (aprioriPositionSigmas.size() >= 2 && aprioriPositionSigmas[1] > 0.0) {
      velWeight = aprioriPositionSigmas[1];
      velWeight = 1.0 / (velWeight  *velWeight * 1.0e-6);
    }
    if (aprioriPositionSigmas.size() >= 3 && aprioriPositionSigmas[2] > 0.0) {
      accWeight = aprioriPositionSigmas[2];
      accWeight = 1.0 / (accWeight  *accWeight * 1.0e-6);
    }

    if (aprioriPointingSigmas.size() >= 1 && aprioriPointingSigmas[0] > 0.0) {
      angWeight = aprioriPointingSigmas[0];
      angWeight = 1.0 / (angWeight  *angWeight * DEG2RAD * DEG2RAD);
    }
    if (aprioriPointingSigmas.size() >= 2 && aprioriPointingSigmas[1] > 0.0) {
      angvelWeight = aprioriPointingSigmas[1];
      angvelWeight = 1.0 / (angvelWeight * angvelWeight * DEG2RAD * DEG2RAD);
    }
    if (aprioriPointingSigmas.size() >= 3 && aprioriPointingSigmas[2] > 0.0) {
      angaccWeight = aprioriPointingSigmas[2];
      angaccWeight = 1.0 / (angaccWeight * angaccWeight * DEG2RAD * DEG2RAD);
    }

    int nspkTerms = m_solveSettings->spkSolveDegree()+1;
    nspkTerms = m_solveSettings->numberCameraPositionCoefficientsSolved();
    for ( int i = 0; i < nCamPosCoeffsSolved; i++) {
      if (i % nspkTerms == 0) {
       m_aprioriSigmas[i] = aprioriPositionSigmas[0];
       m_weights[i] = posWeight;
      }
      if (i % nspkTerms == 1) {
       m_aprioriSigmas[i] = aprioriPositionSigmas[1];
       m_weights[i] = velWeight;
      }
      if (i % nspkTerms == 2) {
       m_aprioriSigmas[i] = aprioriPositionSigmas[2];
       m_weights[i] = accWeight;
      }
    }

    int nckTerms = m_solveSettings->ckSolveDegree()+1;
    nckTerms = m_solveSettings->numberCameraAngleCoefficientsSolved()    ;
    for ( int i = 0; i < nCamAngleCoeffsSolved; i++) {
      if (i % nckTerms == 0) {
        m_aprioriSigmas[nCamPosCoeffsSolved + i] = aprioriPointingSigmas[0];
        m_weights[nCamPosCoeffsSolved + i] = angWeight;
      }
      if (i % nckTerms == 1) {
        m_aprioriSigmas[nCamPosCoeffsSolved + i] = aprioriPointingSigmas[1];
        m_weights[nCamPosCoeffsSolved + i] = angvelWeight;
      }
      if (i % nckTerms == 2) {
        m_aprioriSigmas[nCamPosCoeffsSolved + i] = aprioriPointingSigmas[2];
        m_weights[nCamPosCoeffsSolved + i] = angaccWeight;
      }
    }

//    for ( int i = 0; i < (int)m_weights.size(); i++ )
//      std::cout << m_weights[i] << std::endl;

    return true;
  }


  /**
   * TODO
   */  
  bool BundleObservation::applyParameterCorrections(boost::numeric::ublas::vector<double> corrections) {
    int index=0;

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
          coefX[i] += corrections[index];
          index++;
        }

        // update Y coordinate coefficient(s) and sum parameter correction
        for (int i = 0; i < nCameraPositionCoefficients; i++) {
          coefY[i] += corrections[index];
          index++;
        }

        // update Z coordinate coefficient(s) and sum parameter correction
        for (int i = 0; i < nCameraPositionCoefficients; i++) {
          coefZ[i] += corrections[index];
          index++;
        }

        // apply updates to all images in observation
        for (int i = 0; i < size(); i++) {
          BundleImage *image = at(i);
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

        std::vector< double > coefRA(nCameraPositionCoefficients);
        std::vector< double > coefDEC(nCameraPositionCoefficients);
        std::vector< double > coefTWI(nCameraPositionCoefficients);

        m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI);

        // update RA coefficient(s)
        for (int i = 0; i < nCameraAngleCoefficients; i++) {
          coefRA[i] += corrections[index];
          index++;
        }

        // update DEC coefficient(s)
        for (int i = 0; i < nCameraAngleCoefficients; i++) {
          coefDEC[i] += corrections[index];
          index++;
        }

        if (m_solveSettings->solveTwist()) {
          // update TWIST coefficient(s)
          for (int i = 0; i < nCameraAngleCoefficients; i++) {
            coefTWI[i] += corrections[index];
            index++;
          }
        }

        // apply updates to all images in observation
        for (int i = 0; i < size(); i++) {
          BundleImage *image = at(i);
          SpiceRotation *spicerotation = image->camera()->instrumentRotation();
          spicerotation->SetPolynomial(coefRA, coefDEC, coefTWI,
                                       m_solveSettings->pointingInterpolationType());
        }
      }

      // update corrections
      m_corrections += corrections;

    } 
    catch (IException &e) {
      QString msg = "Unable to apply parameter corrections to BundleObservation.";
      IException(e, IException::Unknown, msg, _FILEINFO_);
    }
    return true;
  }


  /**
   * TODO
   */
  int BundleObservation::numberPositionParameters() {
    return 3.0 * m_solveSettings->numberCameraPositionCoefficientsSolved();
  }


  /**
   * TODO
   */
  int BundleObservation::numberPointingParameters() {
    int angleCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    if (m_solveSettings->solveTwist()) {
      return 3.0 * angleCoefficients;
    }
    return 2.0 * angleCoefficients;
  }


  /**
   * TODO
   */
  int BundleObservation::numberParameters() {
    return numberPositionParameters() + numberPointingParameters();
  }


  /**
   * TODO
   */
  void BundleObservation::setIndex(int n) {
    m_Index = n;
  }


  /**
   * TODO
   */
  int BundleObservation::index() {
    return m_Index;
  }


  /**
   * TODO
   */
  QString BundleObservation::formatBundleOutputString(bool errorPropagation) {
    std::vector<double> coefX;
    std::vector<double> coefY;
    std::vector<double> coefZ;
    std::vector<double> coefRA;
    std::vector<double> coefDEC;
    std::vector<double> coefTWI;

    int nPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();
    int nPointingCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

    int nPositionParameters = numberPositionParameters();
    int nPointingParameters = numberPointingParameters();
    int nParameters = nPositionParameters + nPointingParameters;

    coefX.resize(nPositionCoefficients);
    coefY.resize(nPositionCoefficients);
    coefZ.resize(nPositionCoefficients);
    coefRA.resize(nPointingCoefficients);
    coefDEC.resize(nPointingCoefficients);
    coefTWI.resize(nPointingCoefficients);

    if (nPositionCoefficients > 0 && m_instrumentPosition) {
      m_instrumentPosition->GetPolynomial(coefX, coefY, coefZ);
    }

    if (nPointingCoefficients > 0 && m_instrumentRotation) {
      m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI);
    }

    // for convenience, create vectors of parameters names and values in the correct sequence
    std::vector<double> finalParameterValues;
    QStringList parameterNamesList;
    QString str("%1(t%2)");
    if (nPositionCoefficients > 0) {
      for (int i = 0; i < nPositionCoefficients; i++) {
        finalParameterValues.push_back(coefX[i]);
        if (i == 0) {
          parameterNamesList.append(str.arg("  X  ").arg("0"));
        }
        else {
          parameterNamesList.append(str.arg("     ").arg(i));
        }
      }
      for (int i = 0;i < nPositionCoefficients;i++) {
        finalParameterValues.push_back(coefY[i]);
        if (i == 0) {
          parameterNamesList.append(str.arg("  Y  ").arg("0"));
        }
        else {
          parameterNamesList.append(str.arg("     ").arg(i));
        }
      }
      for (int i = 0;i < nPositionCoefficients;i++) {
        finalParameterValues.push_back(coefZ[i]);
        if (i == 0) {
          parameterNamesList.append(str.arg("  Z  ").arg("0"));
        }
        else {
          parameterNamesList.append(str.arg("     ").arg(i));
        }
      }
    }
    if (nPointingCoefficients > 0) {
      for (int i = 0;i < nPointingCoefficients;i++) {
        finalParameterValues.push_back(coefRA[i] * RAD2DEG);
        if (i == 0) {
          parameterNamesList.append(str.arg(" RA  ").arg("0"));
        }
        else {
          parameterNamesList.append(str.arg("     ").arg(i));
        }
      }
      for (int i = 0;i < nPointingCoefficients;i++) {
        finalParameterValues.push_back(coefDEC[i] * RAD2DEG);
        if (i == 0) {
          parameterNamesList.append(str.arg("DEC  ").arg("0"));
        }
        else {
          parameterNamesList.append(str.arg("     ").arg(i));
        }
      }
      for (int i = 0;i < nPointingCoefficients;i++) {
        finalParameterValues.push_back(coefTWI[i] * RAD2DEG);
        if (i == 0) {
          parameterNamesList.append(str.arg("TWI  ").arg("0"));
        }
        else {
          parameterNamesList.append(str.arg("     ").arg(i));
        }
      }
    }

    m_parameterNamesList = parameterNamesList;
    QString finalqStr = "";
    QString qStr  = "";
    QString sigma = "";

    // position parameters
    for (int i = 0; i < nPositionParameters; i++) {

      sigma = (IsNullPixel(m_aprioriSigmas[i]) ? "N/A" : toString(m_aprioriSigmas[i], 8));

      if (errorPropagation) {
        qStr = QString("%1%2%3%4%5%6\n").
               arg(parameterNamesList.at(i)).
               arg(finalParameterValues[i] - m_corrections[i], 17, 'f', 8).
               arg(m_corrections[i], 21, 'f', 8).
               arg(finalParameterValues[i], 20, 'f', 8).
               arg(sigma, 18).
               arg(m_adjustedSigmas[i], 18, 'f', 8);
      }
      else {
        qStr = QString("%1%2%3%4%5%6\n").
               arg(parameterNamesList.at(i)).
               arg(finalParameterValues[i] - m_corrections[i], 17, 'f', 8).
               arg(m_corrections[i], 21, 'f', 8).
               arg(finalParameterValues[i], 20, 'f', 8).
               arg(sigma, 18).
               arg("N/A", 18);
      }

      finalqStr += qStr;
    }

    // pointing parameters
    for (int i = nPositionParameters;i < nParameters;i++) {

      sigma = (IsNullPixel(m_aprioriSigmas[i]) ? "N/A" : toString(m_aprioriSigmas[i], 8));

      if (errorPropagation) {
        qStr = QString("%1%2%3%4%5%6\n").
               arg(parameterNamesList.at(i)).
               arg((finalParameterValues[i] - m_corrections[i] * RAD2DEG), 17, 'f', 8).
               arg(m_corrections[i] * RAD2DEG, 21, 'f', 8).
               arg(finalParameterValues[i], 20, 'f', 8).
               arg(sigma, 18).
               arg(m_adjustedSigmas[i] * RAD2DEG, 18, 'f', 8);
      }
      else {
        qStr = QString("%1%2%3%4%5%6\n").
               arg(parameterNamesList.at(i)).
               arg((finalParameterValues[i] - m_corrections[i] * RAD2DEG), 17, 'f', 8).
               arg(m_corrections[i] * RAD2DEG, 21, 'f', 8).
               arg(finalParameterValues[i], 20, 'f', 8).
               arg(sigma, 18).
               arg("N/A", 18);
      }

      finalqStr += qStr;
    }

    return finalqStr;
  }


  /**
   * Access to parameters for CorrelationMatrix to use.
   */
  QStringList BundleObservation::parameterList() {
    return m_parameterNamesList;
  }


  /**
   * Access to image names for CorrelationMatrix to use.
   */
  QStringList BundleObservation::imageNames() {
    return m_imageNames;
  }

}
