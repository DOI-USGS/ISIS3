#ifndef MatcherAlgorithms_h
#define MatcherAlgorithms_h
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

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>

#include <QList>

#include "FeatureAlgorithm.h"
#include "PvlFlatMap.h"
#include "PvlObject.h"


namespace Isis {

/**
 * @brief Generic container for OpenCV-type feature matcher algorithms
 *  
 * @author  2016-11-29 Kris Becker
 *  
 * @internal 
 *   @history 2016-11-29 Kris Becker - Original Version
 */

class MatcherAlgorithms {
  public:
    MatcherAlgorithms();

    MatcherAlgorithms(FeatureAlgorithmPtr &detector, 
                      FeatureAlgorithmPtr &extractor,
                      MatcherAlgorithmPtr &matcher,
                      PvlFlatMap parameters = PvlFlatMap());

    virtual ~MatcherAlgorithms();

    virtual bool isValid() const;
    bool validate(const bool &throwOnErrors = true) const;

    Feature2DAlgorithm &detector() const;
    Feature2DAlgorithm &extractor() const;
    DescriptorMatcherAlgorithm &matcher() const;

    const PvlFlatMap &parameters() const;

    PvlObject info(const QString &name = "MatcherAlgorithms") const;

  private:
    FeatureAlgorithmPtr  m_detector;  //!< Detector algorithm
    FeatureAlgorithmPtr  m_extractor; //!< Extractor algorithm
    MatcherAlgorithmPtr  m_matcher;   //!< Matcher algorithm
    PvlFlatMap m_parameters;                //!< Parameters for matcher

};

};  // namespace Isis
#endif
