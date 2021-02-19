#ifndef XmlStackedHandlerReader_H
#define XmlStackedHandlerReader_H
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QXmlSimpleReader>

template <typename T> class QStack;

namespace Isis {
  class XmlStackedHandler;

  /**
   *
   * @brief Manage a stack of content handlers for reading XML files.
   *
   *   This class is designed to work with the XmlStackedHandler class. Use this
   *   in-place of a QXmlSimpleReader if you want to use stack-based Xml parsing. The
   *   XmlStackedHandler class has an explanation as to how this is designed to work.
   *
   * @see XmlStackedHandler
   *
   * @author 2012-??-?? Steven Lambright
   *
   * @internal
   */
  class XmlStackedHandlerReader : public QXmlSimpleReader {
    public:
      XmlStackedHandlerReader();
      ~XmlStackedHandlerReader();

      virtual void popContentHandler();
      virtual void pushContentHandler(XmlStackedHandler *newHandler);
      XmlStackedHandler *topContentHandler();

    private:
      Q_DISABLE_COPY(XmlStackedHandlerReader);

      QStack<XmlStackedHandler *> *m_contentHandlers;
  };
}

#endif
