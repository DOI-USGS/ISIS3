#ifndef MSERExtractor_h
#define MSERExtractor_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "opencv2/xfeatures2d.hpp"
#include "opencv2/opencv.hpp"

namespace Isis {

/**
 * Wrap of OpenCV3 MSER algorithm to implement pure virtual functions.
 *
 * @author 2016-12-20 Jesse Mapel
 *
 * @internal
 *  @history 2016-12-20 Jesse Mapel - Original Version
 *  @history 2016-12-27 Kristin Berry - Added documentation
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
