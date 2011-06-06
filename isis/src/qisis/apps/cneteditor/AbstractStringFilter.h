#ifndef AbstractStringFilter_H
#define AbstractStringFilter_H


// parent
#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class AbstractStringFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      AbstractStringFilter(AbstractFilter::FilterEffectivenessFlag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~AbstractStringFilter();
      

    protected:
      virtual void nullify();
      virtual void createWidget();
      bool evaluate(QString) const;
      QString descriptionSuffix() const;
      
      
    private slots:
      void updateLineEditText(QString);


    private:
      QLineEdit * lineEdit;
      QString * lineEditText;
  };
}

#endif
