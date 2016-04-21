/**                                                                       
 * @file                                                                  
 * $Revision: 6600 $
 * $Date: 2016-03-09 10:30:30 -0700 (Wed, 09 Mar 2016) $
 * $Id: FeatureAlgorithmFactory.cpp 6600 2016-03-09 17:30:30Z kbecker@GS.DOI.NET $
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
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include <QtGlobal>
#include <QCoreApplication>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

#include <boost/foreach.hpp>

#include <opencv2/opencv.hpp>

#include <opencv2/superres/superres.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include "FileName.h"
#include "IString.h"
#include "IException.h"
#include "FeatureAlgorithmFactory.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "RobustMatcher.h"

using namespace std;

namespace Isis {

/** Initialize the singleton factory pointer */
FeatureAlgorithmFactory *FeatureAlgorithmFactory::m_maker = 0;


/**
 * @brief Constructor 
 *  
 * This constructor will initialize the OpenCV interface within the ISIS 
 * environment by setting necessary parsing elements, setting the OpenCV error 
 * handler to an ISIS handler method, and enabling all OpenCV algorithms for 
 * parsing and resolving algorithms. 
 */
FeatureAlgorithmFactory::FeatureAlgorithmFactory() : AlgorithmParameters() {
//  This ensures this singleton is shut down when the application exists
   qAddPostRoutine(DieAtExit);
   m_nMade = 0;

   m_Feature2DModuleName = "Feature2D";
   m_DetectorModuleName  = "Detector";
   m_ExtractorModuleName = "Extractor";
   m_MatcherModuleName   = "Matcher";

   // Disable OpenCV's error handler by default to suppress text on screen
   disableOpenCVErrorHandler();

   // List here all those that require something other than uppercase names 
   // (OpenCV is very case-sensitive when it comes to algorithm names)
   m_algorithmNameMap << "SimpleBlob" << "Dense" << "FlannBased" 
                      << "BruteForce-Hamming(2)" << "BruteForce-Hamming" 
                      << "BruteForce-L1" << "BruteForce" << "BFMatcher";

   m_parameters = PvlFlatMap();

   // Initialize algorithms
   cv::initModule_contrib();
   cv::initModule_ml();
   cv::initModule_video();
   cv::superres::initModule_superres();
   cv::initModule_nonfree();  // Initialize non-free algorithms
   return;
}

/**
 * @brief Destructor 
 *  
 * All formal shutdown processing is handled by the DieAtExit() method that is 
 * envoked when Qt exists. 
 * 
 */
FeatureAlgorithmFactory::~FeatureAlgorithmFactory() { } 

/**
 * @brief Retrieve reference to Singleton instance of this object 
 *  
 * The only access provided for Singleton instance of this object. All method 
 * access is made through the pointer returned by this method. The object is 
 * created upon the first call to this method. The object is deleted when Qt 
 * shuts down. 
 * 
 * @return FeatureAlgorithmFactory* Pointer to Singleton instance of this object
 */
FeatureAlgorithmFactory *FeatureAlgorithmFactory::getInstance() { 
  if (!m_maker) {
    m_maker = new FeatureAlgorithmFactory();
  }
  return (m_maker);
}

/**
 * @brief Disable the OpenCV error handler and replace with ISIS exceptions 
 *  
 * This method can be called at any time to reestablish ISIS exception handling 
 * on behalf of OpenCV errors when they occur. This method is called when the 
 * object is first created via the getInstance() method. Users can establish 
 * their own handler by reenabling OpenCV handling calling the 
 * enableOpenCVErrors() method and using the OpenCV API to establish their own 
 * handler. 
 */
void FeatureAlgorithmFactory::disableOpenCVErrorHandler() {
   // Set up a CV error handler to suppress text on screen
   cv::redirectError(handleOpenCVErrors);
   return;
}

/**
 * @brief Re-enable OpenCV error handling 
 *  
 * This method will terminate ISIS error handling of OpenCV errors and return 
 * handling to OpenCV. 
 */
