/**
 * Unit Test for OriginalXmlLabel class.
 */


#include <iostream>
#include <fstream>

#include <QDomDocument>
#include <QFile>

#include "Blob.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "OriginalXmlLabel.h"
#include "Preference.h"
#include "Pvl.h"

using namespace Isis;
using namespace std;

QDomDocument parseXmlFile(const FileName &xmlFileName);

int main(int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  OriginalXmlLabel testLabel;
  FileName labelFileName("$base/testData/xmlTestLabel.xml");

  cout << "Ingesting label file into check object." << endl << endl;
  QDomDocument checkDoc = parseXmlFile(labelFileName);

  cout << "Ingesting label file into OriginalXmlLabel object:" << endl << endl;
  testLabel.readFromXmlFile(labelFileName);

  cout << "Testing accessor:" << endl;
  cout << "  Xml is the same?: ";
  if ( testLabel.ReturnLabels().toString() == checkDoc.toString() ) {
    cout << "Yes." << endl << endl;
  }
  else {
    cout << "No." << endl << endl;
  }
}

/**
 * Read the original label from an Xml file.
 * 
 * @param FileName The Xml file containing the original label.
 * 
 * @throws IException::Io "Could not open label file."
 * @throws IException::Unknown "XML read/parse error in file."
 */
QDomDocument parseXmlFile(const FileName &xmlFileName) {
  QDomDocument parsedXmlFile;

  QFile xmlFile(xmlFileName.expanded());
  if ( !xmlFile.open(QIODevice::ReadOnly) ) {
    QString msg = "Could not open label file [" + xmlFileName.expanded() +
                  "].";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  QString errmsg;
  int errline, errcol;
  if ( !parsedXmlFile.setContent(&xmlFile, false, &errmsg, &errline, &errcol) ) {
    xmlFile.close();
    QString msg = "XML read/parse error in file [" + xmlFileName.expanded()
        + "] at line [" + toString(errline) + "], column [" + toString(errcol)
        + "], message: " + errmsg;
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  xmlFile.close();

  return parsedXmlFile;
}
