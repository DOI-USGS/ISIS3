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

#include "iString.h"

#include "IsisXMLParameter.h"
#include "IsisXMLChTrans.h"


  // Constructors

  IsisXMLParameter::IsisXMLParameter (char* PencodingName,
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

  IsisXMLParameter::~IsisXMLParameter () {
    if (generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }
    if (multipleValuesHandler != NULL) {
      delete multipleValuesHandler;
      multipleValuesHandler = NULL;
    }
    if (ignoreHandler != NULL) {
      delete ignoreHandler;
      ignoreHandler = NULL;
    }
    if (listHandler != NULL) {
      delete listHandler;
      listHandler = NULL;
    }
    if (helpersHandler != NULL) {
      delete helpersHandler;
      helpersHandler = NULL;
    }
  }


  //  IsisXMLParameter: Overrides of the SAX DocumentHandler interface
  void IsisXMLParameter::characters(const XMLCh* const chars,
                                    const unsigned int length) {}


  void IsisXMLParameter::endElement(const XMLCh* const uri,
                                    const XMLCh* const localname,
                                    const XMLCh* const qname) {
    if ((string)XERCES::XMLString::transcode(localname) != "list") {
      parser->setContentHandler(prevDocHandler);
      parser->setErrorHandler(prevErrorHandler);
    }
  }

  void IsisXMLParameter::startElement(const XMLCh* const uri,
                                      const XMLCh* const localname,
                                      const XMLCh* const qname,
                                      const XERCES::Attributes& attributes) {

    if ((string)XERCES::XMLString::transcode(localname) == (string)"type") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &parameter->type);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"brief")  {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &parameter->brief);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"description") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                           parser, &parameter->description);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"default") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->defaultValues);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"internalDefault") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser,
                                           &parameter->internalDefault);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"count") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser,
                                           &parameter->count);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"list") {
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"option") {
      if (listHandler != NULL) {
        delete listHandler;
        listHandler = NULL;
      }
      unsigned int index = parameter->listOptions.size();
      parameter->listOptions.resize (index + 1);
      Isis::iString lo = XERCES::XMLString::transcode (attributes.getValue((unsigned int)0));
      lo.UpCase();
      parameter->listOptions[index].value = lo;
      listHandler = new IsisXMLList (encodingName, expandNamespaces, parser,
                                     &parameter->listOptions[index]);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"minimum") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      Isis::iString mi = XERCES::XMLString::transcode (attributes.getValue((unsigned int)0));
      mi.DownCase();
      parameter->minimum_inclusive = mi;
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces,
                                             parser, &parameter->minimum);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"maximum") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      Isis::iString mi = XERCES::XMLString::transcode (attributes.getValue((unsigned int)0));
      mi.DownCase();
      parameter->maximum_inclusive = mi;
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser,
                                           &parameter->maximum);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"greaterThan") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                           parser, &parameter->greaterThan);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"greaterThanOrEqual") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->greaterThanOrEqual);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"lessThan") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->lessThan);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"lessThanOrEqual") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->lessThanOrEqual);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"notEqual") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->notEqual);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"odd") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      parameter->odd = "TRUE";
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser);

    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"exclusions") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->exclude);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"inclusions") {
      if (multipleValuesHandler != NULL) {
        delete multipleValuesHandler;
        multipleValuesHandler = NULL;
      }
      multipleValuesHandler = new IsisXMLMultipleValues (encodingName, expandNamespaces,
                                                         parser, &parameter->include);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"filter") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser, &parameter->filter);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"defaultPath") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser,
                                           &parameter->path);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"fileMode") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser, &parameter->fileMode);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"pixelType") {
      if (generalHandler != NULL) {
        delete generalHandler;
        generalHandler = NULL;
      }
      generalHandler = new IsisXMLHandler (encodingName, expandNamespaces, parser, &parameter->pixelType);
    }
    else if ((string)XERCES::XMLString::transcode(localname) == (string)"helpers") {
      if (helpersHandler != NULL) {
        delete helpersHandler;
        helpersHandler = NULL;
      }
      helpersHandler = new IsisXMLHelpers (encodingName, expandNamespaces, parser, 
                                       &parameter->helpers);
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




