#ifndef JIGSAWSETUPDIALOG_H
#define JIGSAWSETUPDIALOG_H

#include <QDialog>

namespace Ui {
  class JigsawSetupDialog;
}

namespace Isis {
  class Project;
  class Control;

  class JigsawSetupDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit JigsawSetupDialog(Project* project, QWidget *parent = 0);
    ~JigsawSetupDialog();

    QString solveMethod();
    bool errorPropagation();
    bool radius();
    bool twist();
    bool observationMode();
    int maxIterations();
    bool outlierRejection();
    double outlierRejectionMultiplier();
    bool updateCubeLabel();
    double sigma0();
    double pointLatitudeSigma();
    double pointLongitudeSigma();
    double pointRadiusSigma();
    double spacecraftPositionSigma();
    double spacecraftVelocitySigma();
    double spacecraftAccelerationSigma();
    double sensorAngleSigma();
    double sensorAngularVelocitySigma();
    double sensorAngularAccelerationSigma();
    Control* selectedControl();
    QString spSolve();
    QString ckSolve();

  private slots:
    void on_OutlierRejection_toggled(bool checked);

    void on_SpacecraftCombobox_currentIndexChanged(int index);

    void on_PointingCombobox_currentIndexChanged(int index);

  private:
    Ui::JigsawSetupDialog *ui;

  private:

    Project * m_project;
  };
};
#endif // JIGSAWSETUPDIALOG_H
