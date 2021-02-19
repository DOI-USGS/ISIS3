/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "IsisXMLApplication.h"
#include "IsisXMLChTrans.h"

#include "IString.h"

using namespace std;

// Constructors
IsisXMLApplication::IsisXMLApplication(char *PencodingName,
                                       bool &PexpandNamespaces,
                                       XERCES::SAX2XMLReader* &Pparser,
                                       IsisAmlData *PappData) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  appData = PappData;

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  briefHandler = NULL;
  descriptionHandler = NULL;
  groupsHandler = NULL;
  multipleValuesHandler = NULL;
  ignoreHandler = NULL;
  historyHandler = NULL;

}

IsisXMLApplication::~IsisXMLApplication() {

  if(briefHandler != NULL) delete briefHandler;
  if(descriptionHandler != NULL) delete descriptionHandler;
  if(groupsHandler != NULL) delete groupsHandler;
  if(multipleValuesHandler != NULL) delete multipleValuesHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
  if(historyHandler != NULL) delete historyHandler;
}


// Callback methodes for handling pieces of the xml file

//  IsisXMLApplication: Overrides of the SAX DocumentHandler interface
void IsisXMLApplication::characters(const XMLCh *const chars,
                                    const XMLSize_t length) {}



void IsisXMLApplication::endDocument() {}


void IsisXMLApplication::endElement(const XMLCh *const uri,
                                    const XMLCh *const localname,
                                    const XMLCh *const qname) {
}


void IsisXMLApplication::processingInstruction(const XMLCh *const target,
    const XMLCh *const data) {}


void IsisXMLApplication::startDocument() {}


void IsisXMLApplication::startElement(const XMLCh *const uri,
                                      const XMLCh *const localname,
                                      const XMLCh *const qname,
                                      const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"application")  {
    QString name = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
    appData->name = name.toLower();
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
    if(briefHandler != NULL) {
      delete briefHandler;
      briefHandler = NULL;
    }
    briefHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                      parser, &appData->brief);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"description")  {
    if(descriptionHandler != NULL) {
      delete descriptionHandler;
      descriptionHandler = NULL;
    }
    descriptionHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                            parser, &appData->description);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"groups")  {
    if(groupsHandler != NULL) {
      delete groupsHandler;
      groupsHandler = NULL;
    }
    groupsHandler = new IsisXMLGroups(encodingName, expandNamespaces, parser,
                                      &appData->groups);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"category")  {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &appData->categorys);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"history")  {
    if(historyHandler != NULL) {
      delete historyHandler;
      historyHandler = NULL;
    }
    historyHandler = new IsisXMLHistory(encodingName, expandNamespaces,
                                        parser, &appData->changes);
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
