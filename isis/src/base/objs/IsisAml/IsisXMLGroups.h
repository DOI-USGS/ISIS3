/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#ifndef IsisXMLGroups_h
#define IsisXMLGroups_h

//#include <sax2/DefaultHandler.hpp>

#include "IsisAmlData.h"
#include "IsisXMLGroup.h"
#include "IsisXMLIgnore.h"
/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class IsisXMLGroups : public IsisXMLHandler {

  public:

    ContentHandler *prevDocHandler;
    ErrorHandler *prevErrorHandler;


    IsisXMLGroups(char *PencodingName,
                  bool &PexpandNamespaces,
                  XERCES::SAX2XMLReader* &Pparser,
                  std::vector<IsisGroupData> *Pgroups);

    ~IsisXMLGroups();

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
    std::vector<IsisGroupData> *groups;

    // Handlers this handler knows how to create
    IsisXMLGroup *groupHandler;
    IsisXMLIgnore *ignoreHandler;

};


#endif





