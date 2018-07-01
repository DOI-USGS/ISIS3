#ifndef MeasureCountFilter_H
#define MeasureCountFilter_H


// parent
#include "AbstractFilter.h"


class QButtonGroup;
class QLineEdit;
class QSpinBox;
class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Allows filtering by the number of measures in a control point
   *
   * This class allows the user to filter control points by the number of
   * measures they contain. This allows the user to make a list of control
   * points which are shallow or deep.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class MeasureCountFilter : public AbstractFilter {
      Q_OBJECT

    public:
      MeasureCountFilter(AbstractFilter::FilterEffectivenessFlag,
          int minimumForSuccess = -1);
      MeasureCountFilter(const MeasureCountFilter &other);
      virtual ~MeasureCountFilter();

      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;

      AbstractFilter *clone() const;

      QString getImageDescription() const;
      QString getPointDescription() const;


    private:
      void createWidget();
      void init();


    private slots:
      void updateMinMax(int);
      void updateMeasureCount(int);


    private:
      QButtonGroup *m_minMaxGroup;
      QSpinBox *m_countSpinBox;
      int m_count;
      bool m_minimum;
  };
}

#endif
