/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2008/08/25 21:58:30 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                       

#include <string>  
#include "iException.h"
#include "Filename.h"
#include "Message.h"
#include "CisscalFile.h"
#include "TextFile.h"
#include "iString.h"

using namespace std;
namespace Isis {
 /** 
  * Constructs a CisscalFile object and opens the specified file 
  * (including path). 
  * 
  * @param filename Filename (including path) to be opened by <b>TextFile</b>  object.
  * 
  * @param openmode Open Mode of file opened by <b>TextFile</b>  object. 
  *                 <UL> 
  *                   <LI><TT><I>"input"</I></TT> Opens file for
  *                       Input: Read Only. Fails if file does
  *                       not exist.
  *                   <LI><TT><I>"output"</I></TT> Opens file
  *                       for Output: Read/Write.  Creates file,
  *                       fails if file exists.
  *                   <LI><TT><I>"overwrite"</I></TT> Opens file
  *                       for Output: Read/Write.  Creates file,
  *                       truncates if file exists.
  *                   <LI><TT><I>"append"</I></TT> Opens file
  *                       for Append: Read/Write.  Creates file,
  *                       appends if file exists.
  *   </UL> 
  *                 Defaults to <TT><I>"input"</I></TT>.
  * 
  * @param extension Extension to be added to filename (added only if not
  *                  already on filename). Defaults to ""
  */
  CisscalFile::CisscalFile (const string &filename, const char *openmode,
                const char *extension):TextFile(filename, openmode, extension){
      p_begindataFound = false;
  }

 /** 
  * @brief Get next line of valid data, protected
  *  
  * This method overwrites <b>TextFile</b>'s protected method of 
  * the same name.  It is called by the public overwritten method
  * <b>GetLine()</b>, as in the parent class.
  *
  * @param line String to be rewritten with contents of the
  *             next line read from file
  * @returns <b>bool</b> Indicates whether line was read
  * @throws Isis::iException::Io "Error reading text file" 
  * @see GetLine() 
  */
  bool CisscalFile::p_GetLine(string &line) {
   OpenChk(true);
   // Try to read the next line
   getline(p_stream, line);
   // Check for end of file
   if (p_stream.eof()) {
     line = "";
     return false;
   }
   // See if an error occured
   if (!p_stream.good()) {
     line = "";
     string message = "TextFile:GetLine: -> Error reading text file: ["
                    + p_filename + "]";
     throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);    
   }
   // Search for tag "\begindata" if it was not already found by recursively using this method
     if (!p_begindataFound) {
       if(line.find("\\begindata") == string::npos){
         return p_GetLine(line);
       }
       p_begindataFound = true;
       return p_GetLine(line);
     }
   // We have a good line
   return true;
  }

 /** 
  * @brief Get next line of valid data 
  *  
  * This protected method overrides the base class method from  
  * <b>TextFile</b> of the same name. It finds the next line of 
  * valid data. If the <tt>"\begindata"</tt> tag has not been 
  * already found, it searches for that tag. Once found, every 
  * line beyond that is considered data and the method will 
  * retrieve the next line. Returns True if read a line, False if 
  * End Of File. 
  *
  * @param line String to be rewritten with contents of the
  *             next line read from file
  * @returns <b>bool</b> Indicates whether line was read
  * @throws Isis::iException::Io "Error reading text file" 
  * @see p_GetLine() 
  */
  bool CisscalFile::GetLine(string &line) {
    return p_GetLine(line);
  }

}//end CisscalFile.cpp
