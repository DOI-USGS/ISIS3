#include "BundleObservationVector.h"

#include <QDebug>

#include "IException.h"


namespace Isis {

  /**
   * constructor
   */
  BundleObservationVector::BundleObservationVector() {
  }


  /**
   * destructor
   */
  BundleObservationVector::~BundleObservationVector() {
    clear();
  }


  /**
   * add new BundleObservation method
   */
  BundleObservationQsp BundleObservationVector::addnew(BundleImageQsp bundleImage,
                                                       QString observationNumber,
                                                       QString instrumentId,
                                                       BundleSettingsQsp bundleSettings) {
    BundleObservationQsp bundleObservation;
    bool bAddToExisting = false;

    if (bundleSettings->solveObservationMode() &&
        m_observationNumberToObservationMap.contains(observationNumber)) {
      bundleObservation = m_observationNumberToObservationMap.value(observationNumber);

      if (bundleObservation->instrumentId() == instrumentId){
        bAddToExisting = true;
      }
    }

    if (bAddToExisting) {
      // if we have already added a BundleObservation with this number, we have to add the new
      // BundleImage to this observation
      bundleObservation->append(bundleImage);

      bundleImage->setParentObservation(bundleObservation);

      // update observation number to observation ptr map
      m_observationNumberToObservationMap.insertMulti(observationNumber,bundleObservation);

      // update image serial number to observation ptr map
      m_imageSerialToObservationMap.insertMulti(bundleImage->serialNumber(),bundleObservation);
    }
    else {
      // create new BundleObservation and append to this vector
      bundleObservation = BundleObservationQsp( new BundleObservation(bundleImage,
                                                                      observationNumber,
                                                                      instrumentId,
                                                bundleSettings->bundleTargetBody()));

      if (!bundleObservation) {
        QString message = "unable to allocate new BundleObservation ";
        message += "for " + bundleImage->fileName();
        throw IException(IException::Programmer, message, _FILEINFO_);
      }

      bundleImage->setParentObservation(bundleObservation);
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // TODO: problem when using settings from gui that no instrument id is set
      //
      BundleObservationSolveSettings solveSettings;
      if ( bundleSettings->numberSolveSettings() == 1) {
        solveSettings = bundleSettings->observationSolveSettings(0);
      }
      else {
        solveSettings = bundleSettings->observationSolveSettings(instrumentId);
      }

      bundleObservation->setSolveSettings(solveSettings);

      bundleObservation->setIndex(size());

      append(bundleObservation);

      // update observation number to observation ptr map
      m_observationNumberToObservationMap.insertMulti(observationNumber,bundleObservation);

      // update image serial number to observation ptr map
      m_imageSerialToObservationMap.insertMulti(bundleImage->serialNumber(),bundleObservation);
    }

    return bundleObservation;
  }


  /**
   * TODO
   */
  int BundleObservationVector::numberPositionParameters() {
    int positionParameters = 0;

    for (int i = 0; i < size(); i++) {
      BundleObservationQsp observation = at(i);
      positionParameters += observation->numberPositionParameters();
    }

    return positionParameters;
  }


  /**
   * TODO
   */
  int BundleObservationVector::numberPointingParameters() {
    int pointingParameters = 0;

    for (int i = 0; i < size(); i++) {
      BundleObservationQsp observation = at(i);
      pointingParameters += observation->numberPointingParameters();
    }

    return pointingParameters;
  }


  /**
   * TODO
   */
  int BundleObservationVector::numberParameters() {
    return numberPositionParameters() + numberPointingParameters();
  }


  /**
   * TODO
   */
  BundleObservationQsp BundleObservationVector::
      observationByCubeSerialNumber(QString cubeSerialNumber) {
    BundleObservationQsp bundleObservation;

    if (m_imageSerialToObservationMap.contains(cubeSerialNumber))
      bundleObservation = m_imageSerialToObservationMap.value(cubeSerialNumber);

    return bundleObservation;
  }


  /**
   * TODO
   */
  bool BundleObservationVector::initializeExteriorOrientation() {
    int nObservations = size();
    for (int i = 0; i < nObservations; i++) {
      BundleObservationQsp observation = at(i);
      observation->initializeExteriorOrientation();
    }

    return true;
  }


  /**
   * TODO
   */
  bool BundleObservationVector::initializeBodyRotation() {
    int nObservations = size();
    for (int i = 0; i < nObservations; i++) {
      BundleObservationQsp observation = at(i);
      observation->initializeBodyRotation();
    }

    return true;
  }
}


