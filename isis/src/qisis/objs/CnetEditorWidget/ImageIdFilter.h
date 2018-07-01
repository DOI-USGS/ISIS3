#ifndef ImageIdFilter_H
#define ImageIdFilter_H

#include "AbstractStringFilter.h"


class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @brief Allows filtering by image ID
   *
   * This class allows the user to filter control points and control measures
   * by an image ID (either a cube serial number or a cube filename). This
   * allows the user to make a list of control points and measures for a
   * particular image or set of images with similar serial numbers or
   * filenames.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal 
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054. 
   */
  class ImageIdFilter : public AbstractStringFilter {
      Q_OBJECT

    public:
      ImageIdFilter(AbstractFilter::FilterEffectivenessFlag,
          int minimumForSuccess = -1);
      ImageIdFilter(const ImageIdFilter &other);
      virtual ~ImageIdFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
