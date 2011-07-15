#ifndef AbstractFilterSelector_H
#define AbstractFilterSelector_H


// parent
#include <QWidget>

// included because they are needed inside a templated method
#include "AbstractFilter.h"


class QComboBox;
class QHBoxLayout;
class QPushButton;
class QReadWriteLock;

namespace Isis
{
  class AbstractFilter;
  class ControlCubeGraphNode;
  class ControlMeasure;
  class ControlPoint;

  class AbstractFilterSelector : public QWidget
  {
      Q_OBJECT

    signals:
      void close(AbstractFilterSelector *);
      void filterChanged();
      void sizeChanged();


    public:
      AbstractFilterSelector();
      virtual ~AbstractFilterSelector();

      template< typename Evaluatable >
      bool evaluate(const Evaluatable * evaluatable) const
      {
        return filter && filter->evaluate(evaluatable);
      }

      bool hasFilter() const;
      bool hasFilter(bool (AbstractFilter:: *)() const) const;

      QString getDescription(QString(AbstractFilter:: *)() const) const;

      AbstractFilterSelector & operator=(const AbstractFilterSelector & other);


    public slots:
      void sendClose();


    protected:
      void nullify();
      virtual void createSelector();
      QComboBox * getSelector() const;
      QHBoxLayout * getMainLayout() const;
      AbstractFilter * getFilter() const;
      void setFilter(AbstractFilter *);


    protected slots:
      virtual void changeFilter(int index) = 0;
      virtual void deleteFilter();


      // disable copying of this class which can't exist anyway ....uhhuh...yeah
    private:
      AbstractFilterSelector(const AbstractFilterSelector & other);


    private:
      QComboBox * selector;
      QHBoxLayout * mainLayout;
      QPushButton * closeButton;
      AbstractFilter * filter;
  };
}

#endif

