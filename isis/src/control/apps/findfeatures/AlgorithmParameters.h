#ifndef AlgorithmParameters_h
#define AlgorithmParameters_h
/**                                                                       
 * @file                                                                  
 * $Revision $ 
 * $Date: 2016-03-08 11:22:39 -0700 (Tue, 08 Mar 2016) $ 
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
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

#include <QMetaType>
#include <QString>
#include <QVariant>

#include "IException.h"
#include "PvlObject.h"

namespace Isis {
/**
 * @brief Provides parameterization of OpenCV Algorithms 
 *  
 * This class provides a generic interface to OpenCV Algorithms. THere are 
 * getter and setter methods that will retrieve and set, respectively, 
 * named parameters in an OpenCV Algorithm object. Note that matrices, vectors 
 * and Algorithm types contained within Algorithms are not yet suppported, 
 *  
 * Parameters in OpenCV algorithms are case sensitive, bur this class does not 
 * require to know the case. All parameters in the given algorithm are available 
 * in the OpenCV API so they are retrieved and a case insensitive lookup is 
 * performed to determine the requested algorithm. 
 *  
 * In general, algorithm parameters can be specified in a string in the 
 * following genral format: 
 *  
 * @code 
 *   [type.]algorithm[@parameter:value@...] 
 * @endcode 
 *  
 * The name of each parameter to modify is provided right after the @ and a 
 * colon (:) separates the value from the parameter name. 
 *  
 * Example: 
 * @code 
 *   algorithm=surf@hessianThreshold:100/surf
 * @endcode 
 *  
 * The above example selects a SURF detector that sets the SURF parameter 
 * HessianThreshHold to 100 and also selects a SUFR extractor with no parameter 
 * changes. The extractor is also set to a SURF algorithm.
 *  
 * @author 2015-08-18 Kris Becker 
 * @internal 
 *   @history 2015-08-18 Kris Becker - Original Version 
 *   @history 2016-04-12 Kris Becker Added class documentation 
 */
class AlgorithmParameters {
  public:
    AlgorithmParameters();
    virtual ~AlgorithmParameters();

    PvlObject getDescription(const cv::Algorithm *algorithm,
                             const QString &aType = "") const;

    int getParameterType(const cv::Algorithm *algorithm, 
                         const QString &parameter) const;

    bool hasParameter(const cv::Algorithm *algorithm, 
                      const QString &parameter) const;

    QString getParameter(const cv::Algorithm *algorithm, 
                         const QString &parameter) const;
    QVariant getParameterVariant(const cv::Algorithm *algorithm, 
                                 const QString &parameter) const;

    bool setParameter(cv::Algorithm *algorithm, const QString &parameter, 
                      const QString &value) const;
    bool setParameterVariant(cv::Algorithm *algorithm, const QString &parameter, 
                             const QVariant &value) const;

    cv::Ptr<cv::Algorithm> getAlgorithm(const cv::Algorithm *algorithm, 
                                        const QString &parameter) const;

    cv::Mat getMat(const cv::Algorithm *algorithm, 
                    const QString &parameter) const;
    std::vector<cv::Mat> getMatVector(const cv::Algorithm *algorithm, 
                                      const QString &parameter) const;

    void checkPtr(const cv::Algorithm *algorithm, 
                  const QString &mess = "Null pointer detected!",
                  const char *sourceFile = __FILE__, 
                  int lineno = __LINE__) const;

  protected:
    std::string getParameterName(const cv::Algorithm *algorithm,
                                 const QString &name) const;

    void setFormattedParameter(cv::Algorithm *algorithm,
                               const QStringList &parameters) const;

    /*  Efficent conversion of standard string to Qt QString */
    inline QString toQt(const std::string &s) const {
      return (QString::fromStdString(s));
    }

    /* Efficient conversion of QString to standard string */
    inline std::string toStd(const QString &s) const {
      return (s.toStdString());
    }

    /* Efficient conversion of string to float value */
    inline float toFloat(const QString &value) const {
      return ( value.toFloat() );
    }

};

}  // namespace Isis

// Declarations so they can be stored as QVariants
Q_DECLARE_METATYPE(cv::Mat);
Q_DECLARE_METATYPE(std::vector<cv::Mat>);
Q_DECLARE_METATYPE(cv::Ptr<cv::Algorithm>);

#endif


