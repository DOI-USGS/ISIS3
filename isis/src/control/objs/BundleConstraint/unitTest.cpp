#include <QDebug>
#include <QString>

#include "BundleImage.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"
#include "BundlePolynomialContinuityConstraint.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "Cube.h"
#include "FileName.h"
#include "ObservationNumber.h"
#include "Preference.h"
#include "Pvl.h"
#include "SerialNumber.h"

using namespace std;
using namespace Isis;

void outputConstraint(BundlePolynomialContinuityConstraint &constraint);

/**
 * Unit test for BundleConstraint.
 *
 * @internal
 *   @history 2017-10-29 Jesse Mapel - Original Version.
 */
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);
  qDebug() << "Unit test for BundlePolynomialContinuityConstraint...";
  qDebug() << "";

  try {
    qDebug() << "Test a default constraint...";
    qDebug() << "";

    BundlePolynomialContinuityConstraint defConst;
    outputConstraint(defConst);

    qDebug() << "Test a constraint for an actual image...";
    qDebug() << "";

    QString testCubeFileName("$lro/testData/M111607830RE_crop.cub");
    FileName testCubeFile(testCubeFileName);
    qDebug() << "Test cube: " << testCubeFileName;
    qDebug() << "";

    // Setup an observation in order to create a constraint
    Cube testCube(testCubeFile);
    Pvl *testLabel = testCube.label();
    Camera *testCam = testCube.camera();
    QString testSerialNumber = SerialNumber::Compose(*testLabel, true);
    QString testObservationNumber = ObservationNumber::Compose(*testLabel, true);
    QString testInstrumentId = testLabel->findObject("IsisCube")
                                         .findGroup("Instrument")["InstrumentId"][0];
    BundleImageQsp testImage( new BundleImage(testCam, testSerialNumber,
                                              testCubeFileName) );
    BundleTargetBodyQsp testTargetBody( new BundleTargetBody(testCam->target()) );
    BundleObservationQsp acc3Spk3CkObservation(
                               new BundleObservation(testImage, testObservationNumber,
                                                     testInstrumentId, testTargetBody) );
#if 0
    The observation solve settings are as follows:

    Solve for twist
    Solve for angular acceleration
    Do not solve for bias over existing pointing
    Pointing apriori degree:            2
    Pointing solve degree:              2
    Pointing segment count:             3
    Angle apriori sigma:                2
    Angular velocity apriori sigma:     1
    Angular acceleration apriori sigma: 0.1

    Solve for acceleration
    Do not solve for bias over existing position
    Position apriori degree:    2
    Position solve degree:      2
    Position segment count:     3
    Position apriori sigma:     50
    Velocity apriori sigma:     1
    Acceleration apriori sigma: 0.1
#endif
    BundleObservationSolveSettings solveAcc3Spk3CkSettings;
    solveAcc3Spk3CkSettings.setInstrumentPointingSettings(
                                  BundleObservationSolveSettings::AnglesVelocityAcceleration,
                                  true, 2, 2, 3, false, 2, 1, 0.1);
    solveAcc3Spk3CkSettings.setInstrumentPositionSettings(
                                  BundleObservationSolveSettings::PositionVelocityAcceleration,
                                  2, 2, 3, false, 50, 1, 0.1);
    acc3Spk3CkObservation->setSolveSettings(solveAcc3Spk3CkSettings);
    acc3Spk3CkObservation->initializeExteriorOrientation();

    // Finally create a constraint
    BundlePolynomialContinuityConstraint acc3Spk3CkConstraint(acc3Spk3CkObservation);
    outputConstraint(acc3Spk3CkConstraint);

    qDebug() << "Test with 1 position segment...";
    qDebug() << "";

    BundleObservationQsp acc1Spk3CkObservation(
                               new BundleObservation(testImage, testObservationNumber,
                                                     testInstrumentId, testTargetBody) );
#if 0
    The observation solve settings are as follows:

    Solve for twist
    Solve for angular acceleration
    Do not solve for bias over existing pointing
    Pointing apriori degree:            2
    Pointing solve degree:              2
    Pointing segment count:             3
    Angle apriori sigma:                2
    Angular velocity apriori sigma:     1
    Angular acceleration apriori sigma: 0.1

    Solve for acceleration
    Do not solve for bias over existing position
    Position apriori degree:    2
    Position solve degree:      2
    Position segment count:     1
    Position apriori sigma:     50
    Velocity apriori sigma:     1
    Acceleration apriori sigma: 0.1
#endif
    BundleObservationSolveSettings acc1Spk3CkSettings;
    acc1Spk3CkSettings.setInstrumentPointingSettings(
                             BundleObservationSolveSettings::AnglesVelocityAcceleration,
                             true, 2, 2, 3, false, 2, 1, 0.1);
    acc1Spk3CkSettings.setInstrumentPositionSettings(
                             BundleObservationSolveSettings::PositionVelocity,
                             2, 2, 1, false, 50, 1, 0.1);
    acc1Spk3CkObservation->setSolveSettings(acc1Spk3CkSettings);
    acc1Spk3CkObservation->initializeExteriorOrientation();

    BundlePolynomialContinuityConstraint acc1Spk3CkConstraint(acc1Spk3CkObservation);
    outputConstraint(acc1Spk3CkConstraint);

    qDebug() << "Test with 1 pointing segment...";
    qDebug() << "";

    BundleObservationQsp acc3Spk1CkObservation(
                               new BundleObservation(testImage, testObservationNumber,
                                                     testInstrumentId, testTargetBody) );
#if 0
    The observation solve settings are as follows:

    Solve for twist
    Solve for angular acceleration
    Do not solve for bias over existing pointing
    Pointing apriori degree:            2
    Pointing solve degree:              2
    Pointing segment count:             1
    Angle apriori sigma:                2
    Angular velocity apriori sigma:     1
    Angular acceleration apriori sigma: 0.1

    Solve for acceleration
    Do not solve for bias over existing position
    Position apriori degree:    2
    Position solve degree:      2
    Position segment count:     3
    Position apriori sigma:     50
    Velocity apriori sigma:     3
    Acceleration apriori sigma: 0.1
#endif
    BundleObservationSolveSettings acc3Spk1CkSettings;
    acc3Spk1CkSettings.setInstrumentPointingSettings(
                             BundleObservationSolveSettings::AnglesVelocityAcceleration,
                             true, 2, 2, 1, false, 2, 1, 0.1);
    acc3Spk1CkSettings.setInstrumentPositionSettings(
                             BundleObservationSolveSettings::PositionVelocity,
                             2, 2, 3, false, 50, 1, 0.1);
    acc3Spk1CkObservation->setSolveSettings(acc3Spk1CkSettings);
    acc3Spk1CkObservation->initializeExteriorOrientation();

    BundlePolynomialContinuityConstraint acc3Spk1CkConstraint(acc3Spk1CkObservation);
    outputConstraint(acc3Spk1CkConstraint);

    qDebug() << "Test when not solving for twist...";
    qDebug() << "";

    BundleObservationQsp noTwistObservation(
                               new BundleObservation(testImage, testObservationNumber,
                                                     testInstrumentId, testTargetBody) );
#if 0
    The observation solve settings are as follows:

    Do not solve for twist
    Solve for angular acceleration
    Do not solve for bias over existing pointing
    Pointing apriori degree:            2
    Pointing solve degree:              2
    Pointing segment count:             3
    Angle apriori sigma:                2
    Angular velocity apriori sigma:     1
    Angular acceleration apriori sigma: 0.1

    Solve for velocity
    Do not solve for bias over existing position
    Position apriori degree: 2
    Position solve degree:   1
    Position segment count:  3
    Position apriori sigma:  50
    Velocity apriori sigma:  1
#endif
    BundleObservationSolveSettings noTwistSettings;
    noTwistSettings.setInstrumentPointingSettings(
                          BundleObservationSolveSettings::AnglesVelocityAcceleration,
                          false, 2, 2, 3, false, 2, 1, 0.1);
    noTwistSettings.setInstrumentPositionSettings(
                          BundleObservationSolveSettings::PositionVelocity,
                          2, 1, 3, false, 50, 1, -1);
    noTwistObservation->setSolveSettings(noTwistSettings);
    noTwistObservation->initializeExteriorOrientation();

    BundlePolynomialContinuityConstraint noTwistConstraint(noTwistObservation);
    outputConstraint(noTwistConstraint);

    qDebug() << "Test when not solving for pointing...";
    qDebug() << "";

    BundleObservationQsp noPointingObservation(
                               new BundleObservation(testImage, testObservationNumber,
                                                     testInstrumentId, testTargetBody) );
#if 0
    The observation solve settings are as follows:

    Do not solve for pointing

    Solve for velocity
    Do not solve for bias over existing position
    Position apriori degree: 1
    Position solve degree:   1
    Position segment count:  3
    Position apriori sigma:  50
    Velocity apriori sigma:  1
#endif
    BundleObservationSolveSettings noPointingSettings;
    noPointingSettings.setInstrumentPointingSettings(
                             BundleObservationSolveSettings::NoPointingFactors,
                             true, 1, 1, 3, false, -1, -1, -1);
    noPointingSettings.setInstrumentPositionSettings(
                             BundleObservationSolveSettings::PositionVelocity,
                             1, 1, 3, false, 50, 1, -1);
    noPointingObservation->setSolveSettings(noPointingSettings);
    noPointingObservation->initializeExteriorOrientation();

    BundlePolynomialContinuityConstraint noPointingConstraint(noPointingObservation);
    outputConstraint(noPointingConstraint);

    qDebug() << "Test when not solving for position...";
    qDebug() << "";

    BundleObservationQsp noPositionObservation(
                               new BundleObservation(testImage, testObservationNumber,
                                                     testInstrumentId, testTargetBody) );
#if 0
    The observation solve settings are as follows:

    Do solve for twist
    Solve for angular velocity
    Do not solve for bias over existing pointing
    Pointing apriori degree:        1
    Pointing solve degree:          1
    Pointing segment count:         3
    Angle apriori sigma:            2
    Angular velocity apriori sigma: 1

    Do not solve for position
#endif
    BundleObservationSolveSettings noPositionSettings;
    noPositionSettings.setInstrumentPointingSettings(
                             BundleObservationSolveSettings::AnglesVelocity,
                             true, 1, 1, 3, false, 2, 1, -1);
    noPositionSettings.setInstrumentPositionSettings(
                             BundleObservationSolveSettings::NoPositionFactors,
                             1, 1, 3, false, -1, -1, -1);
    noPositionObservation->setSolveSettings(noPointingSettings);
    noPositionObservation->initializeExteriorOrientation();

    BundlePolynomialContinuityConstraint noPositionConstraint(noPositionObservation);
    outputConstraint(noPositionConstraint);

    qDebug() << "Test copy constructor...";
    qDebug() << "";

    BundlePolynomialContinuityConstraint copyConstraint(noTwistConstraint);
    outputConstraint(copyConstraint);

    qDebug() << "Test assignment operator...";
    qDebug() << "";

    BundlePolynomialContinuityConstraint assignedConstraint;
    assignedConstraint = noPointingConstraint;
    outputConstraint(copyConstraint);

    qDebug() << "Test bundle output string...";
    qDebug() << "";

    qDebug() << "When solving with everything";
    qDebug() << acc3Spk3CkConstraint.formatBundleOutputString();
    qDebug() << "";

    qDebug() << "When solving with 1 position segment";
    qDebug() << acc1Spk3CkConstraint.formatBundleOutputString();
    qDebug() << "";

    qDebug() << "When solving with 1 pointing segment";
    qDebug() << acc3Spk1CkConstraint.formatBundleOutputString();
    qDebug() << "";

    qDebug() << "When not solving for twist";
    qDebug() << noTwistConstraint.formatBundleOutputString();
    qDebug() << "";

    qDebug() << "When not solving for pointing";
    qDebug() << noPointingConstraint.formatBundleOutputString();
    qDebug() << "";

    qDebug() << "When not solving for position";
    qDebug() << noPositionConstraint.formatBundleOutputString();
  }
  catch (IException &e) {
    e.print();
  }

}


void outputConstraint(BundlePolynomialContinuityConstraint &constraint) {
    qDebug() << "BundlePolynomialContinuityConstraint status...";
    qDebug() << "";
    qDebug() << "Number of position segements: "
             << constraint.numberSpkSegments();
    qDebug() << "Number of position coefficients: "
             << constraint.numberSpkCoefficients();
    qDebug() << "";
    qDebug() << "Number of pointing segements: "
             << constraint.numberCkSegments();
    qDebug() << "Number of pointing coefficients: "
             << constraint.numberCkCoefficients();
    qDebug() << "";
    qDebug() << "Number of constraint equations: "
             << constraint.numberConstraintEquations();
    qDebug() << "";
    qDebug() << "Normals matrix: ";
    qDebug() << constraint.normalsMatrix();
    qDebug() << "";
    qDebug() << "Right hand side vector: ";
    qDebug() << constraint.rightHandSideVector();
    qDebug() << "";
}
