/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLHelpers.h"
#include "IsisXMLChTrans.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;


// Constructors

IsisXMLHelpers::IsisXMLHelpers(char *PencodingName,
                               bool &PexpandNamespaces,
                               XERCES::SAX2XMLReader* &Pparser,
                               std::vector<IsisHelperData> *Phelpers) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  helpers = Phelpers;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  helperHandler = NULL;
  ignoreHandler = NULL;
}

IsisXMLHelpers::~IsisXMLHelpers() {
  if(helperHandler != NULL) delete helperHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLHelpers: Overrides of the SAX DocumentHandler interface
void IsisXMLHelpers::characters(const XMLCh *const chars,
                                const XMLSize_t length) {}


void IsisXMLHelpers::endElement(const XMLCh *const uri,
                                const XMLCh *const localname,
                                const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}


void IsisXMLHelpers::startElement(const XMLCh *const uri,
                                  const XMLCh *const localname,
                                  const XMLCh *const qname,
                                  const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"helper") {
    if(helperHandler != NULL) {
      delete helperHandler;
      helperHandler = NULL;
    }
    unsigned int index = helpers->size();
    helpers->resize(index + 1);
    (*helpers)[index].name = XERCES::XMLString::transcode(attributes.getValue((XMLSize_t)0));
    helperHandler = new IsisXMLHelper(encodingName, expandNamespaces, parser,
                                      &(*helpers)[index]);
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
