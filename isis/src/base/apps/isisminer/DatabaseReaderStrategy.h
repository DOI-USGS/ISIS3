#ifndef DatabaseReaderStrategy_h
#define DatabaseReaderStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: DatabaseReaderStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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
#include <QString>

// ResourceList and SharedResource typedefs
#include "Resource.h"

namespace Isis {

  class Database;
  class PvlObject;
  class SqlRecord;
  
  /**
   * @brief DatabaseReaderStrategy - Provides direct SQL queries to databases.
   *
   * This Strategy provides SQL queries to databases to read from records to
   * resources or write from resources to records. The query can contain
   * resource keywords which are replaced by their values. Created resources
   * can be stored in the global resource list or as assets in each resource in
   * the list.
   *
   * @code
   * Object = Strategy
   *   Name = MDISSelection
   *   Type = DatabaseReader
   *
   *   DbConfig = mdis.conf
   *   DbProfile = MDIS
   *   Mode = Select
   *
   *   Query = "select  i.SourceProductId, i.YearDoy, 
   *             g.StartTime, i.EtStartTime, i.ExposureDuration,
   *             g.CenterLongitude, g.CenterLatitude, i.ObservationType,
   *             g.PixelResolution, g.MeanGroundResolution,
   *             g.IncidenceAngle, g.EmissionAngle, g.PhaseAngle, 
   *             g.SubSolarGroundAzimuth, g.SubSpacecraftGroundAzimuth, 
   *             g.ParallaxX, g.ParallaxY, g.ShadowX, g.ShadowY,
   *             p.GisFootprint
   *               from Polygon p
   *               INNER JOIN Image i
   *               ON p.SourceProductId=i.SourceProductId 
   *               INNER JOIN Geometry g
   *               ON p.SourceProductId=g.SourceProductId 
   *               INNER JOIN Statistics s
   *               ON p.SourceProductId=s.SourceProductId
   *               where (i.Center=747.7 or i.Center=748.7) 
   *                order by g.PixelResolution"
   *   Target = Resource
   *   Identity = "%1"
   *   IdentityArgs = "SourceProductId"
   *   GisGeometryRef = GisFootprint
   *   GisType        = WKB
   *   RemoveGisKeywordAfterImport = True
   * EndObject
   *
   * @endcode
   *
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version. 
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-04-23 Jeffrey Covington - Documented class, members, and
   *                       methods. Added DbConfigArgs parameter.
   *   @history 2015-05-01 Jeffrey Covington - Added DbFile and DbFileArgs
   *                       parameters for application testing.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-06-11 Kris Becker add special DbDisconnect function to
   *                         properly close the database connection and delete
   *                         resources.
   *   @history 2015-07-13 Kris Becker - Changed AssetName parameter to Asset to
   *                         be consistent with other strategies
   */
  class DatabaseReaderStrategy : public Strategy {
  
    public:
      DatabaseReaderStrategy();
      DatabaseReaderStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~DatabaseReaderStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
  
    private:
      Database *connect( const ResourceList &globals ) const;
      QString  configureQuery(const ResourceList &globals) const;
      int executeQuery(ResourceList &resource, const ResourceList &globals) const;
      SharedResource importQuery(const QString &rowId,const SqlRecord *record,
                                 const ResourceList &globals) const;
  
      Database    *m_db;         //!< The Database object of the database connection
      // Strictly an unmanaged reference to current *db
  
  };

} // Namespace Isis

#endif
