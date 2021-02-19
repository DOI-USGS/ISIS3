/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IString.h"

#include "IsisXMLParameter.h"
#include "IsisXMLChTrans.h"

using namespace std;

// Constructors

IsisXMLParameter::IsisXMLParameter(char *PencodingName,
                                   bool &PexpandNamespaces,
                                   XERCES::SAX2XMLReader* &Pparser,
                                   IsisParameterData *Pparameter) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  parameter = Pparameter;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  generalHandler = NULL;
  multipleValuesHandler = NULL;
  ignoreHandler = NULL;
  listHandler = NULL;
  helpersHandler = NULL;
}

IsisXMLParameter::~IsisXMLParameter() {
  if(generalHandler != NULL) {
    delete generalHandler;
    generalHandler = NULL;
  }
  if(multipleValuesHandler != NULL) {
    delete multipleValuesHandler;
    multipleValuesHandler = NULL;
  }
  if(ignoreHandler != NULL) {
    delete ignoreHandler;
    ignoreHandler = NULL;
  }
  if(listHandler != NULL) {
    delete listHandler;
    listHandler = NULL;
  }
  if(helpersHandler != NULL) {
    delete helpersHandler;
    helpersHandler = NULL;
  }
}


//  IsisXMLParameter: Overrides of the SAX DocumentHandler interface
void IsisXMLParameter::characters(const XMLCh *const chars,
                                  const XMLSize_t length) {}


void IsisXMLParameter::endElement(const XMLCh *const uri,
                                  const XMLCh *const localname,
                                  const XMLCh *const qname) {
  if((string)XERCES::XMLString::transcode(localname) != "list") {
    parser->setContentHandler(prevDocHandler);
    parser->setErrorHandler(prevErrorHandler);
  }
}

void IsisXMLParameter::startElement(const XMLCh *const uri,
                                    const XMLCh *const localname,
                                    const XMLCh *const qname,
                                    const XERCES::Attributes &attributes) {
  if((string)XERCES::XMLString::transcode(localname) == (string)"type") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &parameter->type);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &parameter->brief);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"description") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &parameter->description);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"default") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->defaultValues);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"internalDefault") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser,
                                        &parameter->internalDefault);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"count") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser,
                                        &parameter->count);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"list") {
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"option") {
    if(listHandler != NULL) {
      delete listHandler;
      listHandler = NULL;
    }
    unsigned int index = parameter->listOptions.size();
    parameter->listOptions.resize(index + 1);
    QString lo = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
    lo = lo.toUpper();
    parameter->listOptions[index].value = lo;
    listHandler = new IsisXMLList(encodingName, expandNamespaces, parser,
                                  &parameter->listOptions[index]);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"minimum") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    QString mi = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
    parameter->minimum_inclusive = mi.toLower();
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &parameter->minimum);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"maximum") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    QString mi = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
    parameter->maximum_inclusive = mi.toLower();
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser,
                                        &parameter->maximum);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"greaterThan") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->greaterThan);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"greaterThanOrEqual") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->greaterThanOrEqual);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"lessThan") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->lessThan);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"lessThanOrEqual") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->lessThanOrEqual);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"notEqual") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->notEqual);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"odd") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    parameter->odd = "TRUE";
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser);

  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"exclusions") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->exclude);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"inclusions") {
    if(multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    multipleValuesHandler = new IsisXMLMultipleValues(encodingName, expandNamespaces,
        parser, &parameter->include);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"filter") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser, &parameter->filter);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"defaultPath") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser,
                                        &parameter->path);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"fileMode") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser, &parameter->fileMode);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"pixelType") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces, parser, &parameter->pixelType);
  }
  else if((string)XERCES::XMLString::transcode(localname) == (string)"helpers") {
    if(helpersHandler != NULL) {
      delete helpersHandler;
      helpersHandler = NULL;
    }
    helpersHandler = new IsisXMLHelpers(encodingName, expandNamespaces, parser,
                                        &parameter->helpers);
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
