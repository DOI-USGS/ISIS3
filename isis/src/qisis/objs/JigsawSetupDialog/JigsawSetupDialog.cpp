#include "JigsawSetupDialog.h"

#include <vector>

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItemModel>

#include "BundleSolutionInfo.h"
#include "BundleSettings.h"
#include "BundleTargetBody.h"
#include "Control.h"
#include "IString.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Project.h"
#include "SpecialPixel.h"
#include "ui_JigsawSetupDialog.h"

namespace Isis {

  JigsawSetupDialog::JigsawSetupDialog(Project *project, bool useLastSettings, bool readOnly,
                                       QWidget *parent) : QDialog(parent),
                                       m_ui(new Ui::JigsawSetupDialog) {
    //
    // Note: When the ui is set up, all initializations to enabled/disabled
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

    m_ui->JigsawSetup->setCurrentIndex(0);

    if (readOnly) {
      makeReadOnly();
    }

    // initializations for general tab

    // fill control net combo box from project
    for (int i = 0; i < project->controls().size(); i++) {
      ControlList* conlist = project->controls().at(i);
      for (int j = 0; j < conlist->size(); j++) {
        Control *control = conlist->at(j);

        QVariant v = qVariantFromValue((void*)control);

        m_ui->controlNetworkComboBox->addItem(control->displayProperties()->displayName(), v);
      }
    }
    // add control nets from bundle solutions, if any
    int numBundles = project->bundleSolutionInfo().size();
    for (int i = 0; i < numBundles; i++) {
      Control *bundleControl = project->bundleSolutionInfo().at(i)->control();

      QVariant v = qVariantFromValue((void*)bundleControl);

      m_ui->controlNetworkComboBox->addItem(bundleControl->displayProperties()->displayName(), v);
    }

    // initialize default output control network filename
    FileName fname = m_ui->controlNetworkComboBox->currentText();
    m_ui->outputControlNet->setText(fname.baseName() + "-out.net");

    QList<BundleSolutionInfo *> bundleSolutionInfo = m_project->bundleSolutionInfo();
    if (useLastSettings && bundleSolutionInfo.size() > 0) {
     BundleSettingsQsp lastBundleSettings = (bundleSolutionInfo.last())->bundleSettings();
     // Retrieve the control net name used in the last bundle adjustment.
     // Note that this returns a fully specified path and filename, while the cnet combo box
     // only stores file names.
     selectControl(bundleSolutionInfo.last()->inputControlNetFileName());
     fillFromSettings(lastBundleSettings);
    }

    // Update setup dialog with settings from any active (current) settings in jigsaw dialog.

    // initializations for observation solve settings tab
    m_ui->spkSolveDegreeSpinBox_2->setValue(-1);

    QStringList tableHeaders;
    tableHeaders << "coefficients" << "a priori sigma" << "units";
    m_ui->positionAprioriSigmaTable->setHorizontalHeaderLabels(tableHeaders);

    m_ui->positionAprioriSigmaTable->setColumnWidth(0, fontMetrics().width(tableHeaders.at(0)));
    m_ui->positionAprioriSigmaTable->setColumnWidth(1, fontMetrics().width(tableHeaders.at(1)));
    m_ui->positionAprioriSigmaTable->setColumnWidth(2, fontMetrics().width(tableHeaders.at(2)));

    m_ui->pointingAprioriSigmaTable->setHorizontalHeaderLabels(tableHeaders);

    // initializations for target body tab

    // fill target combo box from project
    for (int i = 0; i < project->targetBodies().size(); i++) {
      TargetBodyQsp target = project->targetBodies().at(i);

      QVariant v = qVariantFromValue((void*)target.data());

      QString name = target->displayProperties()->displayName();

      if (name == "MOON")
        m_ui->targetBodyComboBox->addItem(QIcon(FileName(
                  "$base/icons/weather-clear-night.png").expanded()), name, v);
      else if (name == "Enceladus")
        m_ui->targetBodyComboBox->addItem(QIcon(FileName(
                  "$base/icons/nasa_enceladus.png").expanded()), name, v);
      else if (name == "Mars")
        m_ui->targetBodyComboBox->addItem(QIcon(FileName(
                  "$base/icons/nasa_mars.png").expanded()), name, v);
      else if (name == "Titan")
        m_ui->targetBodyComboBox->addItem(QIcon(FileName(
                  "$base/icons/nasa_titan.png").expanded()), name, v);
      else
        m_ui->targetBodyComboBox->addItem(QIcon(FileName(
                  "$base/icons/weather-clear-night.png").expanded()), name, v);
    }

    m_ui->radiiButtonGroup->setId(m_ui->noneRadiiRadioButton,0);
    m_ui->radiiButtonGroup->setId(m_ui->triaxialRadiiRadioButton,1);
    m_ui->radiiButtonGroup->setId(m_ui->meanRadiusRadioButton,2);
    m_ui->noneRadiiRadioButton->setChecked(true);

    // validators for target body entries...
    QDoubleValidator *sigmaValidator = new QDoubleValidator(0.0, 1.0e+10, 8, this);
    sigmaValidator->setNotation(QDoubleValidator::ScientificNotation);

    // right ascension valid range is from 0 to 360 degrees
    QDoubleValidator *raValidator = new QDoubleValidator(0.0, 360.0, 8, this);
    raValidator->setNotation(QDoubleValidator::StandardNotation);
    m_ui->rightAscensionLineEdit->setValidator(raValidator);
    m_ui->rightAscensionSigmaLineEdit->setValidator(sigmaValidator);

    m_ui->rightAscensionVelocityLineEdit->setValidator(raValidator);
    m_ui->rightAscensionVelocitySigmaLineEdit->setValidator(sigmaValidator);

    // declination valid range is from -90 to +90 degrees
    QDoubleValidator *decValidator = new QDoubleValidator(-90.0, 90.0, 8,
                                                          m_ui->declinationLineEdit);
    decValidator->setNotation(QDoubleValidator::StandardNotation);
    m_ui->declinationLineEdit->setValidator(decValidator);
    m_ui->declinationSigmaLineEdit->setValidator(sigmaValidator);

    m_ui->declinationVelocityLineEdit->setValidator(new QDoubleValidator(0.0, 1.0e+10, 8,
                                                    m_ui->declinationVelocityLineEdit));
    m_ui->declinationVelocitySigmaLineEdit->setValidator(sigmaValidator);

    m_ui->primeMeridianOffsetLineEdit->setValidator(raValidator);
    m_ui->primeMeridianOffsetSigmaLineEdit->setValidator(sigmaValidator);

    // spin rate valid for non-negative values
    m_ui->spinRateLineEdit->setValidator(new QDoubleValidator(0.0, 1.0e+10, 8,
                                                              m_ui->spinRateLineEdit));
    m_ui->spinRateSigmaLineEdit->setValidator(sigmaValidator);

    m_ui->aRadiusLineEdit->setValidator(new QDoubleValidator(0.0, 1.0e+10, 8,
                                                             m_ui->aRadiusLineEdit));
    m_ui->aRadiusSigmaLineEdit->setValidator(sigmaValidator);

    m_ui->bRadiusLineEdit->setValidator(new QDoubleValidator(0.0, 1.0e+10, 8,
                                                             m_ui->bRadiusLineEdit));
    m_ui->bRadiusSigmaLineEdit->setValidator(sigmaValidator);

    m_ui->cRadiusLineEdit->setValidator(new QDoubleValidator(0.0, 1.0e+10, 8,
                                                             m_ui->cRadiusLineEdit));
    m_ui->cRadiusSigmaLineEdit->setValidator(sigmaValidator);

    m_ui->meanRadiusLineEdit->setValidator(new QDoubleValidator(0.0, 1.0e+10, 8,
                                                                m_ui->meanRadiusLineEdit));
    m_ui->meanRadiusSigmaLineEdit->setValidator(sigmaValidator);

    // signals for target body tab
//    connect(m_ui->radiiButtonGroup, SIGNAL(buttonClicked(int)),
//            this, SLOT(on_radiiGroupClicked(int)));
    connect(m_ui->radiiButtonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(on_radiiButtonGroupClicked(int)));
    connect(m_ui->aRadiusLineEdit, SIGNAL(textChanged(QString)), SLOT(slotTextChanged(QString)));
    connect(m_ui->aRadiusLineEdit, SIGNAL(returnPressed()), SLOT(checkIsValid()));
    connect(m_ui->aRadiusLineEdit, SIGNAL(editingFinished()), SLOT(checkIsValid()));
    connect(m_ui->aRadiusLineEdit, SIGNAL(textChanged(QString)), SLOT(on_aRadiusLineEdit_textChanged(QString)));
  }