void FeatureAlgorithmFactory::enableOpenCVErrorHandler() {
   // Enable OpenCV error handling
   cv::redirectError(0);
   return;
}

/**
 * @brief Retrieve a list of all available algorithms from OpenCV 
 *  
 * This method returns a vector of strings that provide the names of all OpenCV 
 * algorithms available for use in this class for feature detection activities.
 * 
 * @return QStringList List of all OpenCV algorithms
 */
QStringList FeatureAlgorithmFactory::getListAll() const {
  vector<string> algorithms;
  cv::Algorithm::getList(algorithms);

  QStringList qsAlgos;
  BOOST_FOREACH ( string a, algorithms ) {
    qsAlgos.push_back(QString::fromStdString(a));
  }
  return (qsAlgos);
}

/** Return a list of all OpenCV Feature2D algorithms */
QStringList FeatureAlgorithmFactory::getListFeature2D() const {
  return (findWithPattern(getListAll(), m_Feature2DModuleName));
}

/** Return list of all OpenCV feature detector algorithms */
QStringList FeatureAlgorithmFactory::getListDetectors() const {
  return (findWithPattern(getListAll(), m_DetectorModuleName));
}

/** Return list of all OpenCV feature extractor algorithms */
QStringList FeatureAlgorithmFactory::getListExtractors() const {
  return (findWithPattern(getListAll(), m_ExtractorModuleName));
}

/** Return list of all OpenCV feature matcher algorithms */
QStringList FeatureAlgorithmFactory::getListMatchers() const {
  return (findWithPattern(getListAll(), m_MatcherModuleName));
}

/**
 * @brief Return reference to global matcher parameters 
 *  
 * This method returns a reference to all the parameters that will be passed on 
 * to the RobustMatcher algorithms. 
 * 
 * @return const PvlFlatMap&  Reference to matcher algorithms
 */
const PvlFlatMap &FeatureAlgorithmFactory::getGlobalParameters() const {
  return ( m_parameters );
}

/**
 * @brief Set the global parameters to use in all matchers created 
 *  
 * This method will accept a set of global paramters that will be applied to 
 * all RobustMatcher algorithms created by this factory.  It will replace any 
 * existing set of parameters with the contents of the globals parameter. 
 * 
 * @param globals Global parameters to set for all RobustMatcher algorithms
 */
void FeatureAlgorithmFactory::setGlobalParameters(const PvlFlatMap &globals) {
  m_parameters = globals;
}

/**
 * @brief Add parameters to the global set of RobustMatcher parameters 
 *  
 * This method will be added to the existing set of global parameters for 
 * creation of all RobustMatcher algorithms. It will replace/overried any 
 * existing parameters of the same name and add any new ones.
 * 
 * @param globals Set of new parameters to add to exist global parameters
 */
void FeatureAlgorithmFactory::addGlobalParameters(const PvlFlatMap &globals) {
  m_parameters = PvlFlatMap(m_parameters, globals);
}

/**
 * @brief Add a single parameter to the global RobustMatcher parameters 
 *  
 * This method is used to add a new or replace an exist parameter with value 
 * provided in the arguemts. 
 * 
 * @param name  Name of paramter to add/replace
 * @param value Value of the parameter
 */
void FeatureAlgorithmFactory::addParameter(const QString &name, 
                                           const QString &value) {
  m_parameters.add(name, value);
}

/**
 * @brief Provide Pvl Object describing parameters of an OpenCV algorithm
 *  
 * The complete parameter list of the specified OpenCV algorithm is retrieved 
 * and a Pvl Object is created that contains this description. In some cases, 
 * callers may want to simplify ore replace the real name. Use of the 
 * genericName parameter will be used as the name of the PvlObject return by 
 * this method rather than the full name (which may cause Pvl parse problems). 
 *  
 * @param name        Name of the OpenCV algorithm to return parameters for
 * @param genericName Optional name to use instead of real algorithm name
 * 
 * @return PvlObject Contains all parameters and values for the named algorithm
 */
