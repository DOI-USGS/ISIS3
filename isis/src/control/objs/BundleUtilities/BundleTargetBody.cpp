#include "BundleTargetBody.h"

#include <QDebug>
#include <QObject>

#include "BundleSettings.h"
#include "IException.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "Target.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * default constructor
   */
  BundleTargetBody::BundleTargetBody() {
    m_solveTargetBodyRadiusMethod = None;
    m_parameterNamesList.clear();

    m_weights.clear();
    m_corrections.clear();
    m_solution.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();

    m_radii.resize(3);
    m_raPole.resize(3);
    m_decPole.resize(3);
    m_pm.resize(3);
  }


  /**
   * constructor
   */
  BundleTargetBody::BundleTargetBody(Target *target) {
    m_solveTargetBodyRadiusMethod = None;
    m_parameterNamesList.clear();

    m_weights.clear();
    m_corrections.clear();
    m_solution.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();

    // initialize radii and coefficients from target
    m_radii.resize(3);
    m_radii[0] = target->radii().at(0);
    m_radii[1] = target->radii().at(1);
    m_radii[2] = target->radii().at(2);

    m_raPole.resize(3);
    m_decPole.resize(3);
    m_pm.resize(3);

    m_raPole = target->poleRaCoefs();
    m_decPole = target->poleDecCoefs();
    m_pm = target->pmCoefs();    
  }


  /**
   * copy constructor
   */
//  BundleTargetBody::BundleTargetBody(const BundleTargetBody &src) {
//    copy(src);
//  }


  /**
   * destructor
   */
  BundleTargetBody::~BundleTargetBody() {
  }


  /**
   * copy method
   */
//  void BundleTargetBody::copy(const BundleTargetBody &src) {
//    m_solveTargetBodyRaPole = src.m_solveTargetBodyRaPole;
//    m_aprioriRaPole = src.m_aprioriRaPole;
//    m_sigmaRaPole = src.m_sigmaRaPole;
//    m_solveTargetBodyDecPole = src.m_solveTargetBodyDecPole;
//    m_aprioriDecPole = src.m_aprioriDecPole;
//    m_sigmaDecPole = src.m_sigmaDecPole;
//    m_solveTargetBodyZeroMeridian = src.m_solveTargetBodyZeroMeridian;
//    m_aprioriW0 = src.m_aprioriW0;
//    m_sigmaW0 = src.m_sigmaW0;
//    m_solveTargetBodyRotationRate = src.m_solveTargetBodyRotationRate;
//    m_aprioriWDot = src.m_aprioriWDot;
//    m_sigmaWDot = src.m_sigmaWDot;
//    m_solveTargetBodyRadiusMethod = src.m_solveTargetBodyRadiusMethod;
//    m_aprioriRadiusA = src.m_aprioriRadiusA;
//    m_sigmaRadiusA = src.m_sigmaRadiusA;
//    m_aprioriRadiusB = src.m_aprioriRadiusB;
//    m_sigmaRadiusB = src.m_sigmaRadiusB;
//    m_aprioriRadiusC = src.m_aprioriRadiusC;
//    m_sigmaRadiusC = src.m_sigmaRadiusC;
//    m_aprioriMeanRadius = src.m_aprioriMeanRadius;
//    m_sigmaMeanRadius = src.m_sigmaMeanRadius;

