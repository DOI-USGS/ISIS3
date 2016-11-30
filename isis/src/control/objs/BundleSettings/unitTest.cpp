#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QtDebug>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include "BundleObservationSolveSettings.h"
#include "BundleSettings.h"
#include "FileName.h"
#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Preference.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


using namespace std;
using namespace Isis;

/**
  * Test BundleSettings 
  *  
  *
  * @author 2014-05-14 Jeannie Backer
  * @internal
  *   @history 2014-05-14 Jeannie Backer - Original version. Code test coverage:
  *                           Scope (100%), Line (100%), Function(100%)
  *   @history 2016-07-07 Jeannie Backer - Updated to include TargetBody tests.
  *                           Note: This is still incomplete until we refactor how TargetBody
  *   @history 2016-08-12 Jeannie Backer - Removed references to solve method. References #4162.
  *   @history 2016-08-18 Jeannie Backer - Removed references to BundleSettings solve method.
  *                           References #4162.
  *   @history 2016-10-13 Ian Humphrey - Replaced setInstrumentId() with addObservationNumber()
  *                           to associate the bundle observation solve settings with an
  *                           observation number. References #4293.
  *   @history 2016-10-24 Makayla Shepherd - Commented out the test for setting the default settings 
  *                           when reading from an empty XML. This test was failing on prog24 and we 
  *                           are not sure if we are going to allow XML reading/writing as we are 
  *                           most likely going to move to HDF5. Fixes #4327.
  *
  *   @todo Truth updated so that the name of the BundleObservationSolveSettings object is Null,
  *         this should be fixed as part of #4292.
  *   @todo Test hdf5 methods when added.
  *   @todo Test setBundleTargetBody()
  *   @todo Test non-null bundleTargetBody()
  *  
  */


