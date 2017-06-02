#ifndef AKAZEAlgorithm_h
#define AKAZEAlgorithm_h
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

#include <boost/bimap.hpp>
#include "FeatureAlgorithm.h"
#include "opencv2/features2d.hpp"

#include <QMap>
#include <QVariant>

namespace Isis {

/**
 * @brief AKAZE Feature matcher algorithm 
 *  
 * This class provides the OpenCV3 AKAZE Feature2D algortithm. Only the 
 * necesary methods are implemented here. 
 *  
 * @author  2016-12-07 Jesse Mapel
 *  
 * @internal 
 *   @history 2016-12-07 Jesse Mapel - Original Version 
 */

class AKAZEAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    AKAZEAlgorithm();
    AKAZEAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
    virtual ~AKAZEAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create(const PvlFlatMap &vars,
                                     const QString &config = QString());

    // Provide information about the abilities of the algorithm
    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);
    void setupMaps();

    boost::bimap<QString, int> m_descriptorTypeMap; /**!< Bi-directional map for converting
                                                          DescriptorType values.*/
    boost::bimap<QString, int> m_diffusivityMap;    /**!< Bi-directional map for converting
                                                          Diffusivity values.*/

  
  private:
    typedef cv::AKAZE          AKAZEType;
    typedef cv::Ptr<AKAZEType> AKAZEPtr;
};

};  // namespace Isis
#endif
