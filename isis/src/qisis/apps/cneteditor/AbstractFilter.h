#ifndef AbstractFilter_H
#define AbstractFilter_H

#include <QWidget>


class QBoxLayout;
class QButtonGroup;
template< typename T > class QFlags;
class QMenu;
class QReadWriteLock;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;
  class ControlCubeGraphNode;

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
      AbstractFilter(FilterEffectivenessFlag, AbstractFilterSelector *,
          int minimumForSuccess = -1);
      virtual ~AbstractFilter();

      virtual bool canFilterImages() const;
      virtual bool canFilterPoints() const;
      virtual bool canFilterMeasures() const;
      
      virtual bool evaluate(const ControlCubeGraphNode *) const = 0;
      virtual bool evaluate(const ControlPoint *) const = 0;
      virtual bool evaluate(const ControlMeasure *) const = 0;
      
      virtual QString getImageDescription() const;
      virtual QString getPointDescription() const;
      virtual QString getMeasureDescription() const;
    

    signals:
      void filterChanged();


    protected:
      virtual void nullify();
      virtual void createWidget();
      bool inclusive() const;
      int getMinForSuccess() const { return minForSuccess; }
      AbstractFilter::FilterEffectivenessFlag * getEffectivenessFlags() const;
      QBoxLayout * getMainLayout() const;
      QBoxLayout * getInclusiveExclusiveLayout() const;
      
      bool evaluateImageFromPointFilter(const ControlCubeGraphNode *) const;
      bool evaluateImageFromMeasureFilter(const ControlCubeGraphNode *) const;
      bool evaluatePointFromMeasureFilter(const ControlPoint *) const;
          
      virtual bool evaluate(const ControlPoint *,
          bool (ControlPoint::*)() const) const;
      virtual bool evaluate(const ControlMeasure *,
          bool (ControlMeasure::*)() const) const;
    
  
    private:
      bool evaluateFromCount(QList< ControlMeasure * >, bool) const;
    
      
    private slots:
      void showHideEffectivenessMenu();
      void updateEffectiveness();
      void updateMinForSuccess(int);
      
      
    private:
      QAction * createEffectivenessAction(QString);


    protected:
      QReadWriteLock * lock;


    private:
      QBoxLayout * mainLayout;
      QBoxLayout * inclusiveExclusiveLayout;
      QButtonGroup * inclusiveExclusiveGroup;
      QMenu * effectivenessMenu;
      QWidget * minWidget;
      
      
    private:
      int minForSuccess;
      FilterEffectivenessFlag * effectivenessFlags;
      AbstractFilterSelector * parentSelector;
  };
  
  Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFilter::FilterEffectivenessFlag)
}

#endif
