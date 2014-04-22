#ifndef JIGSAWDIALOG_H
#define JIGSAWDIALOG_H

#include "JigsawSetupDialog.h"
#include <QDialog>

namespace Ui {
  class JigsawDialog;
}

namespace Isis {
  class Project;
  class Control;

  class JigsawDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit JigsawDialog(Project *project, QWidget *parent = 0);
    ~JigsawDialog();

  private slots:
    void on_JigsawSetupButton_pressed();
    void on_JigsawRunButton_clicked();

  private:
    Ui::JigsawDialog *ui;

  protected:
    Project *m_project;
    bool   m_errorPropagation;
    bool   m_radius;
    bool   m_twist;
    bool   m_observationMode;
    int    m_maxIterations;
    bool   m_outlierRejection;
    double m_outlierRejectionMultiplier;
    bool   m_updateCubeLabel;
    double m_sigma0;
    double m_globalPointLatitudeSigma;
    double m_globalPointLongitudeSigma;
    double m_globalPointRadiusSigma;
    double m_globalSensorAngleSigma;
    double m_globalSensorAngularVelocitySigma;
    double m_globalSensorAngularAccelerationSigma;
    double m_globalSpacecraftPositionSigma;
    double m_globalSpacecraftVelocitySigma;
    double m_globalSpacecraftAccelerationSigma;
    Control* m_pselectedControl;
    QString m_solveMethod;
    QString m_spSolve;
    QString m_ckSolve;
  };
};
#endif // JIGSAWDIALOG_H
