#ifndef DaisyAlgorithm_h
#define DaisyAlgorithm_h
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

#include "DaisyExtractor.h"
#include "FeatureAlgorithm.h"
#include "opencv2/xfeatures2d.hpp"

#include <boost/bimap.hpp>

#include <QString>
#include <QVariant>

namespace Isis {

/**
 * @brief Daisy Feature matcher algorithm 
 *  
 * This class provides the OpenCV3 SURF Feature2D algortithm. Only the 
 * necesary methods are implemented here. 
 *  
 * @author  2016-11-30 Kristin Berry
 *  
 * @internal 
 *   @history 2016-11-30 Kristin Berry - Original Version 
 */

class DaisyAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    DaisyAlgorithm();
    DaisyAlgorithm(const PvlFlatMap &cvars, const QString &config = QString());
#if 0
    DaisyAlgorithm(float radius=15, int q_radius=3, int q_theta=8, int q_hist=8, 
                   int norm=DAISY::NRM_NONE, InputArray H=noArray(), 
                   bool interpolation=true, bool use_orientation=false);
#endif
    virtual ~DaisyAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm  *create(const PvlFlatMap &vars,
                                      const QString &config = QString());
    
    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

protected:
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int        setAlgorithmVariables(const PvlFlatMap &variables);

    void       setupTypeMap();
    PvlFlatMap setupParameters();

    boost::bimap<QString, int> m_typeMap; //!< Bi-directional map for converting type values.

  private:
    typedef DaisyExtractor       DAISYType;
    typedef cv::Ptr<DAISYType>   DAISYPtr;

};

};  // namespace Isis
#endif
