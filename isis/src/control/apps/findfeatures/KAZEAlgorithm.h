#ifndef KAZEAlgorithm_h
#define KAZEAlgorithm_h
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


#include "FeatureAlgorithm.h"
#include "opencv2/features2d.hpp"

#include <QMap>
#include <QString>
#include <QVariant>

#include <boost/bimap.hpp>

namespace Isis {

/**
 * @brief KAZE Feature matcher algorithm 
 *  
 * This class provides the OpenCV3 KAZE Feature2D algortithm. Only the 
 * necesary methods are implemented here. 
 *  
 * @author  2016-12-09 Kristin Berry
 *  
 * @internal 
 *   @history 2016-12-09 Kristin Berry - Original Version 
 */

class KAZEAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    KAZEAlgorithm();
    KAZEAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
    virtual ~KAZEAlgorithm();

    virtual QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create(const PvlFlatMap &vars,
                                     const QString &config = QString());

    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);
    void setupTypeMap();

    boost::bimap<QString, int> m_typeMap; //!< Bi-directional map for converting type values.

  private:
    typedef cv::KAZE                KAZEType;
    typedef cv::Ptr<KAZEType>       KAZEPtr;

};


};  // namespace Isis
#endif
