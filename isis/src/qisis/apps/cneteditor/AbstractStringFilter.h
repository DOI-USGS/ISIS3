#ifndef AbstractStringFilter_H
#define AbstractStringFilter_H


// parent
#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    /**
     * @brief Base class for filters that are string-based
     *
     * This class is the base class that all filters that are string-based.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     */
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
}

#endif
