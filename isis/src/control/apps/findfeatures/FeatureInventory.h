#ifndef FeatureInventory_h
#define FeatureInventory_h
/**
 * @file
 * $Revision$ 
 * $Date$ 
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

#include "IException.h"
#include "PvlFlatMap.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

#include <QString>
#include <QVariant>

// boost library
#include <boost/foreach.hpp>

#include "FeatureAlgorithm.h"

namespace Isis {

/**
 * @brief Wrapper for generic algorithm OpenCV3 feature matcher algorithms 
 *  
 * This class provides a generic wrapper that is intended to restore some of the 
 * functionality for these algorithms that were lost in the upgrade from version 
 * 2. Hence, the class T is intended to be an OpenCV Ptr type that contains 
 * either a Feature2D or DescriptorMatcher algorithm pointer. 
 *  
 * However, the design does lend itself to overide some the OpenCV specifics 
 * if needed. 
 *  
 * The OpenCV implementation can simply extract and populate the 
 * the PvlFlatMap variable structure to allow the default 
 * implementation to serve up the algorithm variables to the 
 * calling environment - or provide a specific implementation if 
 * inadequate. 
 *  
 * The config variable provides a direct way to store how the 
 * invocation string in the form of 
 * "algorithmname@var:value@var:value...". 
 *  
 * @author  2016-11-30 Kris Becker
 *  
 * @internal 
 *   @history 2016-11-30 Kris Becker - Original Version 
 */

class FeatureInventory {
  public:
    // Creator and storage types
    typedef Feature2DAlgorithm* (*FeatureCreator)(const PvlFlatMap &object, 
                                                  const QString &config);
    typedef DescriptorMatcherAlgorithm* (*MatcherCreator)(const PvlFlatMap &object, 
                                                  const QString &config);

    FeatureInventory();
    virtual ~FeatureInventory();

    int add(const QString &name, FeatureCreator make,
            const QStringList &alias = QStringList());

    int add(const QString &name, MatcherCreator maker,
            const QStringList &alias = QStringList());

    FeatureAlgorithmPtr getFeature(const QString &config) const;
    FeatureAlgorithmPtr getDetector(const QString &config) const;
    FeatureAlgorithmPtr getExtractor(const QString &config) const;
    MatcherAlgorithmPtr getMatcher(const QString &config) const;

    PvlObject info(const QString &name = "Algorithms") const;
    PvlObject info(const QStringList algorithms,
                   const QString &name = "Algorithms") const;

    QStringList detectorNames() const;
    QStringList extractorNames() const;
    QStringList feature2DNames() const;
    QStringList matcherNames() const;
    QStringList allNames() const;

    QStringList parse(const QString &config, const QString &sep) const;
    PvlFlatMap parameters(const QStringList &fromConfig) const;
    PvlKeyword aliases(const QString algorithmName) const;
    PvlObject algorithmInfo(const QString algorithmName) const;

    template <class T> int addFeatureAlgorithm() {
      int numAliases(0);
      FeatureAlgorithmPtr algorithm = T::create( PvlFlatMap() );
      FeatureCreator maker = T::create;
      QString algorithmName = algorithm->name().toLower();

      m_theFeatureCreator.insert(algorithmName, maker);
      numAliases++;
      if ( algorithm->hasDetector() ) {
        m_theFeatureCreator.insert("detector." + algorithmName, maker);
        m_detectorNames.append( algorithmName );
        numAliases++;
      }
      if ( algorithm->hasExtractor() ) {
        m_theFeatureCreator.insert("extractor." + algorithmName, maker);
        m_extractorNames.append( algorithmName );
        numAliases++;
      }
      if ( algorithm->hasDetector() && algorithm->hasExtractor() ) {
        m_theFeatureCreator.insert("feature2d." + algorithmName, maker);
        numAliases++;
      }

      return ( numAliases );
    }

    template <class T> int addMatcherAlgorithm() {
      int numAliases(0);
      MatcherAlgorithmPtr algorithm = T::create( PvlFlatMap() );
      MatcherCreator maker = T::create;
      QString algorithmName = algorithm->name().toLower();

      m_theMatcherCreator.insert(algorithmName, maker);
      numAliases++;
      if ( algorithm->hasMatcher() ) {
        m_theMatcherCreator.insert("matcher." + algorithmName, maker);
        m_matcherNames.append( algorithmName );
        numAliases++;
      }

      return ( numAliases );
    }
    
  private:

    typedef QMap<QString, FeatureCreator>  Features;
    typedef QMap<QString, MatcherCreator>  Matchers;

    Features  m_theFeatureCreator;
    Matchers  m_theMatcherCreator;
    QStringList m_detectorNames;
    QStringList m_extractorNames;
    QStringList m_matcherNames;

};

};  // namespace Isis
#endif
