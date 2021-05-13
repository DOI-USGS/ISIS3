#ifndef IsisXMLHandler_h
#define IsisXMLHandler_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <xercesc/sax2/DefaultHandler.hpp>

namespace XERCES = XERCES_CPP_NAMESPACE;
/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLHandler : public XERCES::DefaultHandler {
  using XERCES::DefaultHandler::ignorableWhitespace;
  
  public:

    ContentHandler *prevDocHandler;
    ErrorHandler *prevErrorHandler;

    IsisXMLHandler();

    IsisXMLHandler(char *PencodingName,
                   bool &PexpandNamespaces,
                   XERCES::SAX2XMLReader* &Pparser,
                   QString *chars);

    IsisXMLHandler(char *PencodingName,
                   bool &PexpandNamespaces,
                   XERCES::SAX2XMLReader* &Pparser);

    ~IsisXMLHandler();

    void endDocument();

    void endElement(const XMLCh *const uri,
                    const XMLCh *const localname,
                    const XMLCh *const qname);

    void characters(const XMLCh *const chars,
                    const XMLSize_t length);

    void ignorableWhitespace(const XMLCh *const chars,
                             const unsigned int length);

    void processingInstruction(const XMLCh *const target,
                               const XMLCh *const data);

    void startDocument();

    void startElement(const XMLCh *const uri,
                      const XMLCh *const localname,
                      const XMLCh *const qname,
                      const XERCES::Attributes  &attributes);


    //  SAX ErrorHandler interface
    void warning(const XERCES::SAXParseException &exception);
    void error(const XERCES::SAXParseException &exception);
    void fatalError(const XERCES::SAXParseException &exception);


  private:

    // Saved argument from the constructor
    char *encodingName;
    bool expandNamespaces;
    XERCES::SAX2XMLReader *parser;
    QString *value;
    int outputEndTag;
};


#endif
