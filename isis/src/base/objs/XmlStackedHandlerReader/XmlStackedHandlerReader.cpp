/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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


  /**
   * @brief Push a contentHandler and maybe continue parsing...
   *
   *   Push a contentHadler on the content handler stack.  If there are currently
   *   no other handlers on the stack that is all that happens.
   *
   *   If there are other content handlers on the stack it is assumed that
   *   a XML file is being processed and processing continues by calling
   *   startElement() of the newly pushed handler.  In this case
   *   pushContentHandler() will not return until the element has been
   *   fully processed.
   *
   *   @see XmlStackedHandler
   *
   */
  void XmlStackedHandlerReader::pushContentHandler(XmlStackedHandler *newHandler) {
    XmlStackedHandler *old = topContentHandler();

    newHandler->setReader(this);
    m_contentHandlers->push(newHandler);

    setContentHandler(m_contentHandlers->top());

    if (old) {
      // Switch to newHandler and continue parsing
      // This will call newHandler->startElement(...)
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

