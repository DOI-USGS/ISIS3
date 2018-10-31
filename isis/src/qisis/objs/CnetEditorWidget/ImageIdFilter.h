#ifndef ImageIdFilter_H
#define ImageIdFilter_H

#include "AbstractStringFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
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
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class ImageIdFilter : public AbstractStringFilter {
      Q_OBJECT

    public:
      ImageIdFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
      ImageIdFilter(const ImageIdFilter &other);
      virtual ~ImageIdFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;
  };
}

#endif
