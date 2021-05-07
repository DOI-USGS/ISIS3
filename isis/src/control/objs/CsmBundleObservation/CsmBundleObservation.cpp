/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CsmBundleObservation.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>

#include "BundleImage.h"
#include "BundleControlPoint.h"
#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "CSMCamera.h"
#include "LinearAlgebra.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a CsmBundleObservation initialized to a default state.
   */
  CsmBundleObservation::CsmBundleObservation() {
  }


  /**
   * Constructs a CsmBundleObservation from an BundleImage, an instrument id, an observation
   * number to assign to this CsmBundleObservation, and a target body.
   *
   * @param image QSharedPointer to the primary image in the observation
   * @param observationNumber Observation number of the observation
   * @param instrumentId Id of the instrument for the observation
   * @param bundleTargetBody QSharedPointer to the target body of the observation
   */
  CsmBundleObservation::CsmBundleObservation(BundleImageQsp image, QString observationNumber,
                                       QString instrumentId, BundleTargetBodyQsp bundleTargetBody) : AbstractBundleObservation(image, observationNumber, instrumentId, bundleTargetBody) {
    if (bundleTargetBody) {
      QString msg = "Target body parameters cannot be solved for with CSM observations.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Creates a copy of another CsmBundleObservation.
   *
   * @param src Reference to the CsmBundleObservation to copy
   */
  CsmBundleObservation::CsmBundleObservation(const CsmBundleObservation &src) : AbstractBundleObservation(src) {
    m_solveSettings = src.m_solveSettings;
    m_paramIndices = src.m_paramIndices;
  }


  /**
   * Destructor.
   *
   * Contained BundleImages will remain until all shared pointers are deleted.
   */
  CsmBundleObservation::~CsmBundleObservation() {
  }


  /**
   * Assignment operator
   *
   * Assigns the state of the source CsmBundleObservation to this CsmBundleObservation
   *
   * @param CsmBundleObservation Reference to the source CsmBundleObservation to assign from
   *
   * @return @b CsmBundleObservation& Reference to this CsmBundleObservation
   */
  CsmBundleObservation &CsmBundleObservation::operator=(const CsmBundleObservation &src) {
    if (&src != this) {
      m_solveSettings = src.m_solveSettings;
      m_paramIndices = src.m_paramIndices;
    }
    return *this;
  }


  /**
   * Set solve parameters
   *
   * @param solveSettings The solve settings to use
   *
   * @return @b bool Returns true if settings were successfully set
   */
  bool CsmBundleObservation::setSolveSettings(BundleObservationSolveSettingsQsp solveSettings) {
    m_solveSettings = solveSettings;

    CSMCamera *csmCamera = dynamic_cast<CSMCamera*>(front()->camera());

    m_paramIndices.clear();
    m_weights.clear();
    m_corrections.clear();
    m_adjustedSigmas.clear();

    if (solveSettings->csmSolveOption() == BundleObservationSolveSettings::Set) {
      m_paramIndices = csmCamera->getParameterIndices(solveSettings->csmParameterSet());
    }
    else if (solveSettings->csmSolveOption() == BundleObservationSolveSettings::Type) {
      m_paramIndices = csmCamera->getParameterIndices(solveSettings->csmParameterType());
    }
    else if (solveSettings->csmSolveOption() == BundleObservationSolveSettings::List) {
      m_paramIndices = csmCamera->getParameterIndices(solveSettings->csmParameterList());
    }
    else {
      return false;
    }

    int nParams = m_paramIndices.size();

    m_weights.resize(nParams);
    m_corrections.resize(nParams);
    m_adjustedSigmas.resize(nParams);
    m_aprioriSigmas.resize(nParams);

    for (int i = 0; i < nParams; i++) {
      m_aprioriSigmas[i] = csmCamera->getParameterCovariance(m_paramIndices[i], m_paramIndices[i]);
    }

    return true;
  }


  // TODO change this to CSM solve settings once compute partials is updated
  /**
   * Accesses the solve settings
   *
   * @return @b const BundleObservationSolveSettingsQsp Returns a pointer to the solve
   *                                                    settings for this CsmBundleObservation
   */
  const BundleObservationSolveSettingsQsp CsmBundleObservation::solveSettings() {
    return BundleObservationSolveSettingsQsp(nullptr);
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
   * @throws IException::Unknown "Unable to apply parameter corrections to CsmBundleObservation."
   *
   * @return @b bool Returns true upon successful application of corrections
   *
   * @internal
   *   @todo always returns true?
   */
  bool CsmBundleObservation::applyParameterCorrections(LinearAlgebra::Vector corrections) {
    // Check that the correction vector is the correct size
    if (corrections.size() != m_paramIndices.size()) {
      QString msg = "Invalid correction vector passed to observation.";
      IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Apply the corrections to the CSM camera
    CSMCamera *csmCamera = dynamic_cast<CSMCamera*>(front()->camera());
    for (size_t i = 0; i < corrections.size(); i++) {
      csmCamera->applyParameterCorrection(m_paramIndices[i], corrections[i]);
    }

    // Accumulate the total corrections
    m_corrections += corrections;

    return true;
  }


  /**
   * Returns the number of total parameters there are for solving
   *
   * The total number of parameters is equal to the number of position parameters and number of
   * pointing parameters
   *
   * @return @b int Returns the number of parameters there are
   */
  int CsmBundleObservation::numberParameters() {
    return m_paramIndices.size();
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
 * @return @b QString Returns a formatted QString representing the CsmBundleObservation
 *
 * @internal
 *   @history 2016-10-26 Ian Humphrey - Default values are now provided for parameters that are
 *                           not being solved. Fixes #4464.
 */
QString CsmBundleObservation::formatBundleOutputString(bool errorPropagation, bool imageCSV) {
  // TODO implement for CSM

  // std::cerr << "The function formatBundleOutputString is depricated as of ISIS 3.9"
  //              "and will be removed in ISIS 4.0" << std::endl;

  // std::vector<double> coefX;
  // std::vector<double> coefY;
  // std::vector<double> coefZ;
  // std::vector<double> coefRA;
  // std::vector<double> coefDEC;
  // std::vector<double> coefTWI;

  // int nPositionCoefficients = m_solveSettings->numberCameraPositionCoefficientsSolved();
  // int nPointingCoefficients = m_solveSettings->numberCameraAngleCoefficientsSolved();

  // // Indicate if we need to obtain default position or pointing values
  // bool useDefaultPosition = false;
  // bool useDefaultPointing = false;
  // // Indicate if we need to use default values when not solving twist
  // bool useDefaultTwist = !(m_solveSettings->solveTwist());

  // // If we aren't solving for position, set the number of coefficients to 1 so we can output the
  // // instrumentPosition's center coordinate values for X, Y, and Z
  // if (nPositionCoefficients == 0) {
  //   nPositionCoefficients = 1;
  //   useDefaultPosition = true;
  // }
  // // If we arent' solving for pointing, set the number of coefficients to 1 so we can output the
  // // instrumentPointing's center angles for RA, DEC, and TWI
  // if (nPointingCoefficients == 0) {
  //   nPointingCoefficients = 1;
  //   useDefaultPointing = true;
  // }

  // // Force number of position and pointing parameters to each be 3 (X,Y,Z; RA,DEC,TWI)
  // // so we can always output a value for them
  // int nPositionParameters = 3 * nPositionCoefficients;
  // int nPointingParameters = 3 * nPointingCoefficients;
  // int nParameters = nPositionParameters + nPointingParameters;

  // coefX.resize(nPositionCoefficients);
  // coefY.resize(nPositionCoefficients);
  // coefZ.resize(nPositionCoefficients);
  // coefRA.resize(nPointingCoefficients);
  // coefDEC.resize(nPointingCoefficients);
  // coefTWI.resize(nPointingCoefficients);

  // if (m_instrumentPosition) {
  //   if (!useDefaultPosition) {
  //     m_instrumentPosition->GetPolynomial(coefX, coefY, coefZ);
  //   }
  //   // Use the position's center coordinate if not solving for spacecraft position
  //   else {
  //     const std::vector<double> centerCoord = m_instrumentPosition->GetCenterCoordinate();
  //     coefX[0] = centerCoord[0];
  //     coefY[0] = centerCoord[1];
  //     coefZ[0] = centerCoord[2];
  //   }
  // }

  // if (m_instrumentRotation) {
  //   if (!useDefaultPointing) {
  //     m_instrumentRotation->GetPolynomial(coefRA, coefDEC, coefTWI);
  //   }
  //   // Use the pointing's center angles if not solving for pointing (rotation)
  //   else {
  //     const std::vector<double> centerAngles = m_instrumentRotation->GetCenterAngles();
  //     coefRA[0] = centerAngles[0];
  //     coefDEC[0] = centerAngles[1];
  //     coefTWI[0] = centerAngles[2];
  //   }
  // }

  // // for convenience, create vectors of parameters names and values in the correct sequence
  // std::vector<double> finalParameterValues;
  // QStringList parameterNamesList;

  // if (!imageCSV) {

  //   QString str("%1(t%2)");

  //   if (nPositionCoefficients > 0) {
  //     for (int i = 0; i < nPositionCoefficients; i++) {
  //       finalParameterValues.push_back(coefX[i]);
  //       if (i == 0)
  //         parameterNamesList.append( str.arg("  X  ").arg("0") );
  //       else
  //         parameterNamesList.append( str.arg("     ").arg(i) );
  //     }
  //     for (int i = 0; i < nPositionCoefficients; i++) {
  //       finalParameterValues.push_back(coefY[i]);
  //       if (i == 0)
  //         parameterNamesList.append( str.arg("  Y  ").arg("0") );
  //       else
  //         parameterNamesList.append( str.arg("     ").arg(i) );
  //     }
  //     for (int i = 0; i < nPositionCoefficients; i++) {
  //       finalParameterValues.push_back(coefZ[i]);
  //       if (i == 0)
  //         parameterNamesList.append( str.arg("  Z  ").arg("0") );
  //       else
  //         parameterNamesList.append( str.arg("     ").arg(i) );
  //     }
  //   }
  //   if (nPointingCoefficients > 0) {
  //     for (int i = 0; i < nPointingCoefficients; i++) {
  //       finalParameterValues.push_back(coefRA[i] * RAD2DEG);
  //       if (i == 0)
  //         parameterNamesList.append( str.arg(" RA  ").arg("0") );
  //       else
  //         parameterNamesList.append( str.arg("     ").arg(i) );
  //     }
  //     for (int i = 0; i < nPointingCoefficients; i++) {
  //       finalParameterValues.push_back(coefDEC[i] * RAD2DEG);
  //       if (i == 0)
  //         parameterNamesList.append( str.arg("DEC  ").arg("0") );
  //       else
  //         parameterNamesList.append( str.arg("     ").arg(i) );
  //     }
  //     for (int i = 0; i < nPointingCoefficients; i++) {
  //       finalParameterValues.push_back(coefTWI[i] * RAD2DEG);
  //       if (i == 0)
  //         parameterNamesList.append( str.arg("TWI  ").arg("0") );
  //       else
  //         parameterNamesList.append( str.arg("     ").arg(i) );
  //     }
  //   }

  // }// end if(!imageCSV)

  // else {
  //   if (nPositionCoefficients > 0) {
  //     for (int i = 0; i < nPositionCoefficients; i++) {
  //       finalParameterValues.push_back(coefX[i]);
  //     }
  //     for (int i = 0; i < nPositionCoefficients; i++) {
  //       finalParameterValues.push_back(coefY[i]);
  //     }
  //     for (int i = 0; i < nPositionCoefficients; i++) {
  //       finalParameterValues.push_back(coefZ[i]);
  //     }
  //   }
  //   if (nPointingCoefficients > 0) {
  //     for (int i = 0; i < nPointingCoefficients; i++) {
  //       finalParameterValues.push_back(coefRA[i] * RAD2DEG);
  //     }
  //     for (int i = 0; i < nPointingCoefficients; i++) {
  //       finalParameterValues.push_back(coefDEC[i] * RAD2DEG);
  //     }
  //     for (int i = 0; i < nPointingCoefficients; i++) {
  //       finalParameterValues.push_back(coefTWI[i] * RAD2DEG);
  //     }
  //   }
  // }//end else

  // // Save the list of parameter names we've accumulated above
  // m_parameterNamesList = parameterNamesList;

  // QString finalqStr = "";
  // QString qStr = "";

  // // Set up default values when we are using default position
  // QString sigma = "N/A";
  // QString adjustedSigma = "N/A";
  // double correction = 0.0;

  // // this implies we're writing to bundleout.txt
  // if (!imageCSV) {
  //   // position parameters
  //   for (int i = 0; i < nPositionParameters; i++) {
  //     // If not using the default position, we can correctly access sigmas and corrections
  //     // members
  //     if (!useDefaultPosition) {
  //       correction = m_corrections(i);
  //       adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
  //       sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
  //     }
  //     if (errorPropagation) {
  //       qStr = QString("%1%2%3%4%5%6\n").
  //       arg( parameterNamesList.at(i) ).
  //       arg(finalParameterValues[i] - correction, 17, 'f', 8).
  //       arg(correction, 21, 'f', 8).
  //       arg(finalParameterValues[i], 20, 'f', 8).
  //       arg(sigma, 18).
  //       arg(adjustedSigma, 18);
  //     }
  //     else {
  //       qStr = QString("%1%2%3%4%5%6\n").
  //       arg( parameterNamesList.at(i) ).
  //       arg(finalParameterValues[i] - correction, 17, 'f', 8).
  //       arg(correction, 21, 'f', 8).
  //       arg(finalParameterValues[i], 20, 'f', 8).
  //       arg(sigma, 18).
  //       arg("N/A", 18);
  //     }
  //     finalqStr += qStr;
  //   }

  //   // We need to use an offset of -3 (1 coef; X,Y,Z) if we used the default center coordinate
  //   // (i.e. we did not solve for position), as m_corrections and m_*sigmas are populated
  //   // according to which parameters are solved
  //   int offset = 0;
  //   if (useDefaultPosition) {
  //     offset = 3;
  //   }
  //   // pointing parameters
  //   for (int i = nPositionParameters; i < nParameters; i++) {
  //     if (!useDefaultPointing) {
  //       // If solving camera and not solving for twist, provide default values for twist to
  //       // prevent bad indexing into m_corrections and m_*sigmas
  //       // TWIST is last parameter, which corresponds to nParameters - nPointingCoefficients
  //       if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
  //         correction = 0.0;
  //         adjustedSigma = "N/A";
  //         sigma = "N/A";
  //       }
  //       else {
  //         correction = m_corrections(i - offset);
  //         adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
  //         sigma = ( IsSpecial(m_aprioriSigmas[i - offset]) ? "FREE" :
  //                 toString(m_aprioriSigmas[i-offset], 8) );
  //       }
  //     }
  //     // We are using default pointing, so provide default correction and sigma values to output
  //     else {
  //       correction = 0.0;
  //       adjustedSigma = "N/A";
  //       sigma = "N/A";
  //     }
  //     if (errorPropagation) {
  //       qStr = QString("%1%2%3%4%5%6\n").
  //       arg( parameterNamesList.at(i) ).
  //       arg( (finalParameterValues[i] - correction * RAD2DEG), 17, 'f', 8).
  //       arg(correction * RAD2DEG, 21, 'f', 8).
  //       arg(finalParameterValues[i], 20, 'f', 8).
  //       arg(sigma, 18).
  //       arg(adjustedSigma, 18);
  //     }
  //     else {
  //       qStr = QString("%1%2%3%4%5%6\n").
  //       arg( parameterNamesList.at(i) ).
  //       arg( (finalParameterValues[i] - correction * RAD2DEG), 17, 'f', 8).
  //       arg(correction * RAD2DEG, 21, 'f', 8).
  //       arg(finalParameterValues[i], 20, 'f', 8).
  //       arg(sigma, 18).
  //       arg("N/A", 18);
  //     }
  //     finalqStr += qStr;
  //   }

  // }
  // // this implies we're writing to images.csv
  // else {
  //   // position parameters
  //   for (int i = 0; i < nPositionParameters; i++) {
  //     if (!useDefaultPosition) {
  //       correction = m_corrections(i);
  //       adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
  //       sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
  //     }
  //     // Provide default values for position if not solving position
  //     else {
  //       correction = 0.0;
  //       adjustedSigma = "N/A";
  //       sigma = "N/A";
  //     }
  //     qStr = "";
  //     if (errorPropagation) {
  //       qStr += toString(finalParameterValues[i] - correction) + ",";
  //       qStr += toString(correction) + ",";
  //       qStr += toString(finalParameterValues[i]) + ",";
  //       qStr += sigma + ",";
  //       qStr += adjustedSigma + ",";
  //     }
  //     else {
  //       qStr += toString(finalParameterValues[i] - correction) + ",";
  //       qStr += toString(correction) + ",";
  //       qStr += toString(finalParameterValues[i]) + ",";
  //       qStr += sigma + ",";
  //       qStr += "N/A,";
  //     }
  //     finalqStr += qStr;
  //   }

  //   // If not solving position, we need to offset access to correction and sigma members by -3
  //   // (X,Y,Z) since m_corrections and m_*sigmas are populated according to which parameters are
  //   // solved
  //   int offset = 0;
  //   if (useDefaultPosition) {
  //     offset = 3;
  //   }
  //   // pointing parameters
  //   for (int i = nPositionParameters; i < nParameters; i++) {
  //     if (!useDefaultPointing) {
  //       // Use default values if solving camera but not solving for TWIST to prevent bad indexing
  //       // into m_corrections and m_*sigmas
  //       if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
  //         correction = 0.0;
  //         adjustedSigma = "N/A";
  //         sigma = "N/A";
  //       }
  //       else {
  //         correction = m_corrections(i - offset);
  //         adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
  //         sigma = ( IsSpecial(m_aprioriSigmas[i-offset]) ? "FREE" :
  //             toString(m_aprioriSigmas[i-offset], 8) );
  //       }
  //     }
  //     // Provide default values for pointing if not solving pointing
  //     else {
  //       correction = 0.0;
  //       adjustedSigma = "N/A";
  //       sigma = "N/A";
  //     }
  //     qStr = "";
  //     if (errorPropagation) {
  //       qStr += toString(finalParameterValues[i] - correction * RAD2DEG) + ",";
  //       qStr += toString(correction * RAD2DEG) + ",";
  //       qStr += toString(finalParameterValues[i]) + ",";
  //       qStr += sigma + ",";
  //       qStr += adjustedSigma + ",";
  //     }
  //     else {
  //       qStr += toString(finalParameterValues[i] - correction * RAD2DEG) + ",";
  //       qStr += toString(correction * RAD2DEG) + ",";
  //       qStr += toString(finalParameterValues[i]) + ",";
  //       qStr += sigma + ",";
  //       qStr += "N/A,";
  //     }
  //     finalqStr += qStr;
  //   }
  // }

  return "";
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
  void CsmBundleObservation::bundleOutputString(std::ostream &fpOut, bool errorPropagation) {
    // TODO implement for CSM

    // char buf[4096];

    // QVector<double> finalParameterValues;
    // int nPositionCoefficients, nPointingCoefficients;
    // bool useDefaultPosition, useDefaultPointing,useDefaultTwist;

    // bundleOutputFetchData(finalParameterValues,
    //                       nPositionCoefficients,nPointingCoefficients,
    //                       useDefaultPosition,useDefaultPointing,useDefaultTwist);

    // int nPositionParameters = 3 * nPositionCoefficients;
    // int nPointingParameters = 3 * nPointingCoefficients;
    // int nParameters = nPositionParameters + nPointingParameters;

    // // for convenience, create vectors of parameters names and values in the correct sequence
    // QStringList parameterNamesListX,parameterNamesListY,parameterNamesListZ,
    //     parameterNamesListRA,parameterNamesListDEC,parameterNamesListTWI,
    //     parameterNamesList;
    // QStringList correctionUnitListX,correctionUnitListY,correctionUnitListZ,
    //     correctionUnitListRA,correctionUnitListDEC,correctionUnitListTWI,
    //     correctionUnitList;

    // QString str("%1(%2)  ");
    // QString str2("%1(%2) ");
    // QString strN("%1(%2)");


    // if (nPositionCoefficients > 0) {
    //   for (int j = 0; j < nPositionCoefficients;j++) {
    //     if (j == 0) {
    //       parameterNamesListX.append(str.arg("  X  ").arg("km"));
    //       parameterNamesListY.append(str.arg("  Y  ").arg("km"));
    //       parameterNamesListZ.append(str.arg("  Z  ").arg("km"));
    //       correctionUnitListX.append("m");
    //       correctionUnitListY.append("m");
    //       correctionUnitListZ.append("m");
    //     } //end inner-if

    //     else if (j==1) {
    //       parameterNamesListX.append( str2.arg("    ").arg("km/s") );
    //       parameterNamesListY.append( str2.arg("    ").arg("km/s") );
    //       parameterNamesListZ.append( str2.arg("    ").arg("km/s") );
    //       correctionUnitListX.append("m/s");
    //       correctionUnitListY.append("m/s");
    //       correctionUnitListZ.append("m/s");
    //     }
    //     else {
    //       QString str("%1(%2)");
    //       parameterNamesListX.append(strN.arg("   ").arg("km/s^"+toString(j) ) );
    //       parameterNamesListY.append(strN.arg("   ").arg("km/s^"+toString(j) ) );
    //       parameterNamesListZ.append(strN.arg("   ").arg("km/s^"+toString(j) ) );
    //       correctionUnitListX.append("m/s^"+toString(j));
    //       correctionUnitListY.append("m/s^"+toString(j));
    //       correctionUnitListZ.append("m/s^"+toString(j));
    //     }
    //   }//end for
    // }//end outer-if

    // if (nPointingCoefficients > 0) {
    //   for (int j = 0; j < nPointingCoefficients;j++) {
    //     if (j == 0) {
    //       parameterNamesListRA.append(str.arg(" RA  ").arg("dd"));
    //       parameterNamesListDEC.append(str.arg("DEC  ").arg("dd"));
    //       parameterNamesListTWI.append(str.arg("TWI  ").arg("dd"));
    //       correctionUnitListRA.append("dd");
    //       correctionUnitListDEC.append("dd");
    //       correctionUnitListTWI.append("dd");
    //     } //end inner-if

    //     else if (j==1) {
    //       parameterNamesListRA.append( str2.arg("    ").arg("dd/s") );
    //       parameterNamesListDEC.append( str2.arg("    ").arg("dd/s") );
    //       parameterNamesListTWI.append( str2.arg("    ").arg("dd/s") );
    //       correctionUnitListRA.append("dd/s");
    //       correctionUnitListDEC.append("dd/s");
    //       correctionUnitListTWI.append("dd/s");
    //     }
    //     else {
    //       parameterNamesListRA.append(strN.arg("   ").arg("dd/s^"+toString(j) ) );
    //       parameterNamesListDEC.append(strN.arg("   ").arg("dd/s^"+toString(j) ) );
    //       parameterNamesListTWI.append(strN.arg("   ").arg("dd/s^"+toString(j) ) );
    //       correctionUnitListRA.append("dd/s^"+toString(j));
    //       correctionUnitListDEC.append("dd/s^"+toString(j));
    //       correctionUnitListTWI.append("dd/s^"+toString(j));
    //     }
    //   }//end for
    // }// end outer-if

    //  //Put all of the parameter names together into one QStringList
    // parameterNamesList.append(parameterNamesListX);
    // parameterNamesList.append(parameterNamesListY);
    // parameterNamesList.append(parameterNamesListZ);
    // parameterNamesList.append(parameterNamesListRA);
    // parameterNamesList.append(parameterNamesListDEC);
    // parameterNamesList.append(parameterNamesListTWI);

    // //Put all of the correction unit names together into one QStringList
    // correctionUnitList.append(correctionUnitListX);
    // correctionUnitList.append(correctionUnitListY);
    // correctionUnitList.append(correctionUnitListZ);
    // correctionUnitList.append(correctionUnitListDEC);
    // correctionUnitList.append(correctionUnitListRA);
    // correctionUnitList.append(correctionUnitListTWI);

    // // Save the list of parameter names we've accumulated above
    // m_parameterNamesList = parameterNamesList;

    // // Set up default values when we are using default position
    // QString sigma = "N/A";
    // QString adjustedSigma = "N/A";
    // double correction = 0.0;

    // // position parameters
    // for (int i = 0; i < nPositionParameters; i++) {
    //   // If not using the default position, we can correctly access sigmas and corrections
    //   // members
    //   if (!useDefaultPosition) {
    //     correction = m_corrections(i);
    //     adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
    //     sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
    //   }

    //   sprintf(buf,"%s",parameterNamesList.at(i).toStdString().c_str() );
    //   fpOut << buf;
    //   sprintf(buf,"%18.8lf  ",finalParameterValues[i] - correction);
    //   fpOut << buf;
    //   sprintf(buf,"%20.8lf  ",correction);
    //   fpOut << buf;
    //   sprintf(buf,"%23.8lf  ",finalParameterValues[i]);
    //   fpOut << buf;
    //   sprintf(buf,"            ");
    //   fpOut << buf;
    //   sprintf(buf,"%6s",sigma.toStdString().c_str());
    //   fpOut << buf;
    //   sprintf(buf,"            ");
    //   fpOut << buf;
    //   if (errorPropagation) {
    //     sprintf(buf,"%s",adjustedSigma.toStdString().c_str());
    //   }
    //   else {
    //     sprintf(buf,"%s","N/A");
    //   }
    //   fpOut<<buf;
    //   sprintf(buf,"        ");
    //   fpOut<<buf;
    //   sprintf(buf,"%s\n",correctionUnitList.at(i).toStdString().c_str() );
    //   fpOut<<buf;

    // }

    // // We need to use an offset of -3 (1 coef; X,Y,Z) if we used the default center coordinate
    // // (i.e. we did not solve for position), as m_corrections and m_*sigmas are populated
    // // according to which parameters are solved
    // int offset = 0;
    // if (useDefaultPosition) {
    //   offset = 3;
    // }

    // // pointing parameters
    // for (int i = nPositionParameters; i < nParameters; i++) {
    //   if (!useDefaultPointing) {
    //     // If solving camera and not solving for twist, provide default values for twist to
    //     // prevent bad indexing into m_corrections and m_*sigmas
    //     // TWIST is last parameter, which corresponds to nParameters - nPointingCoefficients
    //     if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
    //       correction = 0.0;
    //       adjustedSigma = "N/A";
    //       sigma = "N/A";
    //     }
    //     else {
    //       correction = m_corrections(i - offset);
    //       adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
    //       sigma = ( IsSpecial(m_aprioriSigmas[i - offset]) ? "FREE" :
    //               toString(m_aprioriSigmas[i-offset], 8) );
    //     }
    //   }
    //   // We are using default pointing, so provide default correction and sigma values to output
    //   else {
    //     correction = 0.0;
    //     adjustedSigma = "N/A";
    //     sigma = "N/A";
    //   }

    //   sprintf(buf,"%s",parameterNamesList.at(i).toStdString().c_str() );
    //   fpOut << buf;
    //   sprintf(buf,"%18.8lf  ",(finalParameterValues[i]*RAD2DEG - correction*RAD2DEG));
    //   fpOut << buf;
    //   sprintf(buf,"%20.8lf  ",(correction*RAD2DEG));
    //   fpOut << buf;
    //   sprintf(buf,"%23.8lf  ",(finalParameterValues[i]*RAD2DEG));
    //   fpOut << buf;
    //   sprintf(buf,"            ");
    //   fpOut << buf;
    //   sprintf(buf,"%6s",sigma.toStdString().c_str());
    //   fpOut << buf;
    //   sprintf(buf,"            ");
    //   fpOut << buf;
    //   if (errorPropagation) {
    //     sprintf(buf,"%s",adjustedSigma.toStdString().c_str());
    //   }
    //   else {
    //     sprintf(buf,"%s","N/A");
    //   }
    //   fpOut<<buf;
    //   sprintf(buf,"        ");
    //   fpOut<<buf;
    //   sprintf(buf,"%s\n",correctionUnitList.at(i).toStdString().c_str() );
    //   fpOut<<buf;
    // }

  }

  /**
   * @brief Creates and returns a formatted QString representing the bundle coefficients and
   * parameters in csv format.
   *
   * @param errorPropagation Boolean indicating whether or not to attach more information
   *     (corrections, sigmas, adjusted sigmas...) to the output QString
   *
   * @return @b QString Returns a formatted QString representing the CsmBundleObservation in
   * csv format
   */
  QString CsmBundleObservation::bundleOutputCSV(bool errorPropagation) {
    // TODO implement for CSM

    // QVector<double> finalParameterValues;
    // int nPositionCoefficients, nPointingCoefficients;
    // bool useDefaultPosition, useDefaultPointing,useDefaultTwist;

    // bundleOutputFetchData(finalParameterValues,
    //                       nPositionCoefficients,nPointingCoefficients,
    //                       useDefaultPosition,useDefaultPointing,useDefaultTwist);

    // int nPositionParameters = 3 * nPositionCoefficients;
    // int nPointingParameters = 3 * nPointingCoefficients;
    // int nParameters = nPositionParameters + nPointingParameters;

    // QString finalqStr = "";

    // // Set up default values when we are using default position
    // QString sigma = "N/A";
    // QString adjustedSigma = "N/A";
    // double correction = 0.0;

    // // Position parameters
    // for (int i = 0; i < nPositionParameters; i++) {
    //   if (!useDefaultPosition) {
    //     correction = m_corrections(i);
    //     adjustedSigma = QString::number(m_adjustedSigmas[i], 'f', 8);
    //     sigma = ( IsSpecial(m_aprioriSigmas[i]) ? "FREE" : toString(m_aprioriSigmas[i], 8) );
    //   }
    //   // Provide default values for position if not solving position
    //   else {
    //     correction = 0.0;
    //     adjustedSigma = "N/A";
    //     sigma = "N/A";
    //   }

    //   finalqStr += toString(finalParameterValues[i] - correction) + ",";
    //   finalqStr += toString(correction) + ",";
    //   finalqStr += toString(finalParameterValues[i]) + ",";
    //   finalqStr += sigma + ",";
    //   if (errorPropagation) {
    //     finalqStr += adjustedSigma + ",";
    //   }
    //   else {
    //     finalqStr += "N/A,";
    //   }

    // }

    // // If not solving position, we need to offset access to correction and sigma members by -3
    // // (X,Y,Z) since m_corrections and m_*sigmas are populated according to which parameters are
    // // solved
    // int offset = 0;
    // if (useDefaultPosition) {
    //   offset = 3;
    // }
    // // pointing parameters
    // for (int i = nPositionParameters; i < nParameters; i++) {
    //   if (!useDefaultPointing) {
    //     // Use default values if solving camera but not solving for TWIST to prevent bad indexing
    //     // into m_corrections and m_*sigmas
    //     if ( (i >= nParameters - nPointingCoefficients) && useDefaultTwist) {
    //       correction = 0.0;
    //       adjustedSigma = "N/A";
    //       sigma = "N/A";
    //     }
    //     else {
    //       correction = m_corrections(i - offset);
    //       adjustedSigma = QString::number(m_adjustedSigmas(i-offset) * RAD2DEG, 'f', 8);
    //       sigma = ( IsSpecial(m_aprioriSigmas[i-offset]) ? "FREE" :
    //           toString(m_aprioriSigmas[i-offset], 8) );
    //     }
    //   }
    //   // Provide default values for pointing if not solving pointing
    //   else {
    //     correction = 0.0;
    //     adjustedSigma = "N/A";
    //     sigma = "N/A";
    //   }

    //   finalqStr += toString(finalParameterValues[i]*RAD2DEG - correction * RAD2DEG) + ",";
    //   finalqStr += toString(correction * RAD2DEG) + ",";
    //   finalqStr += toString(finalParameterValues[i]*RAD2DEG) + ",";
    //   finalqStr += sigma + ",";
    //   if (errorPropagation) {
    //     finalqStr += adjustedSigma + ",";
    //   }
    //   else {
    //     finalqStr += "N/A,";
    //   }

    // }

    return "";
  }

  /**
   * Cannot compute target body parameters for a CSM observation, 
   * so always throws an exception. 
   * 
   * @param coeffTarget Matrix for target body partial derivatives
   * @param measure The measure that the partials are being 
   *                computed for.
   * @param bundleSettings The settings for the bundle adjustment
   * @param bundleTargetBody QSharedPointer to the target body of 
   *                         the observation
   * 
   * @return bool Always false
   */
  bool CsmBundleObservation::computeTargetPartials(LinearAlgebra::Matrix &coeffTarget, BundleMeasure &measure, BundleSettingsQsp &bundleSettings, BundleTargetBodyQsp &bundleTargetBody) {
    if (bundleTargetBody) {
      QString msg = "Target body parameters cannot be solved for with CSM observations.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    return false;
  }


  /**
   * Calculates the sensor partials with respect to the solve 
   * parameters and populates the coeffImage matrix. 
   * 
   * @param coeffImage A matrix that will be populated with the 
   *                   sensor partials with respect to the solve
   *                   parameters.
   * @param measure The measure that the partials are being 
   *                computed for.
   * 
   * @return bool 
   */
  bool CsmBundleObservation::computeImagePartials(LinearAlgebra::Matrix &coeffImage, BundleMeasure &measure) {
    coeffImage.clear(); 

    CSMCamera *csmCamera = dynamic_cast<CSMCamera*>(measure.camera());
    SurfacePoint groundPoint = measure.parentControlPoint()->adjustedSurfacePoint();

    // Loop over parameters and populate matrix
    for (size_t i = 0; i < m_paramIndices.size(); i++) {
      vector<double> partials = csmCamera->getSensorPartials(m_paramIndices[i], groundPoint);
      coeffImage(0, i) = partials[1];
      coeffImage(1, i) = partials[0];
    }

    return true;
  }


  /**
   * Calculates the ground partials for the line, sample currently 
   * set in the sensor model.  
   * 
   * @param coeffPoint3D A matrix that will be populated with the 
   *                     (line, sample) partials with respect to
   *                     the ground point.
   * @param measure The measure that the partials are being 
   *                computed for.
   * @param coordType Not used in this class. Coordinates are 
   *                  x,y,z
   * 
   * @return bool 
   */
  bool CsmBundleObservation::computePoint3DPartials(LinearAlgebra::Matrix &coeffPoint3D, BundleMeasure &measure, SurfacePoint::CoordinateType coordType) {
    coeffPoint3D.clear();

    CSMCamera *measureCamera = dynamic_cast<CSMCamera*>(measure.camera());

    // do ground partials 
    vector<double> groundPartials = measureCamera->GroundPartials();
    
    // groundPartials is:
    //  line WRT x
    // line WRT y
    // line WRT z
    // sample WRT x
    // sample WRT y
    // sample WRT z
    coeffPoint3D(0,0) = groundPartials[0];
    coeffPoint3D(1,0) = groundPartials[3];
    coeffPoint3D(0,1) = groundPartials[1];
    coeffPoint3D(1,1) = groundPartials[4];
    coeffPoint3D(0,2) = groundPartials[2];
    coeffPoint3D(1,2) = groundPartials[5];

    return true;
  }


  /**
   * Calculates the sample, line residuals between the values 
   * measured in the image and the ground-to-image sample, line 
   * calculated by the sensor model. 
   * 
   * @param coeffRHS  A vector that will contain the sample, line 
   *                  residuals.
   * @param measure The measure that the partials are being 
   *                computed for.
   * 
   * @return bool 
   */
  bool CsmBundleObservation::computeRHSPartials(LinearAlgebra::Vector &coeffRHS, BundleMeasure &measure) {
    // Clear old values
    coeffRHS.clear();

    Camera *measureCamera = measure.camera();
    BundleControlPoint* point = measure.parentControlPoint();

    // Get ground-to-image computed coordinates for this point. 
    if (!(measureCamera->SetGround(point->adjustedSurfacePoint()))) {
      QString msg = "Unable to map apriori surface point for measure ";
      msg += measure.cubeSerialNumber() + " on point " + point->id() + " back into image.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    double computedSample = measureCamera->Sample();
    double computedLine = measureCamera->Line();

    // The RHS is the difference between the measured coordinates on the image
    // and the coordinates calculated by the ground to image call. 
    double deltaSample = measure.sample() - computedSample;
    double deltaLine = measure.line() - computedLine;

    coeffRHS(0) = deltaSample;
    coeffRHS(1) = deltaLine;

    return true;
  }


  /**
   * Returns the observed value in (sample, line) coordinates. 
   * This requires no modification for Csm.  
   * 
   * @param measure measure The measure that the partials are 
   *                being computed for.
   * @param deltaVal The difference between the measured and 
   *                 calculate sample, line coordinates
   * 
   * @return double The The difference between the measured and 
   *                calculated (line, sample) coordinate
   */
  double CsmBundleObservation::computeObservationValue(BundleMeasure &measure, double deltaVal) {
    return deltaVal;
  }
}
