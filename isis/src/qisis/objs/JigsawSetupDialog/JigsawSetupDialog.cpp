#include "JigsawSetupDialog.h"

#include "BundleSettings.h"
#include "Control.h"
#include "Project.h"
#include "ui_JigsawSetupDialog.h"

namespace Isis {
  JigsawSetupDialog::JigsawSetupDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JigsawSetupDialog) {
    ui->setupUi(this);

    m_project = project;

    ui->OutlierRejectionMultiplier->setDisabled(true);
    ui->SPKDegree->setDisabled(true);
    ui->SPKSolveDegree->setDisabled(true);
    ui->CKDegree->setDisabled(true);
    ui->CKSolveDegree->setDisabled(true);

    // fill control net combo box from project
    for (int i = 0; i < project->controls().size(); i++) {
      ControlList* conlist = project->controls().at(i);
      for (int j = 0; j < conlist->size(); j++) {
        Control *control = conlist->at(j);

        QVariant v = qVariantFromValue((void*)control);

        ui->ControlNetCombo->addItem(control->displayProperties()->displayName(), v);
      }
    }

    ui->JigsawSetup->setCurrentIndex(0);
  }



  JigsawSetupDialog::~JigsawSetupDialog() {
    delete ui;
  }



  void JigsawSetupDialog::on_OutlierRejection_toggled(bool checked) {
     if (ui->OutlierRejection->isChecked())
       ui->OutlierRejectionMultiplier->setEnabled(true);
     else
       ui->OutlierRejectionMultiplier->setDisabled(true);
  }



  void JigsawSetupDialog::on_SpacecraftCombobox_currentIndexChanged(int index) {
    if (index != 4) {
      ui->SPKDegree->setDisabled(true);
      ui->SPKSolveDegree->setDisabled(true);
    }
    else {
      ui->SPKDegree->setEnabled(true);
      ui->SPKSolveDegree->setEnabled(true);
    }
  }



  void JigsawSetupDialog::on_PointingCombobox_currentIndexChanged(int index) {
    if (index != 4) {
      ui->CKDegree->setDisabled(true);
      ui->CKSolveDegree->setDisabled(true);
    }
    else {
      ui->CKDegree->setEnabled(true);
      ui->CKSolveDegree->setEnabled(true);
    }
  }



  BundleSettings JigsawSetupDialog::bundleSettings() {
    BundleSettings settings;
    
    settings.setValidateNetwork(true);

    // solve options
    // if wasEntered() ???
    double latitudeSigma  = ui->PointLatitudeSigma->text().toDouble();
    double longitudeSigma = ui->PointLongitudeSigma->text().toDouble();
    double radiusSigma    = ui->PointRadiusSigma->text().toDouble();
    settings.setSolveOptions(BundleSettings::stringToSolveMethod(ui->SolveMethod->currentText()), 
                             ui->ObservationMode->isChecked(),
                             ui->UpdateCubeLabel->isChecked(),
                             ui->ErrorPropagation->isChecked(),
                             ui->Radius->isChecked(),
                             latitudeSigma, longitudeSigma, radiusSigma);
    settings.setOutlierRejection(ui->OutlierRejection->isChecked(),
                                       ui->OutlierRejectionMultiplier->text().toDouble());

    // position options
    // if wasEntered() ???
    double positionSigma     = ui->SpacecraftPositionSigma->text().toDouble();
    double velocitySigma     = ui->SpacecraftVelocitySigma->text().toDouble();
    double accelerationSigma = ui->SpacecraftAccelerationSigma->text().toDouble();
    settings.setInstrumentPositionSolveOptions(
        BundleSettings::stringToInstrumentPositionSolveOption(ui->SpacecraftCombobox->currentText()),
        false, // do not solve over Hermite spline ???
        ui->SPKDegree->text().toInt(), 
        ui->SPKSolveDegree->text().toInt(),
        positionSigma, velocitySigma, accelerationSigma);

    // orientation settings
    // if wasEntered() ???
    double anglesSigma              = ui->SensorAngleSigma->text().toDouble();
    double angularVelocitySigma     = ui->SensorVelocitySigma->text().toDouble();
    double angularAccelerationSigma = ui->SensorAccelerationSigma->text().toDouble();
    settings.setInstrumentPointingSolveOptions(
        BundleSettings::stringToInstrumentPointingSolveOption(ui->PointingCombobox->currentText()),
        ui->Twist->isChecked(), 
        false,  // do not fit over existing pointing ???
        ui->CKDegree->text().toInt(), 
        ui->CKSolveDegree->text().toInt(),
        anglesSigma, angularVelocitySigma, angularAccelerationSigma);

    // convergence criteria
    settings.setConvergenceCriteria(BundleSettings::Sigma0, ui->Sigma0Edit->text().toDouble(), 
                                    ui->MaxIterationsEdit->text().toInt()); // TODO: change this to give user a choice between sigma0 and param corrections???

    // max likelihood estimation
    //if (ui->maxLikelihoodModel1->currentText().compare("NONE") != 0) {
    //  // if model1 is not "NONE", add to the models list with its quantile
    //  settings.addMaximumLikelihoodEstimatorModel(
    //      BundleSettings::stringToMaximumLikelihoodModel(ui->maxLikelihoodModel1->currentText()),
    //      ui->maxLikelihoodCQuantile1->currentText().toDouble());
    //
    //  if (ui->maxLikelihoodModel2->currentText().compare("NONE") != 0) {
    //    // if model2 is not "NONE", add to the models list with its quantile
    //    settings.addMaximumLikelihoodEstimatorModel(
    //        BundleSettings::stringToMaximumLikelihoodModel(ui->maxLikelihoodModel2->currentText()),
    //        ui->maxLikelihoodCQuantile2->currentText().toDouble());
    //
    //    if (ui->maxLikelihoodModel3->currentText().compare("NONE") != 0) {
    //      // if model3 is not "NONE", add to the models list with its quantile
    //      settings.addMaximumLikelihoodEstimatorModel(
    //          BundleSettings::stringToMaximumLikelihoodModel(ui->maxLikelihoodModel3->currentText()),
    //          ui->maxLikelihoodCQuantile3->currentText().toDouble());
    //    }
    //  }
    //}

    // // output options
    // QString outputfileprefix = "";
    // if (ui.WasEntered("FILE_PREFIX"))  {
    //   outputfileprefix = ui.GetString("FILE_PREFIX");
    // }
    // settings.setOutputFiles(outputfileprefix, ui.GetBoolean("BUNDLEOUT_TXT"),
    //                               ui.GetBoolean("OUTPUT_CSV"), ui.GetBoolean("RESIDUALS_CSV"));

    return settings;
  }



  Control *JigsawSetupDialog::selectedControl() {
      int     nIndex             = ui->ControlNetCombo->currentIndex();
      Control *selectedControl   = (Control *)(ui->ControlNetCombo->itemData(nIndex).value< void * >());
      return selectedControl;
    }
  }
