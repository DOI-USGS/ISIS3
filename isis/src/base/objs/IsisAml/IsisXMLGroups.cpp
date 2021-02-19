/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLGroups.h"
#include "IsisXMLChTrans.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;


// Constructors

IsisXMLGroups::IsisXMLGroups(char *PencodingName,
                             bool &PexpandNamespaces,
                             XERCES::SAX2XMLReader* &Pparser,
                             std::vector<IsisGroupData> *Pgroups) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  groups = Pgroups;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  groupHandler = NULL;
  ignoreHandler = NULL;
}

IsisXMLGroups::~IsisXMLGroups() {
  if(groupHandler != NULL) delete groupHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLGroups: Overrides of the SAX DocumentHandler interface
void IsisXMLGroups::characters(const XMLCh *const chars,
                               const XMLSize_t length) {}


void IsisXMLGroups::endElement(const XMLCh *const uri,
                               const XMLCh *const localname,
                               const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}


void IsisXMLGroups::startElement(const XMLCh *const uri,
                                 const XMLCh *const localname,
                                 const XMLCh *const qname,
                                 const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"group") {
    if(groupHandler != NULL) {
      delete groupHandler;
      groupHandler = NULL;
    }
    unsigned int index = groups->size();
    groups->resize(index + 1);
    (*groups)[index].name = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
    groupHandler = new IsisXMLGroup(encodingName, expandNamespaces, parser,
                                    &(*groups)[index]);
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
