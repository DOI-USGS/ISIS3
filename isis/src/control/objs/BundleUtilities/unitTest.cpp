#include <QByteArray>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QXmlInputSource>
#include <QXmlStreamWriter>

#include <boost/numeric/ublas/symmetric.hpp>

#include "Angle.h"
#include "BundleControlPoint.h"
#include "BundleImage.h"
#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"
#include "BundleObservationVector.h"
#include "BundleSettings.h"
#include "Camera.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "XmlStackedHandlerReader.h"

using namespace std;
using namespace Isis;

void printBundleMeasure(BundleMeasure &);

/**
 * @author 2014 Jeannie Backer
 *
 * @internal
 *   @history 2014-12-11 Jeannie Backer - Added test for BundleControlPoint.
 *                           Updated truth file, improved overall test coverage.
 */



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
  qDebug() << "";

  try {
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "Testing BundleObservationSolveSettings...";
    qDebug() << "";
    // default constructor
    qDebug() << "Printing PVL group with settings from the default constructor...";
    BundleObservationSolveSettings boss;
    PvlObject pvl = boss.pvlObject("DefaultBundleObservationSolveSettings");
    cout << pvl << endl << endl;
    
    qDebug() << "Testing copy constructor...";
    BundleObservationSolveSettings copySettings(boss);
    pvl = copySettings.pvlObject("CopySettingsObject");
    cout << pvl << endl << endl;
    
    qDebug() << "Testing assignment operator to set this equal to itself...";
    boss = boss;
    pvl = boss.pvlObject("SelfAssignedSettingsObject");
    cout << pvl << endl << endl;
    
    qDebug() << "Testing assignment operator to create a new settings object...";
    BundleObservationSolveSettings assignmentOpSettings;
    assignmentOpSettings = boss;
    pvl = assignmentOpSettings.pvlObject("AssignedSettingsObject");
    cout << pvl << endl << endl;
    
    qDebug() << "Testing mutator methods...";
    qDebug() << "setInstrument(), setInstrumentPointingSettings(), setInstrumentPositionSettings()";
    boss.setInstrumentId("MyInstrumentId");
    boss.setInstrumentPointingSettings(BundleObservationSolveSettings::AnglesVelocity, true, 1, 2,
                                       false, 3.0, 4.0, 5.0);
    boss.setInstrumentPositionSettings(BundleObservationSolveSettings::PositionOnly, 6, 7,
                                       true, 800.0, 900.0, 1000.0);
    pvl = boss.pvlObject();// using MyInstrumentId as PvlObject name
    cout << pvl << endl << endl;
    
    qDebug() << "setFromPvl()";
    PvlGroup settingsGroup1("SettingsGroupId1");
    settingsGroup1 += PvlKeyword("CAMSOLVE", "NONE");
    settingsGroup1 += PvlKeyword("SPSOLVE", "NONE");
    BundleObservationSolveSettings settingsFromGroup1;
    settingsFromGroup1.setFromPvl(settingsGroup1);
    pvl = settingsFromGroup1.pvlObject("SettingsFromPvlGroup-SolveForNone");
    cout << pvl << endl << endl;
    settingsGroup1.findKeyword("CAMSOLVE").setValue("ANGLES");
    settingsGroup1.findKeyword("SPSOLVE").setValue("POSITIONS");
    BundleObservationSolveSettings settingsFromGroup2;
    settingsFromGroup2.setFromPvl(settingsGroup1);
    pvl = settingsFromGroup2.pvlObject("SettingsFromPvlGroup-SolveForAnglesPositions");
    cout << pvl << endl << endl;
    settingsGroup1.findKeyword("CAMSOLVE").setValue("VELOCITIES");
    settingsGroup1.findKeyword("SPSOLVE").setValue("VELOCITIES");
    BundleObservationSolveSettings settingsFromGroup3;
    settingsFromGroup3.setFromPvl(settingsGroup1);
    pvl = settingsFromGroup3.pvlObject("SettingsFromPvlGroup-SolveForVelocities");
    cout << pvl << endl << endl;
    settingsGroup1.findKeyword("CAMSOLVE").setValue("ACCELERATIONS");
    settingsGroup1.findKeyword("SPSOLVE").setValue("ACCELERATIONS");
    BundleObservationSolveSettings settingsFromGroup4;
    settingsFromGroup4.setFromPvl(settingsGroup1);
    pvl = settingsFromGroup4.pvlObject("SettingsFromPvlGroup-SolveForAccelerations");
    cout << pvl << endl << endl;
    
    PvlGroup settingsGroup2("SettingsGroupId2");
    settingsGroup2 += PvlKeyword("CAMSOLVE", "All");
    settingsGroup2 += PvlKeyword("TWIST", "No");
    settingsGroup2 += PvlKeyword("CKDEGREE", "4");
    settingsGroup2 += PvlKeyword("CKSOLVEDEGREE", "5");
    settingsGroup2 += PvlKeyword("OVEREXISTING", "true");
    settingsGroup2 += PvlKeyword("CAMERA_ANGLES_SIGMA", "1.0");
    settingsGroup2 += PvlKeyword("CAMERA_ANGULAR_VELOCITY_SIGMA", "-1.0");
    settingsGroup2 += PvlKeyword("CAMERA_ANGULAR_ACCELERATION_SIGMA", "3.0");
    settingsGroup2 += PvlKeyword("SPSOLVE", "All");
    settingsGroup2 += PvlKeyword("SPKDEGREE", "6");
    settingsGroup2 += PvlKeyword("SPKSOLVEDEGREE", "7");
    settingsGroup2 += PvlKeyword("OVERHERMITE", "true");
    settingsGroup2 += PvlKeyword("SPACECRAFT_POSITION_SIGMA", "8.0");
    settingsGroup2 += PvlKeyword("SPACECRAFT_VELOCITY_SIGMA", "9.0");
    settingsGroup2 += PvlKeyword("SPACECRAFT_ACCELERATION_SIGMA", "-1.0");
    boss.setFromPvl(settingsGroup2);
    pvl = boss.pvlObject("SettingsFromPvlGroup-SolveForAllCoefficients");
    cout << pvl << endl << endl;

    qDebug() << "Testing static unused enum-to-string and string-to-enum methods...";
    qDebug() << BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(
                                                                              "NOPOINTINGFACTORS"));
    qDebug() << BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(
                                                                                     "anglesonly"));
    qDebug() << BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(
                                                                              "AnglesAndVelocity"));
    qDebug() << BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(
                                                                  "AnglesVelocityAndAcceleration"));
    qDebug() << BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPointingSolveOption(
                                                                      "AllPolynomialCoefficients"));
    qDebug() << BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(
                                                                              "NOPOSITIONFACTORS"));
    qDebug() << BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(
                                                                                   "positiononly"));
    qDebug() << BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(
                                                                            "PositionAndVelocity"));
    qDebug() << BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(
                                                                "PositionVelocityAndAcceleration"));
    qDebug() << BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                BundleObservationSolveSettings::stringToInstrumentPositionSolveOption(
                                                                      "AllPolynomialCoefficients"));
    qDebug() << "";

    qDebug() << "Testing serialization...";
    QByteArray byteArray;
    QDataStream outputData(&byteArray, QIODevice::WriteOnly);
    outputData << boss;
    QDataStream inputData(byteArray);
    BundleObservationSolveSettings newBoss;
    inputData >> newBoss;
    pvl = newBoss.pvlObject("SerializedSettings");
    cout << pvl << endl;
