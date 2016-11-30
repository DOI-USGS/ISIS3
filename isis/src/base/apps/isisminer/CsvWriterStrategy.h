#ifndef CsvWriterStrategy_h
#define CsvWriterStrategy_h
/**
 * @file
 * $Revision: 6513 $
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $
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

// std library
#include <fstream>

// Qt library
#include <QString>
#include <QStringList>

// geos library
#include <geos_c.h>

// ResourceList, SharedResource typedefs
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief CsvWriterStrategy - Writes resources to a CSV file.
   *
   * This strategy is used to write Resources to a Comma-Separated-Value
   * format (CSV) file. Each resources is written as a row, separated by new
   * lines, and the specified keyword values are writen to the columns,
   * separated by the delimiter character. If the header is selected the first
   * row written to the CSV file will contain the keyword names.
   *
   * Here is an example of the CsvWriter Strategy object definition:
   *
   * @code
   * Object = Strategy
   *   Type = CsvWriter
   *   Name = mdismla
   *   Filename = "mdis_mla_ridelong.lis"
   *   Mode = Create
   *   Header = true
   *   Keywords = (YearDoy, SourceProductId, StartTime, EtStartTime,
   *               ExposureDuration, CenterLongitude, CenterLatitude,
   *               PixelResolution, MeanGroundResolution,
   *               IncidenceAngle, EmissionAngle, PhaseAngle,
   *               SubSolarGroundAzimuth, SubSpacecraftGroundAzimuth,
   *               ParallaxX, ParallaxY, ShadowX, ShadowY")
   *   Delimiter = ","
   *   DefaultValue = "NULL"
   * EndObject
   * @endcode
   *
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-02-26 Jeffrey Covington - Added documentation.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-04-16 Jeffrey Covington - Added the ability to write Resource
   *                           geometries to the CSV file. Updated documentation and test.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-06-11 Kris Becker - Ensure GIS keyword takes precidence
   *                          over any other Resource keyword
   *   @history 2016-08-28 Kelvin Rodriguez - Added using Strategy::apply; to avoid
   *                           hidden virtual function warnings in clang.
   *                           Part of porting to OS X 10.11.
   */
  class CsvWriterStrategy : public Strategy {

    public:
      CsvWriterStrategy();
      CsvWriterStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~CsvWriterStrategy();

      virtual int apply(ResourceList &resources, const ResourceList &globals);

    protected:
      using Strategy::apply;
      int apply(SharedResource &resource, std::ofstream &os);

    private:
      void csvwrite(std::ofstream &os, SharedResource &resource,
                    const QStringList &keywords, const QString &delimiter,
                    const QString &defValue, const QString &gisKey,
                    const QString &gisType, const ResourceList &globals);

      QString     m_filename;       //!< The name of the file to write to.
      QString     m_mode;           //!< The mode to write to the file in.
      QStringList m_keywords;       //!< The keywords to be writen to the file.
      bool        m_header;         /**< Indicates whether to write a header to the file. Defaults
                                         to true on construction.*/
      QString     m_delimiter;      //!< The delimiter character for columns.
      QString     m_default;        /**< The default value to write. Defaults to "NULL" on
                                         construction.*/
      bool        m_skipEmptyLists; /**< Indicates whether to skip empty resources.
                                         Defaults to false on construction.*/
      QString     m_gisKey;         //!< Keyword to reference the geometry.
      QString     m_gisType;        /**< The text format to write the geometry. Defaults to "wkb"
                                         on construction.*/

  };

} // Namespace Isis

#endif
