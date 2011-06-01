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
      
      bool evaluate(const ControlCubeGraphNode * node) const;
      bool evaluate(const ControlPoint * point) const;
      bool evaluate(const ControlMeasure * measure) const;
      
      //bool hasFilter() const;
      bool hasFilter(bool (AbstractFilter::*)() const = NULL) const;
      
      QString getDescription(bool (AbstractFilter::*)() const,
          QString (AbstractFilter::*)() const) const;
      
      bool filtersAreAndedTogether() const;
      

    signals:
      void close(FilterGroup *);
      void filterChanged();
      void sizeChanged(FilterGroup *);


    private:
      bool hasSelectorWithCondition(
          bool (AbstractFilterSelector::*)() const) const;
      void nullify();


    private slots:
      void addFilter();
      void deleteFilter(AbstractFilterSelector *);
      void sendClose();
      void sendSizeChanged();
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