//    QFile file("BundleObservationSolveSettingsTest.dat");
//    file.open(QIODevice::WriteOnly);
//    QDataStream out(&file);
//    out << boss;
//    file.close();
//    file.open(QIODevice::ReadOnly);
//    QDataStream in(&file);
//    in >> newBoss;
    qDebug() << "";

    qDebug() << "Testing XML: write XML from BundleObservationSolveSettings object...";
    // write xml 
    FileName xmlFile("./BundleObservationSolveSettings.xml");
    QString xmlPath = xmlFile.expanded();
    QFile qXmlFile(xmlPath);
  
    // For test coverage, we need to write/read BundleObservationSolveSettings objects
    // with 0,1,2,3 apriori sigmas and an empty xml
    if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    QXmlStreamWriter writer(&qXmlFile);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    Project *project = NULL;
    settingsFromGroup1.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleObservationSolveSettings object...";
    XmlStackedHandlerReader reader;
    XmlHandlerTester bsFromXml1(project, &reader, xmlFile);
    pvl = bsFromXml1.pvlObject("FromXml-1");
    cout << pvl << endl << endl;

    if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    settingsFromGroup2.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleObservationSolveSettings object...";
    XmlHandlerTester bsFromXml2(project, &reader, xmlFile);
    pvl = bsFromXml2.pvlObject("FromXml-2");
    cout << pvl << endl << endl;

    if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    settingsFromGroup3.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleObservationSolveSettings object...";
    XmlHandlerTester bsFromXml3(project, &reader, xmlFile);
    pvl = bsFromXml3.pvlObject("FromXml-3");
    cout << pvl << endl << endl;

    if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    settingsFromGroup4.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleObservationSolveSettings object...";
    XmlHandlerTester bsFromXml4(project, &reader, xmlFile);
    pvl = bsFromXml4.pvlObject("FromXml-4");
    cout << pvl << endl << endl;

    if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    boss.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleObservationSolveSettings object...";
    XmlHandlerTester bossToFill(project, &reader, xmlFile);
