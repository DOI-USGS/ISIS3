#ifndef FilterWidget_H
#define FilterWidget_H


// parent
#include <QWidget>

// included because it is needed inside a templated method
#include "FilterGroup.h"


class QLabel;
template< class T > class QList;
class QPushButton;
class QString;
class QTextEdit;
class QVBoxLayout;


namespace Isis
{
  class ControlPoint;
  class FilterGroup;

  class FilterWidget : public QWidget
  {
      Q_OBJECT

    public:
      explicit FilterWidget(QString);
      virtual ~FilterWidget();
      
      template< typename Evaluatable >
      bool evaluate(const Evaluatable * evaluatable) const
      {
        // if andFiltersTogether is true then we break out of the loop as soon
        // as any selectors evaluate to false.  If andFiltersTogether is false
        // then we are ORing them so we break out as soon as any selector
        // evaluates to true.  Whether we are looking for successes or failures
        // depends on whether we are ANDing or ORing the filters (selectors)
        // together!!!
        bool looking = true;
        for (int i = 0; looking && i < filterGroups->size(); i++)
        {
          if (filterGroups->at(i)->hasFilter())
            looking = !(filterGroups->at(i)->evaluate(evaluatable) ^
                        andGroupsTogether);
        }
        
        // It is good that we are still looking for failures if we were ANDing
        // filters together, but it is bad if we were ORing them since in this
        // case we were looking for success.
        return !(looking ^ andGroupsTogether) || !hasFilter();
      }
      
      bool hasFilter() const;

      
      
    signals:
      void filterChanged();


    private:
      void nullify();
 

    private slots:
      void addGroup();
      void deleteGroup(FilterGroup *);
      void changeGroupCombinationLogic(int);
      void updateDescription();


    private:
      QPushButton * addGroupButton;
      QLabel * description;
      QVBoxLayout * mainLayout;
      QWidget * logicWidget;
      
      bool andGroupsTogether;

      QList< FilterGroup * > * filterGroups;
      QString filterType;
  };
}

#endif