  JigsawSetupDialog::~JigsawSetupDialog() {
    // delete/null m_ui since we did "new" this pointers in the constructor
    if (m_ui) {
      delete m_ui;
    }
    m_ui = NULL;
    // do not delete/null m_project since we didn't "new" this pointer
  }


  void JigsawSetupDialog::on_radiusCheckBox_toggled(bool checked) {
    m_ui->pointRadiusSigmaLabel->setEnabled(checked);
    m_ui->pointRadiusSigmaLineEdit->setEnabled(checked);
    m_ui->pointRadiusSigmaUnitsLabel->setEnabled(checked);
  }



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
//    bool solveAllPolynomialCoefficients = (bool) (index > 3);

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

//    m_ui->spkDegreeLabel->setEnabled(solveAllPolynomialCoefficients);
//    m_ui->spkDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);
//    m_ui->spkSolveDegreeLabel->setEnabled(solveAllPolynomialCoefficients);
//    m_ui->spkSolveDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);

  }


  void JigsawSetupDialog::on_pointingComboBox_currentIndexChanged(int index) {

    // indices:
    // 0 = angles, 1 = none, 2 = velocity, 3 = acceleration, 4 = all
    bool solveAngles                    = (bool) (index == 0 || index > 1);
    bool solveAngularVelocity           = (bool) (index > 1);
    bool solveAngularAcceleration       = (bool) (index > 2);
//    bool solveAllPolynomialCoefficients = (bool) (index > 3);

    m_ui->twistCheckBox->setEnabled(solveAngles);
    m_ui->fitOverPointingCheckBox->setEnabled(solveAngles);

//    m_ui->ckDegreeLabel->setEnabled(solveAllPolynomialCoefficients);
//    m_ui->ckDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);
//    m_ui->ckSolveDegreeSpinBox->setEnabled(solveAllPolynomialCoefficients);
//    m_ui->ckSolveDegreeLabel->setEnabled(solveAllPolynomialCoefficients);

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


  void JigsawSetupDialog::fillFromSettings(const BundleSettingsQsp settings) {

    BundleObservationSolveSettings observationSolveSettings = settings->observationSolveSettings(0);

    // general tab
    m_ui->observationModeCheckBox->setChecked(settings->solveObservationMode());
    m_ui->radiusCheckBox->setChecked(settings->solveRadius());
    m_ui->updateCubeLabelCheckBox->setChecked(settings->updateCubeLabel());
    m_ui->errorPropagationCheckBox->setChecked(settings->errorPropagation());
    m_ui->outlierRejectionCheckBox->setChecked(settings->outlierRejection());
    m_ui->outlierRejectionMultiplierLineEdit->setText(toString(settings->outlierRejectionMultiplier()));
    m_ui->sigma0ThresholdLineEdit->setText(toString(settings->convergenceCriteriaThreshold()));
    m_ui->maximumIterationsLineEdit->setText(toString(settings->convergenceCriteriaMaximumIterations()));


    m_ui->positionComboBox->setCurrentIndex(observationSolveSettings.instrumentPositionSolveOption());
    m_ui->hermiteSplineCheckBox->setChecked(observationSolveSettings.solvePositionOverHermite());
    m_ui->spkDegreeSpinBox->setValue(observationSolveSettings.spkDegree());
    m_ui->spkSolveDegreeSpinBox->setValue(observationSolveSettings.spkSolveDegree());


    int pointingOption = observationSolveSettings.instrumentPointingSolveOption();
    if (pointingOption == 0) {
      pointingOption = 1;
    }
    if (pointingOption == 1) {
      pointingOption = 0;
    }

    if ( pointingOption > 0 ) {
      m_ui->twistCheckBox->setEnabled(true);
    }
    else {
      m_ui->twistCheckBox->setEnabled(true);
    }

    m_ui->pointingComboBox->setCurrentIndex(pointingOption);
//    m_ui->pointingComboBox->setCurrentIndex(observationSolveSettings.instrumentPointingSolveOption());


    m_ui->twistCheckBox->setChecked(observationSolveSettings.solveTwist());
    m_ui->fitOverPointingCheckBox->setChecked(observationSolveSettings.solvePolyOverPointing());
    m_ui->ckDegreeSpinBox->setValue(observationSolveSettings.ckDegree());
    m_ui->ckSolveDegreeSpinBox->setValue(observationSolveSettings.ckSolveDegree());

    // weighting tab
    if ( !IsNullPixel(settings->globalLatitudeAprioriSigma()) ) {
      m_ui->pointLatitudeSigmaLineEdit->setText(toString(settings->globalLatitudeAprioriSigma()));
      m_ui->pointLatitudeSigmaLineEdit->setModified(true);
    }
    if ( !IsNullPixel(settings->globalLongitudeAprioriSigma()) ) {
      m_ui->pointLongitudeSigmaLineEdit->setText(toString(settings->globalLongitudeAprioriSigma()));
      m_ui->pointLongitudeSigmaLineEdit->setModified(true);
    }
    if ( !IsNullPixel(settings->globalRadiusAprioriSigma()) ) {
      m_ui->pointRadiusSigmaLineEdit->setText(toString(settings->globalRadiusAprioriSigma()));
      m_ui->pointRadiusSigmaLineEdit->setModified(true);

    }

    QList<double> aprioriPositionSigmas = observationSolveSettings.aprioriPositionSigmas();

    if ( aprioriPositionSigmas.size() > 0 && !IsNullPixel(aprioriPositionSigmas[0]) ) {
      m_ui->positionSigmaLineEdit->setText(toString(aprioriPositionSigmas[0]));
      m_ui->positionSigmaLineEdit->setModified(true);
    }

    if ( aprioriPositionSigmas.size() > 1 && !IsNullPixel(aprioriPositionSigmas[1]) ) {
      m_ui->velocitySigmaLineEdit->setText(toString(aprioriPositionSigmas[1]));
      m_ui->velocitySigmaLineEdit->setModified(true);
    }

    if ( aprioriPositionSigmas.size() > 2 && !IsNullPixel(aprioriPositionSigmas[2]) ) {
      m_ui->accelerationSigmaLineEdit->setText(toString(aprioriPositionSigmas[2]));
      m_ui->accelerationSigmaLineEdit->setModified(true);
    }

    QList<double> aprioriPointingSigmas = observationSolveSettings.aprioriPointingSigmas();

    if ( aprioriPointingSigmas.size() > 0 && !IsNullPixel(aprioriPointingSigmas[0]) ) {
      m_ui->pointingAnglesSigmaLineEdit->setText(toString(aprioriPointingSigmas[0]));
      m_ui->pointingAnglesSigmaLineEdit->setModified(true);
    }

    if ( aprioriPointingSigmas.size() > 1 && !IsNullPixel(aprioriPointingSigmas[1]) ) {
      m_ui->pointingAngularVelocitySigmaLineEdit->setText(toString(aprioriPointingSigmas[1]));
      m_ui->pointingAngularVelocitySigmaLineEdit->setModified(true);
    }

    if ( aprioriPointingSigmas.size() > 2 && !IsNullPixel(aprioriPointingSigmas[2]) ) {
      m_ui->pointingAngularAccelerationSigmaLineEdit->setText(toString(aprioriPointingSigmas[2]));
      m_ui->pointingAngularAccelerationSigmaLineEdit->setModified(true);
    }

    // maximum liklihood tab

    // self-calibration tab

    // target body tab

    update();

  }


  BundleSettingsQsp JigsawSetupDialog::bundleSettings() {

    BundleSettingsQsp settings = BundleSettingsQsp(new BundleSettings);
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
    settings->setSolveOptions(m_ui->observationModeCheckBox->isChecked(),
                              m_ui->updateCubeLabelCheckBox->isChecked(),
                              m_ui->errorPropagationCheckBox->isChecked(),
                              m_ui->radiusCheckBox->isChecked(),
                              latitudeSigma,
                              longitudeSigma,
                              radiusSigma);
    settings->setOutlierRejection(m_ui->outlierRejectionCheckBox->isChecked(),
                                  m_ui->outlierRejectionMultiplierLineEdit->text().toDouble());




    QList<BundleObservationSolveSettings> observationSolveSettingsList;
    BundleObservationSolveSettings observationSolveSettings;

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
    observationSolveSettings.setInstrumentPointingSettings(
        BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(m_ui->pointingComboBox->currentText()),
        m_ui->twistCheckBox->isChecked(),
        m_ui->ckDegreeSpinBox->text().toInt(),
        m_ui->ckSolveDegreeSpinBox->text().toInt(),
        m_ui->fitOverPointingCheckBox->isChecked(),
        anglesSigma, angularVelocitySigma, angularAccelerationSigma);

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
    observationSolveSettings.setInstrumentPositionSettings(
        BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(m_ui->positionComboBox->currentText()),
        m_ui->spkDegreeSpinBox->text().toInt(),
        m_ui->spkSolveDegreeSpinBox->text().toInt(),
        m_ui->hermiteSplineCheckBox->isChecked(),
        positionSigma, velocitySigma, accelerationSigma);

    observationSolveSettingsList.append(observationSolveSettings);
    settings->setObservationSolveOptions(observationSolveSettingsList);
    // convergence criteria
    settings->setConvergenceCriteria(BundleSettings::Sigma0,
                                     m_ui->sigma0ThresholdLineEdit->text().toDouble(),
                                     m_ui->maximumIterationsLineEdit->text().toInt()); // TODO: change this to give user a choice between sigma0 and param corrections???

    // max likelihood estimation
    if (m_ui->maximumLikelihoodModel1ComboBox->currentText().compare("NONE") != 0) {
      // if model1 is not "NONE", add to the models list with its quantile
      settings->addMaximumLikelihoodEstimatorModel(
          MaximumLikelihoodWFunctions::stringToModel(
              m_ui->maximumLikelihoodModel1ComboBox->currentText()),
          m_ui->maximumLikelihoodModel1QuantileLineEdit->text().toDouble());

      if (m_ui->maximumLikelihoodModel2ComboBox->currentText().compare("NONE") != 0) {
        // if model2 is not "NONE", add to the models list with its quantile
        settings->addMaximumLikelihoodEstimatorModel(
            MaximumLikelihoodWFunctions::stringToModel(
                m_ui->maximumLikelihoodModel2ComboBox->currentText()),
            m_ui->maximumLikelihoodModel2QuantileLineEdit->text().toDouble());

        if (m_ui->maximumLikelihoodModel3ComboBox->currentText().compare("NONE") != 0) {
          // if model3 is not "NONE", add to the models list with its quantile
          settings->addMaximumLikelihoodEstimatorModel(
              MaximumLikelihoodWFunctions::stringToModel(
                  m_ui->maximumLikelihoodModel3ComboBox->currentText()),
              m_ui->maximumLikelihoodModel3QuantileLineEdit->text().toDouble());
        }
      }
    }

    // target body
    // ensure user entered something to adjust
    if (m_ui->poleRaCheckBox->isChecked()              ||
        m_ui->poleRaVelocityCheckBox->isChecked()      ||
        m_ui->poleDecCheckBox->isChecked()             ||
        m_ui->poleDecVelocityCheckBox->isChecked()     ||
        m_ui->primeMeridianOffsetCheckBox->isChecked() ||
        m_ui->spinRateCheckBox->isChecked()            ||
        !m_ui->noneRadiiRadioButton->isChecked()) {

      // create BundleTargetBody utility object
      BundleTargetBodyQsp bundleTargetBody = BundleTargetBodyQsp(new BundleTargetBody);

      int radiiOption = 0;
      if (m_ui->meanRadiusRadioButton->isChecked())
        radiiOption = 1;
      else if (m_ui->triaxialRadiiRadioButton->isChecked())
        radiiOption = 2;

      std::set<int> targetParameterSolveCodes;
      if (m_ui->poleRaCheckBox->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::PoleRA);
      if (m_ui->poleRaVelocityCheckBox->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::VelocityPoleRA);
      if (m_ui->poleDecCheckBox->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::PoleDec);
      if (m_ui->poleDecVelocityCheckBox->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::VelocityPoleDec);
      if (m_ui->primeMeridianOffsetCheckBox->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::PM);
      if (m_ui->spinRateCheckBox->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::VelocityPM);
      if (m_ui->triaxialRadiiRadioButton->isChecked()) {
        targetParameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusA);
        targetParameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusB);
        targetParameterSolveCodes.insert(BundleTargetBody::TriaxialRadiusC);
      }
      else if (m_ui->meanRadiusRadioButton->isChecked())
        targetParameterSolveCodes.insert(BundleTargetBody::MeanRadius);

      double poleRASigma              = -1.0;
      double poleRAVelocitySigma      = -1.0;
//    double poleRAAccelerationSigma  = -1.0;
      double poleDecSigma             = -1.0;
      double poleDecVelocitySigma     = -1.0;
//    double poleDecAccelerationSigma = -1.0;
      double pmSigma                  = -1.0;
      double pmVelocitySigma          = -1.0;
//    double pmAccelerationSigma      = -1.0;
      double aRadiusSigma             = 0.0;
      double bRadiusSigma             = 0.0;
      double cRadiusSigma             = 0.0;
      double meanRadiusSigma          = 0.0;

      if (m_ui->rightAscensionSigmaLineEdit->isModified())
        poleRASigma = m_ui->rightAscensionSigmaLineEdit->text().toDouble();
      if (m_ui->rightAscensionVelocityLineEdit->isModified())
        poleRAVelocitySigma = m_ui->rightAscensionVelocityLineEdit->text().toDouble();
//    if (m_ui->rightAscensionAccelerationLineEdit->isModified())
//      poleRAAccelerationSigma = m_ui->rightAscensionAccelerationLineEdit->text().toDouble();
      if (m_ui->declinationSigmaLineEdit->isModified())
        poleDecSigma = m_ui->declinationSigmaLineEdit->text().toDouble();
      if (m_ui->declinationVelocitySigmaLineEdit->isModified())
        poleDecVelocitySigma = m_ui->declinationVelocitySigmaLineEdit->text().toDouble();
//    if (m_ui->declinationAccelerationSigmaLineEdit->isModified())
//      poleDecAccelerationSigma = m_ui->declinationAccelerationSigmaLineEdit->text().toDouble();
      if (m_ui->primeMeridianOffsetSigmaLineEdit->isModified())
        pmSigma = m_ui->primeMeridianOffsetSigmaLineEdit->text().toDouble();
      if (m_ui->spinRateSigmaLineEdit->isModified())
        pmVelocitySigma = m_ui->spinRateSigmaLineEdit->text().toDouble();
//    if (m_ui->pmAccelerationSigmaLineEdit->isModified())
//      pmAccelerationSigma = m_ui->pmAccelerationSigmaLineEdit->text().toDouble();
      if (m_ui->aRadiusSigmaLineEdit->isModified())
        aRadiusSigma = m_ui->aRadiusSigmaLineEdit->text().toDouble();
      if (m_ui->bRadiusSigmaLineEdit->isModified())
        bRadiusSigma = m_ui->bRadiusSigmaLineEdit->text().toDouble();
      if (m_ui->cRadiusSigmaLineEdit->isModified())
        cRadiusSigma = m_ui->cRadiusSigmaLineEdit->text().toDouble();
      if (m_ui->meanRadiusSigmaLineEdit->isModified())
        meanRadiusSigma = m_ui->meanRadiusSigmaLineEdit->text().toDouble();

      bundleTargetBody->setSolveSettings(targetParameterSolveCodes,
           Angle(m_ui->rightAscensionLineEdit->text().toDouble(), Angle::Degrees),
           Angle(poleRASigma, Angle::Degrees),
           Angle(m_ui->rightAscensionVelocityLineEdit->text().toDouble(), Angle::Degrees),
           Angle(poleRAVelocitySigma, Angle::Degrees),
           Angle(m_ui->declinationLineEdit->text().toDouble(), Angle::Degrees),
           Angle(poleDecSigma, Angle::Degrees),
           Angle(m_ui->declinationVelocityLineEdit->text().toDouble(), Angle::Degrees),
           Angle(poleDecVelocitySigma, Angle::Degrees),
           Angle(m_ui->primeMeridianOffsetLineEdit->text().toDouble(), Angle::Degrees),
           Angle(pmSigma, Angle::Degrees),
           Angle(m_ui->spinRateLineEdit->text().toDouble(), Angle::Degrees),
           Angle(pmVelocitySigma, Angle::Degrees),
           (BundleTargetBody::TargetRadiiSolveMethod)radiiOption,
           Distance(m_ui->aRadiusLineEdit->text().toDouble(), Distance::Kilometers),
           Distance(aRadiusSigma, Distance::Meters),
           Distance(m_ui->bRadiusLineEdit->text().toDouble(), Distance::Kilometers),
           Distance(bRadiusSigma, Distance::Meters),
           Distance(m_ui->cRadiusLineEdit->text().toDouble(), Distance::Kilometers),
           Distance(cRadiusSigma, Distance::Meters),
           Distance(m_ui->meanRadiusLineEdit->text().toDouble(), Distance::Kilometers),
           Distance(meanRadiusSigma, Distance::Meters));

      settings->setBundleTargetBody(bundleTargetBody);
    }

     // output options