//    BundleObservationSolveSettings bossToFill(xmlFile, project, &reader);
    pvl = bossToFill.pvlObject("FromXml");
    cout << pvl << endl << endl;

    // read xml with no attributes or values
    qDebug() << "Testing XML: read XML with no attributes or values to object...";
    FileName emptyXmlFile("./unitTest_NoElementValues.xml");
    XmlHandlerTester bsFromEmptyXml(project, &reader, emptyXmlFile);
    pvl = bsFromEmptyXml.pvlObject("DefaultSettingsFromEmptyXml");
    cout << pvl << endl << endl;

    //bool deleted = qXmlFile.remove();
    //if (!deleted) {
    //  QString msg = "Unit Test failed. XML file [" + xmlPath + "not deleted.";
    //  throw IException(IException::Io, msg, _FILEINFO_);
    //}

    qDebug() << "Testing error throws...";
    try {
      BundleObservationSolveSettings::stringToInstrumentPointingSolveOption("Nonsense");
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleObservationSolveSettings::instrumentPointingSolveOptionToString(
                                 BundleObservationSolveSettings::InstrumentPointingSolveOption(-1));
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleObservationSolveSettings::stringToInstrumentPositionSolveOption("Nonsense");
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleObservationSolveSettings::instrumentPositionSolveOptionToString(
                                 BundleObservationSolveSettings::InstrumentPositionSolveOption(-2));
    } 
    catch (IException &e) {
      e.print();
    }
    settingsGroup1 += PvlKeyword("OVERHERMITE", "Nonsense");
    try {
      boss.setFromPvl(settingsGroup1);
    } 
    catch (IException &e) {
      e.print();
    }
    settingsGroup1 += PvlKeyword("OVEREXISTING", "Nonsense");
    try {
      boss.setFromPvl(settingsGroup1);
    } 
    catch (IException &e) {
      e.print();
    }
    settingsGroup1 += PvlKeyword("TWIST", "Nonsense");
    try {
      boss.setFromPvl(settingsGroup1);
    } 
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "Testing BundleImage...";
    #if 0
    TEST COVERAGE (SCOPE) FOR THIS SOURCE FILE: 100%
    #endif
    qDebug() << "Constructing BundleImage object from null camera...";
    Camera *camera = NULL;
    BundleImage bi(camera, "TestImageSerialNumber", "TestImageFileName");
    qDebug() << "setting null parentBundleObservation to BundleImage...";
    BundleObservation *parentObservation = NULL;
    bi.setParentObservation(parentObservation);
    qDebug() << "Access camera and parentObservation ...";
