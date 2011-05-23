#ifndef AbstractFilter_H
#define AbstractFilter_H

#include <QWidget>


class QButtonGroup;
class QHBoxLayout;


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
      
      virtual QString getDescription() const = 0;


    signals:
      void filterChanged();


    protected:
      virtual void nullify();
      virtual void createWidget();
      bool inclusive() const;
      
      virtual bool evaluate(const ControlPoint *,
          bool (ControlPoint::*)() const) const;
      virtual bool evaluate(const ControlMeasure *,
          bool (ControlMeasure::*)() const) const;
      
      
    private slots:
      void updateMinForImageSuccess(int);


    protected:
      QHBoxLayout * mainLayout;
      int minForImageSuccess;


    private:
      QButtonGroup * inclusiveExclusiveGroup;
  };
}

#endif
