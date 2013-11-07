#include "XmlStackedHandlerReader.h"

#include <QDebug>
#include <QStack>

#include "XmlStackedHandler.h"

namespace Isis {
  XmlStackedHandlerReader::XmlStackedHandlerReader() {
    m_contentHandlers = NULL;
    m_contentHandlers = new QStack<XmlStackedHandler *>;
  }


  XmlStackedHandlerReader::~XmlStackedHandlerReader() {
    delete m_contentHandlers;
    m_contentHandlers = NULL;
  }


  void XmlStackedHandlerReader::popContentHandler() {
    m_contentHandlers->pop();

    if (m_contentHandlers->size()) {
      m_contentHandlers->top()->setReader(this);
      setContentHandler(m_contentHandlers->top());
    }
    else {
      setContentHandler(NULL);
    }
  }


  void XmlStackedHandlerReader::pushContentHandler(XmlStackedHandler *newHandler) {
    XmlStackedHandler *old = topContentHandler();

    newHandler->setReader(this);
    m_contentHandlers->push(newHandler);

    setContentHandler(m_contentHandlers->top());

    if (old) {
      old->switchToNewHandler(topContentHandler());
    }
  }


  XmlStackedHandler *XmlStackedHandlerReader::topContentHandler() {
    XmlStackedHandler *result = NULL;

    if (m_contentHandlers->size())
      result = m_contentHandlers->top();

    return result;
  }
}