//    Camera *cam = bi.camera();
//    BundleObservation *parent = bi.parentObservation();
    qDebug() << "serial number = " << bi.serialNumber();
    qDebug() << "file name     = " << bi.fileName();
    qDebug() << "Testing copy constructor...";
    BundleImage *bi2 = new BundleImage(bi);
    qDebug() << "serial number = " << bi2->serialNumber();
    qDebug() << "file name     = " << bi2->fileName();
    qDebug() << "Testing assignment operator to set this equal to itself...";
    bi = bi;
    qDebug() << "serial number = " << bi.serialNumber();
    qDebug() << "file name     = " << bi.fileName();
    qDebug() << "Testing assignment operator to create a new object...";
    BundleImage bi3(camera, "TestImage2SerialNumber", "TestImage2FileName");
    bi3 = bi;
    qDebug() << "serial number = " << bi3.serialNumber();
    qDebug() << "file name     = " << bi3.fileName();
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "Testing BundleObservation...";
    qDebug() << "Constructing empty BundleObservation object...";
    #if 0
    TEST COVERAGE (SCOPE) FOR THIS SOURCE FILE: 71%
    NEED:
        1) construct with image containing camera that is not null
        2) NOT POSSIBLE - setSolveSettings - initParameterWeights() returns false
        3) initializeExteriorOrientation - positionOption!=No, pointingOption=No
        4) applyParameterCorrections -     positionOption!=No, pointingOption=No
        5) initParameterWeights - possigma[0] <= 0, possigma[1] <= 0, possigma[2]  > 0
                                                    pntsigma[1]  > 0, pntsigma[2] <= 0
        6) formatBundleOutputString - with instrumentPosition/instrumentRotation not NULL
    #endif
    BundleObservation bo;
    BundleTargetBodyQsp bundleTargetBody = BundleTargetBodyQsp(new BundleTargetBody);
    qDebug() << "Constructing BundleObservation object from BundleImage...";
    BundleObservation bo2(bi2, 
                          "ObservationNumber2", 
                          "InstrumentId2", 
                          bundleTargetBody);

    BundleImage *nullImage = NULL;
    BundleObservation nullBO(nullImage, 
                             "NullObservationNumber", 
                             "NullInstrumentId", 
                             bundleTargetBody);

    qDebug() << "Testing assignment operator to set this equal to itself...";
    bo2 = bo2;
    qDebug() << "Testing assignment operator to create a new object...";
    bo = bo2;
    qDebug() << "Testing copy constructor...";
    BundleObservation bo3(bo);
    qDebug() << "Testing mutators and accessors...";
    qDebug() << "    Set/get solve settings using with CAMESOLVE=NONE...";
    bo2.setSolveSettings(settingsFromGroup1);
    BundleObservationSolveSettings bossFromBo = *bo2.solveSettings();
    pvl = bossFromBo.pvlObject("NoCamAngles");
    cout << pvl << endl << endl;
    qDebug() << "    output bundle observation...";
    qDebug().noquote() << bo2.formatBundleOutputString(true);
    qDebug().noquote() << bo2.formatBundleOutputString(false);
    qDebug() << "    Set solve settings using with TWIST=FALSE...";
    bo2.setSolveSettings(bossToFill);
    bossFromBo = *bo2.solveSettings();
    pvl = bossFromBo.pvlObject("NoTwist");
    cout << pvl << endl << endl;
    qDebug() << "    output bundle observation...";
    qDebug().noquote() << bo2.formatBundleOutputString(true);
    qDebug().noquote() << bo2.formatBundleOutputString(false);
    qDebug() << "    Set solve settings using with CAMSOLVE=ALL and TWIST=TRUE...";
    bo3.setSolveSettings(bsFromEmptyXml);
    bossFromBo = *bo3.solveSettings();
    pvl = bossFromBo.pvlObject("SettingsFromBundleObservation");
    cout << pvl << endl << endl;
    bo3.setIndex(1);
    qDebug() << "index = " << toString(bo3.index());
    qDebug() << "instrument id = " << bo3.instrumentId();
    qDebug() << "number parameters =     " << toString(bo3.numberParameters());
    qDebug() << "number position param = " << toString(bo3.numberPositionParameters());
    qDebug() << "number pointing param = " << toString(bo3.numberPointingParameters());
    //???BundleObservationSolveSettings bossFromBo = *bo3.solveSettings();
    //???pvl = bossFromBo.pvlObject("SettingsFromBundleObservation");
    //???cout << pvl << endl << endl;
    qDebug() << "parameter list: " << bo3.parameterList();
    qDebug() << "image names:    " << bo3.imageNames();

    #if 0
