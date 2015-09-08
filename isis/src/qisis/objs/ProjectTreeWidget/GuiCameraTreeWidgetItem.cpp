#include "GuiCameraTreeWidgetItem.h"

#include <QDebug>

#include "GuiCameraDisplayProperties.h"

namespace Isis {

  /**
   * GuiCameraTreeWidgetItem constructor.
   * GuiCameraTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  GuiCameraTreeWidgetItem::GuiCameraTreeWidgetItem(GuiCameraQsp guiCamera,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_guiCamera = guiCamera;

    setText(0, m_guiCamera->displayProperties()->displayName());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

//    if (m_guiCamera->displayProperties()->displayName() == "fred")
//      setIcon(0, QIcon(":moon"));
//    else if (m_targetBody->displayProperties()->displayName() == "Enceladus")
//      setIcon(0, QIcon(":enceladus"));
//    else if (m_targetBody->displayProperties()->displayName() == "Mars")
//      setIcon(0, QIcon(":mars"));
//    else
      setIcon(0, QIcon(":camera"));

    // should m_guiCamera be a QSharedPointer? Then the signal below is not needed?
    connect(m_guiCamera.data(), SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  GuiCameraTreeWidgetItem::~GuiCameraTreeWidgetItem() {
  }


  GuiCameraQsp GuiCameraTreeWidgetItem::guiCamera() {
    return m_guiCamera;
  }


  void GuiCameraTreeWidgetItem::selectionChanged() {
//    m_control->displayProperties()->setSelected(isSelected());
  }
}
