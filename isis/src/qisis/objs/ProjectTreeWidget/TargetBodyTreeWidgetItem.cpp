#include "TargetBodyTreeWidgetItem.h"

#include <QDebug>

#include "TargetBodyDisplayProperties.h"

namespace Isis {

  /**
   * TargetBodyTreeWidgetItem constructor.
   * TargetBodyTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  TargetBodyTreeWidgetItem::TargetBodyTreeWidgetItem(TargetBodyQsp targetBody,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_targetBody = targetBody;

    setText(0, m_targetBody->displayProperties()->displayName());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    if (m_targetBody->displayProperties()->displayName() == "MOON")
      setIcon(0, QIcon(":moon"));
    else if (m_targetBody->displayProperties()->displayName() == "Enceladus")
      setIcon(0, QIcon(":enceladus"));
    else if (m_targetBody->displayProperties()->displayName() == "Mars")
      setIcon(0, QIcon(":mars"));
    else if (m_targetBody->displayProperties()->displayName() == "Titan")
      setIcon(0, QIcon(":titan"));
    else
      setIcon(0, QIcon(":moonPhase"));

    // since m_targetBody is a QSharedPointer is the signal below not needed?
    connect(m_targetBody.data(), SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  TargetBodyTreeWidgetItem::~TargetBodyTreeWidgetItem() {
  }


  TargetBodyQsp TargetBodyTreeWidgetItem::targetBody() {
    return m_targetBody;
  }


  void TargetBodyTreeWidgetItem::selectionChanged() {
//    m_control->displayProperties()->setSelected(isSelected());
  }
}
