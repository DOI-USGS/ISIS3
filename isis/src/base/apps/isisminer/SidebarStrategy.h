#ifndef SidebarStrategy_h
#define SidebarStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: SidebarStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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

// parent class - contains SharedStrategy and StrategyList
#include "Strategy.h"

// Qt library
#include <QString>

// SharedResource and ResourceList typedefs
#include "Resource.h"

namespace Isis {

  class PvlObject;

  /**
   * @brief SidebarStrategy executes an isisminer configuration on the
   *                        resources.
   *
   * This strategy provides users with the ability to execute a complete
   * isisminer configuration on the resources. It can execute the
   * configuration without affecting the activeness of the resources with
   * the "SavePoint" parameter in the object definition. It can also restore
   * any resources that are deleted in the configuration with the "SaveDelete"
   * parameter.
   *
   * @code 
   * Object = Strategy
   *   Name = SetUpResources
   *   Type = SideBar
   *   SaveDelete = false
   *   StrategyConfigFile = "%1/setup.conf"
   *   StrategyConfigFileArgs = "inputdir"
   * EndObject
   * @endcode
   *
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-04-07 Jeffrey Covinton - Documented the class, methods, and members.
   *                                          Added support for arguments for StrategyConfigFile.
   *   @history 2015-05-04 Ian Humphrey - Updated tests for increased code coverage
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   */
  class SidebarStrategy : public Strategy {
  
    public:
      SidebarStrategy();
      SidebarStrategy(const PvlObject &definition, const ResourceList &globals);
      virtual ~SidebarStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
  
    protected:
      virtual int preRunProcess(ResourceList &resources, const ResourceList &globals);
      virtual int preStrategyProcess(const int &nth, const Strategy *strategy,
                                     ResourceList &resources, 
                                     const ResourceList &globals);

      virtual int apply(SharedStrategy &strategy, ResourceList &resources,
                        const ResourceList &globals);

      virtual int postStrategyProcess(const int &nth, const Strategy *strategy,
                                      ResourceList &resources, 
                                      const ResourceList &globals);
      virtual int postRunProcess(ResourceList &resources, const ResourceList &globals);
  
    private:
      bool         m_savepoint;   //!< If true the activeness of the resources is maintained
      bool         m_savedeletes; /**!< If true any resources that are deleted
                                        in the configuration will be restored. */
      StrategyList m_strategies;  //!< The list of strategies to be applied to the resoruces.
      
  };

} // Namespace Isis

#endif
