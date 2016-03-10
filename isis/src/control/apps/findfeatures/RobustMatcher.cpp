/**                                                                       
 * @file                                                                  
 * $Revision: 6563 $
 * $Date: 2016-02-10 16:56:52 -0700 (Wed, 10 Feb 2016) $
 * $Id: RobustMatcher.cpp 6563 2016-02-10 23:56:52Z kbecker@GS.DOI.NET $
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


#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include <QDebug>
#include <QtDebug>
#include <QList>
#include <QStringList>
#include <QTime>

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

// boost library
#include <boost/foreach.hpp>

#include "Application.h"
#include "AlgorithmParameters.h"
#include "IException.h"
#include "IString.h"
#include "FileName.h"
#include "RobustMatcher.h"


namespace Isis {

/** Setup a default robust matcher with SURF detector/extractor and
 *  BFMatcher matcher with default parameters to each element
 *  
 *  @see init(const QString &name, const PvlFlatMap(), const bool
 *       useDefaults(false)).
 */
RobustMatcher::RobustMatcher()  {
  init("RobustMatcher", PvlFlatMap(), true);
}


RobustMatcher::RobustMatcher(const QString &name,
                             const PvlFlatMap &parameters )  {
  init("RobustMatcher", parameters, true);
}

/**
 * @brief Construct a matcher with specific componets and parameters
 * 
 * @author 2015-08-28 Kris Becker 
 * 
 * @param name       Name of matcher being created.
 * @param detector   Feature detector
 * @param extractor  Feature extractor
 * @param bfNormType BFMatcher normalization parameter
 * @param crossCheck Do OpenCV ratio test.  NOT recommended.
 */
RobustMatcher::RobustMatcher(const QString &name, 
                             cv::Ptr<cv::FeatureDetector> &detector, 
                             cv::Ptr<cv::DescriptorExtractor> &extractor, 
                             const PvlFlatMap &parameters,
                             const int &bfNormType, 
                             const bool &crossCheck) :
                             QLogger() {
  init(name, parameters, false);
  m_detector  = detector;
  m_extractor = extractor;
  setDescriptorMatcher(bfNormType, crossCheck);
}

/**
 * @brief Construct a matcher with all componets and parameters
 * 
 * @author 2015-08-28 Kris Becker 
 * 
 * @param name       Name of matcher being created.
 * @param detector   Feature detector
 * @param extractor  Feature extractor
 * @param matcher    Feature matcher
 */
RobustMatcher::RobustMatcher(const QString &name, 
                             cv::Ptr<cv::FeatureDetector> &detector, 
                             cv::Ptr<cv::DescriptorExtractor> &extractor,
                             cv::Ptr<cv::DescriptorMatcher> &matcher,
                             const PvlFlatMap &parameters) : 
                             QLogger() {

  init(name, parameters, false);
  m_detector  = detector;
  m_extractor = extractor;
  m_matcher   = matcher;
}

/** Desctructor requires no actions */
RobustMatcher::~RobustMatcher() { }

/** Set the feature detector  */
void RobustMatcher::setFeatureDetector(cv::Ptr<cv::FeatureDetector>& detector) {
   m_detector = detector;
}


/** Set the descriptor extractor  */
void RobustMatcher::setDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor>& extractor) {
   m_extractor = extractor;
}

/** Set the descriptor matcher  */
void RobustMatcher::setDescriptorMatcher(cv::Ptr<cv::DescriptorMatcher>& matcher) {
  m_matcher = matcher;
  return;
}

void RobustMatcher::setDescriptorMatcher(const int &normType, 
                                         const bool &crossCheck) {
  m_matcher = allocateMatcher(m_extractor, normType, crossCheck);
  return;
}

QString RobustMatcher::getMatcherDescription() const {
  return ( m_matcherSpec );
}


void RobustMatcher::setDescription(const QString &description) {
  m_description = description;
  return;
}

QString RobustMatcher::getDescription() const {
  return ( m_description );
}


  /** 
   * @brief Determines whether the PVL keyword with the given name at the given index is null.
   *
   * This method looks for the PVL keyword in this Resource's PvlFlatMap that is mapped to the
   * given name and checks whether the value at the given index is null. If no index is given, by
   * default this method checks the first value (i.e. index 0). If the keyword does not exist in
   * the map, this method returns true.
   * 
   * @param keywordName  Name of the PVL keyword whose values will be accessed.
   * @param index        Zero-based index for the value to be checked.
   *
   * @return bool  Indicates whether the given PVL keyword is null at the given index.
   */ 
  bool RobustMatcher::isParameterNull(const QString &name, const int index) const {
    return ( m_parameters.isNull(name, index) );
  }

  /** 
   * Adds the PVL keywords from the given map of keywords to this Resource.
   * The new keywords are inserted into this Resource's existing PvlFlatMap.
   *
   * @param keys  A PvlFlatMap containing the PvlKeywords to be inserted into the existing
   *              keyword list.
   */ 
  void RobustMatcher::addParameters(const PvlFlatMap &parameters, 
                                    const bool &precedence) { 
    if ( true == precedence) {
      m_parameters = PvlFlatMap(m_parameters, parameters);
    }
    else {
      m_parameters = PvlFlatMap(parameters, m_parameters);
    }
    return; 
  }

  /** 
   * Adds a PVL keyword with the given name and value to this Resource. A PvlKeyword object
   * is created and added to the PvlFlatMap belonging to this Resource.
   *
   * @param keywordName   A string containing the name of the keyword to be added.
   * @param keywordValue  A string containing the value of the keyword to be added.
   */ 
  void RobustMatcher::addParameter(const QString &name, 
                                   const QString &value) { 
     m_parameters.add(name, value);
     return;
  }
  

  /** 
   * Adds the given PVL keyword to this Resource. The PvlKeyword object is added to
   * the PvlFlatMap belonging to this Resource.
   *
   * @param keyword  A reference to the PvlKeyword object to be added.
   */ 
  void RobustMatcher::addParameter(const PvlKeyword &keyword, 
                                   const QString &name)  {
    if ( name.isEmpty() ) {
      m_parameters.add(keyword); 
    }
    else {
      PvlKeyword temp = keyword;
      temp.setName(name);
      m_parameters.add(temp);
    }
  }

  /** 
   * Determines whether a PVL keyword with the given name is in this Resource.
   *
   * @param name  A string containing the keyword name.
   *
   * @return bool  Indicates whether a PVL keyword in this RobustMatcher's 
   *               PvlFlatMap is mapped to the given name.
   */ 
  bool RobustMatcher::hasParameter(const QString &name) const {
    return ( m_parameters.exists(name) );
  }

  /** 
   * @brief Gets the value of the PVL keyword with the given name at the given index.
   *
   * This method looks for the PVL keyword in this RobustMatcher's  PvlFlatMap 
   * that is mapped to the given name and accesses the value at the given 
   * index. If no index is given, by default this method returns the first 
   * value (i.e. index 0). 
   *
   * Note: If the keyword does not exist in the map, PvlFlatMap throws an 
   *       exception. To avoid this, the isNull() method can be called before
   *       invoking this method to verify whether a value exists at the given
   *       index. Otherwise, the value(QString, QString, int) version of this
   *       overloaded method can be called.
   *
   * @see isNull(QString, int)
   * @see value(QString, QString, int)
   *
   * @param keywordName  A string containing the name of the PVL keyword in this Resource's
   *                     list of keywords.
   * @param index        Zero-based index for the value to be accessed.
   *
   * @return QString  A string containing the PVL keyword value at the given index. 
   */ 
  QString RobustMatcher::getParameter(const QString &name, const int &index) const { 
    return ( m_parameters.get(name, index) ); 
  }
  

  /** 
   * @brief Gets the value of the PVL keyword at the given index, if found; otherwise it returns
   *        the given default value.
   * 
   * This method looks for the PVL keyword in this RobustMatcher's PvlFlatMap 
   * that is mapped to the given name and accesses the value at the given 
   * index. If no index is given, by default this method returns the first 
   * value (i.e. index 0). 
   *
   * If the keyword does not exist in the map, the given default value will be 
   * returned. 
   *
   * @see isNull(QString, int)
   * @see value(QString, int)
   *
   * @param name   A string containing the name of the PVL keyword in this 
   *                      Resource's list of keywords.
   * @param defaultValue  A string containing the default value for this 
   *                      keyword if not found in the PvlFlatMap.
   * @param index         Zero-based index for the value to be accessed.
   *
   * @return QString  A string containing the PVL keyword value at the 
   *                  given index, if it exists; otherwise the given default
   *                  value is returned.
   */ 
  QString RobustMatcher::getParameter(const QString &name, 
                                      const QString &defaultValue, 
                                      const int &index) const {
    QString keywordValue(defaultValue);
    if ( !isParameterNull(name, index) ) {
      keywordValue = getParameter(name, index);
    }
    return (keywordValue);
  }

  

  /** 
   * Counts the number of values the PVL keyword with the given name has, if it exists
   * in this Resource. Otherwise, it returns 0.
   *
   * @param keywordName  A string containing the keyword name.
   *
   * @return int  The size of the PvlKeyword object mapped to the given name in
   *               this Resource's PvlFlatMap.
   */ 
  int RobustMatcher::getParameterCount(const QString &name) const {
    return ( m_parameters.count(name) );
  }