//    //m_targetBody;
//    m_weights = src.m_weights;
//    m_corrections = src.m_corrections;
//    m_solution = src.m_solution;
//    m_aprioriSigmas = src.m_aprioriSigmas;
//    m_adjustedSigmas = src.m_adjustedSigmas;
//  }


  /**
   * TODO
   * set solve parameters
   */
  void BundleTargetBody::setSolveSettings(std::set<int> targetParameterSolveCodes,
                                          Angle aprioriPoleRA, Angle sigmaPoleRA,
                                          Angle aprioriVelocityPoleRA, Angle sigmaVelocityPoleRA,
                                          Angle aprioriPoleDec, Angle sigmaPoleDec,
                                          Angle aprioriVelocityPoleDec,
                                          Angle sigmaVelocityPoleDec, Angle aprioriPM,
                                          Angle sigmaPM, Angle aprioriVelocityPM,
                                          Angle sigmaVelocityPM,
                                          TargetRadiiSolveMethod solveRadiiMethod,
                                          Distance aprioriRadiusA, Distance sigmaRadiusA,
                                          Distance aprioriRadiusB, Distance sigmaRadiusB,
                                          Distance aprioriRadiusC, Distance sigmaRadiusC,
                                          Distance aprioriMeanRadius, Distance sigmaMeanRadius) {

    m_solveTargetBodyRadiusMethod = solveRadiiMethod;

    m_parameterSolveCodes = targetParameterSolveCodes;

    // size corrections and adjusted sigma vectors and set to zero
    m_aprioriSigmas.resize(m_parameterSolveCodes.size());
    m_aprioriSigmas.clear();
    m_adjustedSigmas.resize(m_parameterSolveCodes.size());
    m_adjustedSigmas.clear();
    m_weights.resize(m_parameterSolveCodes.size());
    m_weights.clear();
    m_corrections.resize(m_parameterSolveCodes.size());
    m_corrections.clear();

// ken testing
    m_raPole[0] = aprioriPoleRA;
    m_raPole[1] = aprioriVelocityPoleRA;
    m_raPole[2] = Angle(0.0,Angle::Radians);
    m_decPole[0] = aprioriPoleDec;
    m_decPole[1] = aprioriVelocityPoleDec;
    m_decPole[2] = Angle(0.0,Angle::Radians);
    m_pm[0] = aprioriPM;
    m_pm[1] = aprioriVelocityPM;
    m_pm[2] = Angle(0.0,Angle::Radians);
// ken testing


    int n = 0;
    if (solvePoleRA()) {
      m_parameterSolveCodes.insert(PoleRA);
      m_raPole[0] = aprioriPoleRA;

      if (sigmaPoleRA.degrees() > 0.0) {
        m_aprioriSigmas(n) = sigmaPoleRA.degrees();
        m_weights(n) = 1.0/(sigmaPoleRA.radians()*sigmaPoleRA.radians());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
     }

    if (solvePoleRAVelocity()) {
      m_parameterSolveCodes.insert(VelocityPoleRA);
      m_raPole[1] = aprioriVelocityPoleRA;

      if (sigmaVelocityPoleRA.degrees() > 0.0) {
        m_aprioriSigmas(n) = sigmaVelocityPoleRA.degrees();
        m_weights(n) = 1.0/(sigmaVelocityPoleRA.radians()*sigmaVelocityPoleRA.radians());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }
//    if (solvePoleRAAcceleration()) {
//      m_parameterSolveCodes.insert(AccelerationPoleRA);
//      m_raPole[2].setDegrees(aprioriAccelerationPoleRA);
//      m_aprioriSigmas.insert_element(m_aprioriSigmas.size(), sigmaAccelerationPoleRA);
//    }
    if (solvePoleDec()) {
      m_parameterSolveCodes.insert(PoleDec);
      m_decPole[0] = aprioriPoleDec;

      if (sigmaPoleDec.degrees() > 0.0) {
        m_aprioriSigmas(n) = sigmaPoleDec.degrees();
        m_weights(n) = 1.0/(sigmaPoleDec.radians()*sigmaPoleDec.radians());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }
    if (solvePoleDecVelocity()) {
      m_parameterSolveCodes.insert(VelocityPoleDec);
      m_decPole[1] = aprioriVelocityPoleDec;

      if (sigmaVelocityPoleDec.degrees() > 0.0) {
        m_aprioriSigmas(n) = sigmaVelocityPoleDec.degrees();
        m_weights(n) = 1.0/(sigmaVelocityPoleDec.radians()*sigmaVelocityPoleDec.radians());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }

//    bool solveAccelerationPoleDec = false;
//    if (solvePoleDecAcceleration()) {
//      m_parameterSolveCodes.insert(AccelerationPoleDec);
//    }

    if (solvePM()) {
      m_parameterSolveCodes.insert(PM);
      m_pm[0] = aprioriPM;

      if (sigmaPM.degrees() > 0.0) {
        m_aprioriSigmas(n) = sigmaPM.degrees();
        m_weights(n) = 1.0/(sigmaPM.radians()*sigmaPM.radians());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }

    if (solvePMVelocity()) { // programmer's note: also referred to as "spin rate"
      m_parameterSolveCodes.insert(VelocityPM);
      m_pm[1] = aprioriVelocityPM;

      if (sigmaVelocityPM.degrees() > 0.0) {
        m_aprioriSigmas(n) = sigmaVelocityPM.degrees();
        m_weights(n) = 1.0/(sigmaVelocityPM.radians()*sigmaVelocityPM.radians());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }

//    bool solveAccelerationPM = false;
//    if (solvePMAcceleration()) {
//      m_parameterSolveCodes.insert(AccelerationPM);
//    }

    if (m_solveTargetBodyRadiusMethod == All) {
      m_parameterSolveCodes.insert(TriaxialRadiusA);
      m_radii[0] = aprioriRadiusA;

      if (sigmaRadiusA.kilometers() > 0.0) {
        m_aprioriSigmas(n) = sigmaRadiusA.kilometers();
        m_weights(n) = 1.0/(sigmaRadiusA.kilometers()*sigmaRadiusA.kilometers());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;

      m_parameterSolveCodes.insert(TriaxialRadiusB);
      m_radii[1] = aprioriRadiusB;

      if (sigmaRadiusB.kilometers() > 0.0) {
        m_aprioriSigmas(n) = sigmaRadiusB.kilometers();
        m_weights(n) = 1.0/(sigmaRadiusB.kilometers()*sigmaRadiusB.kilometers());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;

      m_parameterSolveCodes.insert(TriaxialRadiusC);
      m_radii[2] = aprioriRadiusC;

      if (sigmaRadiusC.kilometers() > 0.0) {
        m_aprioriSigmas(n) = sigmaRadiusC.kilometers();
        m_weights(n) = 1.0/(sigmaRadiusC.kilometers()*sigmaRadiusC.kilometers());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }

    else if (m_solveTargetBodyRadiusMethod == Mean) {
      m_parameterSolveCodes.insert(MeanRadius);
      m_meanRadius = aprioriMeanRadius;

      if (sigmaMeanRadius.kilometers() > 0.0) {
        m_aprioriSigmas(n) = sigmaMeanRadius.kilometers();
        m_weights(n) = 1.0/(sigmaMeanRadius.kilometers()*sigmaMeanRadius.kilometers());
      }
      else {
        m_aprioriSigmas(n) = -1.0;
        m_weights(n) = -1.0;
      }
      n++;
    }

//    for (int i = 0; i < nParameters; i++) {
//      std::cout << m_solution[i] << " " << m_aprioriSigmas[i] << " " << m_weights[i] << std::endl;
//    }
//    std::cout << "parameters: " << numberParameters() << std::endl;
//    std::cout << "      pole: " << numberPoleParameters() << std::endl;
//    std::cout << "        w0: " << numberW0Parameters() << std::endl;
//    std::cout << "      WDot: " << numberWDotParameters() << std::endl;
//    std::cout << "    Radius: " << numberRadiusParameters() << std::endl;
//    int fred=1;
  }


  bool BundleTargetBody::solvePoleRA() {
    if (m_parameterSolveCodes.find(PoleRA) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePoleRAVelocity() {
    if (m_parameterSolveCodes.find(VelocityPoleRA) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePoleRAAcceleration() {
    if (m_parameterSolveCodes.find(AccelerationPoleRA) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePoleDec() {
    if (m_parameterSolveCodes.find(PoleDec) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePoleDecVelocity() {
    if (m_parameterSolveCodes.find(VelocityPoleDec) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePoleDecAcceleration() {
    if (m_parameterSolveCodes.find(AccelerationPoleDec) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePM() {
    if (m_parameterSolveCodes.find(PM) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePMVelocity() {
    if (m_parameterSolveCodes.find(VelocityPM) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solvePMAcceleration() {
    if (m_parameterSolveCodes.find(AccelerationPM) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solveTriaxialRadii() {
    if (m_parameterSolveCodes.find(TriaxialRadiusA) != m_parameterSolveCodes.end() &&
        m_parameterSolveCodes.find(TriaxialRadiusB) != m_parameterSolveCodes.end() &&
        m_parameterSolveCodes.find(TriaxialRadiusC) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  bool BundleTargetBody::solveMeanRadius() {
    if (m_parameterSolveCodes.find(MeanRadius) != m_parameterSolveCodes.end())
      return true;
    return false;
  }


  /**
   * TODO
   */
  void BundleTargetBody::applyParameterCorrections(boost::numeric::ublas::vector<double> corrections) {
    if (corrections.size() != m_parameterSolveCodes.size()) {
      QString msg = "In BundleTargetBody::applyParameterCorrections: "
                    "correction and m_targetParameter vectors sizes don't match\n";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

//    for (int i = 0; i < corrections.size(); i++ )
//      printf("%lf\n",corrections(i));
//    std::cout << std::endl;

    try {
      int n = 0;
      for (std::set<int>::iterator it=m_parameterSolveCodes.begin(); it!=m_parameterSolveCodes.end(); ++it) {
        switch (*it) {
          case PoleRA:
            m_raPole[0] += Angle(corrections[n], Angle::Radians);
            break;
          case VelocityPoleRA:
            m_raPole[1] += Angle(corrections[n], Angle::Radians);
            break;
          case AccelerationPoleRA:
            m_raPole[2] += Angle(corrections[n], Angle::Radians);
            break;
          case PoleDec:
            m_decPole[0] += Angle(corrections[n], Angle::Radians);
            break;
          case VelocityPoleDec:
            m_decPole[1] += Angle(corrections[n], Angle::Radians);
            break;
          case AccelerationPoleDec:
            m_decPole[2] += Angle(corrections[n], Angle::Radians);
            break;
          case PM:
            m_pm[0] += Angle(corrections[n], Angle::Radians);
            break;
          case VelocityPM:
            m_pm[1] += Angle(corrections[n], Angle::Radians);
            break;
          case AccelerationPM:
            m_pm[2] += Angle(corrections[n], Angle::Radians);
            break;
          case TriaxialRadiusA:
            {
              double c = m_radii[0].kilometers();
              double d = c + corrections[n];
              m_radii[0] = Distance(d, Distance::Kilometers);
//            m_radii[0] += Distance(corrections[n], Distance::Kilometers);
            }
            break;
          case TriaxialRadiusB:
            {
              double c = m_radii[1].kilometers();
              double d = c + corrections[n];
              m_radii[1] = Distance(d, Distance::Kilometers);
//            m_radii[1] += Distance(corrections[n], Distance::Kilometers);
            }
            break;
          case TriaxialRadiusC:
            {
              double c = m_radii[2].kilometers();
              double d = c + corrections[n];
              m_radii[2] = Distance(d, Distance::Kilometers);
//            m_radii[2] += Distance(corrections[n], Distance::Kilometers);
            }
            break;
          case MeanRadius:
            {
            double c = m_meanRadius.kilometers();
            double d = c + corrections[n];
            m_meanRadius = Distance(d, Distance::Kilometers);
//            m_meanRadius += Distance(corrections[n], Distance::Kilometers);
            }
            break;
          default:
            break;
        } // end switch

        m_corrections[n] += corrections[n];
        n++;
      } // end loop over m_parameterSolveCodes
    } // end try

    catch (IException &e) {
      QString msg = "Unable to apply parameter corrections to BundleTargetBody.";
      IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }


  BundleTargetBody::TargetRadiiSolveMethod BundleTargetBody::stringToTargetRadiiOption(QString method) {
    if (method.compare("NONE", Qt::CaseInsensitive) == 0) {
      return None;
    }
    else if (method.compare("MEAN", Qt::CaseInsensitive) == 0) {
      return Mean;
    }
    else if (method.compare("ALL", Qt::CaseInsensitive) == 0) {
      return All;
    }
    else {
      throw IException(IException::Programmer,
                       "Unknown target body radius solution method [" + method + "].",
                       _FILEINFO_);
    }
  }


  QString BundleTargetBody::targetRadiiOptionToString(TargetRadiiSolveMethod method) {
    if (method == None)
      return "None";
    else if (method == Mean)
      return "MeanRadius";
    else if (method == All)
      return "Radii";
    else throw IException(IException::Programmer,
                          "Unknown target body radius solve method enum [" + toString(method) + "].",
                          _FILEINFO_);
  }


  /**
   * TODO
   */
  vector<double> &BundleTargetBody::parameterWeights() {
    return m_weights;
  }


  /**
   * TODO
   */
  vector< double > &BundleTargetBody::parameterCorrections() {
    return m_corrections;
  }


  /**
   * TODO
   */
  vector< double > &BundleTargetBody::parameterSolution() {
    return m_solution;
  }


  /**
   * TODO
   */
  vector<double> &BundleTargetBody::aprioriSigmas() {
    return m_aprioriSigmas;
  }


  /**
   * TODO
   */
  vector< double > &BundleTargetBody::adjustedSigmas() {
    return m_adjustedSigmas;
  }

//  int BundleTargetBody::numberPoleParameters() {
//    int nPoleParameters = 0;
//    if (m_solveTargetBodyRaPole)
//      nPoleParameters++;
//    if (m_solveTargetBodyDecPole)
//      nPoleParameters++;
//    return nPoleParameters;
//  }


//  int BundleTargetBody::numberW0Parameters() {
//    if (m_solveTargetBodyZeroMeridian)
//      return 1;
//    return 0;
//  }


//  int BundleTargetBody::numberWDotParameters() {
//    if (m_solveTargetBodyRotationRate)
//      return 1;
//    return 0;
//  }


  int BundleTargetBody::numberRadiusParameters() {
    if (m_solveTargetBodyRadiusMethod == All)
      return 3;
    else if (m_solveTargetBodyRadiusMethod == Mean)
      return 1;
    return 0;
  }


  int BundleTargetBody::numberParameters() {
    return m_parameterSolveCodes.size();
  }


  std::vector<Angle> BundleTargetBody::poleRaCoefs() {
    return m_raPole;
  }


  std::vector<Angle> BundleTargetBody::poleDecCoefs() {
    return m_decPole;
  }


  std::vector<Angle> BundleTargetBody::pmCoefs() {
    return m_pm;
  }


  std::vector<Distance> BundleTargetBody::radii() {
    return m_radii;
  }


  Distance BundleTargetBody::meanRadius() {
    return m_meanRadius;
  }


  /**
   * TODO
   */
  double BundleTargetBody::vtpv() {
    double vtpv = 0.0;
    double v;

    int nParameters = m_parameterSolveCodes.size();
    for (int i = 0; i < nParameters; i++) {
      v = m_corrections(i);

      if (m_weights(i) > 0.0)
        vtpv += v * v * m_weights(i);
    }

    return vtpv;
  }

  QString BundleTargetBody::formatBundleOutputString(bool errorPropagation) {

    // for convenience, create vectors of parameters names and values in the correct sequence
    std::vector<double> finalParameterValues;
    QStringList parameterNamesList;
    QString str("%1");
    int nAngleParameters = 0;
    int nRadiusParameters = 0;
    if (solvePoleRA()) {
      finalParameterValues.push_back(m_raPole[0].degrees());
      parameterNamesList.append( str.arg("POLE RA  ") );
      nAngleParameters++;
    }
    if (solvePoleRAVelocity()) {
      finalParameterValues.push_back(m_raPole[1].degrees());
      parameterNamesList.append( str.arg("POLE RAv  ") );
      nAngleParameters++;
    }
    if (solvePoleRAAcceleration()) {
      finalParameterValues.push_back(m_raPole[2].degrees());
      parameterNamesList.append( str.arg("POLE RAa  ") );
      nAngleParameters++;
    }
    if (solvePoleDec()) {
      finalParameterValues.push_back(m_decPole[0].degrees());
      parameterNamesList.append( str.arg("POLE DEC ") );
      nAngleParameters++;
    }
    if (solvePoleDecVelocity()) {
      finalParameterValues.push_back(m_decPole[1].degrees());
      parameterNamesList.append( str.arg("POLE DECv  ") );
      nAngleParameters++;
    }
    if (solvePoleDecAcceleration()) {
      finalParameterValues.push_back(m_decPole[2].degrees());
      parameterNamesList.append( str.arg("POLE DECa  ") );
      nAngleParameters++;
    }
    if (solvePM()) {
      finalParameterValues.push_back(m_pm[0].degrees());
      parameterNamesList.append( str.arg("  PM  ") );
      nAngleParameters++;
    }
    if (solvePMVelocity()) {
      finalParameterValues.push_back(m_pm[1].degrees());
      parameterNamesList.append( str.arg("  PMv  ") );
      nAngleParameters++;
    }
    if (solvePoleDecAcceleration()) {
      finalParameterValues.push_back(m_pm[2].degrees());
      parameterNamesList.append( str.arg("  PMa  ") );
      nAngleParameters++;
    }
    if (solveTriaxialRadii()) {
      finalParameterValues.push_back(m_radii[0].kilometers());
      finalParameterValues.push_back(m_radii[1].kilometers());
      finalParameterValues.push_back(m_radii[2].kilometers());
      parameterNamesList.append( str.arg("  RadiusA") );
      parameterNamesList.append( str.arg("  RadiusB") );
      parameterNamesList.append( str.arg("  RadiusC") );
      nRadiusParameters += 3;
    }
    if (solveMeanRadius()) {
      finalParameterValues.push_back(m_meanRadius.kilometers());
      parameterNamesList.append( str.arg("MeanRadius") );
      nRadiusParameters++;
    }

    int nParameters = nAngleParameters + nRadiusParameters;

    m_parameterNamesList = parameterNamesList;
    QString finalqStr = "";
    QString qStr = "";
    QString sigma = "";

//    for (std::set<int>::iterator it=m_parameterSolveCodes.begin(); it!=m_parameterSolveCodes.end(); ++it) {
    for (int i = 0; i < nAngleParameters; i++) {

      if (m_aprioriSigmas[i] <= 0.0)
        sigma = "FREE";
      else
        sigma = toString(m_aprioriSigmas[i], 8);

      if (errorPropagation) {
        Angle corr_temp = Angle(m_corrections(i),Angle::Radians);
        qStr = QString("%1%2%3%4%5%6\n").
            arg( parameterNamesList.at(i) ).
            arg(finalParameterValues[i] - corr_temp.degrees(), 17, 'f', 8).
            arg(corr_temp.degrees(), 21, 'f', 8).
            arg(finalParameterValues[i], 20, 'f', 8).
            arg(sigma, 18).
            arg(m_adjustedSigmas[i], 18, 'f', 8);
      }
      else {
        Angle corr_temp = Angle(m_corrections(i),Angle::Radians);
        qStr = QString("%1%2%3%4%5%6\n").
            arg( parameterNamesList.at(i) ).
            arg(finalParameterValues[i] - corr_temp.degrees(), 17, 'f', 8).
            arg(corr_temp.degrees(), 21, 'f', 8).
            arg(finalParameterValues[i], 20, 'f', 8).
            arg(sigma, 18).
            arg("N/A", 18);
      }
      finalqStr += qStr;
    }

    for (int i = nAngleParameters; i < nParameters; i++) {

    if (m_aprioriSigmas[i] <= 0.0)
      sigma = "FREE";
    else
      sigma = toString(m_aprioriSigmas[i], 8);

      if (errorPropagation) {
        double d1 = finalParameterValues[i];
        double d2 = m_corrections(i);
        qStr = QString("%1%2%3%4%5%6\n").
            arg( parameterNamesList.at(i) ).
            arg(d1 - d2, 17, 'f', 8).
            arg(d2, 21, 'f', 8).
            arg(d1, 20, 'f', 8).
            arg(sigma, 18).
            arg(m_adjustedSigmas[i], 18, 'f', 8);
      }
      else {
        double d1 = finalParameterValues[i];
        double d2 = m_corrections(i);
        qStr = QString("%1%2%3%4%5%6\n").
            arg( parameterNamesList.at(i) ).
            arg(d1 - d2, 17, 'f', 8).
            arg(d2, 21, 'f', 8).
            arg(d1, 20, 'f', 8).
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
  QStringList BundleTargetBody::parameterList() {
    return m_parameterNamesList;
  }


  /**
   * set bundle solve parameters for target body from a pvl
   * file
   *
   * specifically for standard jigsaw interface, not the
   * Integrated Photogrammetric Control Environment (IPCE)
   */
  bool BundleTargetBody::readFromPvl(PvlObject &tbObject) {
    // reset defaults
    //initialize();

    double d = -1.0;
    QString str;
    TargetRadiiSolveMethod solveRadiiMethod = None;

    // determine which parameters we're solving for
    std::set<int> targetParameterSolveCodes;
    PvlObject::PvlGroupIterator g;
    for (g = tbObject.beginGroup(); g != tbObject.endGroup(); ++g) {
      if (g->hasKeyword("Ra")) {
        try {
          str = g->findKeyword("Ra")[0];
        }
        catch (IException &e) {
          QString msg = "Ra must be given as none, position, velocity, or acceleration";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (str == "position") {
          targetParameterSolveCodes.insert(BundleTargetBody::PoleRA);
        }
        else if (str == "velocity") {
          targetParameterSolveCodes.insert(BundleTargetBody::PoleRA);
          targetParameterSolveCodes.insert(BundleTargetBody::VelocityPoleRA);
        }
        else if (str == "acceleration") {
          targetParameterSolveCodes.insert(BundleTargetBody::PoleRA);
          targetParameterSolveCodes.insert(BundleTargetBody::VelocityPoleRA);
          targetParameterSolveCodes.insert(BundleTargetBody::AccelerationPoleRA);
        }
      }

      if (g->hasKeyword("Dec")) {
        try {
          str = g->findKeyword("Dec")[0];
        }
        catch (IException &e) {
          QString msg = "Dec must be given as none, position, velocity, or acceleration";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (str == "position") {
          targetParameterSolveCodes.insert(BundleTargetBody::PoleDec);
        }
        else if (str == "velocity") {
          targetParameterSolveCodes.insert(BundleTargetBody::PoleDec);
          targetParameterSolveCodes.insert(BundleTargetBody::VelocityPoleDec);
        }
        else if (str == "acceleration") {
          targetParameterSolveCodes.insert(BundleTargetBody::PoleDec);
          targetParameterSolveCodes.insert(BundleTargetBody::VelocityPoleDec);
          targetParameterSolveCodes.insert(BundleTargetBody::AccelerationPoleDec);
        }
      }

      if (g->hasKeyword("Pm")) {
        try {
          str = g->findKeyword("Pm")[0];
        }
        catch (IException &e) {
          QString msg = "Pm must be given as none, position, velocity, or acceleration";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (str == "position") {
          targetParameterSolveCodes.insert(BundleTargetBody::PM);
        }
        else if (str == "velocity") {
          targetParameterSolveCodes.insert(BundleTargetBody::PM);
          targetParameterSolveCodes.insert(BundleTargetBody::VelocityPM);
        }
        else if (str == "acceleration") {
          targetParameterSolveCodes.insert(BundleTargetBody::PM);
          targetParameterSolveCodes.insert(BundleTargetBody::VelocityPM);
          targetParameterSolveCodes.insert(BundleTargetBody::AccelerationPM);
        }
      }

      if (g->hasKeyword("RadiiSolveOption")) {
        try {
          str = g->findKeyword("RadiiSolveOption")[0];
        }
        catch (IException &e) {
          QString msg = "RadiiSolveOption must be given as none, triaxial, or mean";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (str == "triaxial") {
          targetParameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusA);
          targetParameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusB);
          targetParameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusC);
          solveRadiiMethod = All;
        }
        else if (str == "mean") {
          targetParameterSolveCodes.insert(BundleTargetBody::MeanRadius);
          solveRadiiMethod = Mean;
        }
        else
          solveRadiiMethod = None;
      }
    }

    Angle aprioriPoleRA;
    Angle poleRASigma;
    Angle aprioriVelocityPoleRA;
    Angle poleRAVelocitySigma;
    Angle aprioriAccelerationPoleRA;
    Angle poleRAAccelerationSigma;
    Angle aprioriPoleDec;
    Angle poleDecSigma;
    Angle aprioriVelocityPoleDec;
    Angle sigmaVelocityPoleDec;
    Angle aprioriAccelerationPoleDec;
    Angle sigmaAccelerationPoleDec;
    Angle aprioriPM;
    Angle sigmaPM;
    Angle aprioriVelocityPM;
    Angle sigmaVelocityPM;
    Angle aprioriAccelerationPM;
    Angle pmAccelerationSigma;
    Distance aprioriRadiusA;
    Distance sigmaRadiusA;
    Distance aprioriRadiusB;
    Distance sigmaRadiusB;
    Distance aprioriRadiusC;
    Distance sigmaRadiusC;
    Distance aprioriMeanRadius;
    Distance sigmaMeanRadius;

    for (g = tbObject.beginGroup(); g != tbObject.endGroup(); ++g) {
      if (g->hasKeyword("RaValue")) {
        try {
          d = (double)(g->findKeyword("RaValue"));
        }
        catch (IException &e) {
          QString msg = "RaValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriPoleRA = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("RaSigma")) {
        try {
          d = (double)(g->findKeyword("RaSigma"));
        }
        catch (IException &e) {
          QString msg = "RaSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        poleRASigma = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("RaVelocityValue")) {
        try {
          d = (double)(g->findKeyword("RaVelocityValue"));
        }
        catch (IException &e) {
          QString msg = "RaVelocityValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriVelocityPoleRA = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("RaVelocitySigma")) {
        try {
          d = (double)(g->findKeyword("RaVelocitySigma"));
        }
        catch (IException &e) {
          QString msg = "RaVelocitySigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        poleRAVelocitySigma = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("RaAccelerationValue")) {
        try {
          d = (double)(g->findKeyword("RaAccelerationValue"));
        }
        catch (IException &e) {
          QString msg = "RaAccelerationValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriAccelerationPoleRA = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("RaAccelerationSigma")) {
        try {
          d = (double)(g->findKeyword("RaAccelerationSigma"));
        }
        catch (IException &e) {
          QString msg = "RaAccelerationSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        poleRAAccelerationSigma = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("DecValue")) {
        try {
          d = (double)(g->findKeyword("DecValue"));
        }
        catch (IException &e) {
          QString msg = "DecValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriPoleDec = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("DecSigma")) {
        try {
          d = (double)(g->findKeyword("DecSigma"));
        }
        catch (IException &e) {
          QString msg = "DecSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        poleDecSigma = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("DecVelocityValue")) {
        try {
          d = (double)(g->findKeyword("DecVelocityValue"));
        }
        catch (IException &e) {
          QString msg = "DecVelocityValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriVelocityPoleDec = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("DecVelocitySigma")) {
        try {
          d = (double)(g->findKeyword("DecVelocitySigma"));
        }
        catch (IException &e) {
          QString msg = "DecVelocitySigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaVelocityPoleDec = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("DecAccelerationValue")) {
        try {
          d = (double)(g->findKeyword("DecAccelerationValue"));
        }
        catch (IException &e) {
          QString msg = "DecAccelerationValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriAccelerationPoleDec = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("DecAccelerationSigma")) {
        try {
          d = (double)(g->findKeyword("DecAccelerationSigma"));
        }
        catch (IException &e) {
          QString msg = "DecAccelerationSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaAccelerationPoleDec = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("PmValue")) {
        try {
          d = (double)(g->findKeyword("PmValue"));
        }
        catch (IException &e) {
          QString msg = "PmValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriPM = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("PmSigma")) {
        try {
          d = (double)(g->findKeyword("PmSigma"));
        }
        catch (IException &e) {
          QString msg = "PmSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaPM = Angle(d, Angle::Degrees);
      }


      if (g->hasKeyword("PmVelocityValue")) {
        try {
          d = (double)(g->findKeyword("PmVelocityValue"));
        }
        catch (IException &e) {
          QString msg = "PmVelocityValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriVelocityPM = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("PmVelocitySigma")) {
        try {
          d = (double)(g->findKeyword("PmVelocitySigma"));
        }
        catch (IException &e) {
          QString msg = "PmVelocitySigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaVelocityPM = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("PmAccelerationValue")) {
        try {
          d = (double)(g->findKeyword("PmAccelerationValue"));
        }
        catch (IException &e) {
          QString msg = "PmAccelerationValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriAccelerationPM = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("PmAccelerationSigma")) {
        try {
          d = (double)(g->findKeyword("PmAccelerationSigma"));
        }
        catch (IException &e) {
          QString msg = "PmAccelerationSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        pmAccelerationSigma = Angle(d, Angle::Degrees);
      }

      if (g->hasKeyword("RadiusAValue")) {
        try {
          d = (double)(g->findKeyword("RadiusAValue"));
        }
        catch (IException &e) {
          QString msg = "RadiusAValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriRadiusA = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("RadiusASigma")) {
        try {
          d = (double)(g->findKeyword("RadiusASigma"));
        }
        catch (IException &e) {
          QString msg = "RadiusASigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaRadiusA = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("RadiusBValue")) {
        try {
          d = (double)(g->findKeyword("RadiusBValue"));
        }
        catch (IException &e) {
          QString msg = "RadiusBValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriRadiusB = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("RadiusBSigma")) {
        try {
          d = (double)(g->findKeyword("RadiusBSigma"));
        }
        catch (IException &e) {
          QString msg = "RadiusBSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaRadiusB = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("RadiusCValue")) {
        try {
          d = (double)(g->findKeyword("RadiusCValue"));
        }
        catch (IException &e) {
          QString msg = "RadiusCValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriRadiusC = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("RadiusCSigma")) {
        try {
          d = (double)(g->findKeyword("RadiusCSigma"));
        }
        catch (IException &e) {
          QString msg = "RadiusCSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaRadiusC = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("MeanRadiusValue")) {
        try {
          d = (double)(g->findKeyword("MeanRadiusValue"));
        }
        catch (IException &e) {
          QString msg = "MeanRadiusValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        aprioriMeanRadius = Distance(d, Distance::Meters);
      }

      if (g->hasKeyword("MeanRadiusSigma")) {
        try {
          d = (double)(g->findKeyword("MeanRadiusSigma"));
        }
        catch (IException &e) {
          QString msg = "MeanRadiusSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        sigmaMeanRadius = Distance(d, Distance::Meters);
      }
    }

/*
    try {
      // set target id
//      setInstrumentId((QString)tbParameterGroup.nameKeyword());

      }
    catch (IException &e) {
      QString msg = "Unable to set target body solve options from the given PVL.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
*/

      setSolveSettings(targetParameterSolveCodes,
           aprioriPoleRA, poleRASigma,
           aprioriVelocityPoleRA, poleRAVelocitySigma,
           aprioriPoleDec, poleDecSigma,
           aprioriVelocityPoleDec, sigmaVelocityPoleDec,
           aprioriPM, sigmaPM,
           aprioriVelocityPM, sigmaVelocityPM,
           solveRadiiMethod,
           aprioriRadiusA, sigmaRadiusA,
           aprioriRadiusB, sigmaRadiusB,
           aprioriRadiusC, sigmaRadiusC,
           aprioriMeanRadius, sigmaMeanRadius);

    return true;
  }

  /**
   * Gets the local radius for the given latitude/longitude coordinate.
   *
   * @return Distance The distance from the center of the ellipsoid to its
   *         surface at the given lat/lon location.
   *
   */
  Distance BundleTargetBody::localRadius(const Latitude &lat, const Longitude &lon) {

    double a = m_radii[0].kilometers();
    double b = m_radii[1].kilometers();
    double c = m_radii[2].kilometers();

    double rlat = lat.radians();
    double rlon = lon.radians();

    double xyradius = a * b / sqrt(pow(b * cos(rlon), 2) +
                      pow(a * sin(rlon), 2));
    const double &radius = xyradius * c / sqrt(pow(c * cos(rlat), 2) +
                           pow(xyradius * sin(rlat), 2));

    return Distance(radius, Distance::Kilometers);
  }

  /**
   * set bundle solve parameters for an observation
   */
/*
  bool BundleTargetBody::readFromPvl(PvlGroup &tbParameterGroup) {
    // reset defaults
    //initialize();

    bool b = false;
    double d = -1.0;

    try {
      // set target id
//      setInstrumentId((QString)tbParameterGroup.nameKeyword());

      // pole right ascension settings
      if (tbParameterGroup.hasKeyword("Ra")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("Ra")[0]);
        }
        catch (IException &e) {
          QString msg = "Ra must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::PoleRA);
      }

      if (tbParameterGroup.hasKeyword("RaValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("RaValue"));
        }
        catch (IException &e) {
          QString msg = "RaValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_raPole[0] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("RaSigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("RaSigma"));
        }
        catch (IException &e) {
          QString msg = "RaSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("RaVelocity")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("RaVelocity")[0]);
        }
        catch (IException &e) {
          QString msg = "RaVelocity must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::VelocityPoleRA);
      }

      if (tbParameterGroup.hasKeyword("RaVelocityValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("RaVelocityValue"));
        }
        catch (IException &e) {
          QString msg = "RaVelocityValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("RaVelocitySigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("RaVelocitySigma"));
        }
        catch (IException &e) {
          QString msg = "RaVelocitySigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("RaAcceleration")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("RaAcceleration")[0]);
        }
        catch (IException &e) {
          QString msg = "RaAcceleration must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::VelocityPoleRA);
      }

      if (tbParameterGroup.hasKeyword("RaAccelerationValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("RaAccelerationValue"));
        }
        catch (IException &e) {
          QString msg = "RaAccelerationValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_raPole[2] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("RaAccelerationSigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("RaAccelerationSigma"));
        }
        catch (IException &e) {
          QString msg = "RaAccelerationSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      // pole declination settings
      if (tbParameterGroup.hasKeyword("Dec")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("Dec")[0]);
        }
        catch (IException &e) {
          QString msg = "Dec parameter must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::PoleDec);
      }

      if (tbParameterGroup.hasKeyword("DecValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("DecValue"));
        }
        catch (IException &e) {
          QString msg = "DecValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_decPole[0] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("DecSigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("DecSigma"));
        }
        catch (IException &e) {
          QString msg = "DecSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("DecVelocity")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("DecVelocity")[0]);
        }
        catch (IException &e) {
          QString msg = "DecVelocity must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::VelocityPoleDec);
      }

      if (tbParameterGroup.hasKeyword("DecVelocityValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("DecVelocityValue"));
        }
        catch (IException &e) {
          QString msg = "DecVelocityValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_decPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("DecVelocitySigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("DecVelocitySigma"));
        }
        catch (IException &e) {
          QString msg = "DecVelocitySigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("DecAcceleration")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("DecAcceleration")[0]);
        }
        catch (IException &e) {
          QString msg = "DecAcceleration must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::AccelerationPoleDec);
      }

      if (tbParameterGroup.hasKeyword("DecAccelerationValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("DecAccelerationValue"));
        }
        catch (IException &e) {
          QString msg = "DecAccelerationValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_decPole[2] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("DecAccelerationSigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("DecAccelerationSigma"));
        }
        catch (IException &e) {
          QString msg = "DecAccelerationSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      // prime meridian settings
      if (tbParameterGroup.hasKeyword("Pm")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("Pm")[0]);
        }
        catch (IException &e) {
          QString msg = "Pm parameter must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::PM);
      }

      if (tbParameterGroup.hasKeyword("PmValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("PmValue"));
        }
        catch (IException &e) {
          QString msg = "PmValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_pm[0] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("PmSigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("PmSigma"));
        }
        catch (IException &e) {
          QString msg = "PmSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("PmVelocity")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("PmVelocity")[0]);
        }
        catch (IException &e) {
          QString msg = "PmVelocity must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::VelocityPM);
      }

      if (tbParameterGroup.hasKeyword("PmVelocityValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("PmVelocityValue"));
        }
        catch (IException &e) {
          QString msg = "PmVelocityValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_pm[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("PmVelocitySigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("PmVelocitySigma"));
        }
        catch (IException &e) {
          QString msg = "PmVelocitySigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("PmAcceleration")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("PmAcceleration")[0]);
        }
        catch (IException &e) {
          QString msg = "PmAcceleration must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b)
          m_parameterSolveCodes.insert(BundleTargetBody::AccelerationPM);
      }

      if (tbParameterGroup.hasKeyword("PmAccelerationValue")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("PmAccelerationValue"));
        }
        catch (IException &e) {
          QString msg = "PmAccelerationValue must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        m_decPole[2] = Angle(d, Angle::Degrees);
      }

      if (tbParameterGroup.hasKeyword("PmAccelerationSigma")) {
        try {
          d = (double)(tbParameterGroup.findKeyword("PmAccelerationSigma"));
        }
        catch (IException &e) {
          QString msg = "PmAccelerationSigma must be a valid double (>= 0; blank defaults to 0).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        //m_raPole[1] = Angle(d, Angle::Degrees);
      }

      // radii settings
      if (tbParameterGroup.hasKeyword("AllRadii")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("AllRadii")[0]);
        }
        catch (IException &e) {
          QString msg = "AllRadii parameter must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b) {
          m_parameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusA);
          m_parameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusB);
          m_parameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusC);

          if (tbParameterGroup.hasKeyword("RadiusAValue")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("RadiusAValue"));
            }
            catch (IException &e) {
              QString msg = "RadiusAValue must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            m_radii[0].setKilometers(d);
          }

          if (tbParameterGroup.hasKeyword("RadiusASigma")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("RadiusASigma"));
            }
            catch (IException &e) {
              QString msg = "RadiusASigma must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            //m_raPole[1] = Angle(d, Angle::Degrees);
          }

          if (tbParameterGroup.hasKeyword("RadiusBValue")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("RadiusBValue"));
            }
            catch (IException &e) {
              QString msg = "RadiusBValue must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            m_radii[1].setKilometers(d);
          }

          if (tbParameterGroup.hasKeyword("RadiusBSigma")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("RadiusBSigma"));
            }
            catch (IException &e) {
              QString msg = "RadiusBSigma must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            //m_raPole[1] = Angle(d, Angle::Degrees);
          }

          if (tbParameterGroup.hasKeyword("RadiusCValue")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("RadiusCValue"));
            }
            catch (IException &e) {
              QString msg = "RadiusCValue must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            m_radii[2].setKilometers(d);
          }

          if (tbParameterGroup.hasKeyword("RadiusCSigma")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("RadiusCSigma"));
            }
            catch (IException &e) {
              QString msg = "RadiusCSigma must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            //m_raPole[1] = Angle(d, Angle::Degrees);
          }
        }
      }
      else if (tbParameterGroup.hasKeyword("MeanRadius")) {
        try {
          b = toBool(tbParameterGroup.findKeyword("MeanRadius")[0]);
        }
        catch (IException &e) {
          QString msg = "MeanRadius parameter must be a valid boolean (yes/no; true/false).";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        if (b) {
          m_parameterSolveCodes.insert(BundleTargetBody::MeanRadius);

          if (tbParameterGroup.hasKeyword("MeanRadiusValue")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("MeanRadiusValue"));
            }
            catch (IException &e) {
              QString msg = "MeanRadiusValue must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            m_radii[2].setKilometers(d);
          }

          if (tbParameterGroup.hasKeyword("MeanRadiusSigma")) {
            try {
              d = (double)(tbParameterGroup.findKeyword("MeanRadiusSigma"));
            }
            catch (IException &e) {
              QString msg = "MeanRadiusSigma must be a valid double (>= 0; blank defaults to 0).";
              throw IException(IException::User, msg, _FILEINFO_);
            }
            //m_raPole[1] = Angle(d, Angle::Degrees);
          }
        }
      }
    }

    catch (IException &e) {
      QString msg = "Unable to set target body solve options from the given PVL.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }

    return true;
  }
*/
}
