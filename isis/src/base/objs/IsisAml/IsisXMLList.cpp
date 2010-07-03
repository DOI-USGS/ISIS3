/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:07 $                                                                 
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

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLList.h" 
#include "IsisXMLChTrans.h"


namespace XERCES = XERCES_CPP_NAMESPACE;

  // Constructors
  IsisXMLList::IsisXMLList (char* PencodingName,
                            bool &PexpandNamespaces,
                            XERCES::SAX2XMLReader* &Pparser,
                            IsisListOptionData *Plist) {
    
    encodingName = PencodingName;
    expandNamespaces = PexpandNamespaces;
    parser = Pparser;    
    list = Plist;
  
    prevDocHandler = parser->getContentHandler();
    prevErrorHandler = parser->getErrorHandler();

    parser->setContentHandler(this);
    parser->setErrorHandler(this);
  
    generalHandler = NULL;
    multipleValuesHandler = NULL;
    ignoreHandler = NULL;  
  
  }
  
  IsisXMLList::~IsisXMLList () {

    if (generalHandler) delete generalHandler;
    if (multipleValuesHandler != NULL) delete multipleValuesHandler;
    if (ignoreHandler != NULL) delete ignoreHandler;  
  }  


  //  IsisXMLList: Overrides of the SAX DocumentHandler interface
  void IsisXMLList::characters(const XMLCh* const chars,
                               const unsigned int length) {
  }


  void IsisXMLList::endElement(const XMLCh* const uri,
                               const XMLCh* const localname,
                               const XMLCh* const qname) {
    parser->setContentHandler(prevDocHandler);
    parser->setErrorHandler(prevErrorHandler);
  }

  void IsisXMLList::startElement(const XMLCh* const uri,
                                 const XMLCh* const localname,
                                 const XMLCh* const qname,
                                 const XERCES::Attributes& attributes) {

    if ((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser,
                                           &list->brief);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"description") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser,
                                           &list->description);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"exclusions") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler =
          new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                     parser, &list->exclude);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"inclusions") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler =
          new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                     parser, &list->include);
    }
    else {
      if (ignoreHandler != NULL) {
        delete ignoreHandler;  
        ignoreHandler = NULL;
      }
      ignoreHandler = 
          new IsisXMLIgnore (encodingName, expandNamespaces, parser,
                             (string)XERCES::XMLString::transcode(localname));
    }
  
  
  
  
  }
