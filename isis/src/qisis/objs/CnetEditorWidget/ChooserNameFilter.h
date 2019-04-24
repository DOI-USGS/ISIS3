#ifndef ChooserNameFilter_H
#define ChooserNameFilter_H

#include "AbstractStringFilter.h"

template< typename U, typename V > struct QPair;
class QString;

namespace Isis {
  class ControlMeasure;
  class ControlNet;
  class ControlPoint;

  /**
   * @brief Allows filtering by the chooser name
   *
   * This class allows the user to filter control points by chooser name. This
   * allows the user to make a list of control points which have been chosen
   * by a particular user or application.
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
  class ChooserNameFilter : public AbstractStringFilter {
      Q_OBJECT

    public:
      ChooserNameFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
      ChooserNameFilter(const ChooserNameFilter &other);
      virtual ~ChooserNameFilter();

      bool evaluate(const QPair<QString, ControlNet *> *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;
  };
}

#endif
