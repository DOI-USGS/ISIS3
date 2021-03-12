/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlNet.h"
#include "FileName.h"
#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"
#include "Preference.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


using namespace std;
using namespace Isis;

void printXml(const BundleResults &);

namespace Isis {
  /**
   * This class is used to test BundleResults' XmlHandler class.
   *
   * @author 2014-07-28 Jeannie Backer
   *
   * @internal
   *   @history 2014-07-28 Jeannie Backer - Original version.
   */
  class BundleResultsXmlHandlerTester : public BundleResults {
    public:
      /**
       * Constructs the tester object from an xml file.
       *
       * @param project The project object the tester belongs to.
       * @param reader The XmlStackedHandlerReader that reads the xml file.
       * @param xmlFile The xml file to construct the tester from.
       */
      BundleResultsXmlHandlerTester(Project *project, XmlStackedHandlerReader *reader,
                                     FileName xmlFile) : BundleResults(project, reader) {

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

      /**
       * Destroys the tester object
       */
      ~BundleResultsXmlHandlerTester() {
      }

  };
}


/**
 * Unit test for BundleResults.
 *
 * @internal
 *   @history 2016-07-01 Jesse Mapel - Added TargetBody tests.
 *   @history 2016-10-13 Ian Humphrey - Changed call from addnew to addNew(). References #4293.
 *   @history 2017-04-24 Ian Humphrey - Removed pvlObject serialization, replaced with XML
 *                           serialization for checking the state of the bundle results during the
 *                           unit test. Fixes #4797.
 */
int main(int argc, char *argv[]) {
  try {
    Preference::Preferences(true);

    cout.precision(6);

    qDebug() << "Unit test for BundleResults...";
    qDebug() << "XML from the default constructor...";
    QObject *parent = NULL;
    BundleResults results(parent);

    printXml(results);

    qDebug() << "Testing copy constructor...";
    BundleResults copyResults(results);
    printXml(copyResults);

    qDebug() << "Add maximum likelihood models, then test the assignment operator...";
    QList< QPair< MaximumLikelihoodWFunctions::Model, double > > modelsWithQuantiles;
    modelsWithQuantiles.append(qMakePair(MaximumLikelihoodWFunctions::Huber, 0.1));
    modelsWithQuantiles.append(qMakePair(MaximumLikelihoodWFunctions::Welsch, 0.2));
    modelsWithQuantiles.append(qMakePair(MaximumLikelihoodWFunctions::Chen, 0.3));
    results.maximumLikelihoodSetUp(modelsWithQuantiles);
    while (results.maximumLikelihoodModelIndex()
           <= results.numberMaximumLikelihoodModels()) {

      for (int i = 0; i < 101; i++) {
        results.addProbabilityDistributionObservation(((double)i) / 101.0);
        results.addResidualsProbabilityDistributionObservation(((double)i) / 101.0);
      }

      results.printMaximumLikelihoodTierInformation();
      results.incrementMaximumLikelihoodModelIndex();

    }
    qDebug() << "";

    qDebug() << "Testing assignment operator=...";
    {
      BundleResults &r = results;
      results = r;
    }
    printXml(results);

    BundleResults assignmentOpResults;
    assignmentOpResults = results;
    printXml(assignmentOpResults);

    qDebug() << "Testing mutator methods...";
    results.resizeSigmaStatisticsVectors(1);
    Statistics s1, s2, s3, s4, s5, s6;
    for (int i = 0; i < 5; i++) {
      s1.AddData(double(i));
      s2.AddData(double(i)/2);
      s3.AddData(double(2*i));
      s4.AddData(double(i+1));
      s5.AddData(double(1-i));
      s6.AddData(double(i-1));
    }
    QList<Statistics> rmsImageLineResiduals;
    rmsImageLineResiduals.push_back(s1);
    rmsImageLineResiduals.push_back(s2);
    QList<Statistics> rmsImageSampleResiduals;
    rmsImageSampleResiduals.push_back(s3);
    QList<Statistics> rmsImageResiduals;
    rmsImageResiduals.push_back(s4);
    rmsImageResiduals.push_back(s5);
    rmsImageResiduals.push_back(s6);
    results.setRmsImageResidualLists(rmsImageLineResiduals,
                                     rmsImageSampleResiduals,
                                     rmsImageResiduals);
    results.setSigmaCoord1Range(Distance(0.5, Distance::Meters),
                                  Distance(89.6, Distance::Meters),
                                  "MinLatId", "MaxLatId");
    results.setSigmaCoord2Range(Distance(0.7, Distance::Meters),
                                  Distance(179.2, Distance::Meters),
                                  "MinLonId", "MaxLonId");
    results.setSigmaCoord3Range(Distance(0.9, Distance::Meters),
                                Distance(354.4, Distance::Meters),
                                "MinRadId", "MaxRadId");

    results.setRmsFromSigmaStatistics(0.123, 0.456, 0.789);
    results.setRmsXYResiduals(4.0, 5.0, 6.0);
    results.setRejectionLimit(7.0);
    results.setNumberRejectedObservations(8);
    results.setNumberObservations(9);
    results.setNumberImageParameters(10);
    results.resetNumberConstrainedPointParameters();
    results.incrementNumberConstrainedPointParameters(11);
    results.resetNumberConstrainedImageParameters();
    results.incrementNumberConstrainedImageParameters(10);
    results.resetNumberConstrainedTargetParameters();
    results.incrementNumberConstrainedTargetParameters(2);
    results.setNumberUnknownParameters(13);
    results.setDegreesOfFreedom(14.0);
    results.setSigma0(15.0);
    results.setElapsedTime(16.0);
    results.setElapsedTimeErrorProp(17.0);
    results.setConverged(true); // or initialze method
    results.incrementFixedPoints();
    results.incrementHeldImages();
    results.incrementIgnoredPoints();
    results.setIterations(6);
    printXml(results);
    qDebug() << "";

    qDebug() << "Testing more computation methods...";
    results.computeSigma0(28.0, BundleSettings::Sigma0);
    // 28 / 14
    qDebug() << "sigma0 = " << toString(results.sigma0());
    results.setNumberUnknownParameters(32); // so that df=0.0
    results.computeSigma0(0.0, BundleSettings::ParameterCorrections);
    qDebug() << "sigma0 = " << toString(results.sigma0());
    results.setCorrMatCovFileName(FileName("covariance.dat"));
    QMap<QString, QStringList> imgsAndParams;
    QStringList list1, list2;
    list1 << "param1" << "param2";
    list2 << "param3" << "param4" << "param5";
    imgsAndParams.insert("img1", list1);
    imgsAndParams.insert("img2", list2);
    results.setCorrMatImgsAndParams(imgsAndParams);
    copyResults = results;
    qDebug() << "";

    qDebug() << "Testing storage for output methods...";
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
    ControlPoint *fixedPoint = new ControlPoint("FixedPoint");
    fixedPoint->SetType(ControlPoint::Fixed);
    SurfacePoint fixedSurfacePoint(Latitude(90.0, Angle::Degrees),
                                   Longitude(180.0, Angle::Degrees),
                                   Distance(10.0, Distance::Meters));
    fixedPoint->SetAdjustedSurfacePoint(fixedSurfacePoint);
    ControlNet outNet;
    outNet.AddPoint(freePoint);
    outNet.AddPoint(fixedPoint);
    BundleSettingsQsp settings = BundleSettingsQsp(new BundleSettings());
    BundleControlPointQsp freeBundleControlPoint(
                                     new BundleControlPoint(settings, freePoint));
    BundleControlPointQsp fixedBundleControlPoint(
                                     new BundleControlPoint(settings, fixedPoint));
    QVector<BundleControlPointQsp> bundleControlPointVector;
    bundleControlPointVector.append(freeBundleControlPoint);
    bundleControlPointVector.append(fixedBundleControlPoint);
    Camera *camera = NULL;
    BundleImage bundleImage(camera,
                            "TestImageSerialNumber",
                            "TestImageFileName");
    BundleObservationVector observationVector;
    observationVector.addNew(BundleImageQsp(new BundleImage(bundleImage)),
                             "ObservationNumber1",
                             "InstrumentId1",
                             BundleSettingsQsp(new BundleSettings()));

    results.setBundleControlPoints(bundleControlPointVector);
    results.setOutputControlNet(ControlNetQsp(new ControlNet(outNet)));
    results.setObservations(observationVector);
    qDebug() << "";

    qDebug() << "Testing accessor methods...";
    qDebug() << "maximum likelihood index = " << toString(results.maximumLikelihoodModelIndex());
    qDebug() << "maximum likelihood median r2 residuals = "
             << toString(results.maximumLikelihoodMedianR2Residuals());

    for (int i = 0; i < results.numberMaximumLikelihoodModels(); i++) {
      qDebug() << "maximum likelihood index,model,quantile = [" << toString(i)
               << ", " << MaximumLikelihoodWFunctions::modelToString(
                              results.maximumLikelihoodModelWFunc(i).model())
               << ", " << toString(results.maximumLikelihoodModelQuantile(i))
               << "]";
  //??       QList< QPair< MaximumLikelihoodWFunctions, double > > maximumLikelihoodModels() const;
    }

    qDebug() << "bundle control points...";
    QVector<BundleControlPointQsp> accessedControlPoints = results.bundleControlPoints();
    for (int i = 0; i < accessedControlPoints.size(); i++) {
      qDebug().noquote() << accessedControlPoints[i]->formatBundleOutputSummaryString(false);
    }

    qDebug() << "output control network";
    ControlNetQsp accessedControlNet = results.outputControlNet();
    qDebug() << accessedControlNet->GetNumMeasures();
    qDebug() << accessedControlNet->GetNumPoints();

    qDebug() << "bundle observations";
    BundleObservationVector accessedBundleObservations = results.observations();
    for (int i = 0; i < accessedBundleObservations.size(); i++) {
      qDebug() << accessedBundleObservations[i]->instrumentId();
      qDebug() << accessedBundleObservations[i]->imageNames();
    }

    qDebug() << "";

    qDebug() << "Testing XML serialization 1: round trip serialization of fully populated BundleSettings object...";
    qDebug() << "Serializing test XML object to file...";
    printXml(results);
    FileName xmlFile("./BundleResults.xml");
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
    results.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();

    qDebug() << "Testing XML: reading serialized BundleResults back in...";
    XmlStackedHandlerReader reader;
    BundleResultsXmlHandlerTester bsFromXml(project, &reader, xmlFile);
    qDebug() << "Testing XML: Object deserialized as (should match object above):";
    printXml(bsFromXml);


    qDebug() << "Testing rectangular coordinate type in control net and settings";
    settings->setSolveOptions(false, false, false, false, SurfacePoint::Rectangular,
                              SurfacePoint::Rectangular);
    // outNetRect.SetCoordType(SurfacePoint::Rectangular);
    outNet.SetCoordType(SurfacePoint::Rectangular);
    BundleControlPointQsp freeRBundleControlPoint(
                                     new BundleControlPoint(settings, freePoint));
    BundleControlPointQsp fixedRBundleControlPoint(
                                     new BundleControlPoint(settings, fixedPoint));
    QVector<BundleControlPointQsp> bundleControlPointVectorR;
    bundleControlPointVectorR.append(freeRBundleControlPoint);
    bundleControlPointVectorR.append(fixedRBundleControlPoint);
    copyResults.setBundleControlPoints(bundleControlPointVectorR);
    copyResults.setOutputControlNet(ControlNetQsp(new ControlNet(outNet)));
    copyResults.setObservations(observationVector);
    qDebug() << "";

    qDebug() << "bundle control points...";
    accessedControlPoints = copyResults.bundleControlPoints();
    for (int i = 0; i < accessedControlPoints.size(); i++) {
      qDebug().noquote() << accessedControlPoints[i]->formatBundleOutputSummaryString(false);
    }

    qDebug() << "";

    qDebug() << "Testing XML serialization for a rectangular net 1: round trip serialization of fully populated BundleSettings object...";
    qDebug() << "Serializing test XML object to file...";
    printXml(copyResults);
    FileName xmlFileR("./BundleResultsR.xml");
    xmlPath = xmlFileR.expanded();
    QFile qXmlFileR(xmlPath);
    if (!qXmlFileR.open(QIODevice::WriteOnly|QIODevice::Text)) {
      throw IException(IException::Io,
                       QString("Unable to open xml file, [%1],  with write access").arg(xmlPath),
                       _FILEINFO_);
    }
    QXmlStreamWriter writerR(&qXmlFileR);
    writerR.setAutoFormatting(true);
    writerR.writeStartDocument();
    project = NULL;
    copyResults.save(writerR, project);
    writerR.writeEndDocument();
    qXmlFileR.close();

    qDebug() << "Testing rectangular XML: reading serialized BundleResults back in...";
    BundleResultsXmlHandlerTester bsRectFromXml(project, &reader, xmlFileR);
    // Set the output control net in bsRectFromXml in order to get the desired coordinate type
    bsRectFromXml.setOutputControlNet(ControlNetQsp(new ControlNet(outNet)));
    qDebug() << "Testing rectangular XML: Object deserialized as (should match object above):";
    printXml(bsRectFromXml);


    qDebug() << "Testing error throws...";
    try {
      results.setNumberObservations(0);
      results.resetNumberConstrainedPointParameters();
      results.resetNumberConstrainedImageParameters();
      results.resetNumberConstrainedTargetParameters();
      results.setNumberUnknownParameters(1);
      results.computeSigma0(1.0, BundleSettings::Sigma0);
    }
    catch (IException &e) {
      e.print();
    }
    try {
      results.setNumberObservations(1);
      results.resetNumberConstrainedPointParameters();
      results.resetNumberConstrainedImageParameters();
      results.resetNumberConstrainedTargetParameters();
      results.setNumberUnknownParameters(1);
      results.computeSigma0(1.0, BundleSettings::Sigma0);
    }
    catch (IException &e) {
      e.print();
    }
    try {
      BundleResults defaultResults;
      defaultResults.outputControlNet();
    }
    catch (IException &e) {
      e.print();
    }
    try {
      outNet.SetCoordType(SurfacePoint::CoordinateType (-1));
      results.setOutputControlNet(ControlNetQsp(new ControlNet(outNet)));
      printXml(results);
    }
    catch (IException &e) {
      e.print();
    }

    bool deleted = qXmlFile.remove();
    if (!deleted) {
      QString msg = "Unit Test failed. XML file [" + xmlPath + "not deleted.";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }
  catch (IException &e) {
    e.print();
  }
}


/**
 * Prints the serialized BundleResults as XML.
 *
 * @param const BundleResults &printable The bundle results to print.
 */
void printXml(const BundleResults &printable) {
  QString output;
  QXmlStreamWriter writer(&output);
  writer.setAutoFormatting(true);
  printable.save(writer, NULL);
  output.remove(QRegExp("<id>[^<]*</id>"));
  // Note Statistics class does not serialize/restore properly as of 2017-04-27
  output.remove(QRegExp("<statistics>.*</statistics>"));
  qDebug().noquote() << output << endl << endl;
}
