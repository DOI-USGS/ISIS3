#ifndef ConnectionModel_H
#define ConnectionModel_H


#include "TreeModel.h"


class QString;
class QTreeView;


namespace Isis
{
  class ControlNet;

  class ConnectionModel : public TreeModel
  {
      Q_OBJECT

    public:
      ConnectionModel(Isis::ControlNet * cNet, QString name, QTreeView * tv,
          QObject * parent = 0);
      virtual ~ConnectionModel();

      // This is a slot!!!  There is no "pubic slots:" because it has already
      // been marked as a slot in the parent (pure virtual).  Adding the slots
      // keyword here would do nothing except make more work for both MOC and
      // the compiler!
      void rebuildItems();
  };
}

#endif
