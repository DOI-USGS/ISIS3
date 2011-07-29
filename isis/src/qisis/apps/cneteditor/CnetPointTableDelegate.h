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

      QWidget * getWidget(QString) const;
      void readData(QWidget *, AbstractTreeItem *, QString) const;
      void saveData(QWidget *, AbstractTreeItem *, QString) const;


    private:
      CnetPointTableDelegate(const CnetPointTableDelegate &);
      CnetPointTableDelegate & operator=(const CnetPointTableDelegate &);
  };
}

#endif

