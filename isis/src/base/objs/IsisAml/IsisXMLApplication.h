#ifndef IsisXMLApplication_h
#define IsisXMLApplication_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IsisAmlData.h"
#include "IsisXMLGroups.h"
#include "IsisXMLHandler.h"
#include "IsisXMLIgnore.h"
#include "IsisXMLHistory.h"
/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLApplication : public IsisXMLHandler {

  public:

    // Constructors
    IsisXMLApplication(char *PencodingName,
                       bool &PexpandNamespaces,
                       XERCES::SAX2XMLReader* &Pparser,
                       IsisAmlData *PappData);

    ~IsisXMLApplication();


    //  SAX DocumentHandler interface
    void endDocument();

    void endElement(const XMLCh *const uri,
                    const XMLCh *const localname,
                    const XMLCh *const qname);

    void characters(const XMLCh *const chars,
                    const XMLSize_t length);

    void processingInstruction(const XMLCh *const target,
                               const XMLCh *const data);

    void startDocument();

    void startElement(const XMLCh *const uri,
                      const XMLCh *const localname,
                      const XMLCh *const qname,
                      const XERCES::Attributes  &attributes);



  private:

    // Saved argument from the constructor
    char *encodingName;
    bool expandNamespaces;
    XERCES::SAX2XMLReader *parser;
    IsisAmlData *appData;

    // Handlers this handler knows how to create
    IsisXMLHandler *briefHandler;
    IsisXMLHandler *descriptionHandler;
    IsisXMLGroups *groupsHandler;
    IsisXMLMultipleValues *multipleValuesHandler;
    IsisXMLIgnore *ignoreHandler;
    IsisXMLHistory *historyHandler;
};


#endif