void RobustMatcher::setRatio(const double ratio) {
  m_ratio = ratio;
  return;
}

void RobustMatcher::setEpiTolerance(const double tolerance) {
  m_epiTolerance = tolerance;
  return;
}

void RobustMatcher::setEpiConfidence(const double confidence) {
  m_epiConfidence = confidence;
  return;
}

void RobustMatcher::setHmgTolerance(const double tolerance) {
  m_hmgTolerance = tolerance;
  return;
}

void RobustMatcher::setMaxPoints(const int maxpoints) {
  m_maxpoints = maxpoints;
  return;
}

double RobustMatcher::getRatio() const {
  return ( m_ratio );
}

double RobustMatcher::getEpiTolerance() const {
  return ( m_epiTolerance );
}

double RobustMatcher::getEpiConfidence() const {
  return ( m_epiConfidence );
}

double RobustMatcher::getHmgTolerance() const {
  return ( m_hmgTolerance );
}


int RobustMatcher::getMaxPoints() const {
  return ( m_maxpoints );
}


/**
 * @brief Construct required interface with match image pairs
 * 
 * @author 2015-10-03 Kris Becker
 * 
 * @param query    Query image
 * @param trainer  Train image
 * 
 * @return MatchPair Returns the results of the matcher
 */
MatchPair RobustMatcher::match(cv::Mat& query, cv::Mat& train) const {
  MatchImage v_query(ImageSource("Query", query, "Query"));
  MatchImage v_train(ImageSource("Train", train, "Train"));
  return ( match(v_query, v_train) );
}

/**
 * @brief Construct required interface for multi-matching images
 * 
 * @author 2015-10-03 Kris Becker
 * 
 * @param query    Query image
 * @param trainers List of train images
 * 
 * @return MatchPairQList List of matched pairs
 */
MatchPairQList RobustMatcher::match(cv::Mat& query, 
                                    std::vector<cv::Mat>& trainers) const {

  MatchImage v_query(ImageSource("Query", query, "Query"));
  MatchImageQList v_trainers;
  for (unsigned int i = 0 ; i < trainers.size() ; i++) {
    QString id = "Train[" + QString::number(i) + "]";
    v_trainers.push_back(MatchImage(ImageSource(id, trainers[i], id)));
  }
  return ( match(v_query, v_trainers) );
}


