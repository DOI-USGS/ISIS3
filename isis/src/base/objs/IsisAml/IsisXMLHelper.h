#ifndef IsisXMLHelper_h
#define IsisXMLHelper_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IsisAmlData.h"
#include "IsisXMLIgnore.h"
#include "IsisXMLHandler.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLHelper : public IsisXMLHandler {

  public:

    ContentHandler *prevDocHandler;
    ErrorHandler *prevErrorHandler;


    IsisXMLHelper(char *PencodingName,
                  bool &PexpandNamespaces,
                  XERCES::SAX2XMLReader* &Pparser,
                  IsisHelperData *Helper);

    ~IsisXMLHelper();

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
    IsisHelperData *helper;

    // Handler this handler knows how to create
    IsisXMLHandler *generalHandler;
    IsisXMLIgnore *ignoreHandler;
};


#endif

