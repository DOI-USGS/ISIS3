#ifndef AbstractFilter_H
#define AbstractFilter_H

#include <QWidget>


class QBoxLayout;
class QButtonGroup;
class QCheckBox;
template< typename T > class QFlags;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;
  class ControlCubeGraphNode;

  namespace CnetViz
  {
    class AbstractFilterSelector;

    /**
     * @brief Base class for control net filters
     *
     * This class is the base class that all other filters derive from. It
     * encompasses both the widget and the filter functionality itself. See the
     * cneteditor architecture document for further information about the
     * filtering system.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
    class AbstractFilter : public QWidget
    {
        Q_OBJECT

      public:
        enum FilterEffectiveness
        {
          Images = 1,
          Points = 2,
          Measures = 4
        };
        Q_DECLARE_FLAGS(FilterEffectivenessFlag, FilterEffectiveness)


      public:
        AbstractFilter(FilterEffectivenessFlag, int minimumForSuccess = -1);
        AbstractFilter(const AbstractFilter & other);
        virtual ~AbstractFilter();

        virtual bool canFilterImages() const;
        virtual bool canFilterPoints() const;
        virtual bool canFilterMeasures() const;

        virtual bool evaluate(const ControlCubeGraphNode *) const = 0;
        virtual bool evaluate(const ControlPoint *) const = 0;
        virtual bool evaluate(const ControlMeasure *) const = 0;

        virtual AbstractFilter * clone() const = 0;

        virtual QString getImageDescription() const;
        virtual QString getPointDescription() const;
        virtual QString getMeasureDescription() const;


      signals:
        void filterChanged();


      protected:
        bool inclusive() const;
        int getMinForSuccess() const
        {
          return minForSuccess;
        }
        AbstractFilter::FilterEffectivenessFlag * getEffectivenessFlags() const;
        QBoxLayout * getMainLayout() const;
        QBoxLayout * getInclusiveExclusiveLayout() const;

        bool evaluateImageFromPointFilter(const ControlCubeGraphNode *) const;
        bool evaluateImageFromMeasureFilter(const ControlCubeGraphNode *) const;
        bool evaluatePointFromMeasureFilter(const ControlPoint *) const;

        virtual bool evaluate(const ControlPoint *,
            bool (ControlPoint:: *)() const) const;
        virtual bool evaluate(const ControlMeasure *,
            bool (ControlMeasure:: *)() const) const;


      private:
        void createWidget();
        bool evaluateFromCount(QList< ControlMeasure * >, bool) const;
        void nullify();


      private slots:
        void updateEffectiveness();
        void updateMinForSuccess(int);


      private:
        QCheckBox * createEffectivenessCheckBox(QString);


      private:
        QBoxLayout * mainLayout;
        QBoxLayout * inclusiveExclusiveLayout;
        QButtonGroup * inclusiveExclusiveGroup;
        QButtonGroup * effectivenessGroup;
        QWidget * minWidget;


      private:
        int minForSuccess;
        FilterEffectivenessFlag * effectivenessFlags;
        QFont * smallFont;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFilter::FilterEffectivenessFlag)
  }
}

#endif
