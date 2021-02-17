#ifndef IsisXMLHistory_h
#define IsisXMLHistory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IsisAmlData.h"
#include "IsisXMLHandler.h"
#include "IsisXMLIgnore.h"
/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLHistory : public IsisXMLHandler {

  public:

    ContentHandler *prevDocHandler;
    ErrorHandler *prevErrorHandler;


    IsisXMLHistory(char *PencodingName,
                   bool &PexpandNamespaces,
                   XERCES::SAX2XMLReader* &Pparser,
                   std::vector<IsisChangeData> *PChanges);

    ~IsisXMLHistory();

//  void endDocument();

    void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname);

    void characters(const XMLCh *const chars, const XMLSize_t length);


    void startElement(const XMLCh *const uri,
                      const XMLCh *const localname,
                      const XMLCh *const qname,
                      const XERCES::Attributes  &attributes);


  private:

    // Saved arguments from constructor
    char *encodingName;
    bool expandNamespaces;
    XERCES::SAX2XMLReader *parser;
    std::vector<IsisChangeData> *changes;

    // Handlers this handler knows how to create
    IsisXMLHandler *generalHandler;
    IsisXMLIgnore *ignoreHandler;

};


#endif





