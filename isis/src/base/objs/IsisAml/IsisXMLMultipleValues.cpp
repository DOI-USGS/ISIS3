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

#include "IsisXMLMultipleValues.h" 
#include "IsisXMLChTrans.h"
  
  // Constructors

  IsisXMLMultipleValues::IsisXMLMultipleValues (char* PencodingName,
                                                bool &PexpandNamespaces,
                                                XERCES::SAX2XMLReader* &Pparser,
                                                std::vector<std::string> *PmultipleValues) {

    encodingName = PencodingName;
    expandNamespaces = PexpandNamespaces;
    parser = Pparser;    
    multipleValues = PmultipleValues;

    prevDocHandler = parser->getContentHandler ();
    prevErrorHandler = parser->getErrorHandler ();

    parser->setContentHandler (this);
    parser->setErrorHandler (this);
  
    generalHandler = NULL;
    ignoreHandler = NULL;  
  }
  
  IsisXMLMultipleValues::~IsisXMLMultipleValues () {
    if (generalHandler != NULL) delete generalHandler;
    if (ignoreHandler != NULL) delete ignoreHandler;  
  }
 

  //  IsisXMLMultipleValues: Overrides of the SAX DocumentHandler interface
  void IsisXMLMultipleValues::characters(const XMLCh* const chars,
                                         const unsigned int length) {}
    

  void IsisXMLMultipleValues::endElement(const XMLCh* const uri,
                                         const XMLCh* const localname,
                                         const XMLCh* const qname) {
    parser->setContentHandler(prevDocHandler);
    parser->setErrorHandler(prevErrorHandler);
  }

  
  void IsisXMLMultipleValues::startElement(const XMLCh* const uri,
                                           const XMLCh* const localname,
                                           const XMLCh* const qname,
                                           const XERCES::Attributes& attributes) {

    if ((string)XERCES::XMLString::transcode(localname) == (string)"item") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      unsigned int index = multipleValues->size();
      multipleValues->resize(index+1);
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &(*multipleValues)[index]);
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