// Match feature points for image pair sources using robust outlier detection
MatchPair RobustMatcher::match(MatchImage &query, MatchImage &train) const { 

   // Announce our intensions
  if ( isDebug() ) {
     logger() << "\n@@ matcher-pair started on " << Application::DateTime() << "\n";  
     logger() << "\n+++++++++++++++++++++++++++++\n";
     logger() << "Entered RobustMatcher::match(MatchImage &query, MatchImage &trainer)...\n"; 
     logger() << "  Specification:   " << getDescription() << "\n";
     logger().flush();
   }

   const bool onErrorThrow = false;  // Conditions for managed matching

   // Setup
   MatchImage &v_query = query;
   MatchImage &v_train = train;
   MatchPair v_pair(v_query, v_train); 

   // Render images for matching
   cv::Mat i_query = v_query.image();
   cv::Mat i_train = v_train.image();

   if ( toBool(getParameter("SaveRenderedImages", "false")) ) {
     QString savepath = getParameter("SavePath", "$PWD");

     FileName qfile(v_query.source().name());
     QString qfout = savepath + "/" + qfile.baseName() + "_query.png";
     FileName oqfile(qfout);
     imwrite( oqfile.expanded().toStdString(), i_query);

     FileName tfile(v_train.source().name());
     QString tfout = savepath + "/" + tfile.baseName() + "_train.png";
     FileName otfile(tfout);
     imwrite( otfile.expanded().toStdString(), i_train);
   }

   if ( isDebug() ) {
     logger() << "**  Query Image:   " << v_query.name() << "\n"; 
     logger() << "       FullSize:     (" << v_query.source().samples() << ", " 
                                          << v_query.source().lines() << ")\n"; 
     logger() << "       Rendered:     (" << i_query.cols << ", " 
                                          << i_query.rows << ")\n"; 
     logger() << "**  Train Image:   " << v_train.name() << "\n";
     logger() << "       FullSize:     (" << v_train.source().samples() << ", " 
                                          << v_train.source().lines() << ")\n"; 
     logger() << "       Rendered:     (" << i_train.cols << ", " 
                                          << i_train.rows << ")\n"; 
     logger() << "--> Feature detection...\n";
     logger().flush();
  }

   // Do not include in timer up to here
   QTime stime;
   stime.start();

   // 1a. Detection of the features
   m_detector->detect(i_query, v_query.keypoints()); 
   m_detector->detect(i_train, v_train.keypoints());

   int v_query_points = v_query.size();
   int v_train_points = v_train.size();
   int allPoints = v_query_points + v_train_points;

   // Limit keypoints if requested by user
   if ( m_maxpoints > 0 ) {
     logger() << "  Keypoints restricted by user to " << m_maxpoints << " points...\n"; 
     logger().flush();
     cv::KeyPointsFilter::retainBest(v_query.keypoints(), m_maxpoints);
     cv::KeyPointsFilter::retainBest(v_train.keypoints(), m_maxpoints);
   }

   double v_time = elapsed(stime);  // Event timing

   // Log results
   if ( isDebug() ) {
     logger() << "  Total Query keypoints:    " << v_query.size() 
              << " [" << v_query_points << "]\n"; 
     logger() << "  Total Trainer keypoints:  " << v_train.size() 
              << " [" << v_train_points << "]\n";
     logger() << "  Processing Time:          " << v_time << "\n";
     logger() << "  Processing Keypoints/Sec: " 
               << (double) allPoints / v_time << "\n";
     logger() << "--> Extracting descriptors...\n";
     logger().flush();
   }

   // 1b. Extraction of the descriptors
   cv::Mat queryDescriptors, trainerDescriptors;
   m_extractor->compute(i_query, v_query.keypoints(), v_query.descriptors());
   m_extractor->compute(i_train, v_train.keypoints(), v_train.descriptors()); 
   double d_time = elapsed(stime) - v_time;
   v_pair.addTime( v_time + d_time );

   // Do root sift normalization if requested
   if ( m_doRootSift ) {
     if ( isDebug() ) {
       logger() << "  Computing RootSift Descriptors...\n"; 
     }
     RootSift( v_query.descriptors() );
     RootSift( v_train.descriptors() );
   }

   if ( isDebug() ) {
     logger() <<     "  Processing Time(s):         " << d_time << "\n";
     logger() <<     "  Processing Descriptors/Sec: " 
               << (double) allPoints / d_time << "\n";
     logger() << "\n*Removing outliers from image pairs\n";
     logger().flush();
   }


   // OUTLIER DETECTION!!!
   // 2, 3, 4,  5, 6: Apply ratio (2) and symmetric (3) tests, then apply 
   // RANSAC homography (4) outlier followed by epipoloar (5) and final 
   // homography (6) 
    try {
     double mtime; 
     cv::Mat homography, fundamental;
     removeOutliers(v_query.descriptors(), v_train.descriptors(), 
                    v_query.keypoints(), v_train.keypoints(),
                    v_pair.homography_matches(), v_pair.epipolar_matches(),
                    v_pair.matches(), homography, fundamental, 
                    mtime, onErrorThrow);

     v_pair.setFundamental(fundamental);
     v_pair.setHomography(homography);
     v_pair.addTime(mtime);
   }
   catch ( cv::Exception &c ) {
     QString mess = "Outlier removal process failed on image pair: "
                    " Query: " + v_query.name() + 
                    ", Train: " + v_train.name() +
                    ".  CV::Error - " + c.what();
     // throw IException(IException::Programmer, mess, _FILEINFO_);
     v_pair.addError(mess);
     if ( isDebug() ) {
       logger() << "  Outlier Error = " 
                 << v_pair.getError(v_pair.errorCount()-1) << "\n";
       logger().flush();
     }
   }
   catch ( IException &ie) {
     QString mess = "Outlier removal process failed on Query/Train image pair "
                    " Query: "  + v_query.name() + 
                    ", Train: " + v_train.name();
     // throw IException(ie, IException::Programmer, mess, _FILEINFO_);
     v_pair.addError(mess);
     if ( isDebug() ) {
       logger() << "  Outlier Error = " 
                 << v_pair.getError(v_pair.errorCount()-1) << "\n";
       logger().flush();
     }
   }

   // All done...
  if ( isDebug() ) {
    logger() << "%% match-pair complete in " << elapsed(stime) << " seconds!\n\n";
    logger().flush();
  }

   // v_pair->addTime(mtime);  // Not added to time unless uncommented 
   return ( v_pair ); 
 }