PvlObject FeatureAlgorithmFactory::info(const QString &name,
                                        const QString &genericName) const {
//  cout << "Creating algorithm parameter info for " << name << "\n";
  QStringList candidates = getListAll().filter(name);
  if ( (candidates.size() < 1) || ( name.isEmpty() ) ) {
    QString mess = "Algorithm \"" + name + "\" does not exist or is invalid";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  // If there is more than one algorithm that matches this name, defer it to 
  // the method for this purpose
 if ( candidates.size() > 1 )  return ( info(candidates, name) );

  cv::Ptr<cv::Algorithm> a = cv::Algorithm::create<cv::Algorithm>(candidates[0].toStdString());
  if ( a.empty() ) {
    QString mess = "Failed to create algorithm " + name;
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  return ( getDescription(a, genericName) );
}

/**
 * @brief Create Pvl Objects with parameters for list of OpenCV algorithms 
 *  
 * This method creates objects for each algorithm that contains parameters and
 * current default values. Each algorithm object is contained in the top level 
 * object named "Algorithms". If a value is provided in genericName, it is added 
 * as a keyword in the top level Algoriths object. 
 *  
 * If an error should occur, the algorithm object is created but the name and a 
 * description of the error (in the Error keyword) is recorded instead of 
 * parameter/value objects. 
 *  
 * The OpenCV version is recorded in the "OpenCVVersion" keyword in the 
 * Algorithms object. 
 * 
 * @param algorithms  List of algorithms to create parameter/value objects
 * @param genericName Optional name of top level object 
 * 
 * @return PvlObject Top level Algorithms object contain algorithm 
 *                   parameter/value objects
 */
PvlObject FeatureAlgorithmFactory::info(const QStringList &algorithms, 
                                        const QString &genericName) const { 
  PvlObject pvlAlgo("Algorithms");
  pvlAlgo.addKeyword(PvlKeyword("OpenCVVersion", CV_VERSION));
  if ( !genericName.isEmpty() ) pvlAlgo.addKeyword(PvlKeyword("GenericName", 
                                                              genericName));

  BOOST_FOREACH ( QString name,  algorithms ) {
//    cout << "Getting list info for algorithm " << name << "\n";
    try {
      cv::Ptr<cv::Algorithm> a = cv::Algorithm::create<cv::Algorithm>(name.toStdString()); 
      // Check success here if an exception is not thrown
      if ( a.empty() ) {
        QString mess = "Failed to create algorithm - returned null pointer";
        PvlObject errout("Algorithm");
        errout.addKeyword(PvlKeyword("Name", name));
        errout.addKeyword(PvlKeyword("Error", mess));
        pvlAlgo.addObject(errout);
      }
      else {
        pvlAlgo.addObject( getDescription(a) );
      }
    }
    catch (cv::Exception &e) {
      PvlObject errout("Algorithm");
      errout.addKeyword(PvlKeyword("Name", name));
      errout.addKeyword(PvlKeyword("Error", toQt(e.what()) ));
      pvlAlgo.addObject(errout);
      continue;
    }
  }
  return ( pvlAlgo );
}

/**
 * @brief Create Pvl object of parameter/values for RobustMatcher algorithms 
 *  
 * This method generates PvlObjects containing the actual parameter/values for 
 * each RobustMatcher algorithm contained in the list. The parameters for each 
 * RobustMatcher algorithm consists of three individual OpenCV algorithms. 
 * For each RobustMatcher algorithm, there will be objects named "Detector", 
 * "Extractor" and "Matcher" containing the parameter/values for each one. These 
 * three objects will be contained in a PvlObject named "FeatureAlgorithm". In 
 * addition, keywords with the name (Name), version of OpenCV being used 
 * (OpenCVVersion) and the string specification used to create the RobustMatcher
 * algorithm (Specification) are also added to the object. These algorithms are 
 * added to a top level object called "FeatureAlgorithms".
 * 
 * @param algorithms  List of RobustMatcher algorithms to generate info for
 * @param genericName Genric name of the top level PvlObject
 * 
 * @return PvlObject Top level FeatureAlgorithms object containing current 
 *                   parameter/values for each RobustMatcher algorithm
 */
PvlObject FeatureAlgorithmFactory::info(const RobustMatcherList &algorithms, 
                                        const QString &genericName) const {
  PvlObject pvlAlgo("FeatureAlgorithms");
  if ( !genericName.isEmpty() ) pvlAlgo.addKeyword(PvlKeyword("GenericName", 
                                                              genericName));

  BOOST_FOREACH ( SharedRobustMatcher a,  algorithms ) {
    pvlAlgo.addObject( a->info() );
  }
  return ( pvlAlgo );
}

/**
 * @brief Create a series of feature-based RobustMatcher algorithms 
 *  
 * This method will create a list of individual OpenCV feature-based 
 * RobustMatcher algorithms from a strings specification. The specification of 
 * the string cantains a series of individual matcher specifications separated 
 * by a "|" character. 
 *  
 * A single feature matcher algorithm specification is of the basic form:
 * "/detector/extractor/matcher". Multiple algorithms can be provided in the
 * specification string where each feature matcher algorithm spec is separated
 * by a "|", e.g.: 
 *  
 *   "/detector1/extractor1/matcher1[/parameters@name:value...] | 
 *    /detector2/extractor2/matcher2[/parameters@name:value...]"
 *  
 * The input string is parsed and validated by instantiating algorithms using 
 * the OpenCV API. In the former case, a single RobustMatcher algorithm is 
 * instantiated. The latter form returns a list of RobustMatcher algorithms. 
 *  
 * The "/parameters@name:value" portion specifies parameter for the 
 * RobustMatcher algorithm. Many of these parameters are described in the 
 * RobustMatcher class documentation. Each individual algorithm can also have a 
 * parameters modified by adding a series of "@name:value@name:value..." for 
 * each algorithm specified. 
 * 
 * @param specs         String specification of RobustMatcher algorithms
 * @param errorIfEmpty  If the parser finds no valid algorithm specs and this 
 *                      parameter is true, and error is thrown, otherwise an
 *                      empty list is returned
 * 
 * @return RobustMatcherList List of OpenCV feature-based RobustMatcher 
 *                           algorithms
 */
RobustMatcherList FeatureAlgorithmFactory::create(const QString &specs,
                                                  const bool &errorIfEmpty) 
                                                  const {
  RobustMatcherList algoList;

  // Individual algorithm specifications are separated by vertical bars
  QStringList algorithms = specs.split("|", QString::SkipEmptyParts);
  // cout << "create: got " << algorithms.size() << " algorithms...\n";
  if ( algorithms.size() == 0 ) {
    if ( errorIfEmpty ) {
      QString mess = "No feature matcher algorithms provided!"; 
      throw IException(IException::User, mess, _FILEINFO_);
    }
    return (algoList);
  }

  //  Make all feature algorithms
  BOOST_FOREACH ( QString matcher, algorithms ) {
   // cout << "  making " << matcher << " algorithm...\n";
    SharedRobustMatcher algo( make(matcher) );
    if ( algo.isNull() ) {
      QString mess = "Failed to create feature matcher from spec " + matcher;
      throw IException(IException::User, mess, _FILEINFO_);
    }
    algoList.push_back(algo); 
  }
  return (algoList);
}

/**
 * @brief Create OpenCV feature-based RobustMatcher algorithm from string spec 
 *  
 *  This method creates a single OpenCV feature-based RobustMatcher class from a
 *  string specfication containing a detector, extractor and optionally a
 *  matcher. If a matcher is not provided, the RobustMatcher class will create
 *  one that is best suited for the detector/extractor combination.
 *  
 *  In addition, parameters that alter the behavior of the outlier detection
 *  processing, among oother things, in the RobustMatcher can be specified as an
 *  additional part of the string using the "/paramters@name:value..."
 *  specification.
 * 
 * @param definition A single string specification for an OpencCV feature-based 
 *                   algorithm from which to generate a RobustMatcher algorithm
 * 
 * @return RobustMatcher* Pointer to a RobustMatcher algorithm
 */
RobustMatcher *FeatureAlgorithmFactory::make(const QString &definition) const {

  cv::Ptr<cv::FeatureDetector>     detector;
  cv::Ptr<cv::DescriptorExtractor> extractor;
  cv::Ptr<cv::DescriptorMatcher>   matcher;

  QStringList featureName;
  QString     description(definition);

  QStringList parts = definition.split("/", QString::SkipEmptyParts);
  if ( (parts.size() < 2) || (parts.size() > 4) ) {
    QString mess = "Feature algorithm (" + definition + 
                   ") specification is  ill-formed.  Must be of the form: "
                   "Detector/Extractor[/Matcher][/parameters@...] where "
                   "Detector and Extractor can be a Feature2D algorithm or "
                   "of its own type. The Matcher is optional and an "
                   "appropriate one will be allocated based upon the type of "
                   "Extractor descriptor. Parameters that pertain to the "
                   "Robust matcher algorithm can be provided in the Parameters "
                   "option. See findfeatures documentation for what is "
                   "currently available as parameters.";
    throw IException(IException::User, mess, _FILEINFO_);
  }

  QStringList allOfThem = getListAll();
  int partno(0);
  PvlFlatMap algoparms;  // Any algorithm parameters are in spec
  BOOST_FOREACH ( QString specs, parts ) {
    QStringList elements = specs.split("@", QString::SkipEmptyParts);
    if ( elements.size() == 0 )  {
      QString mess = "Ill-formed \"algorithm[@parameter:value]+]\" ALGORITHM "  
                     "specified in " + specs;
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // First check if the spec is the algorithm parameters option and treat it
    // special
    QString feature = elements.takeFirst();
    if ( "parameters" == feature.toLower() ) {
      algoparms = parseParameters(specs);
      continue;
    }

    // Its a matcher component so parse it
    partno++;
    featureName.push_back(feature);
    QStringList candidate = findWithPattern(allOfThem, feature);
    if ( candidate.size() == 0  ) {
      // If its not validated, assume the user knows what they are doing
      candidate.push_back(feature);
    }

    // Got a single algorithm.  Determine which one
    QString ftype = candidate.takeFirst();
    QString realName = mapAlgorithmName(ftype.split(".").takeLast().toUpper());
    string name = toStd(realName);
    if ( isFeature2D(ftype)  ) {
//      cout << "Got a Feature2D engine algorithm: " << ftype 
//           << ", Name(" << name << ")...\n";
      if ( 1 == partno ) {
        detector = cv::Feature2D::create(name); 
        checkPtr(detector, "Failed to create Feature2D algorithm for " + ftype,
                 _FILEINFO_);
        setFormattedParameter(detector, elements); 
      }
      else if ( 2 == partno ) {
        extractor = cv::Feature2D::create(name); 
        checkPtr(extractor, "Failed to create Feature2D algorithm for " + ftype, 
                 _FILEINFO_);
        setFormattedParameter(extractor, elements); 
      }
      else {
        QString mess =  "Feature2D type " + ftype + " is not a valid Matcher type";
        throw IException(IException::User, mess, _FILEINFO_);
      }
    }
    else if ( isDetector( ftype ) ) {
      if ( 1 != partno ) {
        QString mess =  ftype + " is not a valid detector type";
        throw IException(IException::User, mess, _FILEINFO_);
      }
//      cout << "Making Detector: " << name << "\n";
      detector = cv::FeatureDetector::create(name);
      checkPtr(detector, "Failed to create Detector algorithm for " + ftype,
               _FILEINFO_);
      setFormattedParameter(detector, elements);
    }
    else if ( isExtractor( ftype ) ) {
     if ( 2 != partno ) {
        QString mess =  ftype + " is not a valid Extractor type";
        throw IException(IException::User, mess, _FILEINFO_);
      }
//      cout << "Making Extractor: " << name << "\n";
      extractor = cv::DescriptorExtractor::create(name); 
      checkPtr(extractor, "Failed to create Extractor algorithm for " + ftype,
               _FILEINFO_);
      setFormattedParameter(extractor, elements);
    }
    else if ( isMatcher( ftype) ) {
//      cout << "Got a matcher algorithm: " << ftype << " - RealName: " << name << "...\n";
      if ( 3 != partno ) {
        QString mess =  ftype + " is not a valid in this context must be a " +
                        (( 1 == partno ) ? "Detector" : "Extractor");
        throw IException(IException::User, mess, _FILEINFO_);
      }
      matcher = cv::DescriptorMatcher::create(name);
      checkPtr(matcher, "Failed to create Matcher algorithm for " + ftype,
               _FILEINFO_);
      setFormattedParameter(matcher, elements);
    }
    else {
      QString mess = "Algorithm feature \"" + feature + "\" not a valid feature. " +
                     "Must be member of one of the following CV modules: " + 
                     m_Feature2DModuleName + ", " + m_DetectorModuleName + ", " +
                     m_ExtractorModuleName + ", " + m_MatcherModuleName;
      throw IException(IException::User, mess, _FILEINFO_);
    }
  }

  // Verify and construct the RobustMatcher. Parameters specified in each 
  // algorithm specification take precendence over the globals.
  QScopedPointer<RobustMatcher> falgo;
  PvlFlatMap mergedParms(m_parameters, algoparms);
  if ( matcher.empty() ) {
    falgo.reset (new RobustMatcher(featureName.join("+"), detector, extractor,
                                   mergedParms)); 
    description += falgo->getMatcherDescription(); }
  else {
    falgo.reset(new RobustMatcher(featureName.join("/"), detector, extractor, 
                                  matcher, mergedParms));
  }

  // Check for validity
  if ( falgo.isNull()  ) {
    QString mess = "RobustMatcher::" + featureName.join("/") + 
                   " was not created successfully!";
    throw IException(IException::User, mess, _FILEINFO_);
  }

  // Set the specfication string
  falgo->setDescription(description);

  m_nMade++; 
  return ( falgo.take() );
}

/** Returns the number of algorithms created by this object */
unsigned int FeatureAlgorithmFactory::manufactured() const {
  return (m_nMade);
}

/** Find a subset of strings that contain a particular pattern */
QStringList FeatureAlgorithmFactory::findWithPattern(const QStringList &flist, 
                                                     const QString &pattern) 
                                                     const {
  return ( flist.filter(pattern, Qt::CaseInsensitive) );
}

/** Check for special algorithm cases */
QString FeatureAlgorithmFactory::mapAlgorithmName(const QString &name) const {
  // Ok, special check for a specific case:  FlannBasedMatcher = FlannBased
  QString target = name.contains("Flann", Qt::CaseInsensitive) ? "Flann" : name;
  QStringList algoMap = findWithPattern(m_algorithmNameMap, target); 
  if ( !algoMap.empty() ) {
    return ( algoMap.takeLast() );
  }

  return ( name );
}

/** Determine if the string contains an OpenCV Feature2D algorithm spec */
bool FeatureAlgorithmFactory::isFeature2D(const QString &name) const {
  return (name.contains(m_Feature2DModuleName, Qt::CaseInsensitive));
}

/** Determine if the string contains an OpenCV detector algorithm spec */
bool FeatureAlgorithmFactory::isDetector(const QString &name) const {
  return (name.contains(m_DetectorModuleName, Qt::CaseInsensitive));
}

/** Determine if the string contains an OpemCV extractor algorithm spec */
bool FeatureAlgorithmFactory::isExtractor(const QString &name) const {
  return (name.contains(m_ExtractorModuleName, Qt::CaseInsensitive));
}

/** Detemine if the string contains an OpenCV matcher algorithm spec */
bool FeatureAlgorithmFactory::isMatcher(const QString &name) const {
  return (name.contains(m_MatcherModuleName, Qt::CaseInsensitive));
}


/**
 * @brief Parse a parameter string for values and return in parameter map
 *  
 * This method parses the parameter section of the RobustMatcher specification 
 * of the matcher algorithm. The general form of the string specification is 
 * "/parameter@name:value[@name1:value1...]". This will result in a Pvl map 
 * entry of the form "name = value". 
 *  
 * @param parameters Parameter specification part of the feature matcher string
 * 
 * @return PvlFlatMap Pvl map of the parameters parsed from the string
 */
PvlFlatMap FeatureAlgorithmFactory::parseParameters(const QString &parameters) 
                                                    const {
  PvlFlatMap pmap;

  // If the string is empty, return an empty paramter list
  QStringList parts = parameters.split("@");
  if ( parts.size() == 0 ) { return (pmap); }

  // Ensure the first part of the parameter list is "parameters" or an error
  // is assumed.
  QString parmtag = parts.takeFirst();
  if ( "parameters" != parmtag.toLower() ) {
    QString mess = "Illformed parameters string [" + parameters + "] - must be "
                   "of the form \"parameters@parm1:value1@parm2:value2...\"";
    throw IException(IException::User, mess, _FILEINFO_);
  }

  // All good so far, parse each parameter string
  BOOST_FOREACH ( QString specs, parts ) {

    // Is it valid?
    QStringList parmvaltag = specs.split(":");
    if ( 2 != parmvaltag.size() ) {
      QString mess = "Invalid parameter at or near [" + specs + "] in \"" +
                     parameters + "\" - must be of the form "
                     "\"parameters@parm1:value1@parm2:value2...\"";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    // Looks good, set the parameters
    pmap.add(parmvaltag[0], parmvaltag[1]);
  }

  // All done...
  return ( pmap );
}


/*

 */
/**
 * @brief Method to capture OpenCV exceptions/errors and suppress them
 *  
 * This allows us to control the output of the errors. OpenCV outputs them to
 * stderr.
 *
 * If, after you use this function in the call (as in findHomography and
 * computeReprojectionError):
 *
 *      cv::redirectError(handleOpenCVErrors); 
 *
 * you can revert back to the openCV default behavior with the following
 * function call:
 *
 *      cv::redirectError(nullptr);
 * 
 * @param status    Status of OpenCV error
 * @param func_name Name of function 
 * @param err_msg   Error message from OpenCV
 * @param file_name Name of source file
 * @param line      Line of source file
 * @param userdata  Optional user data assocaited with error
 * 
 * @return int 
 */
int FeatureAlgorithmFactory::handleOpenCVErrors( int status,
                                                 const char* func_name,
                                                 const char* err_msg,
                                                 const char* file_name, 
                                                 int line, void* userdata ) {
  //Do nothing -- will suppress console output
  return 0;   //Return value is not used
}

 /**
   * @brief Exit termination routine
   *
   * This (static) method ensure this object is destroyed when Qt exits.  
   *
   * Note that it is error to add this to the system _atexit() routine because
   * this object utilizes Qt classes.  At the time the atexit call stack is
   * executed, Qt is long gone resulting in Very Bad Things.  Fortunately, Qt has
   * an exit stack function as well.  This method is added to the Qt exit call
   * stack.
   */
  void FeatureAlgorithmFactory::DieAtExit() {
    delete  m_maker;
    m_maker = 0;
    return;
  }


} // namespace Isis
