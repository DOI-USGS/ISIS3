/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/01/30 22:12:22 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

using namespace std;

#include <string>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "IsisXMLApplication.h"
#include "IsisXMLChTrans.h"

#include "iString.h"

  // Constructors
  IsisXMLApplication::IsisXMLApplication (char* PencodingName,
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

  IsisXMLApplication::~IsisXMLApplication () {

    if (briefHandler != NULL) delete briefHandler;
    if (descriptionHandler != NULL) delete descriptionHandler;
    if (groupsHandler != NULL) delete groupsHandler;
    if (multipleValuesHandler != NULL) delete multipleValuesHandler;
    if (ignoreHandler != NULL) delete ignoreHandler;
    if (historyHandler != NULL) delete historyHandler;
  }


  // Callback methodes for handling pieces of the xml file

  //  IsisXMLApplication: Overrides of the SAX DocumentHandler interface
  void IsisXMLApplication::characters(const XMLCh* const chars,
                                      const unsigned int length) {}



  void IsisXMLApplication::endDocument() {}


  void IsisXMLApplication::endElement(const XMLCh* const uri,
                                      const XMLCh* const localname,
                                      const XMLCh* const qname) {
  }


  void IsisXMLApplication::processingInstruction(const XMLCh* const target,
                                                 const XMLCh* const data) {}


  void IsisXMLApplication::startDocument() {}


  void IsisXMLApplication::startElement(const XMLCh* const uri,
                                        const XMLCh* const localname,
                                        const XMLCh* const qname,
                                        const XERCES::Attributes& attributes) {

    if ((string)XERCES::XMLString::transcode(localname) == (string)"application")  {
      Isis::iString name = XERCES::XMLString::transcode (attributes.getValue((unsigned int)0));
      appData->name = name.DownCase();
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
      if (briefHandler != NULL) {
        delete briefHandler;
        briefHandler = NULL;
      }
      briefHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                         parser, &appData->brief);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"description")  {
      if (descriptionHandler != NULL) {
        delete descriptionHandler;
        descriptionHandler = NULL;
      }
      descriptionHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                               parser, &appData->description);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"groups")  {
      if (groupsHandler != NULL) {
        delete groupsHandler;
        groupsHandler = NULL;
      }
      groupsHandler = new IsisXMLGroups (encodingName, expandNamespaces, parser,
                                         &appData->groups);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"category")  {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &appData->categorys);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"history")  {
      if (historyHandler != NULL) {
        delete historyHandler;
        historyHandler = NULL;
      }
      historyHandler = new IsisXMLHistory (encodingName, expandNamespaces,
                                           parser, &appData->changes);
    }
    else {
      if (ignoreHandler != NULL) {
        delete ignoreHandler;
        ignoreHandler = NULL;
      }
      ignoreHandler = new IsisXMLIgnore (encodingName, expandNamespaces, parser,
                                         (string)XERCES::XMLString::transcode(localname));
    }
  }
