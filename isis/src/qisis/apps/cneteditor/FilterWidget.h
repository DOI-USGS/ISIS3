#ifndef FilterWidget_H
#define FilterWidget_H

#include <QWidget>

class QVBoxLayout;
class QLabel;
template< class T > class QList;
class QPushButton;

namespace Isis
{
  class FilterGroup;
  
  class FilterWidget : public QWidget
  {
      Q_OBJECT

    public:
      FilterWidget();
      virtual ~FilterWidget();

    
    private:
      void nullify();
    
    
    private slots:
      void addGroup();
      void delGroup(FilterGroup *);
      
    
    private:
      QPushButton * addGroupButton;
      QVBoxLayout * mainLayout;
      
      QList< FilterGroup * > * filterGroups;
  };
}

#endif
