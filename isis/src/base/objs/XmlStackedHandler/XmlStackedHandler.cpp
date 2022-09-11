/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "XmlStackedHandler.h"

#include <QDebug>
#include <QStack>
#include <QXmlParseException>

#include "XmlStackedHandlerReader.h"

namespace Isis {
  XmlStackedHandler::XmlStackedHandler() {
    m_reader = NULL;
    m_depth = 0;
  }


  XmlStackedHandler::~XmlStackedHandler() {
    m_reader = NULL;
    m_depth = 0;
  }


  void XmlStackedHandler::setReader(XmlStackedHandlerReader *reader) {
    m_reader = reader;
  }


  /**
   * @brief Switch to a new content handler and continue processing using the new handler.
   *
   */
  void XmlStackedHandler::switchToNewHandler(XmlStackedHandler *nextHandler) {
    nextHandler->startElement(m_lastStartNamespaceURI, m_lastStartLocalName,
                              m_lastStartQName, m_lastStartAtts);
  }


  bool XmlStackedHandler::startElement(const QString &namespaceURI, const QString &localName,
                                       const QString &qName, const QXmlAttributes &atts) {
    m_lastStartNamespaceURI = namespaceURI;
    m_lastStartLocalName = localName;
    m_lastStartQName = qName;
    m_lastStartAtts = atts;
    m_depth++;

    return true;
  }

  bool XmlStackedHandler::endElement(const QString &namespaceURI, const QString &localName,
                          const QString &qName) {
    m_depth--;

    if (m_depth == 0 && reader()) {
      reader()->popContentHandler();

      if (reader()->topContentHandler())
        reader()->topContentHandler()->endElement(namespaceURI, localName, qName);
    }

    return true;
  }

  XmlStackedHandlerReader *XmlStackedHandler::reader() {
    return m_reader;
  }


  const XmlStackedHandlerReader *XmlStackedHandler::reader() const {
    return m_reader;
  }


  bool XmlStackedHandler::fatalError(const QXmlParseException &exception) {
    qDebug() << "Parse error at line " << exception.lineNumber()
             << ", " << "column " << exception.columnNumber() << ": "
             << qPrintable(exception.message());
    return false;
  }

}
