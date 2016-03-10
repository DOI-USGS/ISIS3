/**                                                                       
 * @file                                                                  
 * $Revision: 6598 $
 * $Date: 2016-03-08 11:22:39 -0700 (Tue, 08 Mar 2016) $
 * $Id: AlgorithmParameters.cpp 6598 2016-03-08 18:22:39Z kbecker@GS.DOI.NET $
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

#include "AlgorithmParameters.h"
#include "FileName.h"
#include "IString.h"
#include "IException.h"
#include "PvlKeyword.h"
#include "PvlObject.h"

using namespace std;

namespace Isis {


/* Constructor */
AlgorithmParameters::AlgorithmParameters() { }

/* Destructor */
AlgorithmParameters::~AlgorithmParameters() { } 

/**
 * @brief Get a PVL Object style description of Algorithm and its parameters
 * 
 * @param algorithm Pointer to algorithm to generate a PVL Object for its 
 *                  parameters
 * @param aType     Specifies the type of algorithm
 * 
 * @return PvlObject 
 */
PvlObject AlgorithmParameters::getDescription(const cv::Algorithm *algorithm, 
                                                  const QString &aType) 
                                                  const {

  checkPtr(algorithm, "Null Algorithm pointer - cannot get description",
           _FILEINFO_);

  PvlObject description("Algorithm");
  if ( !aType.isEmpty() ) {
     description.addKeyword(PvlKeyword("Type", aType));
  }

  description.addKeyword(PvlKeyword("Name", toQt(algorithm->name())) ); 

  vector<string> parameters;
  algorithm->getParams(parameters);
  for (unsigned int i = 0 ; i < parameters.size() ; i++) {
    QString name = toQt(parameters[i]);
    PvlKeyword key = PvlKeyword(name, getParameter(algorithm, name) );
    description.addKeyword(key); 
  }
  return (description);
}

/**
 * @brief Get a type of an OpenCV Algorithm parameters 
 *  
 * The returned value is an OpenCV internal type code that is used in the 
 * proper translation of the parameter.  Valid return types vary - see the 
 * OpenCV documentation for type codes. 
 * 
 * @param algorithm  Algorithm to get parameter type from
 * @param parameter  Name of parameter to get type for
 * 
 * @return int 
 */
int AlgorithmParameters::getParameterType(const cv::Algorithm *algorithm,
                                          const QString &parameter) const {


  checkPtr(algorithm, "Null Algorithm pointer - cannot get parameter type for " + parameter,
           _FILEINFO_);

  string name = getParameterName(algorithm, parameter);
  return (algorithm->paramType(name.c_str()));
}


/**
 * @brief Determine if a parameter exists in the given algorithm 
 *  
 * This method accepts a case insensitive string naming a potential parameter 
 * within the given algorithm. 
 * 
 * @param algorithm Algorithm to look up parameter for
 * @param parameter Name of parameter to look up.
 * 
 * @return bool True if the parameter exists in the given Algorithm
 */
bool AlgorithmParameters::hasParameter(const cv::Algorithm *algorithm, 
                                       const QString &parameter) const { 
  vector<string> names;
  algorithm->getParams(names);
  QString target(parameter.toLower());
  BOOST_FOREACH ( string name, names ) {
    if ( toQt(name).toLower()  == target)  return ( true );
  }
  return ( false );

}

/**
 * @brief Get the value of a named parameter in an Algorithm
 *  
 * This method will retrieve the value of a named parameter in the given 
 * Algorithm and return it as a string. 
 *  
 * Matrices, vectors and Algorithms are not supported value types in this 
 * method. 
 *  
 * @param algorithm Algorithm to get parameter from
 * @param parameter Name of parameter to get value for
 * 
 * @return QString String representation of the value of the parameter. 
 * @see getParameterVariant() 
 */
