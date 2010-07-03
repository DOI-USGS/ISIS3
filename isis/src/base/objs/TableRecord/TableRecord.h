#if !defined(TableRecord_h)
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
  *            capablilities.
  *   @history 2008-06-19 Christopher Austin - Fixed the Packing of text
  *            TableFields
  *   @history 2008-06-25 Christopher Austin - Fixed the swapping of text
  *   @todo Finish class documentation
  */
  class TableRecord {
    public:
      //! Constructs a TableRecord object
      TableRecord() {};

      //! Destroys the TableRecord object
      ~TableRecord() {};

     /**
      * Returns the number of fields that are in the record
      * 
      * @return The number of fields in the record
      */
      int Fields() const { return p_fields.size(); };

     /**
      * Adds a TableField to a TableRecord
      * 
      * @param field - TableField to be added to the record
      */
      void operator+=(Isis::TableField &field) { p_fields.push_back(field); };

      int RecordSize() const;

     /**
      *  Returns the TableField at the specified location in the TableRecord
      * 
      * @param field  Index of desired field
      * 
      * @return The TableField at specified location in the record
      */
      Isis::TableField &operator[](const int field) { return p_fields[field]; };

      Isis::TableField &operator[](const std::string &field);
  
      void Pack(char *buf) const;
      void Unpack(const char *buf);
      void Swap(char *buf) const;
  
    private:
      std::vector<Isis::TableField> p_fields; /**<Vector of TableFields in the 
                                                  record. */
  };
};

#endif
