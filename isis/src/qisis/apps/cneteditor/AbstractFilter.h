#ifndef AbstractFilter_H
#define AbstractFilter_H

#include <QWidget>


class QButtonGroup;
template< typename T > class QFlags;
class QHBoxLayout;
class QMenu;
class QReadWriteLock;


namespace Isis
{
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
      AbstractFilter(FilterEffectivenessFlag effectiveness,
          int minimumForImageSuccess = -1);
      virtual ~AbstractFilter();

      virtual bool canFilterImages() const;
      virtual bool canFilterPoints() const;
      virtual bool canFilterMeasures() const;
      
      virtual bool evaluate(const ControlCubeGraphNode *) const;
      virtual bool evaluate(const ControlPoint *) const { return true; }
      virtual bool evaluate(const ControlMeasure *) const { return true; }
      
      virtual QString getImageDescription() const;
      virtual QString getPointDescription() const;
      virtual QString getMeasureDescription() const;
    

    signals:
      void filterChanged();


    protected:
      virtual void nullify();
      virtual void createWidget();
      bool inclusive() const;
      int getMinForImageSuccess() const { return minForImageSuccess; }
      AbstractFilter::FilterEffectivenessFlag * getEffectivenessFlags() const;
      
      virtual bool evaluate(const ControlPoint *,
          bool (ControlPoint::*)() const) const;
      virtual bool evaluate(const ControlMeasure *,
          bool (ControlMeasure::*)() const) const;
      
      
    private slots:
      void updateEffectiveness();
      void updateMinForImageSuccess(int);
      
      
    private:
      QAction * createEffectivenessAction(QString);


    protected:
      QHBoxLayout * mainLayout;
      QReadWriteLock * lock;


    private:
      QButtonGroup * inclusiveExclusiveGroup;
      QMenu * effectivenessMenu;
      
      
    private:
      int minForImageSuccess;
      FilterEffectivenessFlag * effectivenessFlags;
  };
  
  Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFilter::FilterEffectivenessFlag)
}

#endif