QString AlgorithmParameters::getParameter(const cv::Algorithm *algorithm,
                                          const QString &parameter) 
                                          const { 

  checkPtr(algorithm, "Null Algorithm pointer - cannot get parameter " + parameter,
           _FILEINFO_);

  QString value("Null");
  try {
    int argType = getParameterType(algorithm, parameter); 
    switch ( argType ) {
      case cv::Param::BOOLEAN:
        value = toString(algorithm->getBool(toStd(parameter)));
        break;

      case cv::Param::INT:
      case cv::Param::SHORT:
      case cv::Param::UNSIGNED_INT:
      case cv::Param::UCHAR:
      case cv::Param::UINT64:
        value = toString(algorithm->getInt(toStd(parameter)));
        break;

      case cv::Param::REAL:
      case cv::Param::FLOAT:
        value = toString(algorithm->getDouble(toStd(parameter)));
        break;

      case cv::Param::STRING:
        value = toQt(algorithm->getString(toStd(parameter)));
        break;

      case cv::Param::MAT:
        value = "cv::Mat";
        break;

      case cv::Param::MAT_VECTOR:
        value = "cv::Mat_Vector";
        break;

      case cv::Param::ALGORITHM: {
          cv::Ptr<cv::Algorithm> subalgo = getAlgorithm(algorithm, parameter);
          if ( !subalgo.empty() ) { value = toQt(subalgo->name()); }
          else {                    value = "Null";  }
        }
        break;

    default:
        value = "Null";
        break;
    } 
  }
  catch (cv::Exception &e) {
    QString mess = "Cannot get parameter  " + parameter + ", OpenCV::Error - " +
                    toQt(e.what());
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }

  return (value);
}

/**
 * @brief Get the value of a named parameter in an Algorithm
 *  
 * This method will retrieve the value of a named parameter in the given 
 * Algorithm and return it as a generic QVariant type. 
 *  
 * All OpenCV parameter types are supported by this method. 
 *  
 * @param algorithm Algorithm to get parameter from
 * @param parameter Name of parameter to get value for
 * 
 * @return QVariant Returns the value as a QVariant
 * @see getParameter() 
 */
QVariant AlgorithmParameters::getParameterVariant(const cv::Algorithm *algorithm, 
                                                  const QString &parameter) const {


  checkPtr(algorithm, "Null Algorithm pointer - cannot get parameter variant " + parameter,
           _FILEINFO_);

  QVariant value;
  try {
    int argType = getParameterType(algorithm, parameter); 
    switch ( argType ) {
      case cv::Param::BOOLEAN:
        value = QVariant(algorithm->getBool(toStd(parameter)));
        break;

      case cv::Param::INT:
      case cv::Param::SHORT:
      case cv::Param::UNSIGNED_INT:
      case cv::Param::UCHAR:
      case cv::Param::UINT64:
        value = QVariant(algorithm->getInt(toStd(parameter)));
        break;

      case cv::Param::REAL:
      case cv::Param::FLOAT:
        value = QVariant(algorithm->getDouble(toStd(parameter)));
        break;

      case cv::Param::STRING:
        value = QVariant(toQt(algorithm->getString(toStd(parameter))));
        break;

      case cv::Param::MAT:
        value = QVariant::fromValue(algorithm->getMat(toStd(parameter)));
        break;

      case cv::Param::MAT_VECTOR:
        value = QVariant::fromValue(algorithm->getMatVector(toStd(parameter)));
        break;

      case cv::Param::ALGORITHM:
        value = QVariant::fromValue(getAlgorithm(algorithm, parameter));
        break;

      default:
        break;
    } 
  }
  catch (cv::Exception &e) {
    QString mess = "Cannot get parameter to variant " + parameter + ", OpenCV::Error - " +
                    toQt(e.what());
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }

  return (value);
}

/**
 * @brief Set an Algorithm parameter with the given value string representation
 *  
 * This method will take a string representation of an Algorithm parameter 
 * value, convert it to the proper internal representation and set in the given 
 * algorithm. 
 *  
 * Matrices, vectors and algorithm types are not supported in this method 
 *  
 * @param algorithm Algorithm to set the parameter
 * @param parameter Name of the paramter to set
 * @param value     String of the value to convert and set in the algorithm
 * 
 * @return bool True if successful, false if failed 
 * @see setParameterVariant() 
 */
bool AlgorithmParameters::setParameter(cv::Algorithm *algorithm, 
                                       const QString &parameter, 
                                       const QString &value) const {


  checkPtr(algorithm, "Null Algorithm pointer - cannot set parameter " + parameter,
           _FILEINFO_);

  try {
    string name = getParameterName(algorithm, parameter); 
    int argType = getParameterType(algorithm, parameter);
    switch ( argType ) {
      case cv::Param::BOOLEAN:
        algorithm->set(name, toBool(value));
        break;
   
      case cv::Param::UCHAR: {
          unsigned char uval = cv::saturate_cast<uchar>(toInt(value));
          algorithm->set(name, uval);
        }
        break;

      case cv::Param::INT:
      case cv::Param::SHORT:
      case cv::Param::UNSIGNED_INT:
      case cv::Param::UINT64:
        algorithm->set(name, toInt(value));
        break;

      case cv::Param::REAL:
      case cv::Param::FLOAT:
        algorithm->set(name, toDouble(value));
        break;

      case cv::Param::STRING:
        algorithm->set(name, toStd(value));
        break;

      case cv::Param::MAT:
      case cv::Param::MAT_VECTOR:
      case cv::Param::ALGORITHM:
      default: {
        QString mess = "Data type " + QString::number(argType) + 
                       " not supported in this method for parameter " + parameter;
        throw IException(IException::Programmer, mess, _FILEINFO_);
      }
    } 
  }
  catch (cv::Exception &e) {
    QString mess = "Cannot set parameter " + parameter + ", OpenCV::Error - " +
                    toQt(e.what());
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }
  return (true);
}

