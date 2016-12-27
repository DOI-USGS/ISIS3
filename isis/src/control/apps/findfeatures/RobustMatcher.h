#ifndef RobustMatcher_h
#define RobustMatcher_h
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

#include <QString>
#include <QSharedPointer>
#include <QTime>

#include <opencv2/opencv.hpp>

#include "FeatureMatcherTypes.h"
#include "MatcherAlgorithms.h"
#include "MatchImage.h"
#include "MatchPair.h"
#include "PvlFlatMap.h"
#include "PvlObject.h"
#include "QDebugLogger.h"

namespace Isis {

class QDebug;

/**
 * @brief Container for a feature match pair of data sources
 *  
 *  
 * @author 2015-08-18 Kris Becker 
 * @internal 
 *   @history 2015-08-18 Kris Becker - Original Version 
 *   @history 2016-10-05 Ian Humphrey & Makayla Shepherd - Changed headers to OpenCV2.
 */
  class RobustMatcher : public MatcherAlgorithms, public QLogger {
  
    public:
      RobustMatcher();
      RobustMatcher(const QString &name);
      RobustMatcher(const QString &name, 
                    const MatcherAlgorithms &algorithms,
                    const PvlFlatMap &parameters = PvlFlatMap(),
                    const QLogger &logger = QLogger());
      virtual ~RobustMatcher();
  
      void setName(const QString &name);
      inline QString name() const {  return ( m_name );  }
  
      // For just images, MatchImage objects are created generically using the 
      // other match interfaces
      MatchPair      match(cv::Mat& query, cv::Mat& trainer) const;
      MatchPairQList match(cv::Mat& query, std::vector<cv::Mat>& trainers) const;
  
      // Formal matching methods
      MatchPair      match(MatchImage &query, MatchImage &train) const;
      MatchPairQList match(MatchImage &query, MatchImageQList &trainers) const;
  
      // Robust outlier removal
      bool removeOutliers(const cv::Mat &queryDescriptors,
                          const cv::Mat &trainDescriptors,
                          std::vector<cv::KeyPoint>& queryKeypoints,
                          std::vector<cv::KeyPoint>& trainKeypoints,
                          std::vector<cv::DMatch> &homography_matches, 
                          std::vector<cv::DMatch> &epipolar_matches, 
                          std::vector<cv::DMatch> &matches,
                          cv::Mat &homography, cv::Mat &fundamental,
                          double &mtime, const bool onErrorThrow = true) 
                          const;
  
      int ratioTest( std::vector<std::vector<cv::DMatch> > &matches,
                     double &mtime) const;
  
      void symmetryTest(const std::vector<std::vector<cv::DMatch> >& matches1,
                        const std::vector<std::vector<cv::DMatch> >& matches2,
                        std::vector<cv::DMatch>& symMatches, double &mtime) const;
  
      cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
                         const std::vector<cv::KeyPoint>& keypoints1,
                         const std::vector<cv::KeyPoint>& keypoints2,
                         std::vector<cv::DMatch>& outMatches, double &mtime,
                         const bool onErrorThrow = true) const;
  
      cv::Mat computeHomography(const std::vector<cv::KeyPoint>& query,
                                const std::vector<cv::KeyPoint>& train, 
                                const std::vector<cv::DMatch> &matches,
                                std::vector<cv::DMatch> &inliers,
                                double &mtime,
                                const int method = CV_FM_RANSAC,
                                const double tolerance = 3.0,
                                const bool refine = true,
                                const bool onErrorThrow = true) 
                                const;

      const PvlFlatMap &parameters() const;
    
      // This hides the MatcherAlgorithm version which will be augmented
      PvlObject info(const QString &p_name = "RobustMatcher") const;
  
    private:
      QString      m_name;        // Name of matcher
      PvlFlatMap   m_parameters;  // Parameters for matcher

      void init(const PvlFlatMap &parameters = PvlFlatMap());
      void RootSift(cv::Mat &descriptors, const float eps = 1.0E-7) const;
      double elapsed(const QTime &runtime) const;  // returns seconds
  
  };
  
  ///!<   Shared FeatureAlgorithm pointer that everyone can use 
  typedef QSharedPointer<RobustMatcher> SharedRobustMatcher;
  typedef QList<SharedRobustMatcher>    RobustMatcherList;


}  // namespace Isis

#endif
