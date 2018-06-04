#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QtDebug>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include "BundleControlPoint.h"
#include "BundleObservation.h"
#include "BundleObservationVector.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "BundleSolutionInfo.h"
#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlNet.h"
#include "FileName.h"
#include "IException.h"
#include "ImageList.h"
#include "Preference.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


using namespace std;
using namespace Isis;

void printXml(const BundleSolutionInfo &);

/**
 * This class is needed to test the xml read/write methods.
 * @author 2015-??-?? Jeannie Backer
 *
 * @internal
 *   @history 2015-??-?? Jeannie Backer - Original version.
 */
namespace Isis {
  class BundleSolutionInfoXmlHandlerTester : public BundleSolutionInfo {
    public:
      BundleSolutionInfoXmlHandlerTester(Project *project, XmlStackedHandlerReader *reader,
                                     FileName xmlFile) : BundleSolutionInfo(project, reader) {

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

      ~BundleSolutionInfoXmlHandlerTester() {
      }

  };
}


/**
 * Unit Test for BundleSolutionInfo class.
 *
 * @internal
 *   @history 2015-09-03 Jeannie Backer - Commented out xml code test until we determine whether
 *                           we will keep this code.
 *   @history 2016-10-13 Ian Humphrey -  Changed addnew call to addNew(). References #4293.
 *   @history 2017-04-24 Ian Humphrey -  Replaced pvlObject() with XML save(). Fixes #4797.
 *   @history 2018-05-24 Ken Edmundson - Removed testing for copy constructor and assignment
 *                                       operator because these have been removed from
 *                                       BundleSolutionInfo.
 */
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);

  try {
    qDebug() << "Unit test for BundleSolutionInfo...";
    qDebug() << "Serializing results from the settings/cnet/statistics constructor...";

    // create default settings and statistics objects to pass into results object
    BundleSettingsQsp settings = BundleSettingsQsp(new BundleSettings);
    qDebug() << "Created new BundleSettings...";

    settings->setOutputFilePrefix("");
    FileName cnetFile("cnetfile.net");
    BundleResults statistics;
    ControlPoint *freePoint = new ControlPoint("FreePoint");
    ControlMeasure *freeMeasure1 = new ControlMeasure;
    freeMeasure1->SetCubeSerialNumber("Ignored");
    freeMeasure1->SetIgnored(true);
    freePoint->Add(freeMeasure1);
    ControlMeasure *freeMeasure2 = new ControlMeasure;
    freeMeasure2->SetCubeSerialNumber("NotIgnored");
    freeMeasure2->SetIgnored(false);
    freeMeasure2->SetCoordinate(1.0, 2.0);
    freeMeasure2->SetResidual(-3.0, 4.0);
    freePoint->Add(freeMeasure2);
    SurfacePoint freeSurfacePoint(Latitude(45.0, Angle::Degrees),
                                  Longitude(120.0, Angle::Degrees),
                                  Distance(6.0, Distance::Meters));
    freePoint->SetAdjustedSurfacePoint(freeSurfacePoint);
    ControlPoint *fixedPoint = new ControlPoint("FixedPoint");
    fixedPoint->SetType(ControlPoint::Fixed);
    SurfacePoint fixedSurfacePoint(Latitude(90.0, Angle::Degrees),
                                   Longitude(180.0, Angle::Degrees),
                                   Distance(10.0, Distance::Meters));
    fixedPoint->SetAdjustedSurfacePoint(fixedSurfacePoint);
    ControlNet outNet;
    outNet.AddPoint(freePoint);
    outNet.AddPoint(fixedPoint);
    BundleControlPointQsp freeBundleControlPoint(new BundleControlPoint(freePoint));
    BundleControlPointQsp fixedBundleControlPoint(new BundleControlPoint(fixedPoint));
    QVector<BundleControlPointQsp> bundleControlPointVector;
    bundleControlPointVector.append(freeBundleControlPoint);
    bundleControlPointVector.append(fixedBundleControlPoint);
    Camera *camera = NULL;
    BundleImage bundleImage(camera,
                            "TestImageSerialNumber",
                            "TestImageFileName");
    BundleObservationVector observationVector;
    QObject *parent = NULL;
    observationVector.addNew(BundleImageQsp(new BundleImage(bundleImage)),
                             "ObservationNumber1",
                             "Instrument1",
                             BundleSettingsQsp(new BundleSettings));
    statistics.setBundleControlPoints(bundleControlPointVector);
    statistics.setOutputControlNet(ControlNetQsp(new ControlNet(outNet)));
    statistics.setObservations(observationVector);
    QList<ImageList *> imgList;
    BundleSolutionInfo results(settings, cnetFile, statistics, imgList, parent);
    printXml(results);

    qDebug() << "";
    qDebug() << "Testing XML serialization 1: round trip serialization of BundleSolution object...";
    qDebug() << "Serializing test XML object to file...";
    printXml(results);    // save XML to test log for comparison
    FileName xmlFile1("./BundleSolutionInfo1.xml");
    QString xmlPath1 = xmlFile1.expanded();
    QFile qXmlFile1(xmlPath1);
    if (!qXmlFile1.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath1),
                       _FILEINFO_);
    }
    QXmlStreamWriter writer(&qXmlFile1);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    Project *project = NULL;
    results.save(writer, project, "");
    writer.writeEndDocument();
    qXmlFile1.close();

    qDebug() << "Testing XML: reading serialized BundleResults back in...";
    XmlStackedHandlerReader reader1;
    BundleSolutionInfoXmlHandlerTester bsFromXml1(project, &reader1, xmlFile1);
    qDebug() << "Testing XML: Object deserialized as (should match object above):";
    printXml(bsFromXml1);  // Save comparison output to log file

    qDebug() << "Testing mutator methods...";
    statistics.setRejectionLimit(0.5);
    results.setOutputStatistics(statistics);
    results.setRunTime("xxx"); //???
    printXml(results);

    qDebug() << "Testing accessor methods...";
    // Can't print this value out since it changes for every run,
    // qDebug() << "quuid = ";
    // but we will call the method for test coverage
    results.id();
    qDebug() << "runTime = " << results.runTime();
    qDebug() << "";

    qDebug() << "Because we cannot create a Directory with a null parent, ";
    qDebug() << "we cannot test updateFileName().";
    qDebug() << "";

    qDebug() << "Testing error throws...";
    try {
      // bundleResults exception cannot be tested because the BundleResults cannot be NULL
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    Statistics rmsStats;
    rmsStats.SetValidRange(0, 100);
    rmsStats.AddData(0);
    rmsStats.AddData(1);
    rmsStats.AddData(2);
    rmsStats.AddData(3);
    rmsStats.AddData(Isis::Null);// 1 NULL
    rmsStats.AddData(Isis::Lrs); // 2 LRS
    rmsStats.AddData(Isis::Lrs);
    rmsStats.AddData(Isis::Lis); // 3 LIS
    rmsStats.AddData(Isis::Lis);
    rmsStats.AddData(Isis::Lis);
    rmsStats.AddData(Isis::Hrs); // 4 HRS
    rmsStats.AddData(Isis::Hrs);
    rmsStats.AddData(Isis::Hrs);
    rmsStats.AddData(Isis::Hrs);
    rmsStats.AddData(Isis::His); // 5 HIS
    rmsStats.AddData(Isis::His);
    rmsStats.AddData(Isis::His);
    rmsStats.AddData(Isis::His);
    rmsStats.AddData(Isis::His);
    rmsStats.AddData(-1);        // 1 below range
    rmsStats.AddData(1000);      // 2 above range
    rmsStats.AddData(1001);
    // 6, 14, 0, 3, 0, 100, 22, 4, 1, 2, 3, 4, 5, 1, 2, false

    QList<Statistics> rmsImageLineResiduals;
    rmsImageLineResiduals += rmsStats;
    rmsStats.AddData(4);
    // 10, 30, 0, 4, 0, 100, 23, 5, 1, 2, 3, 4, 5, 1, 2, false
    rmsImageLineResiduals += rmsStats;
    rmsStats.AddData(5);
    rmsStats.RemoveData(5);
    // 10, 30, 0, 5, 0, 100, 23, 5, 1, 2, 3, 4, 5, 1, 2, true
    rmsImageLineResiduals += rmsStats;

    QList<Statistics> rmsImageSampleResiduals = rmsImageLineResiduals;
    rmsImageSampleResiduals[0].RemoveData(0);
    rmsImageSampleResiduals[0].AddData(4);
    rmsImageSampleResiduals[2].RemoveData(2);
    // 10, 30, 0, 3, 0, 100, 22, 4, 1, 2, 3, 4, 5, 1, 2, true
    // 10, 30, 0, 4, 0, 100, 23, 5, 1, 2, 3, 4, 5, 1, 2, false
    // 8, 26, 0, 5, 0, 100, 22, 4, 1, 2, 3, 4, 5, 1, 2, true

    QList<Statistics> rmsImageResiduals = rmsImageSampleResiduals;
    rmsImageResiduals[0].AddData(0);
    rmsImageResiduals[0].AddData(1);
    rmsImageResiduals[0].AddData(2);
    rmsImageResiduals[0].AddData(3);
    rmsImageResiduals[1].AddData(0);
    rmsImageResiduals[1].AddData(1);
    rmsImageResiduals[1].AddData(2);
    rmsImageResiduals[1].AddData(3);
    rmsImageResiduals[2].AddData(0);
    rmsImageResiduals[2].AddData(1);
    rmsImageResiduals[2].AddData(2);
    rmsImageResiduals[2].AddData(3);
    // 16, 44, 0, 3, 0, 100, 26, 8, 1, 2, 3, 4, 5, 1, 2, true
    // 16, 44, 0, 4, 0, 100, 27, 9, 1, 2, 3, 4, 5, 1, 2, false
    // 14, 40, 0, 5, 0, 100, 26, 8, 1, 2, 3, 4, 5, 1, 2, true

    statistics.setRmsImageResidualLists(rmsImageLineResiduals,
                                        rmsImageSampleResiduals,
                                        rmsImageResiduals);
    results.setOutputStatistics(statistics);

    qDebug() << "Testing output methods";

    results.outputText();
    results.outputImagesCSV();
    results.outputPointsCSV();
    results.outputResiduals();


    qDebug() << "";
    qDebug() << "Testing XML serialization 2: round trip serialization of fully populated BundleSolution object...";
    qDebug() << "Serializing test XML object to file...";
    printXml(results);    // save XML to test log for comparison
    FileName xmlFile2("./BundleSolutionInfo2.xml");
    QString xmlPath2 = xmlFile2.expanded();
    QFile qXmlFile2(xmlPath2);
    if (!qXmlFile2.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath2),
                       _FILEINFO_);
    }
    QXmlStreamWriter writer2(&qXmlFile2);
    writer2.setAutoFormatting(true);
    writer2.writeStartDocument();
    results.save(writer2, project, "");
    writer2.writeEndDocument();
    qXmlFile2.close();

    qDebug() << "Testing XML: reading serialized BundleResults back in...";
    XmlStackedHandlerReader reader;
    BundleSolutionInfoXmlHandlerTester bsFromXml2(project, &reader, xmlFile2);
    qDebug() << "Testing XML: Object deserialized as (should match object above):";
    printXml(bsFromXml2);  // Save comparison output to log file


    qDebug() << "";
    qDebug() << "Writing text ouput file...";
    FileName txtFile("./bundleout.txt");
    QFile txtOutput(txtFile.expanded());
    if (txtOutput.exists()) {
      txtOutput.remove();
    }
    qDebug() << "Writing csv ouput files...";
    FileName pointsCsv("./bundleout_points.csv");
    QFile pointsOutput(pointsCsv.expanded());
    if (pointsOutput.exists()) {
      pointsOutput.remove();
    }
    FileName residualsCsv("./residuals.csv");
    QFile residualsOutput(residualsCsv.expanded());
    if (residualsOutput.exists()) {
      residualsOutput.remove();
    }
    FileName imagesCsv("./bundleout_images.csv");
    QFile imagesOutput(imagesCsv.expanded());
    if (imagesOutput.exists()) {
      imagesOutput.remove();
    }

    qXmlFile1.remove();
    qXmlFile2.remove();

  }
  catch (IException &e) {
    e.print();
  }
}


/**
 * Prints the serialzed BundleSolutionInfo as XML.
 *
 * @param const BundleSolutionInfo &printable The BundleSolutionInfo to print.
 */
void printXml(const BundleSolutionInfo &printable) {
  QString output;
  QXmlStreamWriter writer(&output);
  writer.setAutoFormatting(true);
  printable.save(writer, NULL, "");
  // Note Statistics class does not serialize/restore properly as of 2017-04-27
  output.remove(QRegExp("<statistics>.*</statistics>"));
  output.remove(QRegExp("<id>[^<]*</id>"));
  qDebug().noquote() << output << endl << endl;
}
