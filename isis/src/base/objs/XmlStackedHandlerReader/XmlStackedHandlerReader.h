#ifndef XmlStackedHandlerReader_H
#define XmlStackedHandlerReader_H

#include <QXmlSimpleReader>

template <typename T> class QStack;

namespace Isis {
  class XmlStackedHandler;

  /**
   * his enables stack-based XML parsing of XML files. This class is designed to work with the
   *   XmlStackedHandler class. Use this in-place of a QXmlSimpleReader if you want to use
   *   stack-based Xml parsing. The XmlStackedHandler class has an explanation as to how this
   *   is designed to work.
   *
   * The naming of this class is:
   *   XmlStackedHandler + Reader (parent class).
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
