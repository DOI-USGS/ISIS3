#ifndef PvlFormat_h
#define PvlFormat_h
/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/02/04 22:36:41 $
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

#include <map>
#include <string>

#include "Pvl.h"

#include "PvlKeyword.h"

namespace Isis {

  //! The different types of keywords that can be formatted
  enum KeywordType { NoTypeKeyword,
                     StringKeyword,
                     BoolKeyword,
                     IntegerKeyword,
                     RealKeyword,
                     OctalKeyword,
                     HexKeyword,
                     BinaryKeyword,
                     EnumKeyword };

  /**
  * Convert a string representing a type of keyword to the corresponding
  * enumeration. All white space, quotes, underscores, and dashes will be
  * removed from the input string.
  * 
  * @param type The string to be converted.
  * @return The corresponding KeywordType enum.
  */
  inline KeywordType ToKeywordType (const std::string type) {

    iString t(type);
    t.Remove("_- \r\n\f\t\v\"\'");
    t.UpCase();

    if (t == "STRING") return StringKeyword;
    else if (t == "BOOL") return BoolKeyword;
    else if (t == "INTEGER") return IntegerKeyword;
    else if (t == "REAL") return RealKeyword;
    else if (t == "OCTAL") return OctalKeyword;
    else if (t == "HEX") return HexKeyword;
    else if (t == "BINARY") return BinaryKeyword;
    else if (t == "ENUM") return EnumKeyword;
    return NoTypeKeyword;
  }

/**
 * @brief Formats a Pvl name value pair to Isis standards.
 * 
 * This class is used to format a single PVL keyword-value pair using normal
 * Isis formatting. The class serves as a base class for others to override and
 * implement their own formatting.
 * 
 * This class uses a Pvl or Pvl formatted file to populate its internal data
 * structure. This structure is used to lookup the type of a keyword and/or
 * other information for another Pvl{Object|Group|Keyword}. The format of the
 * file or Pvl is:
 * 
 * @code
 * NAME=TYPE
 * NAME2=(TYPE,PLACES)
 * @endcode
 * 
 * Example:
 * @code
 *   DESCRIPTION = STRING
 *   MinimumLatitude = (REAL,5)
 *   BITTYPE = ENUM
 *   RECORDS = INTEGER
 * @endcode
 * 
 * Where NAME is the name of a keyword in the Pvl to be formatted. TYPE is the
 * type of keyword (STRING,BOOL,INTEGER,REAL,OCTAL,HEX,BINARY,ENUM) (see
 * ToKeywordType). PLACES is the number of digits to the right of the decimal
 * place for a keyword of TYPE REAL.
 * 
 * NOTE: The capabilities to use this Pvl are not implemented in this base
 * class. They are provided as a convience for child classes only. This class
 * only implements the normal Isis foramatting, which is not dependent on the
 * type of the keyword. It is dependent on value of the keyword.
 * 
 * @ingroup Parsing                                                  
 *                                                                        
 * @author 2006-09-05 Stuart Sides
 *                                                                        
 * @internal
 *  @history 2006-09-05 Stuart Sides - Original version
 *  @history 2008-09-30 Christopher Austin - added FormatEOL()
 *  @history 2009-12-17 Steven Lambright - {} are now treated the same as ()
 *  @history 2010-02-04 Travis Addair - Added SetCharLimit
 *           method allowing users to set the point at which a
 *           keyword value is output to the next line down.
 */                                                                       
  class PvlFormat {

    public:

      PvlFormat();
      PvlFormat(const std::string &file);
      PvlFormat(Pvl &keymap);
      virtual ~PvlFormat () {};

      void Add(const std::string &file);
      void Add(Pvl &keymap);

      /**
       * Sets the maximum number of characters in a keyword value that
       * can be printed to a line before it wraps to the next line. By
       * default, the limit is set to 80 characters. 
       *  
       * @param limit The new character limit. 
       */
      void SetCharLimit(const unsigned int limit) { p_charLimit = limit; };

      /**
       * Retrieves the maximum number of characters in a keyword value
       * that can be printed to a line before it wraps to the next 
       * line. By default, the limit is set to 80 characters. 
       *  
       * @return <B>unsigned int</B> Maximum number of characters.
       */
      unsigned int CharLimit() const { return p_charLimit; };

      virtual std::string FormatValue (const PvlKeyword &keyword,
                                       int valueIndex = 0);
      virtual std::string FormatName (const PvlKeyword &keyword);
      virtual std::string FormatEnd (const std::string name,
                                     const PvlKeyword &keyword);
      virtual std::string FormatEOL () { return "\n"; }

      virtual KeywordType Type (const PvlKeyword &keyword);
      virtual int Accuracy (const PvlKeyword &keyword);

    protected:

      virtual std::string AddQuotes (const std::string value);
      bool IsSingleUnit (const PvlKeyword &keyword);

      std::string p_keywordMapFile;
      Pvl p_keywordMap;

      //! Maximum number of characters on a single line of a keyword value.
      unsigned int p_charLimit;

    private:
      void Init();
  };
};

#endif

