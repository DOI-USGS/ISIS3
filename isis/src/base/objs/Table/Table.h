#if !defined(Table_h)
#define Table_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/14 19:17:09 $
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

#include "Blob.h"
#include "TableRecord.h"

namespace Isis {
  /**
   * @brief Read table blobs
   *
   * This class reads table blobs from the cubes (or detached
   * tables).  It is record based, N records in a table.  Each
   * record will have the same number of fields, F. The fields can
   * be of different types including integer, string, and double.
   * See the classes TableRecord and TableField for more
   * information.  The class uses PVL to store the structure of
   * the table N, F, and Field types and binary to store the table
   * data.
   *
   * If you would like to see Table being used in implementation, see histats.cpp
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2004-09-01 Jeff Anderson
   *
   * @internal
   * @history 2005-03-18 Elizabeth Ribelin - Added documentation
   *          to class
   * @history 2006-09-19 Jeff Anderson - Added clear method
   * @history 2006-09-19 Jeff Anderson - Fixed bug in ReadInit
   *          method which needed to cleanup pointers to records
   *          if a re-read occurred.
   * @history 2009-02-18 Steven Lambright - Added copy constructor and
   *          assignment operator.
   */
  class Table : public Isis::Blob {
    public:
      // Constructors and Destructors
      Table(const std::string &tableName);
      Table(const std::string &tableName, Isis::TableRecord &rec);
      Table(const std::string &tableName, const std::string &file);
      Table(const Table &other);

      ~Table();

      // Read a record
      Isis::TableRecord &operator[](const int index);

      // Add a record
      void operator+=(Isis::TableRecord &rec);

      Table &operator=(const Isis::Table &other);

      // Update a record
      void Update(const Isis::TableRecord &rec, const int index);

      // Delete a record
      void Delete(const int index);

      /**
       * Returns the number of records
       *
       * @return Number of records
       */
      inline int Records() const {
        return p_recbufs.size();
      };

      /**
       * Returns the number of bytes per record
       *
       * @return Number of bytes per record
       */
      inline int RecordSize() const {
        return p_record.RecordSize();
      };

      enum Association { None, Samples, Lines, Bands };

      /**
       * Sets the association to the input parameter
       *
       * @param assoc Association type
       */
      void SetAssociation(const Table::Association assoc) {
        p_assoc = assoc;
      }

      /**
       * Checks to see if association is Samples
       *
       * @return Returns true if association is Samples, and false if it is not
       */
      bool IsSampleAssociated() {
        return (p_assoc == Table::Samples);
      };

      /**
       * Checks to see if association is Lines
       *
       * @return Returns true if association is Lines, and false if it is not
       */
      bool IsLineAssociated() {
        return (p_assoc == Table::Lines);
      };

      /**
       * Checks to see if association is Bands
       *
       * @return Returns true if association is Bands, and false if it is not
       */
      bool IsBandAssociated() {
        return (p_assoc == Table::Bands);
      };

      void Clear();

    protected:
      void ReadInit();
      void ReadData(std::istream &stream);
      void WriteInit();
      void WriteData(std::fstream &os);

      Isis::TableRecord p_record;    //!<
      std::vector<char *> p_recbufs; //!<

      int p_records; /**<Holds record count read from labels, may differ from
                         the size of p_recbufs.*/

      Association p_assoc; //!< Association Type of the table
      bool p_swap;         //!< Only used for reading
  };
};

#endif

