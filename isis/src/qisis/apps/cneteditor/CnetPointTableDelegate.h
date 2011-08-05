#ifndef CnetPointTableDelegate_H
#define CnetPointTableDelegate_H

#include "AbstractCnetTableDelegate.h"

class QString;
class QWidget;

namespace Isis
{
  class AbstractTreeItem;

  class CnetPointTableDelegate : public AbstractCnetTableDelegate
  {
    public:
      CnetPointTableDelegate();
      virtual ~CnetPointTableDelegate();

      QWidget * getWidget(CnetTableColumn const *) const;

      void readData(QWidget *, AbstractTreeItem *,
                    CnetTableColumn const *) const;
                    
      void readData(QWidget *, AbstractTreeItem *, CnetTableColumn const *,
                    QString) const;
                    
      void saveData(QWidget *, AbstractTreeItem *,
                    CnetTableColumn const *) const;


    private:
      CnetPointTableDelegate(const CnetPointTableDelegate &);
      CnetPointTableDelegate & operator=(const CnetPointTableDelegate &);
  };
}

#endif

