#ifndef TableRecord_h
#define TableRecord_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2008/06/25 18:13:35 $
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


#include <vector>
#include "TableField.h"

namespace Isis {
  /**
   * @brief
   *
   *
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2004-09-01 Jeff Anderson
   *
   * @internal
   *   @history 2005-03-18 Elizabeth Ribelin - Added documentation to class
   *   @history 2007-05-28 Steven Lambright - Added 4 byte float
   *                           capablilities.
   *   @history 2008-06-19 Christopher Austin - Fixed the Packing of text
   *                           TableFields
   *   @history 2008-06-25 Christopher Austin - Fixed the swapping of text
   *   @history 2012-10-04 Jeannie Backer Changed references to TableField
   *                           methods in implementation and unitTest files to
   *                           lower camel case. Added and ordered includes.
   *                           Moved method implementation to cpp. Fixed header
   *                           definition statement. Fixed indentation of
   *                           history entries. Ordered methods in cpp file.
   *                           Improved test coverage in all categories. Added
   *                           padding to control statements. References #1169.
   *  
   *   @todo Finish class documentation
   */
  class TableRecord {
    public:
      TableRecord();
      ~TableRecord();

      void operator+=(Isis::TableField &field);
      TableField &operator[](const int field);
      TableField &operator[](const QString &field);

      int Fields() const;
      int RecordSize() const;

      void Pack(char *buf) const;
      void Unpack(const char *buf);
      void Swap(char *buf) const;

    private:
      std::vector<TableField> p_fields; /**< Vector of TableFields in the
                                                  record. */
  };
};

#endif
