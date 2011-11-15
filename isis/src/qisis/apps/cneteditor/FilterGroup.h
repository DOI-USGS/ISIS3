#ifndef FilterGroup_H
#define FilterGroup_H


// parent
#include <QWidget>

// included because it is needed inside a templated method
#include "AbstractFilterSelector.h"


template< class T > class QList;
class QPushButton;
class QVBoxLayout;


namespace Isis
{
  class ControlPoint;

  namespace CnetViz
  {
    class AbstractFilterSelector;
    
    class FilterGroup : public QWidget
    {
        Q_OBJECT

      public:
        explicit FilterGroup(QString type);
        FilterGroup(const FilterGroup & other);
        virtual ~FilterGroup();

        template< typename T >
        bool evaluate(const T * t, bool (AbstractFilter::*meth)() const) const
        {
          // if andFiltersTogether is true then we break out of the loop as soon
          // as any selectors evaluate to false.  If andFiltersTogether is false
          // then we are ORing them so we break out as soon as any selector
          // evaluates to true.  Whether we are looking for successes or failures
          // depends on whether we are ANDing or ORing the filters (selectors)
          // together!
          bool looking = true;
          for (int i = 0; looking && i < selectors->size(); i++)
            if (selectors->at(i)->hasFilter(meth))
              looking = !(selectors->at(i)->evaluate(t) ^ andFiltersTogether);

          // It is good that we are still looking for failures if we were ANDing
          // filters together, but it is bad if we were ORing them since in this
          // case we were looking for success.
          return !(looking ^ andFiltersTogether) || !hasFilter(meth);
        }

        //bool hasFilter() const;
        bool hasFilter(bool (AbstractFilter:: *)() const = NULL) const;

        QString getDescription(bool (AbstractFilter:: *)() const,
            QString(AbstractFilter:: *)() const) const;

        bool filtersAreAndedTogether() const;

        FilterGroup & operator=(FilterGroup other);


      signals:
        void close(FilterGroup *);
        void filterChanged();
        void sizeChanged(FilterGroup *);


      private:
        bool hasSelectorWithCondition(
          bool (AbstractFilterSelector:: *)() const) const;
        void nullify();
        void init();
        void addSelector(AbstractFilterSelector * newSelector);


      private slots:
        void addSelector();
        void deleteSelector(AbstractFilterSelector *);
        void sendClose();
        void sendSizeChanged();
        void changeFilterCombinationLogic(int);


      private: // widgets
        QButtonGroup * buttonGroup;
        QPushButton * closeButton;
        QPushButton * newSelectorButton;
        QVBoxLayout * groupBoxLayout;
        QWidget * logicWidget;


      private:
        QList< AbstractFilterSelector * > * selectors;
        bool andFiltersTogether;
        QString * filterType;
    };
  }
}

#endif
