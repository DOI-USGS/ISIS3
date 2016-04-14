#ifndef CsvReaderStrategy_h
#define CsvReaderStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: CsvReaderStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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

// parent class
#include "Strategy.h"

// Qt library
#include <QPair>
#include <QString>
#include <QVector>

// SharedResource, ResourceList typedefs
#include "Resource.h"

namespace Isis {

  class CSVReader;
  class PvlObject;
  
  /**
   * @brief CsvReaderStrategy - Imports rows from a CSV file as Resources.
   *  
   * This strategy reads a Comma-Separated Values (CSV) file and imports each
   * row as a Resource. If the CSV file has a header, the keywords in the
   * header become the keyword names of the Resources. The values in the 
   * corresponding columns in each row become the keyword values. If the CSV
   * file does not have a header then the keywords are named according to a
   * base name and a number, such as "Column1", "Column2", etc. A column name
   * can be specified as the column to read Resource geometries from. 
   *
   * In the CSV file, the rows must be delimitted by a new line. The columns
   * must be delimitted by a single character. The number of columns in each
   * row must be the same for all rows. If allowed, comments must be on their
   * own line and preceded by a '#'.
   *
   * Here is an example of a CsvReader Strategy object definition:
   *
   * @code
   * Object = Strategy
   *   Name = ReadFilterData
   *   Type = CsvReader
   *   CsvFile        = "%1/csvreader_data.csv"
   *   CsvFileArgs    = "inputdir"
   *   HasHeader      = False
   *   SkipLines      = 0
   *   IgnoreComments = False
   *   Delimiter      = ","
   *   Identity = "%1_%2"
   *   IdentityArgs = ( SourceProductId,  StereoSource )
   * EndObject
   * @endcode  
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-05 Jeffrey Covington - Added documentation.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-04-16 Jeffrey Covington - Updated documentation. Removed test3.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-11-01 Kris Becker - Use the given header size to determine
   *                          number of columns to read from each row in the CSV
   *                          file.
   */
  class CsvReaderStrategy : public Strategy {
  
    public:
      CsvReaderStrategy();
      CsvReaderStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~CsvReaderStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
  
    private:
      /**
       * Definition for a ColumnHeader, a vector of pairs  (string and int) 
       * corresponding to the column 
       */
      typedef QVector<QPair<QString, int> >  ColumnHeader;
  
      ColumnHeader makeHeader(const CSVReader *csv) const;
      int findColumnHeader(const QString &name, const ColumnHeader &header) const;
  
      bool        m_hasHeader;      //!< Whether the CSV file has a header
      bool        m_ignoreComments; //!< Whether comments are ignored or processed
      int         m_skipLines;      //!< The number of lines to skip at the top
      QString     m_delimiter;      //!< The delimiter character of the CSV file
      QString     m_rowBase;        //!< The base name for naming Resources
  };

} // Namespace Isis

#endif