// Match multiple images to the query image using robust outlier detection.
MatchPairQList RobustMatcher::match(MatchImage &query,
                                    MatchImageQList &trainers) const {

   // Announce our intensions
  if ( isDebug() ) {
     logger() << "\n@@ matcher-multi started on " << Application::DateTime() << "\n";
     logger() << "\n+++++++++++++++++++++++++++++\n";
     logger() << "Entered RobustMatcher::match(MatchImage &query,""MatchImageList &trainer)...\n";
     logger() << "  Specification:   " << getDescription() << "\n";
     logger().flush();
  }

  if (trainers.size() == 0 ) {
    QString mess = "No trainer images provided!!"; 
    if (isDebug() ) logger() << "  " << mess << "\n";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }

   bool onErrorThrow = false;  // Conditions for managed matching

   // Setup
   MatchImage &v_query = query;
   MatchImageQList &v_trainers = trainers;


  // Create rendered trainer images for matching
  // Render images for efficiency
   cv::Mat i_query = v_query.image();
   std::vector<cv::Mat> i_trainers;
   bool saveRendered = toBool(getParameter("SaveRenderedImages", "false"));
   QString savepath = getParameter("SavePath", "$PWD");

   if ( true == saveRendered ) {
     // Save the query image first
     FileName qfile(v_query.source().name());
     QString qfout = savepath + "/" + qfile.baseName() + "_query.png";
     FileName oqfile(qfout);
     imwrite( oqfile.expanded().toStdString(), i_query);
   }

   // Now process the rest of the trainer images
   for (int i = 0 ; i < v_trainers.size() ; i++) {
     i_trainers.push_back(v_trainers[i].image());

     if ( true == saveRendered ) {
       FileName tfile(v_trainers[i].source().name()); 
       QString tfout = savepath + "/" + tfile.baseName() + "_train.png";
       FileName ofile(tfout);
       imwrite( ofile.expanded().toStdString(), i_trainers[i]);
     }
   }

   if ( isDebug() ) {
     logger() << "**  Query Image:   " << v_query.name() << "\n"; 
     logger() << "       FullSize:     (" << v_query.source().samples() << ", " 
                                          << v_query.source().lines() << ")\n"; 
     logger() << "       Rendered:     (" << i_query.cols << ", " 
                                          << i_query.rows << ")\n"; 

     logger() << "v^v Matching " << v_trainers.size() << " trainer images.\n"; 
     for (int i = 0 ; i < v_trainers.size() ; i++) {
       logger() << "**  Train Image[" << i <<  "] " <<  v_trainers[i].name()  << "\n";
       logger() << "       FullSize:     (" << v_trainers[i].source().samples() << ", " 
                                            << v_trainers[i].source().lines() << ")\n"; 
       logger() << "       Rendered:     (" << i_trainers[i].cols << ", " 
                                              << i_trainers[i].rows << ")\n"; 
     }
     logger() << "--> Feature detection...\n";
     logger().flush();
   }

   // Start timer
   QTime stime;
   stime.start(); 

   // 1a. Run detection of features
   std::vector<std::vector<cv::KeyPoint> > trainerKeypoints;
   m_detector->detect(i_query, v_query.keypoints());
   m_detector->detect(i_trainers, trainerKeypoints);

   int v_query_points = v_query.size();
   int allPoints = v_query_points;
   QList<int> v_train_points;
   for (unsigned int i = 0; i < trainerKeypoints.size() ; i++) {
     v_train_points.append(trainerKeypoints[i].size());
     allPoints += trainerKeypoints[i].size();
   }

   // Limit keypoints if requested by user
   if ( m_maxpoints > 0 ) {
     logger() << "  Keypoints restricted by user to " << m_maxpoints << " points...\n"; 
     logger().flush();
     cv::KeyPointsFilter::retainBest(v_query.keypoints(), m_maxpoints);
     for (unsigned int i = 0 ; i < trainerKeypoints.size() ; i++) {
       cv::KeyPointsFilter::retainBest(trainerKeypoints[i], m_maxpoints); 
     }
   }

   double d_time = elapsed(stime);

   // Prep for computing an accurate duration
   double allKeypoints = v_query.size();
   for (unsigned int k = 0 ; k < trainerKeypoints.size() ; k++) {
     allKeypoints += trainerKeypoints[k].size();
   }

   if ( isDebug() ) {
     logger() << "  Total Query keypoints:    " << v_query.size() 
              << " [" << v_query_points << "]\n"; 
     logger() << "  Total Trainer keypoints:  " << trainerKeypoints.size() 
               << " @ ("; 
     QString sep("");
     for ( unsigned int t = 0 ; t < trainerKeypoints.size() ; t++ ) { 
       logger() << sep << trainerKeypoints[t].size() 
                 << " [" << v_train_points[t] << "]";
       sep = ",";
     }
     logger() << ") = " << allKeypoints - v_query.size() << "\n"; 
     logger() << "  Total keypoints:          " << allKeypoints << "\n";
     logger() << "  Processing Time:          " << d_time << "\n";
     logger() << "  Processing Keypoints/Sec: " 
               << allPoints / d_time  << "\n";
     logger() << "--> Extracting descriptors...\n";
     logger().flush();
   }

   // 1b. Extraction of the descriptors
   std::vector<cv::Mat> trainerDescriptors;
   m_extractor->compute(i_query, v_query.keypoints(), v_query.descriptors());
   m_extractor->compute(i_trainers, trainerKeypoints, trainerDescriptors);

    // Record time to detect features and extract descriptors for all images
   double e_time = elapsed(stime) - d_time;

   // Update times for query and train images by distributing the total time
   // by the factor of image keypoints over sum of all keypoints
   v_query.addTime( (d_time  + e_time) * ( v_query.size() / allPoints ) );

   // Do root sift normalization if requested
   if ( m_doRootSift ) {
     if ( isDebug() ) {
       logger() << "  Computing RootSift Descriptors...\n"; 
     }
     RootSift( v_query.descriptors() );
     for (unsigned int i = 0; i < trainerDescriptors.size() ; i++) { 
       RootSift( trainerDescriptors[i] );
     }
   }

   MatchPairQList pairs;
   for ( int i = 0 ; i < v_trainers.size() ; i++) {

     double kpRatio = (trainerKeypoints[i].size() / allPoints);
     MatchImage &v_train = v_trainers[i];

     // Compute the distributed time to train images
     double t_time = (d_time + e_time) * kpRatio;
     v_train.keypoints() = trainerKeypoints[i];
     v_train.setDescriptors(trainerDescriptors[i]);
     v_train.addTime(t_time);
     MatchPair v_pair(v_query, v_train); 
     if ( isDebug() ) {
       logger() << "  Processing Time(s):         " << d_time * kpRatio << "\n"; 
       logger() << "  Processing Descriptors/Sec: " 
                << (double) v_pair.keyPointTotal() / (d_time * kpRatio) << "\n"; 
       logger() << "\n*Removing outliers from image pairs:"
                << "\n *  Query: " << v_query.name() 
                << "\n *  Train: " << v_train.name() 
                << "\n"; 
       logger().flush();
     }

     try {
       // OUTLIER DETECTION!!!
       // 2, 3, 4,  5, 6: Apply ratio (2) and symmetric (3) tests, then apply 
       // RANSAC homography (4) outlier followed by epipoloar (5) and final 
       // homography (6) 
       double mtime(0);
       cv::Mat homography, fundamental;
       removeOutliers(v_query.descriptors(), v_train.descriptors(), 
                      v_query.keypoints(), v_train.keypoints(),
                      v_pair.homography_matches(), v_pair.epipolar_matches(),
                      v_pair.matches(), homography, fundamental, 
                      t_time, onErrorThrow);

       v_pair.setFundamental(fundamental);
       v_pair.setHomography(homography);
       v_pair.addTime(mtime);
       pairs.push_back( v_pair );
     }
     catch ( cv::Exception &c ) {
       QString mess = "Outlier removal process failed on Query/Train image pair "
                      " Query=" + v_query.name() + 
                      ", Train[" + QString::number(i) + "]: " + v_train.name() + 
                      ".  cv::Error - " + c.what();
       // throw IException(IException::Programmer, mess, _FILEINFO_);
       v_pair.addError(mess);
       pairs.push_back( v_pair ) ;
       if ( isDebug() ) {
         logger() << "  Outlier Error = " 
                   << v_pair.getError(v_pair.errorCount()-1) << "\n";
         logger().flush();
       }
     }
     catch ( IException &ie) {
       QString mess = "Outlier removal process failed on Query/Train image pair "
                      " Query=" + v_query.name() + 
                      ", Train[" + QString::number(i) + "]: " + v_train.name();
       // throw IException(ie, IException::Programmer, mess, _FILEINFO_);
       v_pair.addError(mess);
       pairs.push_back( v_pair );
       if ( isDebug() ) {
         logger() << "  Outlier Error = " 
                   << v_pair.getError(v_pair.errorCount()-1) << "\n";
         logger().flush();
       }
     }
   }

   // All done...
  if ( isDebug() ) {
    logger() << "%% match-multi complete in " << elapsed(stime) << " seconds!\n\n";
    logger().flush();
  }

  // return the match pair
  return ( pairs );

}