/**
 * @brief Set an Algorithm parameter with value in QVariant
 *  
 * This method will take a QVariant representation of an Algorithm parameter 
 * value and set in the given algorithm. All types are assumed to be of the 
 * cooresponding type if the parameter 
 *  
 * All OpenCV paremeter types are supported.  Exception are thrown if the 
 * QVairiant type cannot be converted to the parameter type. 
 *  
 * @param algorithm Algorithm to set the parameter
 * @param parameter Name of the paramter to set
 * @param value     QVarint value to set in the algorithm
 * 
 * @return bool True if successful, false if failed 
 * @see setParameter() 
 */
bool AlgorithmParameters::setParameterVariant(cv::Algorithm *algorithm, 
                                              const QString &parameter, 
                                              const QVariant &value) const {

  checkPtr(algorithm, "Null Algorithm pointer - cannot set parameter variant " + parameter,
           _FILEINFO_);

  try {
    string name = getParameterName(algorithm, parameter); 
    int argType = getParameterType(algorithm, parameter);
    switch ( argType ) {
      case cv::Param::BOOLEAN:
        algorithm->set(name, value.toBool());
        break;
   
      case cv::Param::UCHAR: {
          unsigned char uval = cv::saturate_cast<uchar>(value.toInt());
          algorithm->set(name, uval);
        }
        break;

      case cv::Param::INT:
      case cv::Param::SHORT:
      case cv::Param::UNSIGNED_INT:
      case cv::Param::UINT64:
        algorithm->set(name, value.toInt());
        break;

      case cv::Param::REAL:
      case cv::Param::FLOAT:
        algorithm->set(name, value.toDouble());
        break;

      case cv::Param::STRING:
        algorithm->set(name, toStd(value.toString()));
        break;

      case cv::Param::MAT:
        if ( value.canConvert<cv::Mat>() ) {
          algorithm->set(name, value.value<cv::Mat>());
        }
        else {
          QString mess = "Cannot convert " + parameter + " parameter to cv::Mat";
          throw IException(IException::Programmer, mess, _FILEINFO_);
        }
        break;

      case cv::Param::MAT_VECTOR:
        if ( value.canConvert<vector<cv::Mat> >() ) {
          algorithm->set(name, value.value<vector<cv::Mat> >());
        }
        else {
          QString mess = "Cannot convert " + parameter + " parameter to vector<cv::Mat>";
          throw IException(IException::Programmer, mess, _FILEINFO_);
        }
        break;

      case cv::Param::ALGORITHM:
        if ( value.canConvert<cv::Ptr<cv::Algorithm> >() ) {
          algorithm->set(name, value.value<cv::Ptr<cv::Algorithm> >());
        }
        else {
          QString mess = "Cannot convert " + parameter + " parameter to cv::Prt<Algorithm>";
          throw IException(IException::Programmer, mess, _FILEINFO_);
        }
        break;

      default: {
        QString mess = "Data type " + QString::number(argType) + 
                       " not supported in this method for parameter " + parameter;
        throw IException(IException::Programmer, mess, _FILEINFO_);
      }
    } 
  }
  catch (cv::Exception &e) {
    QString mess = "Cannot set parameter w/variant " + parameter + ", OpenCV::Error - " +
                    toQt(e.what());
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }
  return (true);
}

/**
 * @brief Retrieve an Algorithm type parameter from an Algorithm
 *  
 * This method will get the Algorithm type from a parameter in the given 
 * Algorithm. 
 *  
 * @param algorithm Algorithm to get the parameter for
 * @param parameter Name of the algorithm in the Algorithm
 * 
 * @return cv::Ptr<cv::Algorithm> Pointer to Algorithm if sucessful
 */
cv::Ptr<cv::Algorithm> AlgorithmParameters::getAlgorithm(const cv::Algorithm *algorithm, 
                                                         const QString &parameter) const {
  checkPtr(algorithm, "Null Algorithm pointer - cannot get parameter " + parameter,
           _FILEINFO_);
  int argType = getParameterType(algorithm, parameter);
  if ( argType != cv::Param::ALGORITHM ) {
    throw IException(IException::Programmer, 
                     "Parameter " + parameter + " is not an algorithm", 
                     _FILEINFO_); 
  } 
  return (algorithm->getAlgorithm(toStd(parameter)));
}

