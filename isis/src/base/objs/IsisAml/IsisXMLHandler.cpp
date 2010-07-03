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
#include <xercesc/util/XMLException.hpp>

#include <sstream>

#include "iException.h"
#include "iException.h"

#include "iString.h"

#include "IsisXMLHandler.h"
#include "IsisXMLChTrans.h"

namespace XERCES = XERCES_CPP_NAMESPACE;


  // Constructors

  IsisXMLHandler::IsisXMLHandler () {
    value=NULL;
    outputEndTag = 0;
  }


  IsisXMLHandler::IsisXMLHandler (char* PencodingName,
                                  bool &PexpandNamespaces,
                                  XERCES::SAX2XMLReader* &Pparser,
                                  std::string *Pvalue) {

    encodingName = PencodingName;
    expandNamespaces = PexpandNamespaces;
    parser = Pparser;
    value = Pvalue;

    outputEndTag = 0;

    prevDocHandler = parser->getContentHandler();
    prevErrorHandler = parser->getErrorHandler();

    parser->setContentHandler(this);
    parser->setErrorHandler(this);
  }


  IsisXMLHandler::IsisXMLHandler (char* PencodingName,
                                  bool &PexpandNamespaces,
                                  XERCES::SAX2XMLReader* &Pparser) {

    value = NULL;
    outputEndTag = 0;

    encodingName = PencodingName;
    expandNamespaces = PexpandNamespaces;
    parser = Pparser;

    prevDocHandler = parser->getContentHandler();
    prevErrorHandler = parser->getErrorHandler();

    parser->setContentHandler(this);
    parser->setErrorHandler(this);
  }


  IsisXMLHandler::~IsisXMLHandler () {}


  //  IsisXMLHandler: Overrides the SAX ErrorHandler
  void IsisXMLHandler::error(const XERCES::SAXParseException& e) {
    ostringstream os;
    os << "Error in application XML file line: " << e.getLineNumber ()
       << " char: " << e.getColumnNumber() << ". "
       << XERCES::XMLString::transcode(e.getMessage());
    throw Isis::iException::Message(Isis::iException::Programmer,os.str(), _FILEINFO_);
  }

  void IsisXMLHandler::fatalError(const XERCES::SAXParseException& e) {
    ostringstream os;
    os << "Error in application XML file line: " << e.getLineNumber ()
       << " char: " << e.getColumnNumber() << ". "
       << XERCES::XMLString::transcode(e.getMessage());
    throw Isis::iException::Message(Isis::iException::Programmer,os.str(), _FILEINFO_);
  }

  void IsisXMLHandler::warning(const XERCES::SAXParseException& e) {
    ostringstream os;
    os << "Error in application XML file line: " << e.getLineNumber ()
       << " char: " << e.getColumnNumber() << ". "
       << XERCES::XMLString::transcode(e.getMessage());
    throw Isis::iException::Message(Isis::iException::Programmer,os.str(), _FILEINFO_);
  }


  //  IsisXMLHandler: Overrides of the SAX DocumentHandler interface
  void IsisXMLHandler::characters(const XMLCh* const chars,
                                  const unsigned int length) {

    if (value != NULL) {
      Isis::iString str;
      str = (string)XERCES::XMLString::transcode(chars);
      str.Trim (" \n\r");
      *value += str;
    }
  }


  void IsisXMLHandler::endDocument() {
  }


  void IsisXMLHandler::endElement(const XMLCh* const uri,
                                  const XMLCh* const localname,
                                  const XMLCh* const qname) {

    if (outputEndTag > 0) {
      Isis::iString str;
      str = (string)XERCES::XMLString::transcode(localname);
      *value += "</" + str + ">";
      outputEndTag--;
    }
    else {
      parser->setContentHandler(prevDocHandler);
      parser->setErrorHandler(prevErrorHandler);
    }
  }


  void IsisXMLHandler::ignorableWhitespace(const XMLCh* const chars,
                                           const unsigned int length) {
  }



  void IsisXMLHandler::processingInstruction(const XMLCh* const target,
                                             const XMLCh* const data) {
  }


  void IsisXMLHandler::startDocument() {
  }


  void IsisXMLHandler::startElement(const XMLCh* const uri,
                                    const XMLCh* const localname,
                                    const XMLCh* const qname,
                                    const XERCES::Attributes& attributes) {

    if (value != NULL) {
      Isis::iString str;
      str = (string)XERCES::XMLString::transcode(localname);
      // Note: need to build the attributes into the string too
      *value += "<" + str + ">";
      outputEndTag++;
    }
  }


