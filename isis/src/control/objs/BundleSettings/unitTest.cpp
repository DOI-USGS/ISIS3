#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QString>

#include "BundleObservationSolveSettings.h"
#include "BundleSettings.h"
#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Preference.h"
#include "PvlObject.h"


using namespace std;
using namespace Isis;
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);

  try {
    qDebug() << "Unit test for BundleSettings...";
    qDebug() << "Printing PVL group with settings from the default constructor...";
    BundleSettings settings;
    // tested fully by call to pvlObject()
    //      bool validateNetwork() const;
    //      SolveMethod solveMethod() const;
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
    //      bool createBundleOutputFile() const;
    //      bool createCSVPointsFile() const;
    //      bool createResidualsFile() const;
    //      QString outputFilePrefix() const;
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
    BundleSettings assignmentOpSettings = settings;
    assignmentOpSettings = settings;
    pvl = assignmentOpSettings.pvlObject("AssignedSettingsObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing mutator methods...";
    // reset all...
    // validate the network
    settings.setValidateNetwork(true);
    // set the solve options
    settings.setSolveOptions(BundleSettings::stringToSolveMethod("specialk"), 
                             true, true, true, true, 
                             1000.0, 2000.0, 3000.0);
    // set outlier rejection
    settings.setOutlierRejection(true, 4.0);
    // create and fill the list of observation solve settings... then set
    BundleObservationSolveSettings boss1;
    boss1.setInstrumentId("Instrument1");
    boss1.setInstrumentPositionSettings(BundleObservationSolveSettings::AllPositionCoefficients,
                                        5, 6, true, 7000.0, 8000.0, 9000.0);
    boss1.setInstrumentPointingSettings(BundleObservationSolveSettings::NoPointingFactors, 
                                        10, 11, true, true, 12.0, 13.0, 14.0);
    // TODO: why am i getting QList index error???
//    pvl = boss1.pvlObject();
//    cout << pvl << endl << endl;
    QList<BundleObservationSolveSettings> observationSolveSettings;
    observationSolveSettings.append(boss1);
    boss1.setInstrumentId("Instrument2");
    boss1.setInstrumentPositionSettings(BundleObservationSolveSettings::PositionOnly,
                                        15, 16, true, 17000.0, 18000.0, 19000.0);
    boss1.setInstrumentPointingSettings(BundleObservationSolveSettings::AllPointingCoefficients, 
                                        20, 21, true, true, 22.0, 23.0, 24.0);
    // TODO: why am i getting QList index error???
//    pvl = boss1.pvlObject();
//    cout << pvl << endl << endl;
    observationSolveSettings.append(boss1);
    settings.setObservationSolveOptions(observationSolveSettings);
    // set convergence criteria values
    settings.setConvergenceCriteria(
                     BundleSettings::stringToConvergenceCriteria("parametercorrections"), 0.25, 26);
    // set maximum likelihood models
    settings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Huber, 0.27); 
    settings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Welsch, 28); 
    settings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::HuberModified, 29); 
    settings.addMaximumLikelihoodEstimatorModel(MaximumLikelihoodWFunctions::Chen, 30); 
    // set output file options
    settings.setOutputFiles("TestFilePrefix", false, false, false);
    pvl = settings.pvlObject("ResetAllOptions");
    cout << pvl << endl << endl;

    // now for test coverage, call some more resets
    // SolveObservationMode = UpdateCubeLabel = ErrorPropagation = true and
    // SolveRadius = OutlierRejection = false
    settings.setSolveOptions(BundleSettings::stringToSolveMethod("sparse"), 
                             true, true, true, false);
    settings.setOutlierRejection(false);
    settings.setOutputFiles("TestFilePrefix", false, true, false);
    pvl = settings.pvlObject("ResetSolveOptions");
    cout << pvl << endl << endl;

    // reset output options - test coverage
    settings.setOutputFiles("TestFilePrefix", false, false, true);
    pvl = settings.pvlObject("ResetOutputOptions");
    cout << pvl << endl << endl;


    qDebug() << "Testing accessor methods...";
    BundleObservationSolveSettings boss2 = settings.observationSolveSettings("Instrument1");
    qDebug() << boss2.instrumentId();
    // TODO: why am i getting QList index error???
    // pvl = boss2.pvlObject();
    // cout << pvl << endl << endl;

    boss2 = settings.observationSolveSettings(1);
    qDebug() << boss2.instrumentId();
    // TODO: why am i getting QList index error???
    // pvl = boss2.pvlObject();
    // cout << pvl << endl << endl;

    QList< QPair< MaximumLikelihoodWFunctions::Model, double > > models
           = settings.maximumLikelihoodEstimatorModels();
    for (int i = 0; i < models.size(); i++) {
      qDebug() << MaximumLikelihoodWFunctions::modelToString(models[i].first)
               << toString(models[i].second);
    }
    qDebug();

    qDebug() << "Testing static enum-to-string and string-to-enum methods...";
    qDebug() << BundleSettings::solveMethodToString(BundleSettings::stringToSolveMethod("SPARSE"));
    qDebug() << BundleSettings::solveMethodToString(BundleSettings::stringToSolveMethod("SPECIALK"));
    qDebug() << BundleSettings::convergenceCriteriaToString(
                               BundleSettings::stringToConvergenceCriteria("SIGMA0")); 
    qDebug() << BundleSettings::convergenceCriteriaToString(
                               BundleSettings::stringToConvergenceCriteria("PARAMETERCORRECTIONS")); 
    qDebug();
 
    qDebug() << "Testing serialization...";
    QByteArray byteArray;
    QDataStream outputData(&byteArray, QIODevice::WriteOnly);
    outputData << settings;
    QDataStream inputData(byteArray);
    BundleSettings newSettings;
    inputData >> newSettings;
    pvl = newSettings.pvlObject();
    cout << pvl << endl;
    qDebug();

    qDebug() << "Testing error throws...";
    try {
      BundleSettings::stringToSolveMethod("Nonsense");
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      BundleSettings::solveMethodToString(BundleSettings::SolveMethod(31));
    } 
    catch (IException &e) {
      e.print();
    }
    try {
      settings.observationSolveSettings("NoSuchInstrumentId");
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
  } 
  catch (IException &e) {
    e.print();
  }
}
