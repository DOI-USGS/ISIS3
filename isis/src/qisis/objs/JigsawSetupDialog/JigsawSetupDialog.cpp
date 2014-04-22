#include "JigsawSetupDialog.h"
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


  QString JigsawSetupDialog::spSolve() {
    return ui->SpacecraftCombobox->currentText();
  }


  QString JigsawSetupDialog::ckSolve() {
    return ui->PointingCombobox->currentText();
  }

  QString JigsawSetupDialog::solveMethod() {
    return ui->SolveMethod->currentText();
  }


  bool JigsawSetupDialog::errorPropagation() {
    return ui->ErrorPropagation->isChecked();
  }


  bool JigsawSetupDialog::radius() {
    return ui->Radius->isChecked();
  }


  bool JigsawSetupDialog::twist() {
    return ui->Twist->isChecked();
  }


  bool JigsawSetupDialog::observationMode() {
    return ui->ObservationMode->isChecked();
  }


  int JigsawSetupDialog::maxIterations() {
    return ui->MaxIterationsEdit->text().toInt();
  }


  bool JigsawSetupDialog::outlierRejection() {
    return ui->OutlierRejection->isChecked();
  }

  double JigsawSetupDialog::outlierRejectionMultiplier() {
    return ui->OutlierRejectionMultiplier->text().toDouble();
  }


  bool JigsawSetupDialog::updateCubeLabel() {
    return ui->UpdateCubeLabel->isChecked();
  }


  double JigsawSetupDialog::sigma0() {
    return ui->Sigma0Edit->text().toDouble();
  }


  double JigsawSetupDialog::pointLatitudeSigma() {
    return ui->PointLatitudeSigma->text().toDouble();
  }


  double JigsawSetupDialog::pointLongitudeSigma() {
    return ui->PointLongitudeSigma->text().toDouble();
  }


  double JigsawSetupDialog::pointRadiusSigma() {
    return ui->PointRadiusSigma->text().toDouble();
  }


  double JigsawSetupDialog::spacecraftPositionSigma() {
    return ui->SpacecraftPositionSigma->text().toDouble();
  }


  double JigsawSetupDialog::spacecraftVelocitySigma() {
    return ui->SpacecraftVelocitySigma->text().toDouble();
  }


  double JigsawSetupDialog::spacecraftAccelerationSigma() {
    return ui->SpacecraftAccelerationSigma->text().toDouble();
  }


  double JigsawSetupDialog::sensorAngleSigma() {
    return ui->SensorAngleSigma->text().toDouble();
  }


  double JigsawSetupDialog::sensorAngularVelocitySigma() {
    return ui->SensorVelocitySigma->text().toDouble();
  }


  double JigsawSetupDialog::sensorAngularAccelerationSigma() {
    return ui->SensorAccelerationSigma->text().toDouble();
  }


  Control* JigsawSetupDialog::selectedControl() {

    int nIndex = ui->ControlNetCombo->currentIndex();

    Control *selectedControl = (Control*) (ui->ControlNetCombo->itemData(nIndex).value<void*>());

    return selectedControl;
  }
}
