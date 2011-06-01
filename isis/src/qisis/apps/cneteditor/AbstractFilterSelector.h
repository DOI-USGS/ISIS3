#ifndef AbstractFilterSelector_H
#define AbstractFilterSelector_H


// parent
#include <QWidget>

// included because they are needed inside a templated method
#include "AbstractFilter.h"


class QComboBox;
class QHBoxLayout;
class QPushButton;


namespace Isis
{
  class AbstractFilter;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  class AbstractFilterSelector : public QWidget
  {
      Q_OBJECT

    public:
      AbstractFilterSelector();
      virtual ~AbstractFilterSelector();
      
      template< typename Evaluatable >
      bool evaluate(const Evaluatable * evaluatable) const
      {
        return filter && filter->evaluate(evaluatable);
      }
      
      bool hasFilter() const;
      bool hasFilter(bool (AbstractFilter::*)() const) const;
      
      QString getDescription(QString (AbstractFilter::*)() const) const;


    signals:
      void close(AbstractFilterSelector *);
      void filterChanged();
      void sizeChanged();


    protected:
      virtual void nullify();
      virtual void createSelector();
      
      
    protected slots:
      virtual void changeFilter(int);
      

    private slots:
      void sendClose();


    protected:
      QComboBox * selector;
      QHBoxLayout * mainLayout;
      AbstractFilter * filter;
      

    private:
      QPushButton * closeButton;
  };
}

#endif
