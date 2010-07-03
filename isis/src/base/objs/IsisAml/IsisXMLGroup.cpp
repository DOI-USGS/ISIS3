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

#include "IsisXMLGroup.h" 
#include "IsisXMLChTrans.h"

#include "iString.h"

namespace XERCES = XERCES_CPP_NAMESPACE;

  // Constructors
  IsisXMLGroup::IsisXMLGroup (char* PencodingName,
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
  
  IsisXMLGroup::~IsisXMLGroup () {
    
    if (parameterHandler != NULL) delete parameterHandler;
    if (ignoreHandler != NULL) delete ignoreHandler;  
  }  
  

  //  IsisXMLGroup: Overrides of the SAX DocumentHandler interface
  void IsisXMLGroup::characters(const XMLCh* const chars,
                                const unsigned int length) {
  }
  
  
  void IsisXMLGroup::endElement(const XMLCh* const uri,
                                const XMLCh* const localname,
                                const XMLCh* const qname) {
    parser->setContentHandler(prevDocHandler);
    parser->setErrorHandler(prevErrorHandler);
  }

  void IsisXMLGroup::startElement(const XMLCh* const uri,
                                        const XMLCh* const localname,
                                        const XMLCh* const qname,
                                        const XERCES::Attributes& attributes) {
    
    if ((string)XERCES::XMLString::transcode(localname) == (string)"parameter")  {
      if (parameterHandler != NULL) {
        delete parameterHandler;
        parameterHandler = NULL;
      }
      unsigned int index = group->parameters.size();
      group->parameters.resize(index+1);
      Isis::iString name = XERCES::XMLString::transcode (attributes.getValue((unsigned int)0));
// Taken out after PVL refactor      name.UpCase();
      group->parameters[index].name = name;
      parameterHandler = new IsisXMLParameter (encodingName, expandNamespaces,
                                               parser, &group->parameters[index]);
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
