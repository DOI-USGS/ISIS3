#ifndef AbstractFilter_H
#define AbstractFilter_H

#include <QWidget>


class QButtonGroup;
class QHBoxLayout;


namespace Isis
{
  class AbstractFilter : public QWidget
  {
      Q_OBJECT

    public:
      AbstractFilter();
      virtual ~AbstractFilter();


    signals:
      void filterChanged();


    protected:
      virtual void nullify();
      virtual void createWidget();
      bool inclusive() const;


    protected:
      QHBoxLayout * mainLayout;


    private:
      QButtonGroup * buttonGroup;
  };
}

#endif
