#ifndef AgastAlgorithm_h
#define AgastAlgorithm_h
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

#include <boost/bimap.hpp>

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"

namespace Isis {

/**
 * @brief AGAST Feature matcher algorithm 
 *  
 * This class provides the OpenCV3 AGAST Feature2D algortithm. Only the 
 * necesary methods are implemented here. 
 *  
 * @author  2016-12-07 Jesse Mapel
 *  
 * @internal 
 *   @history 2016-12-07 Jesse Mapel - Original Version 
 */

class AgastAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    AgastAlgorithm();
    AgastAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
    virtual ~AgastAlgorithm();

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
    void setupTypeMap();

    boost::bimap<QString, int> m_typeMap; //!< Bi-directional map for converting type values.

  private:
    typedef cv::AgastFeatureDetector AgastType;
    typedef cv::Ptr<AgastType>       AgastPtr;
};

};  // namespace Isis
#endif
