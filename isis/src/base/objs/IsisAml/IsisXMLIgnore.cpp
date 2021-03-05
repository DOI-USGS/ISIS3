/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

#include "IsisXMLIgnore.h"
#include "IsisXMLChTrans.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;

// Constructors

IsisXMLIgnore::IsisXMLIgnore(char *PencodingName,
                             bool &PexpandNamespaces,
                             XERCES::SAX2XMLReader* &Pparser,
                             const std::string Pignore) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  ignore = Pignore;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

}


IsisXMLIgnore::~IsisXMLIgnore() {}


void IsisXMLIgnore::characters(const XMLCh *const chars,
                               const XMLSize_t length) {
}

void IsisXMLIgnore::endElement(const XMLCh *const uri,
                               const XMLCh *const localname,
                               const XMLCh *const qname) {

  if((string)XERCES::XMLString::transcode(localname) == ignore) {
    parser->setContentHandler(prevDocHandler);
    parser->setErrorHandler(prevErrorHandler);
  }
}
