#ifndef FeatureAlgorithmFactory_h
#define FeatureAlgorithmFactory_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <cstdarg>
#include <istream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "IException.h"
#include "FeatureInventory.h"
#include "PvlFlatMap.h"
#include "PvlObject.h"
#include "RobustMatcher.h"

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
 *   @history 2016-10-05 Ian Humphrey & Makayla Shepherd - Changed headers to OpenCV2.
 *   @history 2019-05-16 Aaron Giroux & Eric Gault - Added a regular expression to
 *                           formatSpecifications method to allow for pathnames to be entered
 *                           using the savepath parameter. Fixes 2474.
 *   @history 2022-02-10 Kris Becker Added parseGlobalParamters() static method
 */
class FeatureAlgorithmFactory  {
  public:
    static FeatureAlgorithmFactory *getInstance();

    void enableOpenCVErrorHandler();
    void disableOpenCVErrorHandler();

    QStringList getListAll() const;

    static PvlFlatMap parseGlobalParameters(const QString &globals);
    void setGlobalParameters(const PvlFlatMap &globals);
    void addGlobalParameters(const PvlFlatMap &globals);
    void addParameter(const QString &name, const QString &value);
    const PvlFlatMap &globalParameters() const;

    PvlObject info(const QString &name) const;
    PvlObject info(const QStringList &algorithms) const;
    PvlObject info(const RobustMatcherList &algorithmList) const;

    RobustMatcherList create(const QString &specifications,
                             const bool &errorIfEmpty = true) const;
    SharedRobustMatcher make(const QString &definition) const;

    unsigned int manufactured() const;

  private:
    static FeatureAlgorithmFactory *m_maker;
    mutable unsigned int m_nMade;
    FeatureInventory m_algorithmInventory;
    PvlFlatMap m_globalParameters;

    // The Singleton constructor/destructor are private
    FeatureAlgorithmFactory();
    ~FeatureAlgorithmFactory();

    int initialize();

    QStringList formatSpecifications(QString specification) const;

    MatcherAlgorithmPtr createMatcher(FeatureAlgorithmPtr extractor,
                                      const QString &normalize = "NORM_L2",
                                      const QString &crossCheck = "false") const;

    static int handleOpenCVErrors( int status, const char* func_name,
                                   const char* err_msg,const char* file_name,
                                   int line, void* userdata );
    static void DieAtExit();

};

}  // namespace Isis

#endif