/**
 * @brief Apply ratio and symmetric outlier tests
 * 
 * @author kbecker (8/21/2015)
 * 
 * @param queryDescriptors 
 * @param trainDescriptors 
 * @param matches 
 */
bool RobustMatcher::removeOutliers(const cv::Mat &queryDescriptors, 
                                   const cv::Mat &trainDescriptors,
                                   std::vector<cv::KeyPoint>& queryKeypoints,
                                   std::vector<cv::KeyPoint>& trainKeypoints,
                                   std::vector<cv::DMatch> &homography_matches,
                                   std::vector<cv::DMatch> &epipolar_matches,
                                   std::vector<cv::DMatch> &matches,
                                   cv::Mat &homography, cv::Mat &fundamental,
                                   double &mtime, const bool onErrorThrow) 
                                   const {

  // Introduce ourselves
  if ( isDebug() ) {
    logger() << "Entered RobustMatcher::removeOutliers(Mat &query, vector<Mat> &trainer)...\n";
    logger() << "--> Matching 2 nearest neighbors for ratio tests..\n";
    logger() << "  Query, Train Descriptors: " << queryDescriptors.rows
              << ", " << trainDescriptors.rows << "\n";
    logger().flush();
  }

  homography_matches.clear();
  epipolar_matches.clear();
  matches.clear();

  // Start the timer
  QTime stime;
  stime.start();  
  double v_time;  // Ratio test timing

  // 2) Run two nearest neighbor matches for ratio test from query to train 
  //    images based on k nearest neighbours (with k=2)
  std::vector<std::vector<cv::DMatch> > matches1;
  try {
    if ( isDebug() ) {
      logger() << "  Computing query->train Matches...\n";
      logger().flush();
    } 
    m_matcher->knnMatch(queryDescriptors, trainDescriptors, 
                        matches1, // vector of matches (up to 2 per entry)
                        2); // return 2 nearest neighbours
  }
  catch (cv::Exception &e) {
    QString mess = "RobustMatcher::MatcherFailed: "+ QString(e.what()) +
                   " - if its an assertion failure, you may be enabling "
                   "crosschecking with a BFMatcher. Must use a FlannBased "
                   "matcher if you want inherent ratio testing. "
                   "NOTE a ratio test is implemented in the RobustMatcher "
                   "so its not needed!";
   throw IException(IException::User, mess, _FILEINFO_);
  }

  // Only record the first matching test
  mtime = elapsed(stime);
  v_time = mtime;

  if ( isDebug() ) {
    logger() << "  Total Matches Found:   " << matches1.size() << "\n";
    logger() << "  Processing Time:       " << v_time << "\n";
    logger() << "  Matches/second:        " << (double) matches1.size() / v_time << "\n";
    logger().flush();
  } 

  // 2a) Run two nearest neighbor matches for ratio test from train to query
  //    images based on k nearest neighbours (with k=2)
  std::vector<std::vector<cv::DMatch> > matches2;
  if ( isDebug() ) {
    logger() << "  Computing train->query Matches...\n";
    logger().flush();
  } 
  m_matcher->knnMatch(trainDescriptors, queryDescriptors, 
                      matches2, // vector of matches (up to 2 per entry)
                      2); // return 2 nearest neighbours
  v_time =  elapsed(stime) - mtime;
  // mtime = elapsed(stime);

  if ( isDebug() ) {
    logger() << "  Total Matches Found:   " << matches2.size() << "\n";
    logger() << "  Processing Time:       " << v_time << " <seconds>\n";
    logger() << "  Matches/second:        " << (double) matches2.size() / v_time << "\n"; 
    logger().flush();
  } 

  // 2b) Remove matches for which NN ratio is than threshold clean image 1 ->
  // image 2 matches

  if ( isDebug() ) {
    logger() << " -Ratio test on query->train matches...\n";
    logger().flush();
  } 

  // "removed" is unused variable, so it has been commented out
  /* int removed = */ 
  ratioTest(matches1,  v_time);
  // mtime += v_time;

  // clean image 2 -> image 1 matches
  if ( isDebug() ) {
    logger() << " -Ratio test on train->query matches...\n";
    logger().flush();
  } 

  // "removed" is unused variable, so it has been commented out
  /* removed = */
  ratioTest(matches2, v_time);
  // mtime += v_time;

  // 3) Remove non-symmetrical matches
  std::vector<cv::DMatch> symMatches;
  symmetryTest(matches1, matches2, symMatches, v_time);
  // mtime += v_time;

  // 4) Compute the homography matrix with outlier removal options
  bool refineHomography(true);
  homography = computeHomography(queryKeypoints, trainKeypoints,
                                 symMatches, homography_matches,   
                                 mtime, CV_FM_RANSAC, getHmgTolerance(),
                                 refineHomography, onErrorThrow); 

  // 5) Compute the fundamental matrix with outliers
  fundamental = ransacTest(homography_matches, queryKeypoints, trainKeypoints, 
                           epipolar_matches, v_time, onErrorThrow);

  // 6) Compute final homography after all outlier removal has been done
  refineHomography = false;
  homography = computeHomography(queryKeypoints, trainKeypoints,
                                 epipolar_matches, matches,   
                                 mtime, CV_FM_RANSAC, getHmgTolerance(),
                                 refineHomography, onErrorThrow); 


  return ( matches.size() >  0 );
}

// Clear matches for which NN ratio is > than threshold
// return the number of removed points
// (corresponding entries being cleared,
// i.e. size will be 0)
int RobustMatcher::ratioTest(std::vector<std::vector<cv::DMatch> > &matches,
                             double &mtime) const {

  if ( isDebug() ) {
    logger() << "Entered RobustMatcher::ratioTest(matches[2]) for 2 NearestNeighbors (NN)...\n";
    logger() << "  RobustMatcher::Ratio:       " << getRatio() << "\n";
    logger().flush();
  }

  QTime stime;
  stime.start();  // Start the timer

  int removed(0);
  int nInput(0);
  int noTwoNN(0);
  int nfailed(0);
  // for all matches
  for (std::vector<std::vector<cv::DMatch> >::iterator matchIterator= matches.begin();
       matchIterator!= matches.end(); ++matchIterator) {

    // if 2 NN has been identified
    if (matchIterator->size() > 1) {
      // check distance ratio
      if ((*matchIterator)[0].distance/
          (*matchIterator)[1].distance > getRatio()) {
         matchIterator->clear(); // remove match
         removed++;
         nfailed++;
      }
    } 
    else { // does not have 2 neighbours
      matchIterator->clear(); // remove match
      removed++;
      noTwoNN++;
    }
    nInput++;
  }

  mtime = elapsed(stime);
  if ( isDebug() ) {
    logger() << "  Total Input Matches Tested: " << nInput << "\n";
    logger() << "  Total Passing Ratio Tests:  " 
              << matches.size()-removed << "\n"; 
    logger() << "  Total Matches Removed:      " << removed << "\n";
    logger() << "  Total Failing NN Test:      " << nfailed << "\n";
    logger() << "  Processing Time:            " << mtime << "\n"; 
    logger().flush();
  }
  return (removed);
}

