#include "BundleSolutionInfoTreeWidgetItem.h"

#include <QDebug>

#include "BundleSolutionInfo.h"
//#include "BundleSolutionInfoDisplayProperties.h"

namespace Isis {

  /**
   * BundleSolutionInfoTreeWidgetItem constructor.
   * BundleSolutionInfoTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  BundleSolutionInfoTreeWidgetItem::BundleSolutionInfoTreeWidgetItem(BundleSolutionInfo *bundleSolutionInfo,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_bundleSolutionInfo = bundleSolutionInfo;

//  setText(0, m_control->displayProperties()->displayName());
    setText(0, m_bundleSolutionInfo->runTime());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":results"));

    connect(m_bundleSolutionInfo, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  BundleSolutionInfoTreeWidgetItem::~BundleSolutionInfoTreeWidgetItem() {
    m_bundleSolutionInfo = NULL;
  }


  BundleSolutionInfo *BundleSolutionInfoTreeWidgetItem::bundleSolutionInfo() {
    return m_bundleSolutionInfo;
  }


  void BundleSolutionInfoTreeWidgetItem::selectionChanged() {
//    m_control->displayProperties()->setSelected(isSelected());
  }
}
