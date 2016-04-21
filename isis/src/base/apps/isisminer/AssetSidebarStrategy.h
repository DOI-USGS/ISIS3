#ifndef AssetSidebarStrategy_h
#define AssetSidebarStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: AssetSidebarStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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
#include "SidebarStrategy.h"

// Qt library
#include <QString>

// for ResourceList, SharedResource, and SharedStrategy typedefs
#include "Resource.h"
#include "Strategy.h"

namespace Isis {
  class PvlObject;

  /**
   * @brief AssetSidebarStrategy - allows assets to be processed with most strategies.
   * 
   * This strategy gives users the ability to process assets with most other isisminer 
   * strategies. 
   * 
   * If the user gives the Operation keyword a value of "create", then this strategy will create
   * new assets to add to the active resources. In other words, the specified strategy will not be 
   * applied to any existing assets contained in the active resources. Therefore, this operation
   * would be useful with strategies that create/obtain resources, such as PvlReader. 
   * The created assets would then added to the active resources.
   * 
   * If the Operation keyword does not exist or has a value other than "create", then this 
   * AssetSidebar strategy will apply the provided strategy to the assets contained in the active
   * resources and add the processed assets to the active resources.
   *  
   * @code
   * Object = Strategy
   * Name        = TestCreateAsset
   * Type        = AssetSidebar
   * Asset       = AssetA
   * Operation   = create
   * Description = "Adds AssetA (assets read from test.csv) to the active resources"
   *  Object = IsisMiner
   *    Object = Strategy
   *      Name = ReadAssetTest
   *      Type = CsvReader
   *      CsvFile = "test.csv"
   *      HasHeader = True
   *      SkipLines = 0
   *      IgnoreComments = False
   *      Delimiter = ", "
   *    EndObject
   *  EndObject
   * EndObject
   * @endcode
   *  
   * @author 2012-07-15 Kris Becker
   * @internal
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-05-01 Ian Humphrey - Updated documentation.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-09-15 Kris Becker - Reworked how the create option is
   *                         implemented.
   */
  class AssetSidebarStrategy : public SidebarStrategy {
  
    public:
      AssetSidebarStrategy();
      AssetSidebarStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~AssetSidebarStrategy();
  
    protected:
      virtual int preRunProcess(ResourceList &resources, const ResourceList &globals);

      virtual int apply(SharedStrategy &strategy, ResourceList &resources,
                        const ResourceList &globals);

      
      virtual int postRunProcess(ResourceList &resources, const ResourceList &globals);
  
    private:
      enum CreateSource{ FromNone, FromCopy, FromClone };
      QString      m_asset;         //!< Name (identifier) of the asset to process
      bool         m_create;        //!< Is the opertatio to create an asset?
      bool         m_removeEmpties; //!< Remove asset if empty after all have been processed
      CreateSource m_source;        //!< Source of Asset list creation

  };

} // Namespace Isis

#endif
