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

#ifndef IsisXMLApplication_h
#define IsisXMLApplication_h

//#include <sax2/DefaultHandler.hpp>

#include "IsisAmlData.h"
#include "IsisXMLGroups.h"
#include "IsisXMLHandler.h"
#include "IsisXMLIgnore.h"
#include "IsisXMLHistory.h"

class IsisXMLApplication : public IsisXMLHandler {

public:

  // Constructors
  IsisXMLApplication (char* PencodingName,
                      bool &PexpandNamespaces,
                      XERCES::SAX2XMLReader* &Pparser,
                      IsisAmlData *PappData);

  ~IsisXMLApplication ();


  //  SAX DocumentHandler interface
  void endDocument();

  void endElement(const XMLCh* const uri,
                  const XMLCh* const localname,
                  const XMLCh* const qname);

  void characters(const XMLCh* const chars,
                  const unsigned int length);

  void processingInstruction (const XMLCh* const target,
                              const XMLCh* const data);

  void startDocument();

  void startElement (const XMLCh* const uri,
                     const XMLCh* const localname,
                     const XMLCh* const qname,
                     const XERCES::Attributes&  attributes);



private:

  // Saved argument from the constructor
  char* encodingName;
  bool expandNamespaces;
  XERCES::SAX2XMLReader *parser;
  IsisAmlData *appData;

  // Handlers this handler knows how to create
  IsisXMLHandler *briefHandler;
  IsisXMLHandler *descriptionHandler;
  IsisXMLGroups *groupsHandler;
  IsisXMLMultipleValues *multipleValuesHandler;
  IsisXMLIgnore *ignoreHandler;
  IsisXMLHistory *historyHandler;
};


#endif
