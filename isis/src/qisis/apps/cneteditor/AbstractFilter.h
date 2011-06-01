#ifndef AbstractFilter_H
#define AbstractFilter_H

#include <QWidget>


class QButtonGroup;
class QHBoxLayout;
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
      AbstractFilter(int minimumForImageSuccess = -1);
      virtual ~AbstractFilter();

      virtual bool canFilterImages() const = 0;
      virtual bool canFilterPoints() const = 0;
      virtual bool canFilterMeasures() const = 0;
      
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
      int getMinForImageSuccess() const { return minForImageSuccess; }
      
      virtual bool evaluate(const ControlPoint *,
          bool (ControlPoint::*)() const) const;
      virtual bool evaluate(const ControlMeasure *,
          bool (ControlMeasure::*)() const) const;
      
      
    private slots:
      void updateMinForImageSuccess(int);


    protected:
      QHBoxLayout * mainLayout;
      QReadWriteLock * lock;


    private:
      QButtonGroup * inclusiveExclusiveGroup;
      int minForImageSuccess;
  };
}

#endif
