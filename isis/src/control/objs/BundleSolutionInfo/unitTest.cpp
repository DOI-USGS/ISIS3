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
#include "FileName.h"
#include "IException.h"
#include "ImageList.h"
#include "Preference.h"
#include "PvlObject.h"
#include "XmlStackedHandlerReader.h"


using namespace std;
using namespace Isis;

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


int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(6);

  try {
    qDebug() << "Unit test for BundleSolutionInfo...";
    qDebug() << "Printing PVL group with results from the settings/cnet/statistics constructor...";
    // create default settings and statistics objects to pass into results object
    BundleSettings settings;
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
    results.setRunTime("xxx");//???
    pvl = results.pvlObject("MutatorTest");
    cout << pvl << endl << endl;

    qDebug() << "Testing accessor methods...";
    // Can't print this value out since it changes for every run, 
    // qDebug() << "quuid = ";
    // but we will call the method for test coverage
    results.id();
    qDebug() << "runTime = " << results.runTime();
    qDebug();

    qDebug() << "Testing error throws...";
    try {
    } 
    catch (IException &e) {
      e.print();
    }
    qDebug();

    qDebug() << "Testing XML write/read...";
    // write xml 
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

  } 
  catch (IException &e) {
    e.print();
  }
}
