#ifndef PvlFormatPds_h
#define PvlFormatPds_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/09/15 21:13:25 $
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

#include "PvlFormat.h"

namespace Isis {
/**
 * @brief Formats the value of a PvlKeyword into a PDS complient string
 * 
 * This class is used to format a single PVL keyword-value pair. The supported
 * formats are Normal and PDS. The keyword to type mapping is not defined until
 * a Pvl or Pvl formatted file is supplied.
 * 
 * @ingroup Parsing
 * 
 * @author 2006-09-05 Stuart Sides
 * 
 * @internal
 *  @history 2006-09-05 Stuart Sides - Original version
 *  @history 2006-12-14 Stuart Sides - Took out the upcaseing of units
 *  @history 2008-09-19 Kris Becker - Put quotes around "N/A", "NULL", "UNK";
 *                                    ensure units are placed after each element
 *                                    in array instead of one at the end and
 *                                    outside the closing right parenthesis.
 *                                    These changes bring us more in line with
 *                                    PDS compliancy.
 * @history 2008-09-30 Christopher Austin - added FormatEOL() 
 * @history 2009-09-15 Jeannie Walldren - Fixed bug where code was adding 2
 *                                        sets of quotes to N/A when formatting
 *                                        value.  These changes were made in
 *                                        AddQuotes(), FormatString() and
 *                                        FormatUnknown() methods.
 */

  class PvlFormatPds : public PvlFormat {

    public:

      PvlFormatPds();
      PvlFormatPds(const std::string &file);
      PvlFormatPds(Pvl &keymap);
      virtual ~PvlFormatPds () {};

      virtual std::string FormatValue (const PvlKeyword &keyword,
                                       int valueIndex = 0);
      virtual std::string FormatName (const PvlKeyword &keyword);
      virtual std::string FormatEnd (const std::string name,
                                     const PvlKeyword &keyword);
      virtual std::string FormatEOL () { return "\015\012"; }

    protected:
      virtual std::string AddQuotes (const std::string value);

      std::string FormatString (const PvlKeyword &keyword, int num);
      std::string FormatInteger (const PvlKeyword &keyword, int num, int bytes);
      std::string FormatReal (const PvlKeyword &keyword, int num, int precision);
      std::string FormatEnum (const PvlKeyword &keyword, int num);
      std::string FormatBinary (const PvlKeyword &keyword, int num, int bytes);
      std::string FormatHex (const PvlKeyword &keyword, int num, int bytes);
      std::string FormatBool (const PvlKeyword &keyword, int num);
      std::string FormatUnknown (const PvlKeyword &keyword, int num);

    private:
      void Init();
  };
};

#endif

