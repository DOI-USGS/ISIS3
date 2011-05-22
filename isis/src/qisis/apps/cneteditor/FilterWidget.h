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
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;
  class FilterGroup;

  class FilterWidget : public QWidget
  {
      Q_OBJECT

    public:
      explicit FilterWidget(QString);
      virtual ~FilterWidget();
      
      bool evaluate(const ControlCubeGraphNode * node) const;
      bool evaluate(const ControlPoint * point) const;
      bool evaluate(const ControlMeasure * measure) const;
      
      bool hasImageFilter() const;
      bool hasPointFilter() const;
      bool hasMeasureFilter() const;
      
      
    signals:
      void filterChanged();


    private:
      void nullify();
      bool hasGroupWithCondition(bool (FilterGroup::*)() const) const;
      QList< FilterGroup * > groupsWithCondition(
          bool (FilterGroup::*)() const) const;
      void updateDescription(QLabel * label,
          bool (FilterGroup::*)() const,
          QString (FilterGroup::*)() const,
          QString);
 
  
    private slots:
      void addGroup();
      void deleteGroup(FilterGroup *);
      void changeGroupCombinationLogic(int);
      void updateDescription();


    private:
      QPushButton * addGroupButton;
      QLabel * imageDescription;
      QLabel * pointDescription;
      QLabel * measureDescription;
      QVBoxLayout * mainLayout;
      QWidget * logicWidget;
      
      bool andGroupsTogether;

      QList< FilterGroup * > * filterGroups;
      QString filterType;
  };
}

#endif
