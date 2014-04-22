#include "JigsawDialog.h"
#include "JigsawSetupDialog.h"
#include "ui_JigsawDialog.h"

#include "BundleAdjust.h"
#include "Control.h"
#include "Project.h"

namespace Isis {

  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JigsawDialog) {
    ui->setupUi(this);

    m_project = project;
  }

  JigsawDialog::~JigsawDialog() {
    delete ui;
  }

  void JigsawDialog::on_JigsawSetupButton_pressed()
  {
    JigsawSetupDialog setupdlg(m_project, this);
    if (setupdlg.exec() == QDialog::Accepted) {
      m_errorPropagation = setupdlg.errorPropagation();
      m_radius = setupdlg.radius();
      m_twist = setupdlg.twist();
      m_observationMode = setupdlg.observationMode();
      m_maxIterations = setupdlg.maxIterations();
      m_outlierRejection = setupdlg.outlierRejection();
      m_outlierRejectionMultiplier = setupdlg.outlierRejectionMultiplier();
      m_updateCubeLabel = setupdlg.updateCubeLabel();
      m_sigma0 = setupdlg.sigma0();
      m_maxIterations = setupdlg.maxIterations();
      m_globalPointLatitudeSigma = setupdlg.pointLatitudeSigma();
      m_globalPointLongitudeSigma = setupdlg.pointLongitudeSigma();
      m_globalPointRadiusSigma = setupdlg.pointRadiusSigma();
      m_globalSensorAngleSigma = setupdlg.sensorAngleSigma();
      m_globalSensorAngularVelocitySigma = setupdlg.sensorAngularVelocitySigma();
      m_globalSensorAngularAccelerationSigma = setupdlg.sensorAngularAccelerationSigma();
      m_globalSpacecraftPositionSigma = setupdlg.spacecraftPositionSigma();
      m_globalSpacecraftVelocitySigma = setupdlg.spacecraftVelocitySigma();
      m_globalSpacecraftAccelerationSigma = setupdlg.spacecraftAccelerationSigma();
      m_pselectedControl = setupdlg.selectedControl();
      m_solveMethod = setupdlg.solveMethod();
      m_spSolve = setupdlg.spSolve();
      m_ckSolve = setupdlg.ckSolve();
    }
  }

  void JigsawDialog::on_JigsawRunButton_clicked() {

    SerialNumberList snlist;

    QList<ImageList *> imagelist = m_project->images();

    for ( int i = 0; i < imagelist.size(); i++) {
        ImageList* list = imagelist.at(i);
        for (int j = 0; j < list->size(); j++) {
          Image* image = list->at(j);
          snlist.Add(image->fileName());
        }
    }

//    QString filename;
//    foreach(filename, cubeFiles) {
//      try {
//        m_serialNumbers->Add(filename);
//      }
//      catch(IException &) {
//      }
//    }


    ControlNet* cnet = m_pselectedControl->controlNet();
    BundleAdjust ba(*cnet, snlist, false);

    ba.SetErrorPropagation(m_errorPropagation);
    ba.SetSolveRadii(m_radius);
    ba.SetSolveTwist(m_twist);
    ba.SetObservationMode(m_observationMode);
    ba.SetMaxIterations(m_maxIterations);
    ba.SetOutlierRejection(m_outlierRejection);
    ba.SetRejectionMultiplier(m_outlierRejectionMultiplier);
    ba.SetUpdateCubes(m_updateCubeLabel);
    ba.SetConvergenceThreshold(m_sigma0);
    ba.SetGlobalLatitudeAprioriSigma(m_globalPointLatitudeSigma);
    ba.SetGlobalLongitudeAprioriSigma(m_globalPointLongitudeSigma);
    ba.SetGlobalRadiiAprioriSigma(m_globalPointRadiusSigma);
    if (m_globalSensorAngleSigma > 0)
      ba.SetGlobalCameraAnglesAprioriSigma(m_globalSensorAngleSigma);
    if (m_globalSensorAngularVelocitySigma > 0)
      ba.SetGlobalCameraAngularVelocityAprioriSigma(m_globalSensorAngularVelocitySigma);
    if (m_globalSensorAngularAccelerationSigma > 0)
      ba.SetGlobalCameraAngularAccelerationAprioriSigma(m_globalSensorAngularAccelerationSigma);
    if (m_globalSpacecraftPositionSigma > 0)
      ba.SetGlobalSpacecraftPositionAprioriSigma(m_globalSpacecraftPositionSigma);
    if (m_globalSpacecraftVelocitySigma > 0)
    ba.SetGlobalSpacecraftVelocityAprioriSigma(m_globalSpacecraftVelocitySigma);
    if (m_globalSpacecraftAccelerationSigma > 0)
      ba.SetGlobalSpacecraftAccelerationAprioriSigma(m_globalSpacecraftAccelerationSigma);

    ba.SetSolutionMethod(m_solveMethod);

    if ( m_solveMethod == "SPARSE")
      ba.SetDecompositionMethod(BundleAdjust::CHOLMOD);
    else if (m_solveMethod == "SPECIALK")
      ba.SetDecompositionMethod(BundleAdjust::SPECIALK);

    if(m_spSolve == "NONE") {
      ba.SetSolveSpacecraftPosition(BundleAdjust::Nothing);
    }
    else if(m_spSolve == "POSITION") {
      ba.SetSolveSpacecraftPosition(BundleAdjust::PositionOnly);
    }
    else if(m_spSolve == "VELOCITIES") {
      ba.SetSolveSpacecraftPosition(BundleAdjust::PositionVelocity);
    }
    else if(m_spSolve == "ACCELERATIONS") {
      ba.SetSolveSpacecraftPosition(BundleAdjust::PositionVelocityAcceleration);
    }
    else {
      ba.SetSolveSpacecraftPosition(BundleAdjust::SPKAll);
    }

    if (m_ckSolve == "NONE") {
      ba.SetSolveCmatrix(BundleAdjust::None);
    }
    else if (m_ckSolve == "ANGLES") {
      ba.SetSolveCmatrix(BundleAdjust::AnglesOnly);
    }
    else if (m_ckSolve == "VELOCITIES") {
      ba.SetSolveCmatrix(BundleAdjust::AnglesVelocity);
    }
    else if (m_ckSolve == "ACCELERATIONS") {
      ba.SetSolveCmatrix(BundleAdjust::AnglesVelocityAcceleration);
    }
    else {
      ba.SetSolveCmatrix(BundleAdjust::CKAll);
    }


    // run bundle (thread with BundleThread::QThread) - pump output to modeless dialog
    ba.SolveCholesky();
  }
}
