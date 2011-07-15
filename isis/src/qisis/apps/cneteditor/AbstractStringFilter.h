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
          int minimumForSuccess = -1);
      AbstractStringFilter(const AbstractStringFilter & other);
      virtual ~AbstractStringFilter();


    protected:
      bool evaluate(QString) const;
      QString descriptionSuffix() const;


    private slots:
      void updateLineEditText(QString);


    private:
      void createWidget();
      void nullify();


    private:
      QLineEdit * lineEdit;
      QString * lineEditText;
  };
}

#endif
