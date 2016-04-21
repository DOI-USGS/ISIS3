#ifndef StrategyFactory_h
#define StrategyFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: StrategyFactory.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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

// StrategyList and SharedResource typedefs
#include "Resource.h"
#include "Strategy.h"

class QString;

namespace Isis {

  class PvlObject;

  /**
   * A class used to create a Strategy object from a PVL definition object and a shared pointer to a 
   * global Resource of keywords. This class designed using a singleton and factory design patterns.
   * The instantiation of this class is restricted to a single object using the instance() method.
   * The create() method is used to construct a Strategy object. To build several Strategy objects, 
   * the buildRun() method may be called using a PVL. 
   *
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-04-03 Kristin Berry - Removed reference to deleted class, GisBasicStrategy.
   *   @history 2015-03-26 Jeannie Backer - Updated documentation. 
   *   @history 2015-03-28 Kris Becker - Added GisOverlapStrategy; removed
   *                          MsgrNacWacMatchupStrategy.
   *   @history 2015-04-28 Kris Becker - Accomodate renaming of DatabaseStrategy
   *                          to DatabaseReaderStrategy.
   *   @history 2015-05-08 Kris Becker - Added global parameter storage;
   *                          implemented plugin capability for Strategies
   *   @history 2015-06-04 Kris Becker - Added validateUserParameters() to check
   *                          for parameters required by users.
   *  
   *   @history 2015-07-17 Kris Becker - Added the PdsTableReader strategy
   */
  class StrategyFactory {
    public:
      static StrategyFactory *instance();

      void addGlobal(SharedResource &global);
      const ResourceList &getGlobals() const;
  
      StrategyList buildRun(const QString &configFile) const;
      StrategyList buildRun(const PvlObject &config) const;
      Strategy *create(const PvlObject &definition) const;


      StrategyList buildRun(const QString &configFile, const ResourceList &globals) const;
      StrategyList buildRun(const PvlObject &config, const ResourceList &globals) const;
      Strategy *create(const PvlObject &definition, const ResourceList &globals) const;
  
      int manufactured() const;
  
    private:
      StrategyFactory();
      ~StrategyFactory();
  
      Strategy *findStrategy(const PvlObject &definition, 
                             const ResourceList &globals) const;
      Strategy *loadStrategyPlugin(const PvlContainer &plugindef, 
                                   const PvlObject &definition,
                                   const ResourceList &globals) const;

      void validateUserParameters(const PvlContainer &conf, 
                                  const ResourceList &parameters) const;
  
      static void dieAtExit();
      void destruct();
  
      static StrategyFactory *m_strategymaker; /**< A static member variable representing the 
                                                    single instance of this strategy factory.*/
      ResourceList            m_globals;       //!< List of global parameters for strategies

      mutable int m_numberMade; /**< Number of manufactured strategies. This variable is 
                                     incremented each time create() is invoked. */
  
  };

}  // namespace Isis

#endif
