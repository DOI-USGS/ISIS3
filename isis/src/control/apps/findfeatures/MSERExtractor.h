#ifndef MSERExtractor_h
#define MSERExtractor_h

#include "opencv2/xfeatures2d.hpp"
#include "opencv2/opencv.hpp"

namespace Isis {

/**
 * Wrap of OpenCV3 MSER algorithm to implement pure virtual functions. 
 */

class MSERExtractor : public cv::MSER {  

public:
  virtual void detectRegions (cv::InputArray image,
                              std::vector< std::vector< cv::Point > > &msers,
                              std::vector< cv::Rect > &bboxes);

  virtual int getDelta () const;

  virtual int getMaxArea () const;

  virtual int getMinArea () const;

  virtual bool getPass2Only () const;

  virtual void setDelta (int delta);

  virtual void setMaxArea (int maxArea);

  virtual void setMinArea (int minArea);

  virtual void setPass2Only (bool f);
};

}
#endif
