#ifndef AbstractNumberFilter_H
#define AbstractNumberFilter_H


// parent
#include "AbstractFilter.h"


class QButtonGroup;
class QLineEdit;
class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class AbstractNumberFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      AbstractNumberFilter(AbstractFilter::FilterEffectivenessFlag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~AbstractNumberFilter();
      

    protected:
      virtual void nullify();
      virtual void createWidget();
      bool evaluate(double) const;
      QString descriptionSuffix() const;
      bool lessThan() const;
      
      
    private slots:
      void updateLineEditText(QString);


    private:
      QButtonGroup * greaterThanLessThan;
      QLineEdit * lineEdit;
      QString * lineEditText;
  };
}

#endif
