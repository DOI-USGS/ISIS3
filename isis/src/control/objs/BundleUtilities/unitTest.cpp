#include <QByteArray>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QIODevice>
#include <QString>

#include "BundleControlPoint.h"
#include "BundleControlPointVector.h"
#include "BundleImage.h"
#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"
#include "BundleObservationVector.h"
#include "IException.h"
#include "Preference.h"
#include "PvlObject.h"


using namespace std;
using namespace Isis;



int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);
  qDebug() << "Unit test for BundleUtilities...";
  qDebug();

  try {
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleControlPoint object...";
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleControlPointVector object...";
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleImage object...";
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleMeasure object...";
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleObservation object...";
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();

  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleObservationSolveSettings object and test mutator methods...";
  // default constructor
  BundleObservationSolveSettings boss;
  boss.setInstrumentId("TestBundleObservationSolveSettings");
  boss.setInstrumentPointingSettings(BundleObservationSolveSettings::NoPointingFactors, true, 1, 2,
                                     false, 3.0, 4.0, 5.0);
  boss.setInstrumentPositionSettings(BundleObservationSolveSettings::PositionOnly, 6, 7,
                                     true, 800.0, 900.0, 1000.0);
  PvlObject pvl = boss.pvlObject();
  cout << pvl << endl;

//  QFile file("BundleObservationSolveSettingsTest.dat");
//  file.open(QIODevice::WriteOnly);
//  QDataStream out(&file);
//  out << boss;
//  file.close();
//  file.open(QIODevice::ReadOnly);
//  QDataStream in(&file);
//  in >> newBoss;

  qDebug();
  qDebug() << "Write the BundleObservationSolveSettings object to a byte array using a data stream "
              "and read into a new object ...";
  QByteArray byteArray;
  QDataStream outputData(&byteArray, QIODevice::WriteOnly);
  outputData << boss;
  QDataStream inputData(byteArray);
  BundleObservationSolveSettings newBoss;
  inputData >> newBoss;
  pvl = newBoss.pvlObject();
  cout << pvl << endl;

#if 0
  qDebug();
  // copy constructor
  const BundleObservationSolveSettings constBoss;
  BundleObservationSolveSettings bossCopy(&constBoss);
  // operator=
  BundleObservationSolveSettings bossAssignment = &constBoss;
  // setters
  bool success = boss.setFromPvl(PvlGroup& scParameterGroup);
  // getters
  QString instrumentId() const;
  InstrumentPointingSolveOption instrumentPointingSolveOption() const;
  SpiceRotation::Source pointingInterpolationType() const;
  int ckDegree() const;
  int ckSolveDegree() const;
  int numberCameraAngleCoefficientsSolved() const;
  bool solveTwist() const;
  bool solvePolyOverPointing() const;
  QList<double> aprioriPointingSigmas() const;
  InstrumentPositionSolveOption instrumentPositionSolveOption() const;
  SpicePosition::Source positionInterpolationType() const;
  int spkDegree() const;
  int spkSolveDegree() const;
  int numberCameraPositionCoefficientsSolved() const;
  bool solvePositionOverHermite() const;
  QList<double> aprioriPositionSigmas() const;
  // enum-string converters
  static InstrumentPointingSolveOption stringToInstrumentPointingSolveOption(QString option);
  static QString instrumentPointingSolveOptionToString(InstrumentPointingSolveOption option);
  static InstrumentPositionSolveOption stringToInstrumentPositionSolveOption(QString option);
  static QString instrumentPositionSolveOptionToString(InstrumentPositionSolveOption option);
#endif
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug() << "Create a BundleObservationVector object...";
  qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  qDebug();


  } 
  catch (IException &e) {
    e.print();
  }
}
