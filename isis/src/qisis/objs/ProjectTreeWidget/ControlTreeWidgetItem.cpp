#include "ControlTreeWidgetItem.h"

#include <QDebug>

#include "Control.h"
#include "ControlDisplayProperties.h"

namespace Isis {

  /**
   * ControlTreeWidgetItem constructor.
   * ControlTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  ControlTreeWidgetItem::ControlTreeWidgetItem(Control *control, QTreeWidget *parent) :
      QTreeWidgetItem(parent, UserType) {
    m_control = control;

    setText(0, m_control->displayProperties()->displayName());
//  setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":pointReg"));

    connect(m_control, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  ControlTreeWidgetItem::~ControlTreeWidgetItem() {
    m_control = NULL;
  }


  Control *ControlTreeWidgetItem::control() {
    return m_control;
  }


  void ControlTreeWidgetItem::selectionChanged() {
    m_control->displayProperties()->setSelected(isSelected());
  }
}