// Insert symmetrical matches in symMatches vector
void RobustMatcher::symmetryTest(const std::vector<std::vector<cv::DMatch> >& matches1,
                                 const std::vector<std::vector<cv::DMatch> >& matches2,
                                 std::vector<cv::DMatch>& symMatches,
                                 double &mtime) const {

  // Introduce ourselves
  if ( isDebug() ) {
    logger() << "Entered RobustMatcher::symmetryTest(matches1,matches2,symMatches)...\n";
    logger() << " -Running Symmetric Match tests...\n";
    logger().flush();
  }

  symMatches.clear();
  // Do not include in timer 

  QTime stime;
  stime.start();  // Start the timer

  // for all matches image 1 -> image 2
  int nInput1(0);
  int nInput2(0);
  for (std::vector<std::vector<cv::DMatch> >::const_iterator matchIterator1= matches1.begin();
       matchIterator1!= matches1.end(); ++matchIterator1) {

    // ignore deleted matches
    if (matchIterator1->size() < 2) continue;
    nInput1++;   // Finally got a good one

   // for all matches image 2 -> image 1
    int ntwo(0);
    for (std::vector<std::vector<cv::DMatch> >::const_iterator matchIterator2= matches2.begin();
         matchIterator2!= matches2.end() ;  ++matchIterator2) {

      // ignore deleted matches
      if (matchIterator2->size() < 2) continue;
      ntwo++;

      // Match symmetry test
      if ((*matchIterator1)[0].queryIdx ==
          (*matchIterator2)[0].trainIdx &&
          (*matchIterator2)[0].queryIdx ==
          (*matchIterator1)[0].trainIdx) {
          // add symmetrical match
        symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,
                             (*matchIterator1)[0].trainIdx,
                             (*matchIterator1)[0].distance));
        break; // next match in image 1 -> image 2
      }
    }
    nInput2 = qMax(nInput2, ntwo);
  }

  // Return processing time
  mtime = elapsed(stime);

  if ( isDebug() ) {
    logger() << "  Total Input Matches1x2 Tested: " << nInput1 << " x " << nInput2 << "\n";
    logger() << "  Total Passing Symmetric Test:  " << symMatches.size() << "\n";
    logger() << "  Processing Time:               " << mtime << "\n";
    logger().flush();
  } 

  return;
}

 // Identify good matches using RANSAC outlier detection in epipolar 
 // relationship.
 // Return fundamental matrix
cv::Mat RobustMatcher::ransacTest(const std::vector<cv::DMatch>& matches,
                                  const std::vector<cv::KeyPoint>& keypoints1,
                                  const std::vector<cv::KeyPoint>& keypoints2,
                                  std::vector<cv::DMatch>& outMatches, 
                                  double &mtime, const bool onErrorThrow) 
                                  const {


  // Introduce ourselves
  if ( isDebug() ) {
    logger() << "Entered EpiPolar RobustMatcher::ransacTest(matches, keypoints1/2...)...\n";
    logger() << " -Running EpiPolar Constraints/Fundamental Matrix...\n";
    logger() << "  RobustMatcher::EpiTolerance:    " << getEpiTolerance() << "\n";
    logger() << "  RobustMatcher::EpiConfidence:   " << getEpiConfidence() << "\n";
    logger() << "  Number Initial Matches:         " << matches.size() << "\n";
    logger().flush(); 
  }


  outMatches.clear();
  mtime = 0.0;
  cv::Mat fundamental = cv::Mat::eye(3,3,CV_64F);

  // See if enough points to compute fundamental matrix. Minumum number needed
  // for RANSAC is 8 points.
  if ( (unsigned int) m_minEpiPoints > matches.size() ) {
    if ( isDebug() ) {
      logger() << "->ERROR - Not enough points (need at least "
               << m_minEpiPoints << ") to proceed - returning identity!\n"; 
      logger().flush();
    } 
    return ( fundamental );
  }

  // Got some points, get the timer started
  QTime stime; 
  stime.start();

  // Convert keypoints into Point2f
  std::vector<cv::Point2f> points1, points2;
  for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
          it!= matches.end(); ++it) {

    // Get the position of left keypoints
    points1.push_back(keypoints1[it->queryIdx].pt);
    // Get the position of right keypoints
    points2.push_back(keypoints2[it->trainIdx].pt);
  }

  // Compute fundamental matrix using RANSAC
  std::vector<uchar> inliers(points1.size(),0);
  try {
    fundamental = cv::findFundamentalMat(cv::Mat(points1),cv::Mat(points2), // matching points
                                         CV_FM_RANSAC, // RANSAC method
                                         getEpiTolerance(),   // distance to epipolar line
                                         getEpiConfidence(), // confidence probability
                                         inliers);     // match status (inlier or outlier))
  }
  catch ( cv::Exception &e ) {
    QString mess = "1st fundamental (epipolar) test failed!"
                   " QueryPoints=" + QString::number(points1.size()) + 
                   ", TrainPoints=" + QString::number(points2.size()) + 
                   ".  cv::Error - " + e.what();
    if ( onErrorThrow ) throw IException(IException::Programmer, mess, _FILEINFO_);
    else if ( isDebug()  ) {
      logger() << "->ERROR: " << mess << "return identity matrix for fundamental!\n";
      logger().flush();
    }
    mtime = elapsed(stime);
    return ( fundamental );
  }

  // extract the surviving (inliers) matches (only valid for CV_FM_RANSAC and
  // CV_FM_LMEDS methods)!!!!
  std::vector<uchar>::const_iterator      itIn = inliers.begin();
  std::vector<cv::DMatch>::const_iterator itM  = matches.begin();
  for ( ;itIn != inliers.end(); ++itIn, ++itM) {
    if (*itIn) { // it is a valid match
      outMatches.push_back(*itM);
    }
  }

  if ( isDebug() ) {
    logger() << "  Inliers on 1st Epipolar:        " << outMatches.size() << "\n";
    logger().flush();
  } 

  if ( m_refineF  ) {

    // See if enough points to compute fundamental matrix. Minumum number needed
    // for RANSAC is 8 points.
    if ( (unsigned int) m_minEpiPoints <= outMatches.size() ) {

      // Make a copy of the inliers of the first fundamental processing
      std::vector<cv::DMatch> matches1(outMatches);
      outMatches.clear();

      // The F matrix will be recomputed with all accepted matches Convert
      // keypoints into Point2f for final F computation
      points1.clear();
      points2.clear();
      for (std::vector<cv::DMatch>::const_iterator it = matches1.begin();
           it != matches1.end(); ++it) {

        // Get the position of left keypoints
        points1.push_back(keypoints1[it->queryIdx].pt);
        // Get the position of right keypoints
        points2.push_back(keypoints2[it->trainIdx].pt);
      }

      // Compute 8-point F from all accepted matches
      std::vector<uchar> inliers2(points1.size(),0);
      cv::Mat fundamental2 = fundamental;
      try {
        fundamental2 = cv::findFundamentalMat(cv::Mat(points1), cv::Mat(points2), // matches 
                                             CV_FM_LMEDS,  // Least squares method
                                             getEpiTolerance(),   // distance to epipolar line
                                             getEpiConfidence(), // confidence probability
                                             inliers2);   // New set of inliers
        fundamental = fundamental2;
      }
      catch ( cv::Exception &e ) {
        outMatches.clear();
        QString mess = "2st fundamental (epipolar) test failed!"
                       " QueryPoints=" + QString::number(points1.size()) + 
                       ", TrainPoints=" + QString::number(points2.size()) + 
                       ".  CV::Error - " + e.what();
        if ( onErrorThrow ) throw IException(IException::Programmer, mess, _FILEINFO_);
        else if ( isDebug()  ) {
          logger() << "->Refinement ERROR: " << mess << "return initial matrix for fundamental!\n";
          logger().flush();
        }

        mtime = elapsed(stime);
        return ( fundamental );
      }

      // Now clear the results from the first run and recompute inliers
      std::vector<uchar>::const_iterator      itIn2 = inliers2.begin();
      std::vector<cv::DMatch>::const_iterator itM2  = matches1.begin();
      // for all matches
      for ( ;itIn2 != inliers2.end(); ++itIn2, ++itM2) {
        if (*itIn2) { // it is a valid match
          outMatches.push_back(*itM2);
        }
      }

      if ( isDebug() ) {
        logger() << "  Inliers on 2nd Epipolar:        " << outMatches.size() << "\n";
        logger().flush();
      } 
    }
    else {
      if ( isDebug() ) {
        logger() << "  Not enough points (" << outMatches.size()  
                 << ", needs " << m_minEpiPoints
                 << ") for 2nd Epipolar - returning current state!!\n";
        logger().flush();
      } 
    }
  }

  mtime = elapsed(stime);

  if ( isDebug() ) {
    logger() << "  Total Passing Epipolar:         " << outMatches.size() << "\n";
    logger() << "  Processing Time:                " << mtime << "\n";
    logger().flush();
  } 

 return ( fundamental );
}

