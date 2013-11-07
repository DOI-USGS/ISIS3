#ifndef XmlStackedHandler_H
#define XmlStackedHandler_H

#include <QXmlSimpleReader>

template <typename T> class QStack;

namespace Isis {
  class XmlStackedHandlerReader;

  /**
   * @brief XML Handler that parses XMLs in a stack-oriented way
   *
   * This XML handler is designed to work with the XmlStackedHandlerReader. This XML handler class
   *   handles passing off parsing to another handler. For example, if your XML is:
   *
   *   <pre>
   *     <xmlTag1>
   *       <xmlTag2>
   *         <xmlTag3 />
   *       </xmlTag2>
   *     </xmlTag>
   *   </pre>
   *
   * If this handler is pushed onto the reader (which is the parser stack) when the startElement
   *   of xmlTag2 is seen, then this XML handler will see all of the xml data up to and including
   *   the xmlTag2 close tag. This handler would never see xmlTag1. Here is an example of how
   *   this works:
   *
   * <pre>
   *   --> Push initial XML handler (Handler1)
   *   <xmlTag1> -- Handler1::startElement
   *     <xmlTag2> -- Handler1::startElement: calls reader()->pushContentHandler(Handler2)
   *               -- Handler2::startElement
   *       <xmlTag3 /> -- Handler2::startElement
   *                   -- Handler2::endElement
   *     </xmlTag2> -- Handler2::endElement
   *                -- Handler1::endElement
   *   </xmlTag1> -- Handler1::endElement
   * </pre>
   *
   * @author 2012-??-?? Steven Lambright
   *
   * @internal
   */
  class XmlStackedHandler : public QXmlDefaultHandler {
    public:
      XmlStackedHandler();
      ~XmlStackedHandler();

      virtual void setReader(XmlStackedHandlerReader *);
      void switchToNewHandler(XmlStackedHandler *nextHandler);

      virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                const QString &qName, const QXmlAttributes &atts);

      virtual bool endElement(const QString &namespaceURI, const QString &localName,
                              const QString &qName);

    protected:
      XmlStackedHandlerReader *reader();
      const XmlStackedHandlerReader *reader() const;

    private:
      Q_DISABLE_COPY(XmlStackedHandler);

      XmlStackedHandlerReader *m_reader;
      int m_depth;

      QString m_lastStartNamespaceURI;
      QString m_lastStartLocalName;
      QString m_lastStartQName;
      QXmlAttributes m_lastStartAtts;
  };
}

#endif