// seg fault   qDebug() << bo3.formatBundleOutputString(true);
// seg fault   qDebug() << bo3.formatBundleOutputString(false);
    SpiceRotation sr = *bo3.spiceRotation();
    SpicePosition sp = *bo3.spicePosition();
    #endif
    boost::numeric::ublas::vector< double > paramWts = bo3.parameterWeights();
    boost::numeric::ublas::vector< double > paramCor = bo3.parameterCorrections();
    boost::numeric::ublas::vector< double > aprSigma = bo3.aprioriSigmas();
    boost::numeric::ublas::vector< double > adjSigma = bo3.adjustedSigmas();
    QString vectors = "parameter weights :     ";
    for (unsigned int i = 0; i < paramWts.size(); i++) {
      vectors.append(toString(paramWts[i]));
      vectors.append("     ");
    }
    vectors.append("\nparameter corrections : ");
    for (unsigned int i = 0; i < paramCor.size(); i++) {
      vectors.append(toString(paramCor[i]));
      vectors.append("     ");
    }
    vectors.append("\napriori sigmas :        ");
    for (unsigned int i = 0; i < aprSigma.size(); i++) {
      vectors.append(toString(aprSigma[i]));
      vectors.append("     ");
    }
    vectors.append("\nadjusted sigmas :       ");
    for (unsigned int i = 0; i < adjSigma.size(); i++) {
      vectors.append(toString(adjSigma[i]));
      vectors.append("     ");
    }
    qDebug().noquote() << vectors;
    //???qDebug() << "apply param corrections successful?     " << toString(bo3.applyParameterCorrections(paramCor));
    qDebug() << "    output bundle observation...";
    qDebug().noquote() << bo3.formatBundleOutputString(false);
    qDebug().noquote() << bo3.formatBundleOutputString(true);
    qDebug() << "init exterior orientiation successful?  " << toString(bo3.initializeExteriorOrientation());
    qDebug() << "apply param corrections successful?     " << toString(bo3.applyParameterCorrections(paramCor));
//    SpiceRotation *spicePos = bo3.spiceRotation();
//    SpicePosition *spiceRot = bo3.spicePosition();
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "Testing BundleObservationVector...";
    qDebug() << "";
    #if 0
    TEST COVERAGE (SCOPE) FOR THIS SOURCE FILE: 21%
    Need:
      1) addNew - settings->solveObservationMode() == false
                  map.contains(obsNumber)
      2) addNew - settings->solveObservationMode()
                  map.contains(obsNumber) == false
      3) addNew - settings->solveObservationMode()
                  map.contains(obsNumber)
                  bo.instId() == this.instId
      4) addNew - settings->solveObservationMode()
                  map.contains(obsNumber)
                  bo.instId() != this.instId
                  bundleObservation != null
                  bundleSettings.numberSolveSettings() == 1
      5) addNew - settings->solveObservationMode()
                  map.contains(obsNumber)
                  bo.instId() != this.instId
                  bundleObservation != null
                  bundleSettings.numberSolveSettings() != 1
      6) addNew - 
                  map.contains(obsNumber)
                  bo.instId() != this.instId
                  bundleObservation == null
      7) initializeExteriorOrientation, numberPositionParameters, numberPointingParameters - size > 0
      8) getObsByCubeSerialNumber - map.contains(sn)
      9) getObsByCubeSerialNumber - map.contains(sn) == false

                  
//    bo = *bov.addnew(BundleImage* image, "obs1",
//                              "InstrumentIdBOV", BundleSettings &bundleSettings);
//    bo2 = *bov.addnew(BundleImage* image, "obs2",
//                              "InstrumentIdBOV", BundleSettings &bundleSettings);
//    BundleObservation obs1 = *bov.getObservationByCubeSerialNumber("obs1");
    #endif
    BundleObservationVector bov;
    #if 0
    BundleImage *bundleImage = new BundleImage();
    BundleSettings bundleSettings;
    BundleObservation obs1 = *bov.addnew(bundleImage, "obs1", "InstrumentIdBOV", bundleSettings);
    qDebug() << obs1.formatBundleOutputString(true);
    BundleObservation obs2 = *bov.addnew(bundleImage, "obs2", "InstrumentIdBOV", bundleSettings);
    qDebug() << obs2.formatBundleOutputString(true);
#endif
    qDebug() << "number of position parameters: " << toString(bov.numberPositionParameters());
    qDebug() << "number of pointing parameters: " << toString(bov.numberPointingParameters());
    qDebug() << "number of parameters: " << toString(bov.numberParameters());
    
#if 0
    BundleObservation obs1b = *bov.getObservationByCubeSerialNumber("obs1");
    qDebug() << "same observation?" << toString((obs1 == obs1b));
    qDebug() << obs1b.formatBundleOutputString(true);
