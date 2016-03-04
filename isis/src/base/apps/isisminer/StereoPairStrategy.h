#ifndef StereoPairStrategy_h
#define StereoPairStrategy_h
/**
 * @file                                                                  
 * $Revision: 6513 $ 
 * $Date: 2016-01-14 16:04:44 -0700 (Thu, 14 Jan 2016) $ 
 * $Id: StereoPairStrategy.h 6513 2016-01-14 23:04:44Z kbecker@GS.DOI.NET $
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
#include "GisOverlapStrategy.h"
#include "CalculatorStrategy.h"

// Qt library
#include <QMap>
#include <QScopedPointer>
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
   * @brief StereoPairStrategy Compute stereo pair parameters from Resources 
   *  
   * This Strategy object computes stereo compliments of a a given image but 
   * using the GisOverlapStrategy to determine which geometry provides the 
   * appropriate overlap defined by constraints. 
   *  
   * Once all overlaps have been defined, they are recorded in a parameter named 
   * by the AssetName keyword (Default: GisOverlap). See the GisOverlap strategy 
   * definition for keywords in that strategy that will also apply to 
   * specification and use of this strategy. 
   *  
   * By using the GisOverlapStrategy as a base class we are able to take 
   * advantage of the virtual methods of that class, overlapCandidates() and 
   * processOverlap(), which are reimplemented in this class for extended 
   * functionality. 
   *  
   * overlapCandidates() method is used to appy the level 1 stereo candidate 
   * screening parameters such as resolution and phase, emission and incidence 
   * angle constraints. 
   *  
   * The provision of merging the overlapping pairs provided by the GisOverlap 
   * strategy is utilized here but stereo computations are performed on the two 
   * distinct Resources so keyword maps should specify keywords in that domain 
   * and not the mereged one. 
   *  
   * This strategy will take the list of all overlaping geometries and apply 
   * named keywords that are required to compute the stereo aspects of the 
   * overlaping geometries. This necessary keywords that typically satisfy the 
   * keyword requirements are provided by runs of caminfo.  
   *  
   * processOverlap() computes the stereo strength parameters 
   * ThresholdConstraintsRankA, ThresholdConstraintsRankB, 
   * ThresholdConstraintsRank (which is actually just 
   * ThresholdConstraintsRankA), ParallaxHeighRatio,  ShadowTipDistance, 
   * ResolutionRatio, DeltaSunAzimuthDifference and DeltaSPCAzimuthDifference 
   * (in degrees), VerticalPrecision and finally StereoConstraitsRank. 
   *  
   * The ranking of the total strenght of the stereo is computed using the 
   * CalculatorStrategy. It has the same construct so include the computation of 
   * a stereo index ranking using any of the composite image A or B merged 
   * keywords or ones created by this Strategy. 
   *  
   * @code 
  Object = Strategy
    Type = StereoPair
    Name = DegausStereoPairs
    Description = "Find all stereo pairs in the Degaus Crater region"
    OverlapMerge = None
    ShowProgress = True
    # Debug = true
`
    PixelPrecisionMatch  = 0.2
   
    #  Now specify how the stereo quality is determined.
    Equation = "thresholdconstraintsrank * 1.0 + stereoconstraintsrank * 1.0"
    Result = StereoPairRank

    #  First level contraints
    Group = ImageStrength
      Incidence       = (  30.0,  65.0,  50.0 )
      Emission        = (   0.0,  55.0,  15.0 )
      Phase           = (  15.0, 120.00, 30.0 )
      Resolution      = ( 25.0, 5000.0, 200.0 )
    EndGroup

    #  Second level constraints.
    Group = StereoStrength
      ResolutionRatio      = (  1.0,    2.5,   1.0 )
      ParallaxHeightRatio  = (  0.4,    0.6,   0.4 )
      ShadowTipDistance    = (  0.0,    0.5,   0.0 )
      OverlapRatio         = (  0.3,    1.0,   0.5 )
    EndGroup

    #  This shows the keyword mappings to values required for stereo
    #  matching.
    Group = KeywordMap
      Resolution = PixelResolution
      Emission   = EmissionAngle
      Incidence  = IncidenceAngle
      Phase      = PhaseAngle
      ParallaxX  = ParallaxX
      ParallaxY  = ParallaxY
      ShadowX    = ShadowX
      ShadowY    = ShadowY
    EndGroup

    #  Select candidates from input list.  Matches over itself
    Object = IsisMiner
      Object = Strategy
        Name = PairCandidates
        Type = ResourceManager
        Operations = ResetDiscard
      EndObject
    EndObject
  EndObject
  * 
   * @endcode 
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-05-08 Kris Becker - Modify constructor to take a global
   *                          resources list; modified apply() method to accept
   *                          a global resource list.
  *    @history 2015-05-15 Kris Becker - Added pixel precision match variable
  *    @history 2015-05-29 Kris Becker - Completed implementation and
  *                           documentation for the source code
   */
  class StereoPairStrategy : public GisOverlapStrategy {
  
    public:
      StereoPairStrategy();
      StereoPairStrategy(const PvlObject &definition, 
                         const ResourceList &globals);
      virtual ~StereoPairStrategy();

      virtual int apply(ResourceList &resource, const ResourceList &globals);

    protected:
      virtual ResourceList overlapCandidates(ResourceList &resources, 
                                             const ResourceList &globals);
      virtual SharedResource processOverlap(SharedResource &resourceA,
                                            SharedResource &resourceB,
                                            const double &ovrRatioA,
                                            const double &ovrRatioB, 
                                            const ResourceList &globals);
  
    private:
      /**
       * Definition of ConstraintList, a map between a string (source) and parameter 
       * constraints 
       */
      typedef QMap<QString, QVector<double> > ConstraintList;
  
      ConstraintList           m_imageStrength;   //!< Image strength specifications
      ConstraintList           m_stereoStrength; //!< Stereo strength specifications
      PvlFlatMap               m_keywordMap;     //!< Mapping of keyword values to keys
      QScopedPointer<Strategy> m_calculator;     //!< Rank calculator
      double                   m_pixelPrecisionMatch; //!< Used for validation
      bool                     m_useStereoAngle; //!< Use stereo angle if it
                                                 //!< exists for VerticalPrecision 
      mutable bool             m_myDebug;


      QString getStereoValue(const QString &key, const ResourceList &globals,
                             const QString &defValue = "") const;
      bool computeStereo(const SharedResource &resourceA, 
                          const SharedResource &resourceB,
                          SharedResource &composite,
                          const ResourceList &globals) const;
      ConstraintList getConstraints(const PvlContainer &constraints) const;
      bool passConstraints(const SharedResource &resource, 
                           const ConstraintList &constraints) const;
      bool rankConstraints(const SharedResource &resource, 
                           const ConstraintList &constraints, 
                           double &rank,
                           const SharedResource &out = SharedResource(),
                           const QString &suffix = "Rank") const;
      bool computeDelta(const SharedResource &resourceA, 
                          const SharedResource &resourceB, 
                          const QString &parameter, 
                          const QString &keysrc,
                          SharedResource &composite) const;
      bool computeStereoAngle(const SharedResource &resourceA, 
                              const SharedResource &resourceB, 
                              SharedResource &stereo,
                              const ResourceList &globals) const;
      double computeRank(const double &value, 
                         const QVector<double> &thresholds) const;
  };

} // Namespace Isis

#endif
