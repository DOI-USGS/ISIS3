/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include <QObject>

#include "IException.h"
#include "IString.h"
#include "XmlStackedHandler.h"
#include "XmlStackedHandlerReader.h"

using namespace Isis;
using namespace std;

namespace Isis {
  class XmlHandlerTester : public XmlStackedHandler {
    public:
      XmlHandlerTester(QString name) {
        m_name = name;
      }

      ~XmlHandlerTester() {
      }


      bool startElement(const QString &namespaceURI, const QString &localName,
                        const QString &qName, const QXmlAttributes &atts) {
        cerr << m_name.toStdString() << ": Start Element ["
             << localName.toStdString() << "]" << endl;

        if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {

          for (int attIndex = 0; attIndex < atts.count(); attIndex++) {
            cerr << "\t" << atts.localName(attIndex).toStdString()
                 << " = " << atts.value(attIndex).toStdString() << endl;
          }

          if (localName != m_name) {
            reader()->pushContentHandler(new XmlHandlerTester(localName));
          }
        }

        return true;
      }

      bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName) {
        cerr << m_name.toStdString() << ": End Element ["
             << localName.toStdString() << "]" << endl;

        return XmlStackedHandler::endElement(namespaceURI, localName, qName);
      }

    private:
      QString m_name;
  };
}

// XmlStackedHandlerReader's unit test is relying on this test to adequetly test its functionality.
int main(int argc, char **argv) {
  XmlHandlerTester handler("project");

  XmlStackedHandlerReader reader;
  reader.pushContentHandler(&handler);
  reader.setErrorHandler(&handler);

  QString xmlPath("./testFile.xml");
  QFile file(xmlPath);

  if (!file.open(QFile::ReadOnly)) {
    throw IException(IException::Unknown,
                     QString("Unable to open [%1] with read access")
                       .arg(xmlPath),
                     _FILEINFO_);
  }

  QXmlInputSource xmlInputSource(&file);
  if (!reader.parse(xmlInputSource))
    cerr << QString("Failed to read [%1]").arg(xmlPath).toStdString();

  return 0;
}
