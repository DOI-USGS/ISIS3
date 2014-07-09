#include "JigsawSetupDialog.h"

#include "BundleSettings.h"
#include "Control.h"
#include "IString.h"
#include "Project.h"
#include "ui_JigsawSetupDialog.h"

namespace Isis {
  JigsawSetupDialog::JigsawSetupDialog(Project *project, QWidget *parent) :
                                QDialog(parent), m_ui(new Ui::JigsawSetupDialog) {
    // 
    // Note: When the ui is set up, all intializations to enabled/disabled
    // are taken care of. Also connections between some widgets will be taken
    // care of in the ui setup.

    // For example:
    //   radiusCheckBox is connected to pointRadiusSigmaLabel and pointRadiusSigmaLineEdit
    //   outlierRejectionCheckBox is connected
    //       to outlierRejectionMultiplierLabel and outlierRejectionMultiplierLineEdit
    // 
    // These connections and settings can be found in the JigsawSetupDialog.ui file
    // created by QtDesigner and may be edited by opening the ui file in QtDesigner.
    // 
    // More complex connections such as the relationship between positionSolveOption and
    // spkDegree are handled in this file by the on_widgetName_signal methods.
    m_ui->setupUi(this);

    m_project = project;

    // fill control net combo box from project
    for (int i = 0; i < project->controls().size(); i++) {
      ControlList* conlist = project->controls().at(i);
      for (int j = 0; j < conlist->size(); j++) {
        Control *control = conlist->at(j);

        QVariant v = qVariantFromValue((void*)control);

        m_ui->controlNetworkComboBox->addItem(control->displayProperties()->displayName(), v);
      }
    }

    m_ui->JigsawSetup->setCurrentIndex(0);
  }



  JigsawSetupDialog::~JigsawSetupDialog() {
    // delete/null m_ui since we did "new" this pointers in the constructor
    if (m_ui) {
      delete m_ui;
    }
    m_ui = NULL;
    // do not delete/null m_project since we didn't "new" this pointer
  }



//  void JigsawSetupDialog::on_radiusCheckBox_toggled(bool checked) {
//    m_ui->pointRadiusSigmaLabel->setEnabled(checked);
//    m_ui->pointRadiusSigmaLineEdit->setEnabled(checked);
//  }



//  void JigsawSetupDialog::on_outlierRejectionCheckBox_toggled(bool checked) {
//    m_ui->outlierRejectionMultiplierLabel->setEnabled(checked);
//    m_ui->outlierRejectionMultiplierLineEdit->setEnabled(checked);
//  }



