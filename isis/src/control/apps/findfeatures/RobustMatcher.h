#ifndef RobustMatcher_h
#define RobustMatcher_h
/**
 * @file
 * $Revision: 6563 $ 
 * $Date: 2016-02-10 16:56:52 -0700 (Wed, 10 Feb 2016) $ 
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
 */
  class RobustMatcher : public QLogger {
  
    public:
  
  
      RobustMatcher();
      RobustMatcher(const QString &name, 
                    const PvlFlatMap &parameters = PvlFlatMap());
      RobustMatcher(const QString &name, cv::Ptr<cv::FeatureDetector> &detector, 
                    cv::Ptr<cv::DescriptorExtractor> &extractor,
                    const PvlFlatMap &parameters = PvlFlatMap(),
                    const int &bfNormType = cv::NORM_L2, 
                    const bool &crossCheck = false);
      RobustMatcher(const QString &name, cv::Ptr<cv::FeatureDetector> &detector, 
                    cv::Ptr<cv::DescriptorExtractor> &extractor,
                    cv::Ptr<cv::DescriptorMatcher> &matcher,
                    const PvlFlatMap &parameters = PvlFlatMap());
      virtual ~RobustMatcher();
  
      void setName(const QString &name);
      inline QString name() const {  return ( m_name );  }
  
      void setFeatureDetector(cv::Ptr<cv::FeatureDetector>& detector);
  
      void setDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor>& extractor);
  
      void setDescriptorMatcher(cv::Ptr<cv::DescriptorMatcher>& matcher);
      void setDescriptorMatcher(const int &normType,
                                const bool &crossCheck = false);
      QString getMatcherDescription() const;
  
      void setDescription(const QString &description);
      QString getDescription() const;
  
      void    addParameters(const PvlFlatMap &parameters, 
                            const bool &precedence = false); 
      bool    isParameterNull(const QString &name, const int index = 0) const;
      void    addParameter(const QString &name, const QString &value); 
      void    addParameter(const PvlKeyword &key, const QString &name = "");
      bool    hasParameter(const QString &name) const;
      QString getParameter(const QString &name, const QString &defaultValue,
                           const int &index = 0) const; 
      QString getParameter(const QString &name, const int &index = 0) const; 
      int     getParameterCount(const QString &name) const;
  
      void setRatio(const double ratio = 0.65);
      void setEpiTolerance(const double tolerance = 3.0);
      void setEpiConfidence(const double confidence = 0.99);
      void setHmgTolerance(const double tolerance = 3.0);
      void setMaxPoints(const int maxpoints = 0);
  
      double getRatio() const;
      double getEpiTolerance() const;
      double getEpiConfidence() const;
      double getHmgTolerance() const;
      int    getMaxPoints() const;
  
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
  
      PvlObject info() const;
       
  
    private:
      QString                          m_name;        // Name of matcher
      QString                          m_description; // Specification string
      QString                          m_matcherSpec; // Derived Matcher spec
  
      // pointer to the feature detector, extractor, matcher
      cv::Ptr<cv::FeatureDetector>     m_detector;
      cv::Ptr<cv::DescriptorExtractor> m_extractor;
      cv::Ptr<cv::DescriptorMatcher>   m_matcher;
  
      bool         m_doRootSift;         // Do a rootsift L1 normalization
      double       m_ratio;              // max ratio between 1st and 2nd NN
      bool         m_refineF;            // if true will refine the fundamental/epipolar matrix 
      double       m_epiTolerance;       // min distance to epipolar
      double       m_epiConfidence;      // confidence level (probability)
      double       m_hmgTolerance;       // min distance to homography points 
      int          m_maxpoints;          // Restrict maximum returned keypoints
      int          m_minEpiPoints;       // Minimum number of epipolar points
      int          m_minHomoPoints;      // Minimum number homograpy points
      PvlFlatMap   m_parameters;         // Parameters for matcher
      void init(const QString &name, const PvlFlatMap &parameters, 
                const bool &allocateAlgorithms = false);
      cv::Ptr<cv::DescriptorMatcher> allocateMatcher(cv::Ptr<cv::DescriptorExtractor> &extractor,
                                                     const int &normalize = cv::NORM_L2, 
                                                     const bool &crossCheck = false);
      void RootSift(cv::Mat &descriptors, const float eps = 1.0E-7) const;
      double elapsed(const QTime &runtime) const;  // returns seconds
  
  };
  
  ///!<   Shared FeatureAlgorithm pointer that everyone can use 
  typedef QSharedPointer<RobustMatcher> SharedRobustMatcher;
  typedef QList<SharedRobustMatcher>    RobustMatcherList;


}  // namespace Isis

#endif
