#ifndef TargetBodyTreeWidgetItem_H
#define TargetBodyTreeWidgetItem_H

#include <QObject>
#include <QTreeWidgetItem>

#include "TargetBody.h"

namespace Isis {

  /**
   * @brief A control in the project tree widget
   *
   * This class visualizes a TargetBody * from the project in the project tree widget.
   *
   * @author 2015-06-08 Ken Edmundson
   *
   * @internal
   *   @history 2015-06-08 Ken Edmundson - Original version.
   */
  class TargetBodyTreeWidgetItem : public QObject, public QTreeWidgetItem {
    Q_OBJECT
    public:
      TargetBodyTreeWidgetItem(TargetBodyQsp targetBody,
                                       QTreeWidget *parent = 0);
      virtual ~TargetBodyTreeWidgetItem();

      TargetBodyQsp targetBody();
      void selectionChanged();

    private:
      TargetBodyQsp m_targetBody;
  };
}

#endif