#endif
    qDebug() << "init exterior orientiation successful?  " << toString(bov.initializeExteriorOrientation());
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "Testing BundleControlPoint...";
    #if 0
    TEST COVERAGE (SCOPE) FOR THIS SOURCE FILE: 100%
    #endif
    qDebug() << "Create FreePoint with free point containing 2 measures...";
    ControlPoint *freePoint = new ControlPoint("FreePoint");
    ControlMeasure *cm1 = new ControlMeasure;
    cm1->SetCubeSerialNumber("Ignored");
    cm1->SetIgnored(true);
    freePoint->Add(cm1);
    ControlMeasure *cm2 = new ControlMeasure;
    cm2->SetCubeSerialNumber("NotIgnored");
    cm2->SetIgnored(false);
    cm2->SetCoordinate(1.0, 2.0);
    cm2->SetResidual(-3.0, 4.0);
    freePoint->Add(cm2);
    BundleControlPoint bcp1(freePoint);
    bool errorProp = false;
    double radiansToMeters = 10.0;
    qDebug().noquote() << bcp1.formatBundleOutputSummaryString(errorProp);    // ??? these print outs are not pretty... fix???
    qDebug().noquote() << bcp1.formatBundleOutputDetailString(errorProp, radiansToMeters); // ??? these print outs are not pretty... fix???
    errorProp = true;
    qDebug().noquote() << bcp1.formatBundleOutputSummaryString(errorProp);    // ??? these print outs are not pretty... fix???
    qDebug().noquote() << bcp1.formatBundleOutputDetailString(errorProp, radiansToMeters); // ??? these print outs are not pretty... fix???
    qDebug() << "";

    qDebug() << "Modify FreePoint - setAdjustedSurfacePoint(0,0,10) and addMeasure()";
    SurfacePoint sp1(Latitude(0.0, Angle::Degrees), 
                     Longitude(0.0, Angle::Degrees), 
                     Distance(10.0, Distance::Meters));
    bcp1.setAdjustedSurfacePoint(sp1);
    BundleMeasure bcm = *(bcp1.addMeasure(cm1)); // ???? this appears to do nothing! measure is added to the internal QVector of measures, not the member control point...
                                               // probably need to fix the format string methods to use "this" instead of member control point???
                                               // and accessor methods???
    qDebug().noquote() << bcp1.formatBundleOutputSummaryString(errorProp);    // ??? these print outs are not pretty... fix???
    qDebug().noquote() << bcp1.formatBundleOutputDetailString(errorProp, radiansToMeters); // ??? these print outs are not pretty... fix???

    qDebug() << "Modify FreePoint - setWeights() - solveRadius=false";
    BundleSettingsQsp settings = BundleSettingsQsp(new BundleSettings); // default solveRadius=false
    double metersToRadians = 1.0 / radiansToMeters;
    bcp1.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp1.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp1.formatBundleOutputDetailString(errorProp, radiansToMeters);
    boost::numeric::ublas::bounded_vector< double, 3 > aprioriSigmas = bcp1.aprioriSigmas();
    boost::numeric::ublas::bounded_vector< double, 3 > weights = bcp1.weights();
    boost::numeric::ublas::bounded_vector< double, 3 > corrections = bcp1.corrections(); //??? never set 000??? init to 1.0e+50???
    boost::numeric::ublas::bounded_vector< double, 3 > adjustedSigmas = bcp1.adjustedSigmas(); //??? never set 000??? 1.0e+50???
    boost::numeric::ublas::bounded_vector<double, 3> nicVector = bcp1.nicVector(); //??? never set 000c??? 1.0e+50???
    SparseBlockRowMatrix qMatrix = bcp1.cholmod_QMatrix(); //??? empty matrix...
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "N/A" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "N/A" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "N/A" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "corrections:    " << corrections[0] << corrections[1] << corrections[2];
    qDebug() << "adjustedSigmas: " << (Isis::IsSpecial(adjustedSigmas[0]) ? "N/A" : Isis::toString(adjustedSigmas[0]))
                                   << (Isis::IsSpecial(adjustedSigmas[1]) ? "N/A" : Isis::toString(adjustedSigmas[1]))
                                   << (Isis::IsSpecial(adjustedSigmas[2]) ? "N/A" : Isis::toString(adjustedSigmas[2]));
    qDebug() << "nicVector:      " << nicVector[0] << nicVector[1] << nicVector[2];
    qDebug() << "qMatrix:";
    qDebug() << qMatrix;
    qDebug() << "";

    qDebug() << "Modify FreePoint - setWeights() - solveRadius=true, apriori lat/lon/rad <= 0";
    settings->setSolveOptions(BundleSettings::Sparse, false, false, false, true, Isis::Null);
    bcp1.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp1.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp1.formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp1.aprioriSigmas();
    weights = bcp1.weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "N/A" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "N/A" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "N/A" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";

    qDebug() << "Modify FreePoint - setWeights() - solveRadius=true, apriori lat/lon/rad > 0";
    settings->setSolveOptions(BundleSettings::Sparse, false, false, false, true, 2.0, 3.0, 4.0);
    bcp1.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp1.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp1.formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp1.aprioriSigmas();
    weights = bcp1.weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "N/A" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "N/A" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "N/A" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";

    ControlPoint *cp = bcp1.rawControlPoint();
    qDebug() << "Raw control point equal to original?    " << toString(*cp == *freePoint);
    qDebug() << "Raw control point is rejected?          " << toString(bcp1.isRejected());
    SurfacePoint sp = bcp1.getAdjustedSurfacePoint();
    qDebug() << "Adjusted SurfacePoint (Lat, Lon, Rad) = " << toString(sp.GetLatitude().degrees())
                                                           << toString(sp.GetLongitude().degrees())
                                                           << toString(sp.GetLocalRadius().meters());
    qDebug() << "";

    qDebug() << "Create FixedPoint from empty fixed point, adjusted surface point (90, 180, 10)...";
    ControlPoint *fixedPoint = new ControlPoint("FixedPoint");
    fixedPoint->SetType(ControlPoint::Fixed);
    BundleControlPoint *bcp3 = new BundleControlPoint(fixedPoint);
    SurfacePoint sp2(Latitude(90.0, Angle::Degrees), 
                     Longitude(180.0, Angle::Degrees), 
                     Distance(10.0, Distance::Meters));
    bcp3->setAdjustedSurfacePoint(sp2);
    qDebug().noquote() << bcp3->formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp3->formatBundleOutputDetailString(errorProp, radiansToMeters);

    qDebug() << "Modify FixedPoint - setWeights()";
    bcp3->setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp3->formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp3->formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp3->aprioriSigmas();
    weights = bcp3->weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "NULL" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "NULL" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "NULL" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";

    qDebug() << "Create ConstrainedPoint from empty constrained point, surface point (0, 0, 10)...";
    ControlPoint *constrainedPoint = new ControlPoint("ConstrainedPoint");
    constrainedPoint->SetType(ControlPoint::Constrained);
    BundleControlPoint bcp4(constrainedPoint);
    bcp4.setAdjustedSurfacePoint(sp1);
    qDebug().noquote() << bcp4.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp4.formatBundleOutputDetailString(errorProp, radiansToMeters);

    qDebug() << "Modify ConstrainedPoint - setWeights() - solveRadius=false";
    settings->setSolveOptions(BundleSettings::Sparse, false, false, false, false);
    bcp4.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp4.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp4.formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp4.aprioriSigmas();
    weights = bcp4.weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "NULL" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "NULL" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "NULL" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";

    qDebug() << "Modify ConstrainedPoint - setWeights() - no constraints, solveRadius=true, "
                "apriori lat/lon/rad <= 0";
    settings->setSolveOptions(BundleSettings::Sparse, false, false, false, true);
    bcp4.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp4.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp4.formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp4.aprioriSigmas();
    weights = bcp4.weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "NULL" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "NULL" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "NULL" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";

    qDebug() << "Modify ConstrainedPoint - setWeights() - no constraints, solveRadius=true, "
                "apriori lat/lon/rad > 0";
    settings->setSolveOptions(BundleSettings::Sparse, false, false, false, true, 2.0, 3.0, 4.0);
    bcp4.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp4.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp4.formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp4.aprioriSigmas();
    weights = bcp4.weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "NULL" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "NULL" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "NULL" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";
    qDebug() << "";

    qDebug() << "Create ConstrainedPoint from constrained point with adjusted surface point "
                "(32, 120, 1000)...";
    SurfacePoint aprioriSurfPt;
    aprioriSurfPt.SetRadii(Distance(1000.0, Distance::Meters),
                           Distance(1000.0, Distance::Meters),
                           Distance(1000.0, Distance::Meters));
    boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper> covar;
    covar.resize(3);
    covar.clear();
    covar(0,0) = 100.0;
    covar(1,1) = 2500.0;
    covar(2,2) = 400.0;
    aprioriSurfPt.SetRectangular(Displacement(-424.024048, Displacement::Meters),
                                 Displacement(734.4311949, Displacement::Meters),
                                 Displacement(529.919264, Displacement::Meters), covar);
    constrainedPoint->SetAprioriSurfacePoint(aprioriSurfPt);
    BundleControlPoint bcp5(constrainedPoint);
    SurfacePoint adjustedSurfPt(constrainedPoint->GetAdjustedSurfacePoint());
    adjustedSurfPt.SetSpherical(Latitude(32., Angle::Degrees),
                                Longitude(120., Angle::Degrees),
                                Distance(1000., Distance::Meters),
                                Angle(1.64192315,Angle::Degrees),
                                Angle(1.78752107, Angle::Degrees),
                                Distance(38.454887335682053718134171237789, Distance::Meters));
    adjustedSurfPt.SetRadii(Distance(1000.0, Distance::Meters),
                            Distance(1000.0, Distance::Meters),
                            Distance(1000.0, Distance::Meters));
    bcp5.setAdjustedSurfacePoint(adjustedSurfPt);
    qDebug().noquote() << bcp5.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp5.formatBundleOutputDetailString(errorProp, radiansToMeters);
    qDebug() << "Modify ConstrainedPoint - setWeights() - solveRadius=t, lat/lon/rad constrained";
    bcp5.setWeights(settings, metersToRadians);
    qDebug().noquote() << bcp5.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp5.formatBundleOutputDetailString(errorProp, radiansToMeters);
    aprioriSigmas = bcp5.aprioriSigmas(); // these values were verified by comparing against
                                          // SurfacePoint truth data
    weights = bcp5.weights();
    qDebug() << "aprioriSigmas:  " << (Isis::IsSpecial(aprioriSigmas[0]) ? "NULL" : Isis::toString(aprioriSigmas[0]))
                                   << (Isis::IsSpecial(aprioriSigmas[1]) ? "NULL" : Isis::toString(aprioriSigmas[1]))
                                   << (Isis::IsSpecial(aprioriSigmas[2]) ? "NULL" : Isis::toString(aprioriSigmas[2]));
    qDebug() << "weights:        " << weights[0] << weights[1] << weights[2];
    qDebug() << "";

    qDebug() << "Create copy of FreePoint using copy constructor...";
    BundleControlPoint bcp2(bcp1);
    qDebug().noquote() << bcp2.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp2.formatBundleOutputDetailString(errorProp, radiansToMeters);
    qDebug() << "";

    qDebug() << "Overwrite existing object with FixedPoint information...";
    bcp2.copy(*bcp3);
    qDebug().noquote() << bcp2.formatBundleOutputSummaryString(errorProp);
    qDebug().noquote() << bcp2.formatBundleOutputDetailString(errorProp, radiansToMeters);
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "";
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "Testing BundleMeasure...";

    // TEST COVERAGE (SCOPE) FOR THIS SOURCE FILE: 86% //TODO update when SquishCoco works again
    BundleMeasure bundleMeasure(cm2, bcp3);

    bundleMeasure.setParentObservation(&bo2);
    // const BundleObservationSolveSettings *solveSettings = 
    bundleMeasure.observationSolveSettings();
    // Camera *cam = 
    bundleMeasure.camera();
    // BundleObservation  *parentObs = 
    bundleMeasure.parentBundleObservation();
    BundleControlPoint *parentBCP = bundleMeasure.parentControlPoint();
    qDebug() << "parent control point id" << parentBCP->getId();
    // BundleImage        *parentImage = 
    bundleMeasure.parentBundleImage(); //TODO m_parentBundleImage always NULL ??? 

    // Copy and =
    BundleMeasure bundleMeasureCopy(bundleMeasure);
    BundleMeasure bundleMeasureEq = bundleMeasure;

    // Test self-assignment
    bundleMeasure = bundleMeasure;

    qDebug() << "";
    // Verify state and copies
    printBundleMeasure(bundleMeasure);
    printBundleMeasure(bundleMeasureCopy);
    printBundleMeasure(bundleMeasureEq);
      
    qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    qDebug() << "";
  } 
  catch (IException &e) {
    e.print();
  }

}


/**
 * Outputs the BundleMeasure state fo testing and verification
 *
 * @param m The BundleMeasure to print information on
 */
 void printBundleMeasure(BundleMeasure &m) {
   qDebug() << "rejected?" << toString(m.isRejected());
   qDebug() << "measure sample " << toString(m.sample());
   qDebug() << "measure line   " << toString(m.line());
   qDebug() << "measure serial number" << m.cubeSerialNumber();
   qDebug() << "focal x" << toString(m.focalPlaneMeasuredX());
   qDebug() << "focal y" << toString(m.focalPlaneMeasuredY());
   qDebug() << "observation index" << toString(m.observationIndex());
   qDebug() << "";  
}
