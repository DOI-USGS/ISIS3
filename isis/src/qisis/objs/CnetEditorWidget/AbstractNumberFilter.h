#ifndef AbstractNumberFilter_H
#define AbstractNumberFilter_H


// parent
#include "AbstractFilter.h"


class QButtonGroup;
class QLineEdit;
class QString;


namespace Isis {
  class ControlPoint;
  class ControlMeasure;

  namespace CnetViz {

    /**
     * @brief Base class for filters that are number-based
     *
     * This class is the base class that all filters that are number-based.
     *
     * @author ????-??-?? Eric Hyer
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class AbstractNumberFilter : public AbstractFilter {
        Q_OBJECT

      public:
        AbstractNumberFilter(AbstractFilter::FilterEffectivenessFlag,
            int minimumForSuccess = -1);
        AbstractNumberFilter(const AbstractNumberFilter &other);
        virtual ~AbstractNumberFilter();


      protected:
        using Isis::CnetViz::AbstractFilter::evaluate;
        bool evaluate(double) const;
        QString descriptionSuffix() const;
        bool lessThan() const;


      private:
        void createWidget();
        void nullify();


      private slots:
        void updateLineEditText(QString);


      private:
        QButtonGroup *m_greaterThanLessThan;
        QLineEdit *m_lineEdit;
        QString *m_lineEditText;
    };
  }
}

#endif
