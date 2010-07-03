
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

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLHelper.h" 
#include "IsisXMLChTrans.h"

#include "iString.h"

namespace XERCES = XERCES_CPP_NAMESPACE;

  // Constructors
  IsisXMLHelper::IsisXMLHelper (char* PencodingName,
                                bool &PexpandNamespaces,
                                XERCES::SAX2XMLReader* &Pparser,
                                IsisHelperData *Phelper) {
    
    encodingName = PencodingName;
    expandNamespaces = PexpandNamespaces;
    parser = Pparser;    
    helper = Phelper;
  
    prevDocHandler = parser->getContentHandler();
    prevErrorHandler = parser->getErrorHandler();

    parser->setContentHandler(this);
    parser->setErrorHandler(this);
  
    ignoreHandler = NULL;  
    generalHandler = NULL;
  }
  
  IsisXMLHelper::~IsisXMLHelper () {
    if (ignoreHandler != NULL) delete ignoreHandler;  
    if (generalHandler != NULL) delete generalHandler;  
  }  
  

  //  IsisXMLHelper: Overrides of the SAX DocumentHandler interface
  void IsisXMLHelper::characters(const XMLCh* const chars,
                                 const unsigned int length) {
  }
  
  
  void IsisXMLHelper::endElement(const XMLCh* const uri,
                                 const XMLCh* const localname,
                                 const XMLCh* const qname) {
    parser->setContentHandler(prevDocHandler);
    parser->setErrorHandler(prevErrorHandler);
  }

  void IsisXMLHelper::startElement(const XMLCh* const uri,
                                         const XMLCh* const localname,
                                         const XMLCh* const qname,
                                         const XERCES::Attributes& attributes) {
    
    if ((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &helper->brief);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"description")  {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &helper->description);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"function")  {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &helper->function);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"icon")  {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &helper->icon);
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
