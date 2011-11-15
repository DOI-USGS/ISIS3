#ifndef AbstractNumberFilter_H
#define AbstractNumberFilter_H


// parent
#include "AbstractFilter.h"


class QButtonGroup;
class QLineEdit;
class QString;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class AbstractNumberFilter : public AbstractFilter
    {
        Q_OBJECT

      public:
        AbstractNumberFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        AbstractNumberFilter(const AbstractNumberFilter & other);
        virtual ~AbstractNumberFilter();


      protected:
        bool evaluate(double) const;
        QString descriptionSuffix() const;
        bool lessThan() const;


      private:
        void createWidget();
        void nullify();


      private slots:
        void updateLineEditText(QString);


      private:
        QButtonGroup * greaterThanLessThan;
        QLineEdit * lineEdit;
        QString * lineEditText;
    };
  }
}

#endif
