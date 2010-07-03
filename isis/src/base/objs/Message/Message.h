/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/06/19 15:56:43 $                                                                 
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

#ifndef Message_h
#define Message_h

#include <string>
#include <vector>

namespace Isis {
  /* Documentation for this namespaceis in:
  *    isis/src/docsys/Object/build/isisDoxyDefs.doxydef
  */
  namespace Message {
    using namespace std;
    /**
     * This error should be used when an Isis object or application is checking
     * array bounds and the legal range has been violated.
     * 
     * @param index - Array index which is out of bounds
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string ArraySubscriptNotInRange (const int index); 

    /**
     * This error should be used when a supplied keyword for an Isis application
     * parameter does not contain enough characters to distinguish it from 
     * another keyword.
     * 
     * @param key - The name of the keyword which is ambiguous
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string KeywordAmbiguous (const string &key);

    /**
     * This error should be used when a supplied keyword for an Isis application
     * parameter does not match any of the parameters for that application.
     * 
     * @param key - The name of the keyword which is unrecognized
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string KeywordUnrecognized (const string &key);

    /**
     * This error should be used when a supplied keyword for an Isis application
     * parameter is the same as one previously supplied.
     * 
     * @param key - The name of the keyword which has been duplicated
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string KeywordDuplicated (const string &key);

    /**
     * This error should be used when a supplied keyword is being used as an
     * array (i.e., with a subscript) but is only a scaler entity.
     *  
     * @param key - The name of the keyword which is not an array
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string KeywordNotArray (const string &key);

    /**
     * This error should be used when a supplied keyword is not defined in the
     * application.
     * 
     * @param key - The name of the keyword which was not found
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string KeywordNotFound (const string &key);

    /**
     * This error has been deprecated and should not be used in new code.
     * 
     * @param block - The name of the invalid keyword block
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordBlockInvalid (const string &block);

    /**
     * This error has been deprecated and should not be used in new code.
     * 
     * @param block - The name of the block with the missing start tag
     * @param found - The string found in the place of the missing block start
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordBlockStartMissing (const string &block, const string &found);

    /**
     * This error has been deprecated and should not be used in new code.
     * 
     * @param block - The name of the block with the missing end tag
     * @param found - The string found in the place of the missing block end
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordBlockEndMissing (const string &block, const string &found);

    /**
     * This error should be used when a supplied keyword does not appear in
     * the list (e.g., an Isis cube label).
     * 
     * @param key - The name of the keyword which is bad
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordValueBad (const string &key);

    /**
     * This error should be used when the value of a supplied keyword is
     * incorrect.
     * 
     * @param key - The keyword name which is bad
     * @param value - The value of the keyword which is bad
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordValueBad (const string &key, const string &value);

    /**
     * This error should be used when the value of a supplied keyword was
     * expected but not present.
     * 
     * @param key - The keyword name which is bad
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordValueExpected (const string &key);

    /**
     * This error should be used when the value of a supplied keyword is
     * not within a specific range.
     * 
     * @param key - The keyword name which has a bad value
     * @param value - The value of the keyword
     * @param range - The minimum and maximum allowed values for the keyword
     *                formatted and inserted into a string
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordValueNotInRange (const string &key, const string &value,
                                   const string &range);

    /**
     * This error should be used when the value of a supplied keyword is
     * not one of a specific list of possible values.
     * 
     * @param key - The keyword name which has a bad value 
     * @param value - The value of the keyword 
     * @param list - A vector of all the legal values for the keyword 
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string KeywordValueNotInList (const string &key, const string &value,
                                  const vector<string> &list);
  
    /**
     * This error should be used when a delimiter is missing.
     * 
     * @param delimiter - The delimiter that is missing
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string MissingDelimiter (const char delimiter);

    /**
     * This error should be used when a delimiter is missing.
     * 
     * @param delimiter - The delimiter that is missing
     * @param near - The value the missing delimiter is near
     * 
     * @return string - A standardized Isis error message with the parameter  
     *                  inserted
     */
    string MissingDelimiter (const char delimiter, const string &near);
  
    /**
     * This error should be used when a file could not be opened.
     * 
     * @param filename - The name of the file
     * 
     * @return string - A standardized Isis error message with the parameter  
     *                  inserted
     */
    string FileOpen (const string &filename);

    /**
     * This error should be used when a file could not be created.
     * 
     * @param filename - The name of the file
     * 
     * @return string - A standardized Isis error message with the parameter 
     *                  inserted
     */
    string FileCreate (const string &filename);

    /**
     * This error should be used when an error accrues during a read of a file.
     * 
     * @param filename - The name of the file
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string FileRead (const string &filename);

    /**
     * This error should be used when an error accrues during a write to a file.
     * 
     * @param filename - The name of the file
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string FileWrite (const string &filename);

    /**
     * This error should be used when an error accrues during a memory
     * allocation such as "new".
     * 
     * @return string - A standardized Isis error message with the parameter
     *                  inserted
     */
    string MemoryAllocationFailed ();
  }
}

#endif
