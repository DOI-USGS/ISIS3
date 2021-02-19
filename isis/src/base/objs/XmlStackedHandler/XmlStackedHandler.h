#ifndef XmlStackedHandler_H
#define XmlStackedHandler_H
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QXmlSimpleReader>

template <typename T> class QStack;

class QXmlParseException;
namespace Isis {
  class XmlStackedHandlerReader;

  /**
   * @brief XML Handler that parses XMLs in a stack-oriented way
   *
   * IPCE does not have a single XML file for the whole project
   * but breaks the project into multiple XML files with the Project.xml file
   * being the top level.  @see Project
   *
   * **Serialization Basics**
   *
   * IPCE mostly follows the standard convention of each object being responsible for
   * serializing itself.  Each Object to be serialized must have a "save" method to
   * write the object out as XML. The first thing the save method does is write an XML element
   * indicating what type of object is being serialized. This allows the deserialization to know
   * how to parse the data (in our case what type of XmlHandler to push).  If the object being
   * serialized contains another object the save method of the contained object is called.  This
   * results in a XML file hierarchy as shown below.
   *
   * For deserialization each serialized object implements an XmlHandler class.  The
   * XmlHandler::startElement method handles reading of the XML file and initializing the
   * member variables for the object. The class must also define a constructor that takes a
   * XmlStackedHandlerReader as a parameter (Note the IPCE signatures vary on this method).
   * The constructor pushes it's own content handler (the XmlHandler class) on the reader
   * to allow parsing to continue with this object. Note the push of the content handler
   * does not return until the XML is parsed, specifically the push of the content handler
   * calls XmlHandler::startElement() for the handler just pushed. (Actually the behavior
   * of XmlStackedHandlerReader::pushContentHandler() varies - if there are no contentHandlers
   * on the content handler stack when it is called it returns immediately and parse()
   * must be called to start parsing.  If there is a content handler already on the stack
   * the push results in a call to startElement() and it does not return until the
   * corresponding end element.)
   *
   * If a contained object
   * is found while parsing the XML, the constructor for the contained object that takes a
   * XmlStackedHandlerReader as a parameter is called.  This will result in the contained
   * object pushing it's content handler and parsing the relevant XML. When the constructor
   * returns, XML parsing can continue for this object.
   *
   * *Potential issue*
   * There appears to be no support for cycles or joins in the object graph when serializing
   * or deserializing.  This means that if multiple pointers point to the same object ensure
   * the object is only serialized once and that all pointers are properly restored on
   * deserialization.  Currently many of the ISIS objects have unique IDs and IPCE encapsulates
   * the underlying ISIS object.  One option would be to use the ISIS ids to uniquely identify
   * the objects during serialization.
   *
   * *Versioning*
   * To ensure backwards compatibility versioning is done per object. This keeps version
   * information for a class within a single source file with no need to know Project file
   * structure and where Project level file information is saved.  The version number
   * for a class should be incremented by 1 each time the XML for that object changes.
   * When reading old version XML, files the class should choose a sensible default for the
   * missing XML elements and write out the XML in the newest format.
   *
   * This XML handler is designed to work with the XmlStackedHandlerReader. This XML handler class
   *   handles passing off parsing to another handler. For example, if your XML is:
   *
   *   <pre>
   *     <xmlTag1>
   *        <various xml elements associated with xmlTag1>
   *        <xmlTag2>
   *             <various xml elements associated with xmlTag2>
   *             <xmlTag3 />
   *                <various xml elements associated with xmlTag3>
   *             </xmlTag3>
   *             <more xml elements associated with xmlTag2>
   *        </xmlTag2>
   *        <more xml elements associated with xmlTag1>
   *     </xmlTag>
   *   </pre>
   *
   *   To start the processing of the XML, an initial XML content handler is pushed onto the
   *   stack of content handlers.  In the example above this initial content handler only
   *   processes elements associated with xmlTag1.  In IPCE the xmlTag1 elements will be the
   *   member variables associated with a class.  The xmlTag2 contains XML for an object
   *   contained within the first class.  When the xmlTag2 element is encountered the
   *   XML content handler (the xmlTag2::XmlHandler class) will be pushed and take over
   *   parsing.
   *
   * If this handler is pushed onto the reader (which is the parser stack) when the startElement
   *   of xmlTag2 is seen, then this XML handler will see all of the xml data up to and including
   *   the xmlTag2 close tag. This handler would never see xmlTag1. Here is an example of how
   *   this works:
   *
   * <pre>
   *   --> Push initial XML content handler for xmlTag1 (Handler1)
   *   <xmlTag1> -- Handler1::startElement
   *     <xmlTag2> -- Handler1::startElement: calls reader()->pushContentHandler(HandlerForXmlTag2)
   *               -- HandlerForXmlTag2::startElement
   *       <xmlTag3 /> -- HandlerForXmlTag3::startElement
   *                   -- HandlerForXmlTag3::endElement
   *     </xmlTag2> -- HandlerForXmlTag2::endElement
   *   </xmlTag1> -- Handler1::endElement
   * </pre>
   *
   *
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
      bool fatalError(const QXmlParseException &exception);

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
