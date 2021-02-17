
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLHelper.h"
#include "IsisXMLChTrans.h"

#include "IString.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;

// Constructors
IsisXMLHelper::IsisXMLHelper(char *PencodingName,
                             bool &PexpandNamespaces,
                             XERCES::SAX2XMLReader* &Pparser,
                             IsisHelperData *Phelper) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  helper = Phelper;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  ignoreHandler = NULL;
  generalHandler = NULL;
}

IsisXMLHelper::~IsisXMLHelper() {
  if(ignoreHandler != NULL) delete ignoreHandler;
  if(generalHandler != NULL) delete generalHandler;
}


//  IsisXMLHelper: Overrides of the SAX DocumentHandler interface
void IsisXMLHelper::characters(const XMLCh *const chars,
                               const XMLSize_t length) {
}


void IsisXMLHelper::endElement(const XMLCh *const uri,
                               const XMLCh *const localname,
                               const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}

void IsisXMLHelper::startElement(const XMLCh *const uri,
                                 const XMLCh *const localname,
                                 const XMLCh *const qname,
                                 const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &helper->brief);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"description")  {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &helper->description);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"function")  {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &helper->function);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"icon")  {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &helper->icon);
  }
  else {
    if(ignoreHandler != NULL) {
      delete ignoreHandler;
      ignoreHandler = NULL;
    }
    ignoreHandler = new IsisXMLIgnore(encodingName, expandNamespaces, parser,
                                      (string)XERCES::XMLString::transcode(localname));
  }
}
