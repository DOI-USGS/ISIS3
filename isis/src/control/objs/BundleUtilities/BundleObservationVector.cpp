#include "BundleObservationVector.h"

#include <QDebug>

#include "BundleObservation.h"
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
    qDeleteAll(*this);
    clear();
  }


  /**
   * add new BundleObservation method
   */
  BundleObservation *BundleObservationVector::addnew(BundleImage *bundleImage,
                                                     QString observationNumber,
                                                     QString instrumentId,
                                                     BundleSettingsQsp bundleSettings) {
    BundleObservation *bundleObservation;
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
      bundleObservation = new BundleObservation(bundleImage, observationNumber, instrumentId,
                                                bundleSettings->getBundleTargetBody());

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
/*
      QString imagefileName = bundleImage->fileName();
//      if (imagefileName.contains("Images8/CL1/N1697715606_1") || imagefileName.contains("-27")) {
//        solveSettings.setInstrumentPositionSettings(Isis::BundleObservationSolveSettings::PositionOnly, 2, 2, false, 1.0e-20, -1.0, -1.0);
//        solveSettings.setInstrumentPointingSettings(Isis::BundleObservationSolveSettings::AnglesOnly, true, 2, 2, false, 1.0e-20, -1.0, -1.0);
//        solveSettings.setInstrumentPositionSettings(Isis::BundleObservationSolveSettings::PositionOnly, 2, 2, false, 1000.0, -1.0, -1.0);
//        solveSettings.setInstrumentPointingSettings(Isis::BundleObservationSolveSettings::AnglesOnly, true, 2, 2, false, 2.0, -1.0, -1.0);
//      }
//      if (imagefileName.contains("-25")) {
//        solveSettings.setInstrumentPositionSettings(Isis::BundleObservationSolveSettings::PositionOnly, 2, 2, false, 1000.0, -1.0, -1.0);
//        solveSettings.setInstrumentPointingSettings(Isis::BundleObservationSolveSettings::AnglesOnly, false, 2, 2, false, 2.0, -1.0, -1.0);
//      }

if (imagefileName.contains("N1697715606_1") ||
imagefileName.contains("N1697715692_1") ||
imagefileName.contains("N1697715727_1") ||
imagefileName.contains("N1697715762_1") ||
imagefileName.contains("N1697715797_1") ||
imagefileName.contains("N1697715832_1") ||
imagefileName.contains("N1697715892_1") ||
imagefileName.contains("N1697715952_1") ||
imagefileName.contains("N1697716012_1") ||
imagefileName.contains("N1697716072_1") ||
imagefileName.contains("N1697716132_2") ||
imagefileName.contains("N1697716192_1") ||
imagefileName.contains("N1697716252_1") ||
imagefileName.contains("N1697716312_1") ||
imagefileName.contains("N1697716372_1") ||
imagefileName.contains("N1697716432_1") ||
imagefileName.contains("N1697716492_1") ||
imagefileName.contains("N1697716552_1") ||
imagefileName.contains("N1697716612_1") ||
imagefileName.contains("N1697716672_1") ||
imagefileName.contains("N1697716732_1") ||
imagefileName.contains("N1697716792_1") ||
imagefileName.contains("N1697716852_1") ||
imagefileName.contains("N1697716912_1") ||
imagefileName.contains("N1697716972_1") ||
imagefileName.contains("N1697717032_1") ||
imagefileName.contains("N1697717092_1") ||
imagefileName.contains("N1697717152_1") ||
imagefileName.contains("N1697717212_1") ||
imagefileName.contains("N1697717272_1") ||
imagefileName.contains("N1697717332_1") ||
imagefileName.contains("N1697717367_1") ||
imagefileName.contains("N1697717402_1") ||
imagefileName.contains("N1697717437_1") ||
imagefileName.contains("N1697717472_1") ||
imagefileName.contains("N1697717507_1") ||
imagefileName.contains("N1697717542_1") ||
imagefileName.contains("N1697717577_1") ||
imagefileName.contains("N1697717612_1") ||
imagefileName.contains("N1697717648_1") ||
imagefileName.contains("N1699259820_1") ||
imagefileName.contains("N1699259955_1") ||
imagefileName.contains("N1699260010_1") ||
imagefileName.contains("N1699260065_1") ||
imagefileName.contains("N1699260199_1") ||
imagefileName.contains("N1699260254_1") ||
imagefileName.contains("N1699260315_1") ||
imagefileName.contains("N1699260370_1") ||
imagefileName.contains("N1699260425_1") ||
imagefileName.contains("N1699260535_1")) {
        solveSettings.setInstrumentPositionSettings(Isis::BundleObservationSolveSettings::NoPositionFactors, 2, 2, false, -1.0, -1.0, -1.0);
        solveSettings.setInstrumentPointingSettings(Isis::BundleObservationSolveSettings::AnglesOnly, true, 2, 2, false, 2.0, -1.0, -1.0);
}
else {
  solveSettings.setInstrumentPositionSettings(Isis::BundleObservationSolveSettings::NoPositionFactors, 2, 2, false, -1.0, -1.0, -1.0);
  solveSettings.setInstrumentPointingSettings(Isis::BundleObservationSolveSettings::AnglesOnly, false, 2, 2, false, 2.0, -1.0, -1.0);
}
*/
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
      BundleObservation *observation = at(i);
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
      BundleObservation *observation = at(i);
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
  BundleObservation *BundleObservationVector::
      getObservationByCubeSerialNumber(QString cubeSerialNumber) {

    if (m_imageSerialToObservationMap.contains(cubeSerialNumber))
      return m_imageSerialToObservationMap.value(cubeSerialNumber);

    return NULL;
  }


  /**
   * TODO
   */
  bool BundleObservationVector::initializeExteriorOrientation() {
    int nObservations = size();
    for (int i = 0; i < nObservations; i++) {
      BundleObservation *observation = at(i);
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
      BundleObservation *observation = at(i);
      observation->initializeBodyRotation();
    }

    return true;
  }
}


