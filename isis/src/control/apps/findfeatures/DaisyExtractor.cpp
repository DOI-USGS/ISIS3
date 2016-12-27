#include "DaisyExtractor.h"
#include "IException.h"

#include <QString>

namespace Isis {

/**
 * Always throws an exception. This function is not actually implemented.
 * 
 * @param image image 
 * @param keypoints keypoints
 * @param descriptors descriptors
 *  
 * @throws IException::Programmer "Daisy is unable to run compute with these arguments."
 */
  void DaisyExtractor::compute( cv::InputArray image, std::vector<cv::KeyPoint>& keypoints, 
                          cv::OutputArray descriptors ) {
    QString mess = "Daisy is unable to run compute with these arguments.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }


/**
 * Always throws an exception. This function is not actually implemented. 
 * 
 * @param image image
 * @param roi region of interest
 * @param descriptors descriptors
 *  
 * @throws IException::Programmer "Daisy is unable to run compute with these arguments." 
 */
  void DaisyExtractor::compute( cv::InputArray image, cv::Rect roi, cv::OutputArray descriptors ){
    QString mess = "Daisy is unable to run compute with these arguments.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }


/**
 * 
 * Always throws an exception. This function is not actually implemented. 
 * 
 * @param image image
 * @param descriptors descriptors 
 *  
 * @throws IException::Programmer "Daisy is unable to run compute with these arguments."
 */
  void DaisyExtractor::compute( cv::InputArray image, cv::OutputArray descriptors ){
    QString mess = "Daisy is unable to run compute with these arguments.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }


/**
 * Always throws an exception. This function is not actually implemented.
 * 
 * @param y y coordinate for descriptor
 * @param x x coordinate for descriptor
 * @param orientation orientation
 * @param descriptor descriptor
 *  
 * @throws IException::Programmer "Daisy cannot run GetDescriptor." 
 */
  void DaisyExtractor::GetDescriptor( double y, double x, int orientation, 
                                        float* descriptor ) const {
    QString mess = "Daisy cannot run GetDescriptor";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }


/**
 * Always throws an exception. This function is not actually implemented.
 * 
 * 
 * @param y y coordinate
 * @param x x coordinate
 * @param orientation orientation
 * @param descriptor descriptor
 * @param H transformation matrix
 *  
 * @throws IException::Programmer "Daisy cannot run GetDescriptor." 
 *  
 * @return @b bool Always false.
 */
  bool DaisyExtractor::GetDescriptor( double y, double x, int orientation, float* descriptor, 
                                      double* H ) const {
    QString mess = "Daisy cannot run GetDescriptor.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
    return false;
    }
  
/**
 * 
 * Always throws an exception. This function is not actually implemented. 
 *  
 * @param y y coordinate
 * @param x x coordinate
 * @param orientation orientation
 * @param descriptor descriptor
 *  
 * @throws IException::Programmer "Daisy cannot run GetUnnormalizedDescriptor."
 */

  void DaisyExtractor::GetUnnormalizedDescriptor( double y, double x, int orientation, 
                                            float* descriptor ) const {
    QString mess = "Daisy cannot run GetUnnormalizedDescriptor.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }
  

/**
 * Always throws an exception. This function is not actually implemented. * 
 *  
 * @param y y coordinate
 * @param x x coordinate
 * @param orientation orientation
 * @param descriptor descriptor
 * @param H transformation matrix
 *  
 * @throws IException::Programmer "Daisy cannot run GetUnnormalizedDescriptor."
 * 
 * @return @b bool Always false. 
 */
  bool DaisyExtractor::GetUnnormalizedDescriptor( double y, double x, int orientation, 
                                            float* descriptor , double *H ) const {
    QString mess = "Daisy cannot run GetUnnormalizedDescriptor.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
    return false;
  }

};


