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

#ifndef IsisXMLChTrans_h
#define IsisXMLChTrans_h

#include <string>
#include <iostream>
#include <cstdlib>

namespace XERCES = XERCES_CPP_NAMESPACE;

//  This class converts from the internal XMLCh character format to simple c strings

class XMLChTrans {
public :
  //  Constructors and Destructor
  XMLChTrans(const XMLCh* const toTranscode)
  {
    // Call the private transcoding method
    fLocalForm = XERCES::XMLString::transcode(toTranscode);
  }

  ~XMLChTrans()
  {
    delete [] fLocalForm;
  }

  //  Getter methods
  const char* localForm() const
  {
    return fLocalForm;
  }

private :
  char*   fLocalForm;
};


inline std::ostream& operator<<(std::ostream& target, const XMLChTrans& toDump)
{
  target << toDump.localForm();
  return target;
}

#endif 
