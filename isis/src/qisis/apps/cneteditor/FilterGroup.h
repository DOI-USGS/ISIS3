#ifndef FilterGroup_H
#define FilterGroup_H

#include <QWidget>

class QPushButton;


namespace Isis
{
  class FilterGroup : public QWidget
  {
      Q_OBJECT

    public:
      FilterGroup();
      virtual ~FilterGroup();
      
      
    signals:
      void close(FilterGroup *);

    
    private:
      void nullify();
      
      
    private slots:
      void sendClose();


    private: // widgets
      QPushButton * closeButton;
  };
}

#endif

