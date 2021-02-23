#ifndef IsisXMLGroup_h
#define IsisXMLGroup_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
//#include <sax2/DefaultHandler.hpp>

#include "IsisAmlData.h"
#include "IsisXMLParameter.h"
#include "IsisXMLIgnore.h"
/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLGroup : public IsisXMLHandler {

  public:

    ContentHandler *prevDocHandler;
    ErrorHandler *prevErrorHandler;


    IsisXMLGroup(char *PencodingName,
                 bool &PexpandNamespaces,
                 XERCES::SAX2XMLReader* &Pparser,
                 IsisGroupData *Group);

    ~IsisXMLGroup();

    void endElement(const XMLCh *const uri,
                    const XMLCh *const localname,
                    const XMLCh *const qname);

    void characters(const XMLCh *const chars,
                    const XMLSize_t length);

    void startElement(const XMLCh *const uri,
                      const XMLCh *const localname,
                      const XMLCh *const qname,
                      const XERCES::Attributes  &attributes);


  private:

    // Saved argument from the constructor
    char *encodingName;
    bool expandNamespaces;
    XERCES::SAX2XMLReader *parser;
    IsisGroupData *group;

    // Handlers this handler knows how to create
    IsisXMLParameter *parameterHandler;
    IsisXMLIgnore *ignoreHandler;
};


#endif



