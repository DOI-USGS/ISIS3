#ifndef LUCIDAlgorithm_h
#define LUCIDAlgorithm_h
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
#include <QVariant>

namespace Isis {

/**
 * @brief LUCID Feature matcher algorithm 
 *  
 * This class provides the OpenCV3 LUCID Feature2D algortithm. Only the 
 * necesary methods are implemented here. 
 *  
 * @author  2016-12-12 Jesse Mapel
 *  
 * @internal 
 *   @history 2016-12-12 Jesse Mapel - Original Version 
 */

class LUCIDAlgorithm : public Feature2DAlgorithm {  // See FeatureAlgorithm.h
  //  OpenCV 3 API
  public:
    LUCIDAlgorithm();
    LUCIDAlgorithm(const PvlFlatMap &cvars, const QString &config = QString() );
    virtual ~LUCIDAlgorithm();

    QString description() const;

    // Required for all algorithms
    static Feature2DAlgorithm *create( const PvlFlatMap &vars,
                                       const QString &config = QString() );

    virtual bool hasDetector() const;
    virtual bool hasExtractor() const;
    virtual bool hasMatcher() const;

  protected:
    virtual PvlFlatMap setupParameters();
    virtual PvlFlatMap getAlgorithmVariables() const;
    virtual int setAlgorithmVariables(const PvlFlatMap &variables);

  private:
    typedef cv::xfeatures2d::LUCID  LUCIDType;
    typedef cv::Ptr<LUCIDType>      LUCIDPtr;
};

};  // namespace Isis
#endif