//???     settings->setOutputFilePrefix("");

    return settings;
  }


  /**
   * Loads the passed bundle settings into the setup dialog. This is used by JigsawDialog to
   * load its current settings when not using the last (most recent) bundle settings in the project.
   *
   * @param const BundleSettingsQsp settings Shared pointer to the settings to load up.
   */
  void JigsawSetupDialog::loadSettings(const BundleSettingsQsp settings) {
    fillFromSettings(settings);
  }


  /**
   * Selects a control in the control network combo box by trying to find an item with the
   * matching name. If the name is found in the combo box, the box's index is set to that
   * found control network index. If the name is not found and the box is not empty, the
   * current index is set to 0 (the first item). If the name is not found and the box is
   * empty, the index is set to -1 (see Qt).
   *
   * @param const QString &controlName The name of the control to try to find in the combo box.
   */
  void JigsawSetupDialog::selectControl(const QString &controlName) {
    QComboBox &cnetBox = *(m_ui->controlNetworkComboBox);
    int foundControlIndex = cnetBox.findText(FileName(controlName).name());
    // We did not find it, so we need to see if the combo box is empty or not.
    if (foundControlIndex == -1) {
      if (cnetBox.count() == 0) {
       cnetBox.setCurrentIndex(-1);
      }
      // If it is not empty, just set the current index to the first item.
      else {
        cnetBox.setCurrentIndex(0);
      }
    }
    // Otherwise, set the current index to the found control net index.
    else {
      cnetBox.setCurrentIndex(foundControlIndex);
    }
  }


  Control *JigsawSetupDialog::selectedControl() {

      int nIndex = m_ui->controlNetworkComboBox->currentIndex();
      Control *selectedControl
                   = (Control *)(m_ui->controlNetworkComboBox->itemData(nIndex).value< void * >());
      return selectedControl;

  }


  QString JigsawSetupDialog::selectedControlName() {
    return QString(m_ui->controlNetworkComboBox->currentText());
  }


  QString JigsawSetupDialog::outputControlName() {
    return QString(m_ui->outputControlNet->text());
  }


  void JigsawSetupDialog::makeReadOnly() {
    m_ui->controlNetworkComboBox->setEnabled(false);
    m_ui->observationModeCheckBox->setEnabled(false);
    m_ui->radiusCheckBox->setEnabled(false);
    m_ui->updateCubeLabelCheckBox->setEnabled(false);
    m_ui->errorPropagationCheckBox->setEnabled(false);
    m_ui->outlierRejectionCheckBox->setEnabled(false);
    m_ui->outlierRejectionMultiplierLineEdit->setEnabled(false);
    m_ui->sigma0ThresholdLineEdit->setEnabled(false);
    m_ui->maximumIterationsLineEdit->setEnabled(false);
    m_ui->positionComboBox->setEnabled(false);
    m_ui->hermiteSplineCheckBox->setEnabled(false);
    m_ui->spkDegreeSpinBox->setEnabled(false);
    m_ui->spkSolveDegreeSpinBox->setEnabled(false);
    m_ui->twistCheckBox->setEnabled(false);
    m_ui->pointingComboBox->setEnabled(false);
    m_ui->fitOverPointingCheckBox->setEnabled(false);
    m_ui->ckDegreeSpinBox->setEnabled(false);
    m_ui->ckSolveDegreeSpinBox->setEnabled(false);

    // weighting tab
    m_ui->pointLatitudeSigmaLineEdit->setEnabled(false);
    m_ui->pointLongitudeSigmaLineEdit->setEnabled(false);
    m_ui->pointRadiusSigmaLineEdit->setEnabled(false);
    m_ui->positionSigmaLineEdit->setEnabled(false);
    m_ui->velocitySigmaLineEdit->setEnabled(false);
    m_ui->accelerationSigmaLineEdit->setEnabled(false);
    m_ui->pointingAnglesSigmaLineEdit->setEnabled(false);
    m_ui->pointingAngularVelocitySigmaLineEdit->setEnabled(false);
    m_ui->pointingAngularAccelerationSigmaLineEdit->setEnabled(false);

    // maximum liklihood tab

    // self-calibration tab

    // target body tab
    m_ui->targetBodyComboBox->setEnabled(false);
    m_ui->poleRaCheckBox->setEnabled(false);
    m_ui->rightAscensionLineEdit->setEnabled(false);
    m_ui->rightAscensionSigmaLineEdit->setEnabled(false);
    m_ui->rightAscensionVelocityLineEdit->setEnabled(false);
    m_ui->rightAscensionVelocitySigmaLineEdit->setEnabled(false);
    m_ui->poleDecCheckBox->setEnabled(false);
    m_ui->declinationLineEdit->setEnabled(false);
    m_ui->declinationSigmaLineEdit->setEnabled(false);
    m_ui->declinationVelocityLineEdit->setEnabled(false);
    m_ui->declinationVelocitySigmaLineEdit->setEnabled(false);
    m_ui->primeMeridianOffsetCheckBox->setEnabled(false);
    m_ui->primeMeridianOffsetLineEdit->setEnabled(false);
    m_ui->primeMeridianOffsetSigmaLineEdit->setEnabled(false);
    m_ui->spinRateCheckBox->setEnabled(false);
    m_ui->spinRateLineEdit->setEnabled(false);
    m_ui->spinRateSigmaLineEdit->setEnabled(false);
    m_ui->radiiGroupBox->setEnabled(false);
    m_ui->aRadiusLineEdit->setEnabled(false);
    m_ui->aRadiusSigmaLineEdit->setEnabled(false);
    m_ui->bRadiusLineEdit->setEnabled(false);
    m_ui->bRadiusSigmaLineEdit->setEnabled(false);
    m_ui->cRadiusLineEdit->setEnabled(false);
    m_ui->cRadiusSigmaLineEdit->setEnabled(false);
    m_ui->meanRadiusLineEdit->setEnabled(false);
    m_ui->meanRadiusSigmaLineEdit->setEnabled(false);

    update();
  }


  void Isis::JigsawSetupDialog::on_poleRaCheckBox_stateChanged(int arg1) {
    if (arg1) {
      m_ui->rightAscensionLineEdit->setEnabled(true);
      m_ui->rightAscensionSigmaLineEdit->setEnabled(true);
    }
    else {
      m_ui->rightAscensionLineEdit->setEnabled(false);
      m_ui->rightAscensionSigmaLineEdit->setEnabled(false);
    }

    update();
  }


  void Isis::JigsawSetupDialog::on_poleRaVelocityCheckBox_stateChanged(int arg1) {
    if (arg1) {
      m_ui->rightAscensionVelocityLineEdit->setEnabled(true);
      m_ui->rightAscensionVelocitySigmaLineEdit->setEnabled(true);
    }
    else {
      m_ui->rightAscensionVelocityLineEdit->setEnabled(false);
      m_ui->rightAscensionVelocitySigmaLineEdit->setEnabled(false);
    }

    update();
  }


  void Isis::JigsawSetupDialog::on_poleDecCheckBox_stateChanged(int arg1) {
    if (arg1) {
      m_ui->declinationLineEdit->setEnabled(true);
      m_ui->declinationSigmaLineEdit->setEnabled(true);
    }
    else {
      m_ui->declinationLineEdit->setEnabled(false);
      m_ui->declinationSigmaLineEdit->setEnabled(false);
    }

    update();
  }


  void Isis::JigsawSetupDialog::on_poleDecVelocityCheckBox_stateChanged(int arg1) {
    if (arg1) {
      m_ui->declinationVelocityLineEdit->setEnabled(true);
      m_ui->declinationVelocitySigmaLineEdit->setEnabled(true);
    }
    else {
      m_ui->declinationVelocityLineEdit->setEnabled(false);
      m_ui->declinationVelocitySigmaLineEdit->setEnabled(false);
    }

    update();
  }


  void Isis::JigsawSetupDialog::on_spinRateCheckBox_stateChanged(int arg1) {
    if (arg1) {
      m_ui->spinRateLineEdit->setEnabled(true);
      m_ui->spinRateSigmaLineEdit->setEnabled(true);
    }
    else {
      m_ui->spinRateLineEdit->setEnabled(false);
      m_ui->spinRateSigmaLineEdit->setEnabled(false);
    }

    update();
  }


  void Isis::JigsawSetupDialog::on_primeMeridianOffsetCheckBox_stateChanged(int arg1) {
    if (arg1) {
      m_ui->primeMeridianOffsetLineEdit->setEnabled(true);
      m_ui->primeMeridianOffsetSigmaLineEdit->setEnabled(true);
    }
    else {
      m_ui->primeMeridianOffsetLineEdit->setEnabled(false);
      m_ui->primeMeridianOffsetSigmaLineEdit->setEnabled(false);
    }

    update();
  }


  void Isis::JigsawSetupDialog::on_radiiButtonGroupClicked(int arg1) {

    if (arg1 == 0) {
      m_ui->aRadiusLabel->setEnabled(false);
      m_ui->aRadiusLineEdit->setEnabled(false);
      m_ui->aRadiusSigmaLineEdit->setEnabled(false);
      m_ui->bRadiusLabel->setEnabled(false);
      m_ui->bRadiusLineEdit->setEnabled(false);
      m_ui->bRadiusSigmaLineEdit->setEnabled(false);
      m_ui->cRadiusLabel->setEnabled(false);
      m_ui->cRadiusLineEdit->setEnabled(false);
      m_ui->cRadiusSigmaLineEdit->setEnabled(false);
      m_ui->meanRadiusLineEdit->setEnabled(false);
      m_ui->meanRadiusSigmaLineEdit->setEnabled(false);

      // if we're not solving for target body triaxial radii or mean radius, we CAN solve for point
      // radii so we ensure the point radius checkbox under the general settings tab is enabled
      m_ui->radiusCheckBox->setEnabled(true);
    }
    else if (arg1 == 1) {
      m_ui->aRadiusLabel->setEnabled(true);
      m_ui->aRadiusLineEdit->setEnabled(true);
      m_ui->aRadiusSigmaLineEdit->setEnabled(true);
      m_ui->bRadiusLabel->setEnabled(true);
      m_ui->bRadiusLineEdit->setEnabled(true);
      m_ui->bRadiusSigmaLineEdit->setEnabled(true);
      m_ui->cRadiusLabel->setEnabled(true);
      m_ui->cRadiusLineEdit->setEnabled(true);
      m_ui->cRadiusSigmaLineEdit->setEnabled(true);
      m_ui->meanRadiusLineEdit->setEnabled(false);
      m_ui->meanRadiusSigmaLineEdit->setEnabled(false);

      // if we're solving for target body mean radius, we can't solve for point radii
      // so we uncheck and disable the point radius checkbox under the general settings tab
      // and remind the user
      m_ui->radiusCheckBox->setChecked(false);
      m_ui->radiusCheckBox->setEnabled(false);

      QMessageBox *msgBox = new QMessageBox(QMessageBox::Information, "Triaxial Radii Reminder!",
                  "Individual point radii and target body triaxial radii can't be solved for"
                  " simultaneously so we've unchecked and disabled the Radius checkbox under the"
                  " General Settings tab.", QMessageBox::Ok, this);
      msgBox->exec();
    }
    else if (arg1 == 2) {
      m_ui->aRadiusLabel->setEnabled(false);
      m_ui->aRadiusLineEdit->setEnabled(false);
      m_ui->aRadiusSigmaLineEdit->setEnabled(false);
      m_ui->bRadiusLabel->setEnabled(false);
      m_ui->bRadiusLineEdit->setEnabled(false);
      m_ui->bRadiusSigmaLineEdit->setEnabled(false);
      m_ui->cRadiusLabel->setEnabled(false);
      m_ui->cRadiusLineEdit->setEnabled(false);
      m_ui->cRadiusSigmaLineEdit->setEnabled(false);
      m_ui->meanRadiusLineEdit->setEnabled(true);
      m_ui->meanRadiusSigmaLineEdit->setEnabled((true));

      // if we're solving for target body triaxial radii, we can't solve for point radii
      // so we uncheck and disable the point radius checkbox under the general settings tab
      // and remind the user
      m_ui->radiusCheckBox->setChecked(false);
      m_ui->radiusCheckBox->setEnabled(false);

      QMessageBox *msgBox = new QMessageBox(QMessageBox::Information, "Mean Radius Reminder!",
                  "Individual point radii and target body mean radius can't be solved for"
                  " simultaneously so we've unchecked and disabled the Radius checkbox under the"
                  " General Settings tab.", QMessageBox::Ok, this);
      msgBox->exec();
    }

    update();
  }


  void Isis::JigsawSetupDialog::checkIsValid() {
    if (!m_ui->aRadiusSigmaLineEdit->hasAcceptableInput()) {
//    qDebug() << "invalid input";
    }


  }


  void Isis::JigsawSetupDialog::slotTextChanged(const QString &text) {
//  qDebug() << "Text change to" << text << "and value is valid ==" << m_ui->aRadiusSigmaLineEdit->hasAcceptableInput();
  }


  void Isis::JigsawSetupDialog::on_aRadiusLineEdit_textChanged(const QString &arg1) {
//  qDebug() << "Text change to" << arg1 << "and value is valid ==" << m_ui->aRadiusLineEdit->hasAcceptableInput();
  }


  void Isis::JigsawSetupDialog::on_targetBodyComboBox_currentIndexChanged(int index) {

    TargetBodyQsp target = m_project->targetBodies().at(index);
    if (target) {
      if (target->frameType() != Isis::SpiceRotation::BPC &&
          target->frameType() != Isis::SpiceRotation::UNKNOWN) {
        m_ui->targetParametersMessage->hide();

        std::vector<Angle> raCoefs = target->poleRaCoefs();
        std::vector<Angle> decCoefs = target->poleDecCoefs();
        std::vector<Angle> pmCoefs = target->pmCoefs();

        showTargetParametersGroupBox();

        m_ui->rightAscensionLineEdit->setText(toString(raCoefs[0].degrees()));
        m_ui->rightAscensionVelocityLineEdit->setText(toString(raCoefs[1].degrees()));
        m_ui->declinationLineEdit->setText(toString(decCoefs[0].degrees()));
        m_ui->declinationVelocityLineEdit->setText(toString(decCoefs[1].degrees()));
        m_ui->primeMeridianOffsetLineEdit->setText(toString(pmCoefs[0].degrees()));
        m_ui->spinRateLineEdit->setText(toString(pmCoefs[1].degrees()));
      }
      else {
        // Formulate message indicating why target parameters are disabled
        QString msg;
        if (target->displayProperties()->displayName() == "MOON") {
          msg = "Target body parameter cannot be solved for the Moon.";
        }
        else {
          msg = "The body frame type is unknown.  If you want to solve the target body parameters, "
                "you must run spiceinit on the cubes.";
        }
        m_ui->targetParametersMessage->setText(msg);
        m_ui->targetParametersMessage->show();
        hideTargetParametersGroupBox();
      }

      m_ui->aRadiusLineEdit->setText(toString(target->radiusA().kilometers()));
      //m_ui->aRadiusSigmaLineEdit->setText(toString(target->sigmaRadiusA().kilometers()));

      m_ui->bRadiusLineEdit->setText(toString(target->radiusB().kilometers()));
      //m_ui->bRadiusSigmaLineEdit->setText(toString(target->sigmaRadiusB().kilometers()));

      m_ui->cRadiusLineEdit->setText(toString(target->radiusC().kilometers()));
      //m_ui->cRadiusSigmaLineEdit->setText(toString(target->sigmaRadiusC().kilometers()));

      m_ui->meanRadiusLineEdit->setText(toString(target->meanRadius().kilometers()));
      //m_ui->meanRadiusSigmaLineEdit->setText(toString(target->sigmaMeanRadius().kilometers()));
    }
  }


  void Isis::JigsawSetupDialog::on_spkSolveDegreeSpinBox_2_valueChanged(int arg1) {
    if (arg1 == -1) {
      m_ui->spkSolveDegreeSpinBox_2->setSuffix("(NONE)");
      m_ui->positionAprioriSigmaTable->setRowCount(0);
    }
    m_ui->positionAprioriSigmaTable->setRowCount(arg1+1);
    m_ui->positionAprioriSigmaTable->resizeColumnsToContents();

    if (arg1 == 0) {
      QTableWidgetItem *twItem = new QTableWidgetItem();
      twItem->setText("POSITION");
      m_ui->positionAprioriSigmaTable->setItem(arg1,0, twItem);
      QTableWidgetItem *twItemunits = new QTableWidgetItem();
      twItemunits->setText("meters");
      //m_ui->positionAprioriSigmaTable->item(arg1,2)->setText("meters");
    }
    else if (arg1 == 1) {
      m_ui->positionAprioriSigmaTable->item(arg1,0)->setText("VELOCITY");
      m_ui->positionAprioriSigmaTable->item(arg1,2)->setText("m/sec");
    }
    else if (arg1 == 2) {
      m_ui->positionAprioriSigmaTable->item(arg1,0)->setText("ACCELERATION");
      m_ui->positionAprioriSigmaTable->item(arg1,2)->setText("m/s^2");
    }
  /*
    else if (arg1 == 0) {
      m_ui->spkSolveDegreeSpinBox_2->setSuffix("(POSITION)");
      int nRows = m_ui->positionAprioriSigmaTable->rowCount();

      m_ui->positionAprioriSigmaTable->insertRow(nRows);
    }
    else if (arg1 == 1)
      m_ui->spkSolveDegreeSpinBox_2->setSuffix("(VELOCITY)");
    else if (arg1 == 2)
      m_ui->spkSolveDegreeSpinBox_2->setSuffix("(ACCELERATION)");
    else
      m_ui->spkSolveDegreeSpinBox_2->setSuffix("");
  */
  }


  void Isis::JigsawSetupDialog::on_rightAscensionLineEdit_textChanged(const QString &arg1) {
    if (!m_ui->rightAscensionLineEdit->hasAcceptableInput()) {
      m_ui->rightAscensionLineEdit->setStyleSheet("QLineEdit { background-color: red }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
      m_ui->rightAscensionLineEdit->setStyleSheet("QLineEdit { background-color: white }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    update();
  }


  void Isis::JigsawSetupDialog::on_declinationLineEdit_textChanged(const QString &arg1) {
    if (!m_ui->declinationLineEdit->hasAcceptableInput()) {
      m_ui->declinationLineEdit->setStyleSheet("QLineEdit { background-color: red }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
      m_ui->declinationLineEdit->setStyleSheet("QLineEdit { background-color: white }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    update();
  }


  void Isis::JigsawSetupDialog::on_rightAscensionVelocityLineEdit_textChanged(const QString &arg1) {
    if (!m_ui->rightAscensionVelocityLineEdit->hasAcceptableInput()) {
      m_ui->rightAscensionVelocityLineEdit->setStyleSheet("QLineEdit { background-color: red }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
      m_ui->rightAscensionVelocityLineEdit->setStyleSheet("QLineEdit { background-color: white }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    update();
  }


  void Isis::JigsawSetupDialog::on_declinationVelocityLineEdit_textChanged(const QString &arg1) {
    if (!m_ui->declinationVelocityLineEdit->hasAcceptableInput()) {
      m_ui->declinationVelocityLineEdit->setStyleSheet("QLineEdit { background-color: red }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
      m_ui->declinationVelocityLineEdit->setStyleSheet("QLineEdit { background-color: white }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    update();
  }


  void Isis::JigsawSetupDialog::on_primeMeridianOffsetLineEdit_textChanged(const QString &arg1) {
    if (!m_ui->primeMeridianOffsetLineEdit->hasAcceptableInput()) {
      m_ui->primeMeridianOffsetLineEdit->setStyleSheet("QLineEdit { background-color: red }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
      m_ui->primeMeridianOffsetLineEdit->setStyleSheet("QLineEdit { background-color: white }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    update();
  }


  void Isis::JigsawSetupDialog::on_spinRateLineEdit_textChanged(const QString &arg1) {
    if (!m_ui->spinRateLineEdit->hasAcceptableInput()) {
      m_ui->spinRateLineEdit->setStyleSheet("QLineEdit { background-color: red }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
      m_ui->spinRateLineEdit->setStyleSheet("QLineEdit { background-color: white }");
      m_ui->okCloseButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    update();
  }

  void JigsawSetupDialog::showTargetParametersGroupBox() {
    m_ui->targetParametersGroupBox->setEnabled(true);
  }


  void JigsawSetupDialog::hideTargetParametersGroupBox() {
    m_ui->targetParametersGroupBox->setEnabled(false);
  }

  void Isis::JigsawSetupDialog::on_controlNetworkComboBox_currentTextChanged(const QString &arg1) {
    FileName fname = arg1;
    m_ui->outputControlNet->setText(fname.baseName() + "-out.net");
  }
}
