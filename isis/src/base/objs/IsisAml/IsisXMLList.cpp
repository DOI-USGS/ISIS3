/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLList.h"
#include "IsisXMLChTrans.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;

// Constructors
IsisXMLList::IsisXMLList(char *PencodingName,
                         bool &PexpandNamespaces,
                         XERCES::SAX2XMLReader* &Pparser,
                         IsisListOptionData *Plist) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  list = Plist;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  generalHandler = NULL;
  multipleValuesHandler = NULL;
  ignoreHandler = NULL;

}

IsisXMLList::~IsisXMLList() {

  if(generalHandler) delete generalHandler;
  if(multipleValuesHandler != NULL) delete multipleValuesHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLList: Overrides of the SAX DocumentHandler interface
void IsisXMLList::characters(const XMLCh *const chars,
                             const XMLSize_t length) {
}


void IsisXMLList::endElement(const XMLCh *const uri,
                             const XMLCh *const localname,
                             const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}

void IsisXMLList::startElement(const XMLCh *const uri,
                               const XMLCh *const localname,
                               const XMLCh *const qname,
                               const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser,
                                        &list->brief);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"description") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser,
                                        &list->description);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"exclusions") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler =
      new IsisXMLMultipleValues(encodingName, expandNamespaces,
                                parser, &list->exclude);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"inclusions") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler =
      new IsisXMLMultipleValues(encodingName, expandNamespaces,
                                parser, &list->include);
  }
  else {
    if(ignoreHandler != NULL) {
      delete ignoreHandler;
      ignoreHandler = NULL;
    }
    ignoreHandler =
      new IsisXMLIgnore(encodingName, expandNamespaces, parser,
                        (string)XERCES::XMLString::transcode(localname));
  }




}
