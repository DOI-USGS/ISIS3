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
   *  @history 2013-03-11 Steven Lambright and Mathew Eis - Brought method names and member variable
   *                          names up to the current Isis 3 coding standards. Fixes #1533.
   */

  class PvlFormatPds : public PvlFormat {

    public:

      PvlFormatPds();
      PvlFormatPds(const QString &file);
      PvlFormatPds(Pvl &keymap);
      virtual ~PvlFormatPds() {};

      virtual QString formatValue(const PvlKeyword &keyword,
                                      int valueIndex = 0);
      virtual QString formatName(const PvlKeyword &keyword);
      virtual QString formatEnd(const QString name,
                                    const PvlKeyword &keyword);
      virtual QString formatEOL() {
        return "\015\012";
      }

    protected:
      virtual QString addQuotes(const QString value);

      QString formatString(const PvlKeyword &keyword, int num);
      QString formatInteger(const PvlKeyword &keyword, int num, int bytes);
      QString formatReal(const PvlKeyword &keyword, int num, int precision);
      QString formatEnum(const PvlKeyword &keyword, int num);
      QString formatBinary(const PvlKeyword &keyword, int num, int bytes);
      QString formatHex(const PvlKeyword &keyword, int num, int bytes);
      QString formatBool(const PvlKeyword &keyword, int num);
      QString formatUnknown(const PvlKeyword &keyword, int num);

    private:
      void init();
  };
};

#endif

