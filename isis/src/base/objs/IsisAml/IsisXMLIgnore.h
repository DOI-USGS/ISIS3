#ifndef IsisXMLIgnore_h
#define IsisXMLIgnore_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IsisAmlData.h"
#include "IsisXMLHandler.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLIgnore : public IsisXMLHandler {

  public:

    ContentHandler *prevDocHandler;
    ErrorHandler *prevErrorHandler;


    IsisXMLIgnore(char *PencodingName,
                  bool &PexpandNamespaces,
                  XERCES::SAX2XMLReader* &Pparser,
                  const std::string Pignore);

    ~IsisXMLIgnore();

    void characters(const XMLCh *const chars,
                    const XMLSize_t length);

    void endElement(const XMLCh *const uri, const XMLCh *const localname,
                    const XMLCh *const qname);

  private:

    // Saved arguments from constructor
    char *encodingName;
    bool expandNamespaces;
    XERCES::SAX2XMLReader *parser;
    std::string ignore;

};


#endif






