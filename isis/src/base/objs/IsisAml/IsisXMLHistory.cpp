/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLHistory.h"
#include "IsisXMLChTrans.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;


// Constructors

IsisXMLHistory::IsisXMLHistory(char *PencodingName,
                               bool &PexpandNamespaces,
                               XERCES::SAX2XMLReader* &Pparser,
                               std::vector<IsisChangeData> *Pchanges) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  changes = Pchanges;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  generalHandler = NULL;
  ignoreHandler = NULL;
}

IsisXMLHistory::~IsisXMLHistory() {
  if(generalHandler != NULL) delete generalHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLHistory: Overrides of the SAX DocumentHandler interface
void IsisXMLHistory::characters(const XMLCh *const chars,
                                const XMLSize_t length) {}


void IsisXMLHistory::endElement(const XMLCh *const uri,
                                const XMLCh *const localname,
                                const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}


void IsisXMLHistory::startElement(const XMLCh *const uri,
                                  const XMLCh *const localname,
                                  const XMLCh *const qname,
                                  const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"change") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }

    // Pull the attributes out and save them
    unsigned int index = changes->size();
    changes->resize(index + 1);

    // Get the name and date attributes
    string st;
    for(unsigned int i = 0; i < 2; i++) {
      st = XERCES::XMLString::transcode(attributes.getQName(i));
      if(st == "name") {
        (*changes)[index].name = XERCES::XMLString::transcode(attributes.getValue(i));
      }
      else if(st == "date") {
        (*changes)[index].date = XERCES::XMLString::transcode(attributes.getValue(i));
      }
    }

    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &(*changes)[index].description);
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
