#include "BundleStatisticsTreeWidgetItem.h"

#include <QDebug>

#include "BundleStatistics.h"
//#include "BundleResultsDisplayProperties.h"

namespace Isis {

  /**
   * BundleStatisticsTreeWidgetItem constructor.
   * BundleStatisticsTreeWidgetItem is derived from QTreeWidgetItem
   *
   *
   * @param parent
   */
  BundleStatisticsTreeWidgetItem::BundleStatisticsTreeWidgetItem(BundleStatistics *bundleStatistics,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_bundleStatistics = bundleStatistics;

    setText(0, "Statistics");
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    setIcon(0, QIcon(":statistics"));

    connect(m_bundleStatistics, SIGNAL(destroyed(QObject *)), this, SLOT(deleteLater()));
  }


  BundleStatisticsTreeWidgetItem::~BundleStatisticsTreeWidgetItem() {
    m_bundleStatistics = NULL;
  }


  BundleStatistics *BundleStatisticsTreeWidgetItem::bundleStatistics() {
    return m_bundleStatistics;
  }


  void BundleStatisticsTreeWidgetItem::selectionChanged() {
//    m_control->displayProperties()->setSelected(isSelected());
  }
}
