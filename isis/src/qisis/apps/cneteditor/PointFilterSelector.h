#ifndef PointFilterSelector_H
#define PointFilterSelector_H

#include <QWidget>


class QComboBox;
class QHBoxLayout;


namespace Isis
{
  class AbstractFilter;
  
  class PointFilterSelector : public QWidget
  {
      Q_OBJECT

    public:
      PointFilterSelector();
      virtual ~PointFilterSelector();
      AbstractFilter * getFilter();
    
    
    private:
      void nullify();
    
    
    private slots:
      void filterChanged(int);
    
    
    private:
      QComboBox * selector;
      QHBoxLayout * mainLayout;
    
    
    private:
      AbstractFilter * filter;
  };
}

#endif
