/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/02/20 16:28:21 $                                                                 
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
#ifndef FileList_h
#define FileList_h

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

namespace Isis {
/**                                                                       
 * @brief Internalizes a list of files.                
 *                                                                        
 *  This class reads a list of filenames from a file an internalizes them in a 
 * standard template vector of strings.  Thus, a file like:
 * @code
 * m0035431.imq
 * m0030402.imq
 * m0033231.imq
 *    .
 *    .
 *    .
 * m0203331.imq
 * @endcode
 * Will be internalized as and accessable as an vector.            
 *                                                                        
 * @ingroup Parsing                                                  
 *                                                                        
 * @author 2003-05-01 Jeff Anderson                                        
 *                                                                                                             
 * @internal                                                                                                                        
 *   @history 2003-05-16 Stuart Sides - modified schema from 
 *                                      astrogeology... isis.astrogeology...
 *   @history 2005-03-16 Leah Dahmer - modified file to support Doxygen 
 *                                     documentation.
 *   @history 2006-04-05 Elizabeth Miller - Added error message to catch empty
 *                                          files
 *   @history 2007-01-04 Brendan George - Added comment recognition and it now
 *                                          only reads the first column
 *  @history 2007-02-19 Stacy Alley - modified the Read method
 *           such that if a file name and it's attributes are
 *           surround with double quotes, then don't use commas
 *           as a 'end of line' signal.
 */                                                                       
  class FileList : public std::vector<std::string> {
    public:
      FileList (const std::string &list);
      FileList (std::istream &in);
      FileList ();

      //! Destroys the FileList object.
      ~FileList () {};
  
      void Read(const std::string &list);
      void Read(std::istream &in);
      void Write(const std::string &list);
      void Write(std::ostream &out);
  }; 
};

#endif
