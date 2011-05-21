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
  class AbstractFilterSelector;
  class ControlPoint;
  
  class FilterGroup : public QWidget
  {
      Q_OBJECT

    public:
      explicit FilterGroup(QString);
      virtual ~FilterGroup();
      
      template< typename Evaluatable >
      bool evaluate(const Evaluatable * e) const
      {
        // if andFiltersTogether is true then we break out of the loop as soon
        // as any selectors evaluate to false.  If andFiltersTogether is false
        // then we are ORing them so we break out as soon as any selector
        // evaluates to true.  Whether we are looking for successes or failures
        // depends on whether we are ANDing or ORing the filters (selectors)
        // together!
        bool looking = true;
        for (int i = 0; looking && i < selectors->size(); i++)
          if (selectors->at(i)->hasFilter())
            looking = !(selectors->at(i)->evaluate(e) ^ andFiltersTogether);
    
        // It is good that we are still looking for failures if we were ANDing
        // filters together, but it is bad if we were ORing them since in this
        // case we were looking for success.
        return !(looking ^ andFiltersTogether);
      }
      
      bool hasFilter() const;
      bool filtersAreAndedTogether() const;
      QString getDescription() const;


    signals:
      void close(FilterGroup *);
      void filterChanged();


    private:
      void nullify();


    private slots:
      void addFilter();
      void deleteFilter(AbstractFilterSelector *);
      void sendClose();
      void changeFilterCombinationLogic(int);


    private: // widgets
      QPushButton * closeButton;
      QPushButton * newSelectorButton;
      QVBoxLayout * groupBoxLayout;
      QWidget * logicWidget;
      

    private:
      QList< AbstractFilterSelector * > * selectors;
      bool andFiltersTogether;
      QString filterType;
  };
}

#endif