namespace Isis {
  /**
   * Child class of BundleSettings used to test the embedded XML handler class.
   *  
   * @author 2014-05-14 Jeannie Backer
   * @internal
   *   @history 2014-05-14 Jeannie Backer - Original version.
   */
  class BundleSettingsXmlHandlerTester : public BundleSettings {
    public:
      /**
       * Constructs BundleSettings using XML handler. 
       *  
       * @param project A pointer to the project.
       * @param reader A pointer to a XmlStackedHandlerReader.
       * @param xmlFile The name of the XML file to be used to create a 
       *                BundleSettings object.
       * 
       * 
       * @throw Isis::Exception::Io "Unable to open XML file with read access."
       * @throw Isis::Exception::Unknown "Failed to parse XML file."
       */
      BundleSettingsXmlHandlerTester(Project *project, XmlStackedHandlerReader *reader, 
                                     FileName xmlFile) : BundleSettings(project, reader) {

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

      ~BundleSettingsXmlHandlerTester() {
      }

  };
}


int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);

  try {
    qDebug() << "Unit test for BundleSettings...";
    qDebug() << "Printing PVL group with settings from the default constructor...";
    BundleSettings settings;
    // tested fully by each call to pvlObject()
    //      bool validateNetwork() const;
    //      bool solveObservationMode() const;
    //      bool solveRadius() const;
    //      bool updateCubeLabel() const;
    //      bool errorPropagation() const;
    //      bool outlierRejection() const;
    //      double outlierRejectionMultiplier() const;
    //      double globalLatitudeAprioriSigma() const;
    //      double globalLongitudeAprioriSigma() const;
    //      double globalRadiusAprioriSigma() const;
    //      ConvergenceCriteria convergenceCriteria() const;
    //      double convergenceCriteriaThreshold() const;
    //      int convergenceCriteriaMaximumIterations() const;
    //      QString outputFilePrefix() const;
    //      int numberSolveSettings();
    //      int numberTargetBodyParameters() 
    //      bool solveTargetBody()            
    //      bool solvePoleRA()                
    //      bool solvePoleRAVelocity()        
    //      bool solvePoleDec()               
    //      bool solvePoleDecVelocity()       
    //      bool solvePM()                    
    //      bool solvePMVelocity()   
    //      bool solvePMAcceleration()
    //      bool solveTriaxialRadii()
    //      bool solveMeanRadius()

    PvlObject pvl = settings.pvlObject("DefaultSettingsObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing copy constructor...";
    BundleSettings copySettings(settings);
    pvl = copySettings.pvlObject("CopySettingsObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing assignment operator to set this equal to itself...";
    settings = settings;
    pvl = settings.pvlObject("SelfAssignedSettingsObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing assignment operator to create a new settings object...";
    BundleSettings assignmentOpSettings;
    assignmentOpSettings = settings;
    pvl = assignmentOpSettings.pvlObject("AssignedSettingsObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing mutator methods...";
    // reset all...
    // validate the network
    copySettings.setValidateNetwork(true);
    // set the solve options
    copySettings.setSolveOptions(true, true, true, true, 1000.0, 2000.0, 3000.0);
    // set outlier rejection
    copySettings.setOutlierRejection(true, 4.0);
    // create and fill the list of observation solve settings... then set
    BundleObservationSolveSettings boss1;
    boss1.addObservationNumber("Instrument1");
    boss1.setInstrumentPositionSettings(BundleObservationSolveSettings::AllPositionCoefficients,
                                        5, 6, true, 7000.0, 8000.0, 9000.0);
    boss1.setInstrumentPointingSettings(BundleObservationSolveSettings::NoPointingFactors, 
                                        10, 11, true, true, 12.0, 13.0, 14.0);
    // TODO: why am i getting QList index error???
//    pvl = boss1.pvlObject();
//    cout << pvl << endl << endl;
    QList<BundleObservationSolveSettings> observationSolveSettings;
    observationSolveSettings.append(boss1);
    boss1.addObservationNumber("Instrument2");
    boss1.setInstrumentPositionSettings(BundleObservationSolveSettings::PositionOnly,
                                        15, 16, true, 17000.0, 18000.0, 19000.0);
    boss1.setInstrumentPointingSettings(BundleObservationSolveSettings::AllPointingCoefficients, 
                                        20, 21, true, true, 22.0, 23.0, 24.0);
    // TODO: why am i getting QList index error???
//    pvl = boss1.pvlObject();
//    cout << pvl << endl << endl;
    observationSolveSettings.append(boss1);
    copySettings.setObservationSolveOptions(observationSolveSettings);
    // set convergence criteria values
    copySettings.setConvergenceCriteria(
                     BundleSettings::stringToConvergenceCriteria("parametercorrections"), 0.25, 26);
    // set maximum likelihood models
    copySettings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Huber, 0.27); 
    copySettings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Welsch, 28); 
    copySettings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::HuberModified, 29); 
    copySettings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Chen, 30); 
    // set target body
    // TODO
    // set output file options
    copySettings.setOutputFilePrefix("TestFilePrefix");
    pvl = copySettings.pvlObject("ResetAllOptions");
    cout << pvl << endl << endl;

    // now for test coverage, call some more resets
    // SolveObservationMode = UpdateCubeLabel = ErrorPropagation = true and
    // SolveRadius = OutlierRejection = false
    settings.setSolveOptions(true, true, true, false);
    settings.setOutlierRejection(false);
    settings.setOutputFilePrefix("TestFilePrefix");
    pvl = settings.pvlObject("ResetSolveOptions");
    cout << pvl << endl << endl;

    // reset output options - test coverage
    settings.setOutputFilePrefix("TestFilePrefix");
    pvl = settings.pvlObject("ResetOutputOptions");
    cout << pvl << endl << endl;


    qDebug() << "Testing accessor methods...";
    BundleObservationSolveSettings boss2 = copySettings.observationSolveSettings("Instrument1");
    qDebug() << "Get BundleObservationSolveSettings with name InstrumentId = Instrument1";
    pvl = boss2.pvlObject();
    cout << pvl << endl << endl;
    boss2 = copySettings.observationSolveSettings(1);
    qDebug() << "Now get BundleObservationSolveSettings at index 1";
    pvl = boss2.pvlObject();
    cout << pvl << endl << endl;

    QList< QPair< MaximumLikelihoodWFunctions::Model, double > > models
            = copySettings.maximumLikelihoodEstimatorModels();
    for (int i = 0; i < models.size(); i++) {
      qDebug() << MaximumLikelihoodWFunctions::modelToString(models[i].first)
               << toString(models[i].second);
    }
    qDebug() << "";

    qDebug() << "Testing static enum-to-string and string-to-enum methods...";
    qDebug() << BundleSettings::convergenceCriteriaToString(
                               BundleSettings::stringToConvergenceCriteria("SIGMA0")); 
    qDebug() << BundleSettings::convergenceCriteriaToString(
                               BundleSettings::stringToConvergenceCriteria("PARAMETERCORRECTIONS")); 
    qDebug() << "";
 
    qDebug() << "Testing serialization...";
    QByteArray byteArray;
    QDataStream outputData(&byteArray, QIODevice::WriteOnly);
    outputData << copySettings;
    QDataStream inputData(byteArray);
    BundleSettings newSettings;
    inputData >> newSettings;
    pvl = newSettings.pvlObject();
    cout << pvl << endl;
    qDebug() << "";

    qDebug() << "Testing XML: write XML from BundleSettings object...";
    // write xml 
    FileName xmlFile("./BundleSettings.xml");
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
    settings.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleSettings object...";
    XmlStackedHandlerReader reader;
    BundleSettingsXmlHandlerTester bsFromXml(project, &reader, xmlFile);
    //BundleSettings bsFromXml(xmlFile, project, &reader);
    pvl = bsFromXml.pvlObject("BundleSettingsFromXml");
    cout << pvl << endl << endl;

    // for test coverage, read/write the copySettings object with
    // solveRadius=true, outlierRejection=true, no observationSolveSettings,
    // globalRadiusAprioriSigma != N/A, outlierRejectionMultiplier != N/A
    copySettings.setObservationSolveOptions(QList<BundleObservationSolveSettings>());
    if (!qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    copySettings.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();
    // read xml    
    qDebug() << "Testing XML: read XML to BundleSettings object...";
    BundleSettingsXmlHandlerTester bsFromXml2(project, &reader, xmlFile);
    //BundleSettings bsFromXml(xmlFile, project, &reader);
    pvl = bsFromXml2.pvlObject("BundleSettingsFromXml");
    cout << pvl << endl << endl;
    qXmlFile.remove();


    qDebug() << "Testing error throws..."; // ??? weird error if i move this after read from empty???
    try {
      settings.observationSolveSettings("UnassociatedObservationNumber");
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      settings.observationSolveSettings(32);
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      settings.observationSolveSettings(-1.0);
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleSettings::stringToConvergenceCriteria("Pickles");
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleSettings::convergenceCriteriaToString(BundleSettings::ConvergenceCriteria(33));
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleSettings invalidMaxLikelihoodModel1;
      invalidMaxLikelihoodModel1.addMaximumLikelihoodEstimatorModel(
          MaximumLikelihoodWFunctions::Chen, 33); 
    } 
    catch (IException &e) {
      e.print();
    }
    
    /* 
     * Commenting out this test because it is causing an error on prog24 and we are not sure
     * if we are going to include XML reading/writing
     * 
     * qDebug() << "";    // read xml with no attributes or values
     * qDebug() << "Testing XML: read XML with no attributes or values to object...";
     * FileName emptyXmlFile("./unitTest_NoElementValues.xml");
     * BundleSettingsXmlHandlerTester bsFromEmptyXml(project, &reader, emptyXmlFile);
     * pvl = bsFromEmptyXml.pvlObject("DefaultBundleSettingsFromEmptyXml");
     * cout << pvl << endl << endl;
    */
    
    qDebug() << "";
    qDebug() << "Testing HDF5 write/read...";
    FileName hdfFile("./BundleSettings.hdf");
    if (hdfFile.fileExists()) {
      QFile::remove(hdfFile.expanded());
    }
//    newSettings.createH5Group(hdfFile, "/");
//    BundleSettings fromHDF(hdfFile);
//    pvl = fromHDF.pvlObject("BundleSolutionInfoFromHDF");
//    cout << pvl << endl << endl;
//    QFile::remove(hdfFile.expanded());

  } 
  catch (IException &e) {
    e.print();
  }
}
#if 0
still need test coverage for

commented out bundlesettings constructors, commented out xmlhandler constructor

fatalError
#endif
