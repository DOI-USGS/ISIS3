#ifndef FeatureAlgorithmFactory_h
#define FeatureAlgorithmFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision $ 
 * $Date: 2016-03-09 10:31:51 -0700 (Wed, 09 Mar 2016) $ 
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
#include <cstdarg>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "AlgorithmParameters.h"
#include "RobustMatcher.h"
#include "IException.h"
#include "PvlFlatMap.h"
#include "PvlObject.h"

namespace Isis {

/**
 * @brief Factory providing creation of feature matching algorithms 
 *  
 * This class provides an API to the OpenCV Feature2D Framework suite of 
 * algorithms. 
 *  
 * This singleton class initializes the OpenCV interface by loading all 
 * available feature detection algorithms according the documentation. See 
 * http://docs.opencv.org/2.4.12/ for details of the OpenCV version this class 
 * is implemented for. 
 *  
 *  This factory is designed to construct one or more RobustMatcher objects from
 *  a fully parameterized string specification. The string specification
 *  contains one or more designations of feature matching algorithms
 *  combinations. A feature matching algorithm combination contains three OpenCV
 *  algorithm that specify the name and parameters of a feature detector,
 *  feature extractor and feature matcher algorithm. These algorithms are
 *  selected from the suite of the Feature@2D Framework described at the OpemCV
 *  web location
 *  http://docs.opencv.org/2.4.12/modules/features2d/doc/features2d.html. There
 *  are others available in the contrib location
 *  http://docs.opencv.org/2.4.12/modules/nonfree/doc/nonfree.html.
 *  
 *  A single feature matcher algorithm specification is of the basic form:
 *  "/detector/extractor/matcher". Multiple algorithms can be provided in the
 *  specification string where each feature matcher algorithm spec is separated
 *  by a "|", e.g.,
 *  "/detector1/extractor1/matcher1|/detector2/extractor2/matcher2". The input
 *  string is parsed and validated by instantiating algorithms using the OpenCV
 *  API. In the former case, a single RobustMatcher algorithm is instantiated.
 *  The latter form returns a list of RobustMatcher algorithms.
 *  
 *  In addition to specification of Feature2D algorithms, most parameters for
 *  each algorithm may be modifed in the string specification. The generic form
 *  of this option is for a single algorithm is
 *  "/algorithm@parameter:value[@parameter:value...]". Parameters are specific
 *  to only each algorithm. All algorithm and parameter names are
 *  case-insensitive even though the OpenCV API is not. Steps are taken to
 *  properly apply proper character case.
 *  
 *  Here is an example of a specification:
 *  
 *  detector.fastx@threshold:15@type:2/sift//Matcher.BFMatcher@normType:4@crossCheck:false
 *  
 *  This specification selects the FASTX feature detector algorithm and sets
 *  parameters "threshold" to 25 and "type" to 2.  The extractor is a basic SIFT
 *  algorithm with no parameter modifications. The matcher is a BruteForce
 *  matcher with parameters "normType" set to 4 and "crossCheck" set to false.
 *  Note that "crossCheck" should always be set to false because the
 *  RobustMatcher class has a special specific implementation of this algorithm.
 *  
 *  The create() (multiple) or make() (single) methods are given the
 *  specification string and one or more RobustMatcher algorithms are created
 *  with a set of three distinct algorithms.
 *  
 *  Note that the matcher algorithm is not required to be specified in the
 *  string as the RobustMatcher will determine the best matcher algorithm with
 *  the proper parameterization for each one (this is the recommended approach).
 * 
 * @author 2015-10-01 Kris Becker 
 * @internal 
 *   @history 2015-10-01 Kris Becker - Original Version 
 *   @history 2016-03-06 Kris Becker Completed documentation 
 */ 
class FeatureAlgorithmFactory : public AlgorithmParameters {
  public:
    static FeatureAlgorithmFactory *getInstance();

    void enableOpenCVErrorHandler();
    void disableOpenCVErrorHandler();

    QStringList getListAll() const;
    QStringList getListFeature2D() const;
    QStringList getListDetectors() const;
    QStringList getListExtractors() const;
    QStringList getListMatchers() const;

    void setGlobalParameters(const PvlFlatMap &globals);
    void addGlobalParameters(const PvlFlatMap &globals);
    void addParameter(const QString &name, const QString &value);
    const PvlFlatMap &getGlobalParameters() const;

    PvlObject info(const QString &name, 
                   const QString &genericName = "") const;
    PvlObject info(const QStringList &algorithms, 
                   const QString &genericName = "") const;
    PvlObject info(const RobustMatcherList &algorithms, 
                   const QString &genericName = "") const;

    RobustMatcherList create(const QString &specifications,
                             const bool &errorIfEmpty = true) const;
    RobustMatcher *make(const QString &definition) const;

    unsigned int manufactured() const;

  private:
    static FeatureAlgorithmFactory *m_maker;
    mutable unsigned int m_nMade;

    QString     m_Feature2DModuleName;
    QString     m_DetectorModuleName;
    QString     m_ExtractorModuleName;
    QString     m_MatcherModuleName;
    QStringList m_algorithmNameMap;
    PvlFlatMap  m_parameters;

    // The Singleton constructor/destructor are private
    FeatureAlgorithmFactory();
    ~FeatureAlgorithmFactory();


    QStringList findWithPattern(const QStringList &flist, 
                                const QString &pattern) const;
    QString     mapAlgorithmName(const QString &name) const;

    bool isFeature2D(const QString &name) const;
    bool isDetector(const QString &name) const;
    bool isExtractor(const QString &name) const;
    bool isMatcher(const QString &name) const;
    PvlFlatMap parseParameters(const QString &parameters) const;

    static int handleOpenCVErrors( int status, const char* func_name, 
                                   const char* err_msg,const char* file_name, 
                                   int line, void* userdata );
    static void DieAtExit();

};

}  // namespace Isis

#endif