cv::Mat RobustMatcher::computeHomography(const std::vector<cv::KeyPoint>& query,
                                         const std::vector<cv::KeyPoint>& train,
                                         const std::vector<cv::DMatch> &matches, 
                                         std::vector<cv::DMatch> &inliers,
                                         double &mtime, const int method, 
                                         const double tolerance,
                                         const bool refine,
                                         const bool onErrorThrow) 
                                         const {

  // Introduce ourselves
  if ( isDebug() ) {
    logger() << "Entered RobustMatcher::computeHomography(keypoints1/2, matches...)...\n";
    logger() << " -Running RANSAC Constraints/Homography Matrix...\n";
    logger() << "  RobustMatcher::HmgTolerance:  " << getHmgTolerance() << "\n";
    logger() << "  Number Initial Matches:       " << matches.size() << "\n";
    logger().flush();
  }

  // Initialize return parameters
  mtime = 0.0;
  cv::Mat homography = cv::Mat::eye(3,3,CV_64F);

  QTime stime;
  stime.start();  // Start the timer

  // Prepare source and train points
  std::vector<cv::Point2f> srcPoints, dstPoints;    
  for (unsigned int i = 0; i < matches.size() ; i++) {
     srcPoints.push_back(query[matches[i].queryIdx].pt);
     dstPoints.push_back(train[matches[i].trainIdx].pt);
  }

  // Test intial return condition 
  inliers.clear();
  if ( srcPoints.size() < (unsigned int) m_minHomoPoints ) { 
    if ( isDebug() ) {
      logger() << "  Not enough points  (" << srcPoints.size() 
                << ") to compute initial homography - need at least "
                << m_minHomoPoints << "!\n";
      logger().flush();
    }
    return ( homography );
  }

  try {
    // Find homography using RANSAC algorithm
    cv::Mat h2 = homography;
    h2 = cv::findHomography(srcPoints, dstPoints, method, tolerance);
    homography = h2;
    
    // Warp dstPoints to srcPoints domain using inverted homography transformation
    std::vector<cv::Point2f> srcReprojected;
    cv::perspectiveTransform(dstPoints, srcReprojected, homography.inv());

    // Pass only matches with low reprojection error (less than tolerance 
    // value in pixels) 
    double tolSquared = getHmgTolerance() * getHmgTolerance();
    for (unsigned int i = 0; i < matches.size() ; i++) {
        cv::Point2f actual   = srcPoints[i];
        cv::Point2f expected = srcReprojected[i];
        cv::Point2f v = actual - expected;
        const double distanceSquared = (v.x*v.x + v.y*v.y);
        
        if ( distanceSquared <= tolSquared ) {
          inliers.push_back(matches[i]);
        }
    }

    if ( isDebug() ) {
      logger() << "  Total 1st Inliers Remaining:  " << inliers.size() << "\n";
      logger().flush();
    }

    if ( true == refine ) {
      // Test for bad case
      if ( inliers.size() <  (unsigned int) m_minHomoPoints ) { 
        inliers.clear();
        if ( isDebug() ) {
          logger() << "  Not enough points (" << inliers.size() 
                    << ") to compute refined homography - need at least "
                    << m_minHomoPoints << " - failure!\n";
          logger().flush();
        }

        mtime = elapsed(stime);
        return ( homography );
      }

      // Set up return condition for refined homography
      std::vector<cv::DMatch> inliers0(inliers);
      inliers.clear();

      // Now use only good points to find refined homography:
      std::vector<cv::Point2f> refinedSrc, refinedDst;
      for (unsigned int i = 0; i < inliers0.size(); i++) {
          refinedSrc.push_back(query[inliers0[i].queryIdx].pt);
          refinedDst.push_back(train[inliers0[i].trainIdx].pt);
      }

      // Use least squares method to find precise homography
      h2 = cv::findHomography(refinedSrc, refinedDst, 0, tolerance);
      homography = h2;
      
      // Warp dstPoints to srcPoints domain using inverted homography transformation
      std::vector<cv::Point2f> srcFinal;
      cv::perspectiveTransform(refinedDst, srcFinal, homography.inv());
      // Compute remaining inliers
      for (unsigned int i = 0; i < inliers0.size() ; i++) {
        cv::Point2f actual   = refinedSrc[i];
        cv::Point2f expected = srcFinal[i];
        cv::Point2f v = actual - expected;
        const double distanceSquared = (v.x*v.x + v.y*v.y);
        
        if ( distanceSquared <= tolSquared ) {
          inliers.push_back(inliers0[i]);
        }
      }
    }
  }
  catch ( cv::Exception &e ) {
    QString mess = "RobustMatcher::HomographyFailed: with cv::Error - "+ 
                   QString(e.what());
    if ( onErrorThrow ) throw IException(IException::Programmer, mess , _FILEINFO_);
    else if ( isDebug()  ) {
      logger() << "->Homography ERROR: " << mess << "return current state of homography\n";
      logger().flush();
    }
    mtime = elapsed(stime);
    return ( homography );
  }

  mtime = elapsed(stime);
  if ( isDebug() ) {
    logger() << "  Total 2nd Inliers Remaining:  " << inliers.size() << "\n";
    logger() << "  Processing Time:              " << mtime << "\n";
    logger().flush();
  }

  return ( homography );
}

