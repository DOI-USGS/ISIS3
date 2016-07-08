#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QtDebug>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include "BundleResults.h"
#include "BundleSettings.h"
#include "BundleSolutionInfo.h"
#include "Directory.h"
#include "FileName.h"
#include "IException.h"
#include "ImageList.h"
#include "Preference.h"
#include "Project.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


using namespace std;
using namespace Isis;


//TODO The Unknown H5Error exception in openH5File is untested
//TODO XmlHandler and its methods are untested (would require use of a Project, which requires
//     a non-Null Directory reference) 
//TODO updateFileName slot untested (see above explanation)
//TODO Constructor with 3 parameters untested
namespace Isis {
 /**
  * This class is needed to test the xml read/write methods.
  * @author 2015-??-?? Jeannie Backer
  *
  * @internal
  *   @history 2015-??-?? Jeannie Backer - Original version.
  */ 
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
 *   @history 2016-07-08 Ian Humphrey - Updated testing for some exceptions (H5 open/create...).
 *                           Fixes #3974.
 */
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);

  try {
    qDebug() << "Unit test for BundleSolutionInfo...";
    qDebug() << "Printing PVL group with results from the settings/cnet/statistics constructor...";

    // create default settings and statistics objects to pass into results object
    BundleSettingsQsp settings = BundleSettingsQsp(new BundleSettings);
    FileName cnetFile("cnetfile.net");
    BundleResults statistics;
    QObject *parent = NULL;
    BundleSolutionInfo results(settings, cnetFile, statistics, parent);

    PvlObject pvl = results.pvlObject("DefaultSolutionInfoObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing copy constructor...";

    BundleSolutionInfo copySolutionInfo(results);
    pvl = copySolutionInfo.pvlObject("CopySolutionInfoObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing assignment operator to set this equal to itself...";
    results = results;
    pvl = results.pvlObject("SelfAssignedSolutionInfoObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing assignment operator to create a new results object...";

    BundleSolutionInfo assignmentOpSolutionInfo = results;
    assignmentOpSolutionInfo = results;
    pvl = assignmentOpSolutionInfo.pvlObject("AssignedSolutionInfoObject");
    cout << pvl << endl << endl;

    qDebug() << "Testing mutator methods...";
    statistics.setRejectionLimit(0.5);
    results.setOutputStatistics(statistics);
    results.setRunTime("xxx"); //???
    pvl = results.pvlObject("MutatorTest");
    cout << pvl << endl << endl;

    qDebug() << "Testing accessor methods...";
    // Can't print this value out since it changes for every run,
    // qDebug() << "quuid = ";
    // but we will call the method for test coverage
    results.id();
    qDebug() << "runTime = " << results.runTime();
    qDebug() << "";

    // controlNetworkFileName() is tested by pvlObject() (if BundleSolutionInfo constructed with
    //     non-empty FileName parameter)

    // bundleSettings() and bundleResults() tested by pvlObject()

    qDebug() << "Because we cannot create a Directory with a null parent, ";
    qDebug() << "we cannot test updateFileName().";
    // updateFileName() requires a Project* as a param, Project requires Directory& in constructor
    qDebug() << "";


    qDebug() << "Testing error throws...";
    try {
      // bundleResults exception cannot be tested because the BundleResults cannot be NULL
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    try {
      // openH5File, file does not exist
      results.openH5File(FileName("./dne.hdf"));
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    // create a temporary h5 file for testing open/createH5 exceptions
    FileName tempH5("$temporary/tempBundleSolutionInfo.hd");
    QFile tempH5file(tempH5.expanded()); 
    tempH5file.open(QIODevice::WriteOnly);
    tempH5file.close();
    
    try {
      // openH5File, file extension is not hdf
      results.openH5File(tempH5);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    try {
      // createH5File, file already exists
      // We'll rename it the .hd to a valid .hdf extended file, then try to create it
      tempH5file.rename(tempH5.baseName() + ".hdf");
      results.createH5File(FileName(tempH5file.fileName()));
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    // Cleanup temporary file
    QFile::remove(tempH5file.fileName());


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


    qDebug() << "Testing XML write/read...";
    // write xml
#if 0
    FileName xmlFile("./BundleSolutionInfo.xml");
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
    // read xml
    XmlStackedHandlerReader reader;
// ???     BundleSolutionInfoXmlHandlerTester brToFill(project, &reader, xmlFile);
// ???     pvl = bsToFill.pvlObject("BundleSolutionInfoFromXml");
// ???     cout << pvl << endl << endl;
#endif
    qDebug() << "";

    qDebug() << "Testing HDF5 write/read...";
    // write hdf
    FileName hdfFile("./BundleSolutionInfo.hdf");
    if (hdfFile.fileExists()) {
       QFile::remove(hdfFile.expanded());
    }
    results.createH5File(hdfFile);
    BundleSolutionInfo fromHDF(hdfFile);
    pvl = fromHDF.pvlObject("BundleSolutionInfoFromHDF");
    cout << pvl << endl << endl;
//    QFile::remove(hdfFile.expanded());

  }
  catch (IException &e) {
    e.print();
  }
}
