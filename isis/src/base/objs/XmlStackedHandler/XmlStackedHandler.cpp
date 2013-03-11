#include "IsisDebug.h"
#include "XmlStackedHandler.h"

#include <QDebug>
#include <QStack>

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

    ASSERT(m_depth >= 0);
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
}
