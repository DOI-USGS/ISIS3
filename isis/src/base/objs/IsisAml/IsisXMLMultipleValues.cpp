/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

#include "IsisXMLMultipleValues.h"
#include "IsisXMLChTrans.h"

using namespace std;

// Constructors

IsisXMLMultipleValues::IsisXMLMultipleValues(char *PencodingName,
    bool &PexpandNamespaces,
    XERCES::SAX2XMLReader* &Pparser,
    std::vector<QString> *PmultipleValues) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  multipleValues = PmultipleValues;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  generalHandler = NULL;
  ignoreHandler = NULL;
}

IsisXMLMultipleValues::~IsisXMLMultipleValues() {
  if(generalHandler != NULL) delete generalHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLMultipleValues: Overrides of the SAX DocumentHandler interface
void IsisXMLMultipleValues::characters(const XMLCh *const chars,
                                       const XMLSize_t length) {}


void IsisXMLMultipleValues::endElement(const XMLCh *const uri,
                                       const XMLCh *const localname,
                                       const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}


void IsisXMLMultipleValues::startElement(const XMLCh *const uri,
    const XMLCh *const localname,
    const XMLCh *const qname,
    const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"item") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    unsigned int index = multipleValues->size();
    multipleValues->resize(index + 1);
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &(*multipleValues)[index]);
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
