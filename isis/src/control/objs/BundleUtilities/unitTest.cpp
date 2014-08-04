#include <QByteArray>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QXmlInputSource>
#include <QXmlStreamWriter>

#include "BundleControlPoint.h"
#include "BundleControlPointVector.h"
#include "BundleImage.h"
#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"
#include "BundleObservationVector.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


using namespace std;
using namespace Isis;



namespace Isis {
  class XmlHandlerTester : public BundleObservationSolveSettings {
    public:
      XmlHandlerTester(Project *project, XmlStackedHandlerReader *reader, FileName xmlFile)
          : BundleObservationSolveSettings(project, reader) {

        QString xmlPath(xmlFile.expanded());
        QFile file(xmlPath);

        if (!file.open(QFile::ReadOnly) ) {
          throw IException(IException::Io,
                           QString("Unable to open xml file, [%1],  with read access").arg(xmlPath),
                           _FILEINFO_);
        }

        QXmlInputSource xmlInputSource(&file);
        bool success = reader->parse(xmlInputSource);
        if (!success) {
          throw IException(IException::Unknown, 
                           QString("Failed to parse xml file, [%1]").arg(xmlPath),
                            _FILEINFO_);
        }

      }

      ~XmlHandlerTester() {
      }

  };
}



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
  boss.setInstrumentPointingSettings(BundleObservationSolveSettings::AnglesVelocity, true, 1, 2,
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
  qDebug() << "Testing QDataStream write/read...";
  QByteArray byteArray;
  QDataStream outputData(&byteArray, QIODevice::WriteOnly);
  outputData << boss;
  QDataStream inputData(byteArray);
  BundleObservationSolveSettings newBoss;
  inputData >> newBoss;
  pvl = newBoss.pvlObject();
  cout << pvl << endl;

  qDebug();
  qDebug() << "Testing XML write/read...";
  // write xml 
  FileName xmlFile("./BundleObservationSolveSettings.xml");
  QString xmlPath = xmlFile.expanded();
  QFile qXmlFile(xmlPath);
  if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
    throw IException(IException::Io,
                     QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                     _FILEINFO_);
  }
  QXmlStreamWriter writer(&qXmlFile);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  Project *project = NULL;
  FileName settingsXml();
  boss.save(writer, project);
  writer.writeEndDocument();
  qXmlFile.close();
  // read xml    
  XmlStackedHandlerReader reader;
//  XmlHandlerTester bossToFill(project, &reader, xmlFile);
  BundleObservationSolveSettings bossToFill(xmlFile, project, &reader);
  pvl = bossToFill.pvlObject("FromXml");
  cout << pvl << endl << endl;

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
