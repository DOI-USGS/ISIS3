#include "ControlGroupTreeWidgetItem.h"

#include <QDebug>

#include "ControlList.h"

namespace Isis {

  /**
   * ControlGroupTreeWidgetItem constructor.
   * ControlGroupTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  ControlGroupTreeWidgetItem::ControlGroupTreeWidgetItem(ControlList *controlList, QTreeWidget *parent) :
      QTreeWidgetItem(parent, UserType) {
    m_controlList = controlList;

    setText(0, m_controlList->name());
    updateCount(m_controlList->count());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":folder"));

    connect(m_controlList, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
    connect(m_controlList, SIGNAL(countChanged(int)), this, SLOT(updateCount(int)));
  }


  ControlGroupTreeWidgetItem::~ControlGroupTreeWidgetItem() {
    m_controlList = NULL;
  }


  ControlList *ControlGroupTreeWidgetItem::controlList() {
    return m_controlList;
  }


  void ControlGroupTreeWidgetItem::selectionChanged() {
    foreach (Control *control, *m_controlList) {
      control->displayProperties()->setSelected(isSelected());
    }
  }


  void ControlGroupTreeWidgetItem::updateCount(int newCount) {
    setToolTip(0, tr("%1 Control Networks").arg(newCount));
  }
}