PvlObject RobustMatcher::info() const {
  PvlObject info("FeatureAlgorithm");
  info.addKeyword(PvlKeyword("Name", getDescription()));
  info.addKeyword(PvlKeyword("OpenCVVersion", CV_VERSION));
  info.addKeyword(PvlKeyword("Specification", getDescription()));

  AlgorithmParameters params;
  if ( !m_detector.empty() ) {
    // logger() <<  "FeatureAlgorithm::info(detector): " << m_detector->name() << "\n"; 
    info.addObject(params.getDescription(m_detector, "Detector")); 
  }

  if ( !m_extractor.empty() ) {
    // logger() <<  "FeatureAlgorithm::info(extractor): " << m_extractor->name() << "\n"; 
    info.addObject(params.getDescription(m_extractor, "Extractor")); 
  }

  if ( !m_matcher.empty() ) {
   // logger() <<  "FeatureAlgorithm::info(matcher): " << m_matcher->name() << "\n"; 
    info.addObject(params.getDescription(m_matcher, "Matcher")); 
  }

  if (  m_parameters.size() > 0 ) {
    PvlObject aparms("Parameters");
    BOOST_FOREACH ( const PvlKeyword &key, m_parameters ) {
      aparms.addKeyword(key);
    }
    info.addObject(aparms);
  }
  return ( info );
}


void RobustMatcher::init(const QString &name, const PvlFlatMap &parameters, 
                         const bool &allocateAlgorithms) { 
  m_name          = name;
  m_description   = m_name;
  m_parameters    = parameters;
  m_matcherSpec   = "";

  m_doRootSift    = toBool(getParameter("RootSift", "false"));
  m_ratio         = toDouble(getParameter("RATIO", "0.65"));
  m_epiConfidence = toDouble(getParameter("EPICONFIDENCE", "0.99"));
  m_epiTolerance  = toDouble(getParameter("EPITOLERANCE", "3.0"));
  m_hmgTolerance  = toDouble(getParameter("HMGTOLERANCE", "3.0"));
  m_maxpoints     = toInt(getParameter("MAXPOINTS", "0"));
  m_minEpiPoints  = toInt(getParameter("MinimumFundamentalPoints", "8"));
  m_refineF       = toBool(getParameter("RefineFundamentalMatrix", "true"));
  m_minHomoPoints = toInt(getParameter("MinimumHomographyPoints", "8"));

  // SURF is the default feature detector and extractor. Use Bruteforce matcher
  // with L-2 distance indexing.
  if ( allocateAlgorithms ) {
    m_detector  = new cv::SurfFeatureDetector(); 
    m_extractor = new cv::SurfDescriptorExtractor();
    m_matcher   = allocateMatcher(m_extractor, cv::NORM_L2, false);
  }
  return;
}

/**
 * @brief Allocate a BruteForceMatcher algorithm based upon descriptor extractor 
 *  
 * See 
 * http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_descriptor_matchers.html#bfmatcher-bfmatcher 
 * for details on this implementation. 
 * 
 * @author 2015-08-19 Kris Becker
 * 
 * @param extractor  Descriptor extractor algorithm 
 * @param crossCheck 
 * 
 * @return cv::Ptr<cv::DescriptorMatcher> 
 */
cv::Ptr<cv::DescriptorMatcher> RobustMatcher::allocateMatcher(cv::Ptr<cv::DescriptorExtractor> &extractor,
                                                              const int &normalize,
                                                              const bool &crossCheck) {

  QString name(QString::fromStdString(extractor->name()).toUpper());
  if ( isDebug() ) {
    logger() << "\nDetermining Matcher for " << name << "\n"; 
    logger().flush();
  }

  int normType(normalize);
  if ( name.contains("SURF", Qt::CaseInsensitive) ) {
    normType = cv::NORM_L2;
  } 
  else if ( name.contains("SIFT", Qt::CaseInsensitive)  ) {
    normType = cv::NORM_L2;
  } 
  else if (name.contains("ORB", Qt::CaseInsensitive) ) {
    normType = cv::NORM_HAMMING;
    try {
      if ( extractor->getInt("WTA_K") > 2) {
        normType = cv::NORM_HAMMING2;
      }
    }
    catch ( cv::Exception &e ) {
      //  NOOP - use existing value
    }
  }
  else if ( name.contains("BRISK", Qt::CaseInsensitive) ) {
    normType = cv::NORM_HAMMING;
  }
  else if ( name.contains("BRIEF", Qt::CaseInsensitive) ) {
    normType = cv::NORM_HAMMING;
  }

  // Update name and return matcher
  cv::Ptr<cv::DescriptorMatcher> matcher(new cv::BFMatcher(normType, crossCheck));
  m_matcherSpec = ("/" + QString::fromStdString(matcher->name()) + 
                   "@normType:" + QString::number(normType) +
                   "@crossCheck:" + ( (crossCheck) ? "true" : "false" ) );
  return ( matcher );
}

// Compute RootSift descriptors for better matching potential
void RobustMatcher::RootSift(cv::Mat &descriptors, const float eps) const {
  // Compute sums for L1 Norm
  cv::Mat sums_vec;
  descriptors = cv::abs(descriptors); //otherwise we draw sqrt of negative vals
  cv::reduce(descriptors, sums_vec, 1 /*sum over columns*/, CV_REDUCE_SUM, CV_32FC1);
  for(int row = 0; row < descriptors.rows; row++){
    int offset = row*descriptors.cols;
    for(int col = 0; col < descriptors.cols; col++){
      descriptors.at<float>(offset + col) = sqrt(descriptors.at<float>(offset + col) / 
                                            (sums_vec.at<float>(row) + eps) /*L1-Normalize*/); 
    }
  }
  return;
}


/** Returns elapsed time since the timer was started in seconds */
double RobustMatcher::elapsed(const QTime &runtime) const {
  return (runtime.elapsed() / 1000.0);
}


}   // namespace Isis
