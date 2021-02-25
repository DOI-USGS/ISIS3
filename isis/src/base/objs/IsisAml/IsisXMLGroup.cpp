/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLGroup.h"
#include "IsisXMLChTrans.h"

#include "IString.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;

// Constructors
IsisXMLGroup::IsisXMLGroup(char *PencodingName,
                           bool &PexpandNamespaces,
                           XERCES::SAX2XMLReader* &Pparser,
                           IsisGroupData *Pgroup) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  group = Pgroup;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  parameterHandler = NULL;
  ignoreHandler = NULL;

}

IsisXMLGroup::~IsisXMLGroup() {

  if(parameterHandler != NULL) delete parameterHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLGroup: Overrides of the SAX DocumentHandler interface
void IsisXMLGroup::characters(const XMLCh *const chars,
                              const XMLSize_t length) {
}


void IsisXMLGroup::endElement(const XMLCh *const uri,
                              const XMLCh *const localname,
                              const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}

void IsisXMLGroup::startElement(const XMLCh *const uri,
                                const XMLCh *const localname,
                                const XMLCh *const qname,
                                const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"parameter")  {
    if(parameterHandler != NULL) {
      delete parameterHandler;
      parameterHandler = NULL;
    }
    unsigned int index = group->parameters.size();
    group->parameters.resize(index + 1);
    QString name = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
// Taken out after PVL refactor      name.UpCase();
    group->parameters[index].name = name;
    parameterHandler = new IsisXMLParameter(encodingName, expandNamespaces,
                                            parser, &group->parameters[index]);
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
