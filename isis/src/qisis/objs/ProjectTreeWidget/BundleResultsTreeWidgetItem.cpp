#include "BundleResultsTreeWidgetItem.h"

#include <QDebug>

#include "BundleResults.h"
//#include "BundleResultsDisplayProperties.h"

namespace Isis {

  /**
   * BundleResultsTreeWidgetItem constructor.
   * BundleResultsTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  BundleResultsTreeWidgetItem::BundleResultsTreeWidgetItem(BundleResults *bundleResults,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_bundleResults = bundleResults;

//  setText(0, m_control->displayProperties()->displayName());
    setText(0, m_bundleResults->runTime());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":results"));

    connect(m_bundleResults, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  BundleResultsTreeWidgetItem::~BundleResultsTreeWidgetItem() {
    m_bundleResults = NULL;
  }


  BundleResults *BundleResultsTreeWidgetItem::bundleResults() {
    return m_bundleResults;
  }


  void BundleResultsTreeWidgetItem::selectionChanged() {
//    m_control->displayProperties()->setSelected(isSelected());
  }
}
