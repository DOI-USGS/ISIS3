#ifndef GisOverlapStrategy_h
#define GisOverlapStrategy_h
/**
 * @file                                                                  
 * $Revision: 6109 $ 
 * $Date: 2015-03-18 17:58:32 -0700 (Wed, 18 Mar 2015) $ 
 * $Id: GisOverlapStrategy.h 6109 2015-03-19 00:58:32Z jwbacker@GS.DOI.NET $
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
#include "CalculatorStrategy.h"

// Qt library
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVector>

// These contain SharedResource and ResourceList StrategyList typedefs
#include "PvlFlatMap.h"
#include "Resource.h"
#include "Strategy.h"

namespace Isis {

  class PvlKeyword;
  class PvlObject;

  /**
   * @brief GisOverlapStrategy determines Resources that share common geometry 
   *  
   * This class takes a Resource list and determines other Resources that 
   * contain GIS geometries and computes overlaps. Resources that have overlaps 
   * will have composite Resources created from the merged result of the 
   * overlaping pairs that result in a new Resource added to the named asset. 
   *  
   * This is a potentially useful Strategy for other Strategies that may decide
   * to inherit this class. Therefore, it contains several useful virtual 
   * methods that can be reimplemented by the inheriting class. See 
   * processOverlaps() and processOverlap(). 
   *  
   * @author 2015-03-26 Kris Becker 
   * @internal 
   *   @history 2015-03-26 Kris Becker - Original version.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
   *   @history 2015-06-15 Kris Becker - Ensure at least 2 entries are allocated
   *                          for GOES RTree allocations (required by GEOS)
   *   @history 2017-01-06 Jesse Mapel - Made the "Total Geoms" output a debug
   *                          statement. Fixes #4581.
   */
  class GisOverlapStrategy : public Strategy {
  
    public:
      GisOverlapStrategy();
      GisOverlapStrategy(const PvlObject &definition, 
                         const ResourceList &globals);
      virtual ~GisOverlapStrategy();
  
      virtual int apply(ResourceList &resources, const ResourceList &globals);
      
    protected:
      enum GisMergeOption { None, Default, Intersection, Union, Centroid, 
                            ResourceA, ResourceB };

      GisMergeOption mergeOption() const;
      void setAssetName(const QString &assetName);

      // These methods can be overridden by the deriving class if so desired
      virtual ResourceList overlapCandidates(ResourceList &resources,
                                             const ResourceList &globals);
      virtual ResourceList processOverlaps(SharedResource &resource,
                                           ResourceList &overlaps,
                                           const ResourceList &globals);
      virtual SharedResource processOverlap(SharedResource &resourceA,
                                            SharedResource &resourceB,
                                            const double &ovrRatioA,
                                            const double &ovrRatioB,
                                            const ResourceList &globals);

      StrategyList getMinerStrategies(const QString &minerName, 
                                      const ResourceList &globals) const;

      QString suffixA() const;
      QString suffixB() const;

    private:
      double         m_overlapMin;     //!< Minimum allowable overlap ratio
      double         m_overlapMax;     //!< Maximum allowable overlap ratio
      QString        m_ratioKey;       //!< Keyword to store ratio of intersection
      QString        m_assetName;      //!< Name of asset to create with commons
      GisMergeOption m_merge;          //!< Merge option
      QString        m_suffixA;        //!< Keyword suffix for stereo source
      QString        m_suffixB;        //!< Keyword suffix for stereo match
      StrategyList   m_pairStrategies; //!< Candidate selection strategies
      StrategyList   m_overlapStrategies; //!< Process each overlap set as it is determined

  };

} // Namespace Isis

#endif