/** 
 * @brief Retrieve a Matrix type parameter from an Algorithm
 *  
 * This method will get the matrix type from a parameter in the given 
 * Algorithm. 
 *  
 * @param algorithm Algorithm to get the parameter for
 * @param parameter Name of the matrix in the Algorithm
 * @return cv::Mat Returns the matrix in the given parameter
 */
cv::Mat AlgorithmParameters::getMat(const cv::Algorithm *algorithm, 
                                    const QString &parameter) const {

  checkPtr(algorithm, "Null Algorithm pointer - cannot get cv::Mat " + parameter, 
           _FILEINFO_);
  int argType = getParameterType(algorithm, parameter);
  if ( argType != cv::Param::MAT ) {
    throw IException(IException::Programmer, 
                     "Parameter " + parameter + " is not an cv::Mat", 
                     _FILEINFO_); 
  } 
  return (algorithm->getMat(toStd(parameter)));
}

/** 
 * @brief Retrieve a Matrix Vector type parameter from an Algorithm
 *  
 * This method will get the matrix vector type from a parameter in the given 
 * Algorithm. 
 *  
 * @param algorithm Algorithm to get the parameter for
 * @param parameter Name of the matrix vector in the Algorithm
 * @return cv::Mat Returns the matrix ivector n the given parameter
 */
vector<cv::Mat> AlgorithmParameters::getMatVector(const cv::Algorithm *algorithm, 
                                                  const QString &parameter) const {

  checkPtr(algorithm, "Null Algorithm pointer - cannot get cv::MatVector " + parameter,
            _FILEINFO_);
  int argType = getParameterType(algorithm, parameter);
  if ( argType != cv::Param::MAT_VECTOR ) {
    throw IException(IException::Programmer, 
                     "Parameter " + parameter + " is not an cv::MatVector", 
                     _FILEINFO_); 
  } 
  return (algorithm->getMatVector(toStd(parameter)));
}

/** 
 * @brief Check the validity of an Algorithm pointer 
 *  
 * If the pointer given is invalid an error is thrown. 
 * 
 * @param algorithm Algorithm pointer to check
 * @param mess      Message to include in exception
 * @param sourceFile File data for error message formatting
 * @param lineno     File line for error message formatting
 */
void AlgorithmParameters::checkPtr(const cv::Algorithm *algorithm, 
                                   const QString &mess,
                                   const char *sourceFile,int lineno) const {
  if ( algorithm == 0 ) {
    throw IException(IException::Programmer, mess, sourceFile, lineno);
  }
  return;
}

/**
 * @brief Determine real parameter name from case insensitive version
 * 
 * @param algorithm Algorithm to look for parameter name
 * @param name      Name of parameter without regard to case
 * 
 * @return string Real name of the algorithm parameter
 */
string AlgorithmParameters::getParameterName(const cv::Algorithm *algorithm,
                                             const QString &name) const {

  checkPtr(algorithm, "Null Algorithm pointer - cannot get real parameter name for " + name,
           _FILEINFO_);

  vector<string> params;
  algorithm->getParams(params);
  QString target(name.toLower());
  BOOST_FOREACH ( string realParm, params ) {
    if ( toQt(realParm).toLower() == target ) {
      return (realParm);
    }
  }

  return ( string() );
}


/**
 * @brief Sets a parameter in an Algorithm with formatted parameter/value
 *  
 * This method accepts a formatted parameter/value string and sets all 
 * parameters found in the string. The string should be of the form 
 * "@parameter:value@parameter:value@...". 
 *  
 * @param algorithm Algorithm to set the parameters
 * @param parameters String of formatted paramaters
 */
void AlgorithmParameters::setFormattedParameter(cv::Algorithm *algorithm,
                                                const QStringList &parameters) 
                                                const { 
 checkPtr(algorithm, "Null Algorithm pointer - cannot get set formatted parameter list", 
          _FILEINFO_);

  BOOST_FOREACH ( QString param, parameters ) {
    QStringList parts = param.split(":", QString::SkipEmptyParts);
    if ( parts.size() != 2 ) {
      QString mess = "Bad parameter/value form (" + param + ") for algorithm "
                      + toQt(algorithm->name());
      throw IException(IException::User, mess, _FILEINFO_);
    }

    setParameter(algorithm, parts[0], parts[1]);
  }
  return;
}


} // namespace Isis
