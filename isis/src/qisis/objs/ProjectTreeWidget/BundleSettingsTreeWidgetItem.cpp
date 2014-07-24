#include "BundleSettingsTreeWidgetItem.h"

#include <QDebug>

#include "BundleSettings.h"
//#include "BundleResultsDisplayProperties.h"

namespace Isis {

  /**
   * BundleSettingsTreeWidgetItem constructor.
   * BundleSettingsTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  BundleSettingsTreeWidgetItem::BundleSettingsTreeWidgetItem(BundleSettings *bundleSettings,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_bundleSettings = bundleSettings;

    setText(0, "Settings");
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":settings"));

    connect(m_bundleSettings, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  BundleSettingsTreeWidgetItem::~BundleSettingsTreeWidgetItem() {
    m_bundleSettings = NULL;
  }


  BundleSettings *BundleSettingsTreeWidgetItem::bundleSettings() {
    return m_bundleSettings;
  }


  void BundleSettingsTreeWidgetItem::selectionChanged() {
//    m_control->displayProperties()->setSelected(isSelected());
  }
}