  void JigsawSetupDialog::on_positionComboBox_currentIndexChanged(int index) {

    // indices:
    // 0 = none, 1 = position, 2 = velocity, 3 = acceleration, 4 = all
    bool solvePosition                  = (bool) (index > 0);
    bool solveVelocity                  = (bool) (index > 1);
    bool solveAcceleration              = (bool) (index > 2);
    bool solveAllPolynomialCoefficients = (bool) (index > 3);

    m_ui->hermiteSplineCheckBox->setEnabled(solvePosition);
    m_ui->positionSigmaLabel->setEnabled(solvePosition);
    m_ui->positionSigmaLineEdit->setEnabled(solvePosition);
    m_ui->positionSigmaUnitsLabel->setEnabled(solvePosition);

    m_ui->velocitySigmaLabel->setEnabled(solveVelocity);
    m_ui->velocitySigmaLineEdit->setEnabled(solveVelocity);
    m_ui->velocitySigmaUnitsLabel->setEnabled(solveVelocity);

    m_ui->accelerationSigmaLabel->setEnabled(solveAcceleration);
    m_ui->accelerationSigmaLineEdit->setEnabled(solveAcceleration);
    m_ui->accelerationSigmaUnitsLabel->setEnabled(solveAcceleration);

    m_ui->spkDegreeLabel->setEnabled(solveAllPolynomialCoefficients);
    m_ui->spkDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);
    m_ui->spkSolveDegreeLabel->setEnabled(solveAllPolynomialCoefficients);
    m_ui->spkSolveDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);

  }



  void JigsawSetupDialog::on_pointingComboBox_currentIndexChanged(int index) {

    // indices:
    // 0 = angles, 1 = none, 2 = velocity, 3 = acceleration, 4 = all
    bool solveAngles                    = (bool) (index == 0 || index > 1);
    bool solveAngularVelocity           = (bool) (index > 1);
    bool solveAngularAcceleration       = (bool) (index > 2);
    bool solveAllPolynomialCoefficients = (bool) (index > 3);
    
    m_ui->twistCheckBox->setEnabled(solveAngles);
    m_ui->fitOverPointingCheckBox->setEnabled(solveAngles);

    m_ui->ckDegreeLabel->setEnabled(solveAllPolynomialCoefficients);
    m_ui->ckDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);
    m_ui->ckSolveDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);
    m_ui->ckSolveDegreeLabel->setEnabled(solveAllPolynomialCoefficients);

    m_ui->pointingAnglesSigmaLabel->setEnabled(solveAngles);
    m_ui->pointingAnglesSigmaLineEdit->setEnabled(solveAngles);
    m_ui->pointingAnglesSigmaUnitsLabel->setEnabled(solveAngles);

    m_ui->pointingAngularVelocitySigmaLabel->setEnabled(solveAngularVelocity);
    m_ui->pointingAngularVelocitySigmaLineEdit->setEnabled(solveAngularVelocity);
    m_ui->pointingAngularVelocitySigmaUnitsLabel->setEnabled(solveAngularVelocity);

    m_ui->pointingAngularAccelerationSigmaLabel->setEnabled(solveAngularAcceleration);
    m_ui->pointingAngularAccelerationSigmaLineEdit->setEnabled(solveAngularAcceleration);
    m_ui->pointingAngularAccelerationSigmaUnitsLabel->setEnabled(solveAngularAcceleration);

  }


  void JigsawSetupDialog::on_maximumLikelihoodModel1ComboBox_currentIndexChanged(int index) {

    bool model1Selected = (bool) (index > 0);
    m_ui->maximumLikelihoodModel1QuantileLabel->setEnabled(model1Selected);
    m_ui->maximumLikelihoodModel1QuantileLineEdit->setEnabled(model1Selected);
    m_ui->maximumLikelihoodModel2Label->setEnabled(model1Selected);
    m_ui->maximumLikelihoodModel2ComboBox->setEnabled(model1Selected);

  }



  void JigsawSetupDialog::on_maximumLikelihoodModel2ComboBox_currentIndexChanged(int index) {

    bool model2Selected = (bool)(index > 0);
    m_ui->maximumLikelihoodModel2QuantileLabel->setEnabled(model2Selected);
    m_ui->maximumLikelihoodModel2QuantileLineEdit->setEnabled(model2Selected);
    m_ui->maximumLikelihoodModel3Label->setEnabled(model2Selected);
    m_ui->maximumLikelihoodModel3ComboBox->setEnabled(model2Selected);

  }



  void JigsawSetupDialog::on_maximumLikelihoodModel3ComboBox_currentIndexChanged(int index) {

    bool model3Selected = (bool)(index > 0);
    m_ui->maximumLikelihoodModel3QuantileLabel->setEnabled(model3Selected);
    m_ui->maximumLikelihoodModel3QuantileLineEdit->setEnabled(model3Selected);

  }



  BundleSettings *JigsawSetupDialog::bundleSettings() {

    BundleSettings *settings = new BundleSettings;
    settings->setValidateNetwork(true);

    // solve options
    double latitudeSigma  = -1.0;
    double longitudeSigma = -1.0;
    double radiusSigma    = -1.0;
    if (m_ui->pointLatitudeSigmaLineEdit->isModified()) {
      latitudeSigma = m_ui->pointLatitudeSigmaLineEdit->text().toDouble();
    }
    if (m_ui->pointLongitudeSigmaLineEdit->isModified()) {
      longitudeSigma = m_ui->pointLongitudeSigmaLineEdit->text().toDouble();
    }
    if (m_ui->pointRadiusSigmaLineEdit->isModified()) {
      radiusSigma = m_ui->pointRadiusSigmaLineEdit->text().toDouble();
    }
    settings->setSolveOptions(
        BundleSettings::stringToSolveMethod(m_ui->solveMethodComboBox->currentText()), 
        m_ui->observationModeCheckBox->isChecked(),
        m_ui->updateCubeLabelCheckBox->isChecked(),
        m_ui->errorPropagationCheckBox->isChecked(),
        m_ui->radiusCheckBox->isChecked(),
        latitudeSigma, longitudeSigma, radiusSigma);
    settings->setOutlierRejection(m_ui->outlierRejectionCheckBox->isChecked(),
                                  m_ui->outlierRejectionMultiplierLineEdit->text().toDouble());

    // position option
    double positionSigma     = -1.0;
    double velocitySigma     = -1.0;
    double accelerationSigma = -1.0;
    if (m_ui->positionSigmaLineEdit->isModified()) {
      positionSigma = m_ui->positionSigmaLineEdit->text().toDouble();
    }
    if (m_ui->velocitySigmaLineEdit->isModified()) {
      velocitySigma = m_ui->velocitySigmaLineEdit->text().toDouble();
    }
    if (m_ui->accelerationSigmaLineEdit->isModified()) {
      accelerationSigma = m_ui->accelerationSigmaLineEdit->text().toDouble();
    }
/*
    settings->setInstrumentPositionSolveOptions(
        BundleSettings::stringToInstrumentPositionSolveOption(m_ui->positionComboBox->currentText()),
        m_ui->hermiteSplineCheckBox->isChecked(),
        m_ui->spkDegreeSpinBox->text().toInt(), 
        m_ui->spkSolveDegreeSpinBox->text().toInt(),
        positionSigma, velocitySigma, accelerationSigma);
*/
    // pointing settings
    double anglesSigma              = -1.0;
    double angularVelocitySigma     = -1.0;
    double angularAccelerationSigma = -1.0;

    if (m_ui->pointingAnglesSigmaLineEdit->isModified()) {
      anglesSigma = m_ui->pointingAnglesSigmaLineEdit->text().toDouble();
    }
    if (m_ui->pointingAngularVelocitySigmaLineEdit->isModified()) {
      angularVelocitySigma = m_ui->pointingAngularVelocitySigmaLineEdit->text().toDouble();
    }
    if (m_ui->pointingAngularAccelerationSigmaLineEdit->isModified()) {
      angularAccelerationSigma = m_ui->pointingAngularAccelerationSigmaLineEdit->text().toDouble();
    }
/*
    settings->setInstrumentPointingSolveOptions(
        BundleSettings::stringToInstrumentPointingSolveOption(m_ui->pointingComboBox->currentText()),
        m_ui->twistCheckBox->isChecked(),
        m_ui->fitOverPointingCheckBox->isChecked(),
        m_ui->ckDegreeSpinBox->text().toInt(),
        m_ui->ckSolveDegreeSpinBox->text().toInt(),
        anglesSigma, angularVelocitySigma, angularAccelerationSigma);
*/
    // convergence criteria
    settings->setConvergenceCriteria(BundleSettings::Sigma0, 
                                     m_ui->sigma0ThresholdLineEdit->text().toDouble(), 
                                     m_ui->maximumIterationsLineEdit->text().toInt()); // TODO: change this to give user a choice between sigma0 and param corrections???

    // max likelihood estimation
    if (m_ui->maximumLikelihoodModel1ComboBox->currentText().compare("NONE") != 0) {
      // if model1 is not "NONE", add to the models list with its quantile
      settings->addMaximumLikelihoodEstimatorModel(
          BundleSettings::stringToMaximumLikelihoodModel(
              m_ui->maximumLikelihoodModel1ComboBox->currentText()), 
          m_ui->maximumLikelihoodModel1QuantileLineEdit->text().toDouble());

      if (m_ui->maximumLikelihoodModel2ComboBox->currentText().compare("NONE") != 0) {
        // if model2 is not "NONE", add to the models list with its quantile
        settings->addMaximumLikelihoodEstimatorModel(
            BundleSettings::stringToMaximumLikelihoodModel(
                m_ui->maximumLikelihoodModel2ComboBox->currentText()), 
            m_ui->maximumLikelihoodModel2QuantileLineEdit->text().toDouble());

        if (m_ui->maximumLikelihoodModel3ComboBox->currentText().compare("NONE") != 0) {
          // if model3 is not "NONE", add to the models list with its quantile
          settings->addMaximumLikelihoodEstimatorModel(
              BundleSettings::stringToMaximumLikelihoodModel(
                  m_ui->maximumLikelihoodModel3ComboBox->currentText()), 
              m_ui->maximumLikelihoodModel3QuantileLineEdit->text().toDouble());
        }
      }
    }

     // output options
//???     settings->setOutputFiles("", false, false, false);

    return settings;
  }



  Control *JigsawSetupDialog::selectedControl() {

      int nIndex = m_ui->controlNetworkComboBox->currentIndex();
      Control *selectedControl 
                   = (Control *)(m_ui->controlNetworkComboBox->itemData(nIndex).value< void * >());
      return selectedControl;

  }
}
