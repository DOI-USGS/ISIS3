#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractFilter.h"


class QLineEdit;
class QMutex;
class QString;


namespace Isis
{
  class ControlPoint;

  class PointIdFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointIdFilter(int minimumForImageSuccess = -1);
      virtual ~PointIdFilter();
      
      bool canFilterImages() const;
      bool canFilterPoints() const;
      bool canFilterMeasures() const;
      
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;
      bool evaluate(const ControlCubeGraphNode *) const;

      QString getDescription() const;


    protected:
      void nullify();
      void createWidget();
      
      
    private slots:
      void updateLineEditText(QString);


    private:
      QLineEdit * lineEdit;
      QString * lineEditText;
      QMutex * mutex;
  };
}

#endif
