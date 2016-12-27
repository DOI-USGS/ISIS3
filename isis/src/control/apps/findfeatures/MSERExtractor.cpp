#include "MSERExtractor.h"
#include "IException.h"

#include <QString>

namespace Isis {

  void MSERExtractor::detectRegions (cv::InputArray image,
                                     std::vector< std::vector< cv::Point > > &msers,
                                     std::vector< cv::Rect > &bboxes)  {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  int MSERExtractor::getDelta () const {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  int MSERExtractor::getMaxArea () const {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  int MSERExtractor::getMinArea () const {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  bool MSERExtractor::getPass2Only () const {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  void MSERExtractor::setDelta (int delta) {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  void MSERExtractor::setMaxArea (int maxArea) {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  void MSERExtractor::setMinArea (int minArea) {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }

  void MSERExtractor::setPass2Only (bool f) {
    QString mess = "ISIS does not support this method for the OpenCV MSER algorithm.";
    throw IException(IException::Programmer, mess, _FILEINFO_); 
  }
};  
